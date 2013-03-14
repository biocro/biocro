c4photoR <- function(Qp,Tl,RH,vmax=39,alpha=0.04,kparm=0.7,theta=0.83,
                    beta=0.93,Rd=0.8,Catm=380,b0=0.08,b1=3,
                    StomWS=1,ws=c("gs","vmax"))
{
    if((max(RH) > 1) || (min(RH) < 0))
        stop("RH should be between 0 and 1")
    if(any(Catm < 150))
        warning("Stomatal conductance is not reliable for values of Catm lower than 150\n")
    if(any(Catm < 15))
        stop("Assimilation is not reliable for low (<15) Catm values")
    ws <- match.arg(ws)
    if(ws == "gs") ws <- 1
    else ws <- 0

    if(length(Catm) == 1){
      Catm <- rep(Catm,length(Qp))
    }else{
      if(length(Catm) != length(Qp))
        stop("length of Catm should be either 1 or equal to length of Qp")
    }

    if(StomWS < 0.5) ws = 0

    conv <- 1
    
    bb0 <- b0
    bb1 <- b1
    
    AP = 101325
    P = AP / 1e3
    Q10 = 2

    Csurface = (Catm * 1e-6) * AP

    InterCellularCO2 = Csurface * 0.4
      
    KQ10 =  Q10^((Tl - 25.0) / 10.0);

    kT = kparm * KQ10

    OldAssim = 0.0
    Tol = 0.1
    
    ## /* First chunk of code see Collatz (1992) */
    Vtn = vmax * 2^((Tl-25.0)/10.0)
    Vtd = ( 1 + exp(0.3 * (3.0-Tl)) ) * (1 + exp( 0.3*(Tl-37.5) ));
    VT  = Vtn / Vtd;

    ## /* Second chunk of code see Collatz (1992) */
    Rtn = Rd * 2^((Tl-25)/10) ;
    Rtd =  1 + exp( 1.3 * (Tl-55) ) ;
    RT = Rtn / Rtd ; 

    ## /* Third chunk of code again see Collatz (1992) */
    b0 = VT * alpha  * Qp ;
    b1 = VT + alpha  * Qp ;
    b2 = theta ;

    ## /* Calculate the 2 roots */
    M1 = (b1 + sqrt(b1*b1 - (4 * b0 * b2)))/(2*b2) ;
    M2 = (b1 - sqrt(b1*b1 - (4 * b0 * b2)))/(2*b2) ;

    ## /* This piece of code selects the smalles root */
    if(M1 < M2){
      M = M1;
    }else{
      M = M2;
    }

    ## /* Here the iterations will start */
    iterCounter = 0;
    i = 1

    diffV = numeric(51)
    AssimV = numeric(51)
    GsV = numeric(51)
    CiV = numeric(51)

    while(iterCounter < 52)
      {

        kT_IC_P = kT * (InterCellularCO2 / P*1000);
        Quada = M * kT_IC_P;
        Quadb = M + kT_IC_P;
        Quadc = beta ;

        a2 = (Quadb - sqrt(Quadb*Quadb - (4 * Quada * Quadc))) / (2 * Quadc);

        Assim = a2 - RT;

        if(ws == 0) Assim = Assim * StomWS; 

        if(iterCounter > 50){
          if(Tl < 15){
            if(OldAssim < Assim){
              Assim = OldAssim;
            }
          }else{
##            Assim = (Assim + OldAssim) / 2;
            Assim = Assim 
          }

          conv = 1
          break
        }
        ## /* milimole per meter square per second*/
        csurfaceppm = Csurface * 10 ;

        ## /* Need to create the Ball-Berry function */
        ## print(c(Assim,csurfaceppm, Tl, RH, b0, b1))
        Gs =  ballBerry(Assim,csurfaceppm, Tl, RH, bb0, bb1) ;
        if(ws == 1) Gs =  Gs * StomWS; 

        InterCellularCO2 = Csurface - (Assim * 1e-6 * 1.6 * AP) / (Gs * 0.001);

        if(InterCellularCO2 < 0)
          InterCellularCO2 = 1e-5;

        if(iterCounter == 0){
          Assim0 = Assim;
          Gs0 = Gs;
          InterCellularCO20 = InterCellularCO2;
        }

        diff = OldAssim - Assim;
        if(diff < 0) diff = -diff;
        diffV[i] = diff;
        AssimV[i] = Assim
        GsV[i] = Gs
        CiV[i] = (InterCellularCO2 / AP) * 1e6 ;
        if(diff < Tol){
          conv = 0
          break;
        }else{
          OldAssim = Assim;
        }

        iterCounter = iterCounter + 1
        i = i + 1
      }

    if(diff > Tol){
      Assim = Assim0;
      Gs = Gs0;
      InterCellularCO2 = InterCellularCO20;
    }
    
    miC = (InterCellularCO2 / AP) * 1e6 ;

    if(Gs > 600)
      Gs = 600;

    structure(list(Assim = Assim, Gs=Gs, Ci=miC, iter=i,
              diffV=diffV[1:i] , AssimV=AssimV[1:i], GsV=GsV[1:i],
                   conv=conv))

}


## /* ball Berry stomatal conductance function */ 
ballBerry <- function(Amu, Cappm, Temp, RelH, beta0, beta1){

  gbw = 1.2
  ptotPa = 101325
  ## const double gbw = 1.2; /* According to Collatz et al. (1992) pg. 526*/
  ## const double ptotPa = 101325; /* Atmospheric pressure */

  leafTk = Temp + 273.15;
  pwi = fnpsvp(leafTk);
  pwaPa = RelH * pwi;
  Camf = Cappm * 1e-6;
  assimn = Amu * 1e-6;
  
  ## /* Calculate mole fraction (mixing ratio) of water vapor in */
  ##   /* atmosphere from partial pressure of water in the atmosphere and*/
  ##     /* the total air pressure */
  wa  = pwaPa / ptotPa;
  ## /* Get saturation vapor pressure for water in the leaf based on */
  ##   /* current idea of the leaf temperature in Kelvin*/
  ##     /* Already calculated above */
  ##       /* Calculate mole fraction of water vapor in leaf interior */
  wi  = pwi / ptotPa;

  if(assimn < 0.0){
    ## /* Set stomatal conductance to the minimum value, beta0*/
    gswmol = beta0 ;
    ## /* Calculate leaf surface relative humidity, hs, (as fraction)*/
    ## /* for when C assimilation rate is <= 0*/
    ## /* hs = (beta0 + (wa/wi)*gbw)/(beta0 + gbw); ! unused here??*/
  }
  else{
    ## /* leaf surface CO2, mole fraction */
    Cs  = Camf - (1.4/gbw)*assimn;
    if(Cs < 0.0)
      Cs = 1;
    ## /* Constrain the ratio assimn/cs to be > 1.e-6*/
    acs = assimn/Cs;

    if(acs < 1e-6) 	acs = 1e-6;		

    ## /* Calculate leaf surface relative humidity, hs, from quadratic */
    ##   /* equation: aaa*hs^2 + bbb*hs + ccc = 0 */
    ##     /*  aaa= beta1 * assimn / cs */
    aaa = beta1 * acs;
    ## /*      bbb= beta0 + gbw - (beta1 * assimn / cs)*/
    bbb = beta0 + gbw - (beta1 * acs);
    ccc = -(wa / wi) * gbw - beta0;

    ## /* Solve the quadratic equation for leaf surface relative humidity */
    ##   /* (as fraction) */
    ddd = bbb * bbb - 4*aaa*ccc;

    hs  = (-bbb + sqrt(ddd)) / (2* aaa);

    ## /* Ball-Berry equation (Collatz'91, eqn 1) */
    gswmol = beta1 * hs * acs + beta0;
  }
  gsmol = gswmol * 1000 ## ; /* converting to mmol */

  if(gsmol <= 0) gsmol = 1e-5;

  gsmol
}


fnpsvp <- function(Tkelvin){

	tmp = Tkelvin - 273.15;
	u = (18.678 - tmp/234.5)*tmp;
	v = 257.14 + tmp;
	esat = 6.1121 * exp(u/v);
	esat = esat / 10;

	esat
}

test_c4photoR <- function(data, StomWS = 1,
                          vmax = 39, alpha = 0.04,
                          b0 = 0.01, b1 = 3){

  convs <- numeric(nrow(data))
  iters <- numeric(nrow(data))
  assim <- numeric(nrow(data))
  gs <- numeric(nrow(data))
  
  for(i in 1:nrow(data)){
  
    ans <- c4photoR(data[i,1],data[i,2],data[i,3],
                    vmax = vmax, alpha = alpha,
                    b0 = b0, b1 = b1,
                    StomWS=StomWS)


    convs[i] <- ans$conv
    iters[i] <- ans$iter
    assim[i] <- ans$Assim
    gs[i] <- ans$Gs
  }

  dat <- data.frame(data,conv=convs,iters=iters,Assim=assim,Gs=gs)
  dat

}

test_c4photo <- function(data, StomWS = 1,
                          vmax = 39, alpha = 0.04,
                          b0 = 0.01, b1 = 3){

  assim <- numeric(nrow(data))
  gs <- numeric(nrow(data))
  
  for(i in 1:nrow(data)){
  
    ans <- c4photo(data[i,1],data[i,2],data[i,3],
                    vmax = vmax, alpha = alpha,
                    b0 = b0, b1 = b1,
                    StomWS=StomWS)


    assim[i] <- ans$Assim
    gs[i] <- ans$Gs
  }

  dat <- data.frame(data,Assim=assim,Gs=gs)
  dat

}


test_ballBerry <- function(data, 
                          b0 = 0.01, b1 = 3){

  gs <- numeric(nrow(data))

  for(i in 1:nrow(data)){
  
    ans <- ballBerry(data[i,1],data[i,2],data[i,3],data[i,4],
                     beta0 = b0, beta1 = b1)


    gs[i] <- ans

  }

  dat <- data.frame(data,gs=gs)

}
