## This functions illustrate the effect of soil water content in the soil
##' Simple function to illustrate soil water content effect on plant water
##' stress.
##'
##' This is a very simple function which implements the 'bucket' model for soil
##' water content and it calculates a coefficient of plant water stress.
##'
##' This is a very simple function and the details can be seen in the code.
##'
##' @aliases ws wsRcoef
##' @param precipt Precipitation (mm).
##' @param evapo Evaporation (Mg H2O ha-1 hr-1).
##' @param cws current water content (fraction).
##' @param soildepth Soil depth, typically 1m.
##' @param fieldc Field capacity of the soil (fraction).
##' @param wiltp Wilting point of the soil (fraction).
##' @param phi1 coefficient which controls the spread of the logistic function.
##' @param phi2 coefficient which controls the effect on leaf area expansion.
##' @param wsFun option to control which method is used for the water stress
##' function.
##' @export
##' @return A list with components:
##' \itemize{
##' \item rcoefPhoto coefficient of plant water stress for photosyntheis.
##' \item rcoefSpleaf coefficient of plant water stress for specific leaf
##' area.
##' \item naw New available water in the soil.
##' }
##' @seealso \code{\link{wsRcoef}}
##' @keywords models
##' @examples
##'
##'
##' ## Looking at the three possible models for the effect of soil moisture on water
##' ## stress
##'
##' aws <- seq(0,0.4,0.001)
##' wats.L <- numeric(length(aws)) # linear
##' wats.Log <- numeric(length(aws)) # logistic
##' wats.exp <- numeric(length(aws)) # exp
##' wats.none <- numeric(length(aws)) # none
##' for(i in 1:length(aws)){
##' wats.L[i] <- wtrstr(1,1,aws[i],0.5,0.37,0.2,2e-2,4)$wsPhoto
##' wats.Log[i] <- wtrstr(1,1,aws[i],0.5,0.37,0.2,2e-2,4,wsFun='logistic')$wsPhoto
##' wats.exp[i] <- wtrstr(1,1,aws[i],0.5,0.37,0.2,2e-2,4, wsFun='exp')$wsPhoto
##' wats.none[i] <- wtrstr(1,1,aws[i],0.5,0.37,0.2,2e-2,4, wsFun='none')$wsPhoto
##' }
##'
##' lattice::xyplot(wats.L + wats.Log + wats.exp  + wats.none~ aws,
##'        col=c('blue','green','purple','red'),
##'        type = 'l',
##'        xlab='Soil Water',
##'        ylab='Stress Coefficient',
##'        key = list(text=list(c('linear','logistic','exp', 'none')),
##'        col=c('blue','green','purple','red'), lines = TRUE) )
##'
##' ## This function is sensitive to the soil depth parameter
##'
##' SDepth <- seq(0.05,2,0.05)
##'
##' wats <- numeric(length(SDepth))
##'
##' for(i in 1:length(SDepth)){
##' wats[i] <- wtrstr(1,1,0.3,SDepth[i],0.37,0.2,2e-2,3)$wsPhoto
##' }
##'
##' lattice::xyplot(wats ~ SDepth, ylab='Water Stress Coef',
##'        xlab='Soil depth')
##'
##' ## Difference between the effect on assimilation and leaf expansion rate
##'
##' aws <- seq(0,0.4,0.001)
##' wats.P <- numeric(length(aws))
##' wats.L <- numeric(length(aws))
##' for(i in 1:length(aws)){
##' wats.P[i] <- wtrstr(1,1,aws[i],0.5,0.37,0.2,2e-2,4)$wsPhoto
##' wats.L[i] <- wtrstr(1,1,aws[i],0.5,0.37,0.2,2e-2,4)$wsSpleaf
##' }
##'
##' lattice::xyplot(wats.P + wats.L ~ aws,
##'        xlab='Soil Water',
##'        ylab='Stress Coefficient')
##'
##'
##' ## An example for wsRcoef
##' ## The scale parameter makes a big difference
##'
##' aws <- seq(0.2,0.4,0.001)
##' wats.1 <- wsRcoef(aw=aws,fieldc=0.37,wiltp=0.2,phi1=1e-2,phi2=1, wsFun='logistic')$wsPhoto
##' wats.2 <- wsRcoef(aw=aws,fieldc=0.37,wiltp=0.2,phi1=2e-2,phi2=1, wsFun='logistic')$wsPhoto
##' wats.3 <- wsRcoef(aw=aws,fieldc=0.37,wiltp=0.2,phi1=3e-2,phi2=1, wsFun='logistic')$wsPhoto
##'
##' lattice::xyplot(wats.1 + wats.2 + wats.3 ~ aws,type='l',
##'        col=c('blue','red','green'),
##'        ylab='Water Stress Coef',
##'        xlab='SoilWater Content',
##'        key=list(text=list(c('phi1 = 1e-2','phi1 = 2e-2','phi1 = 3e-2')),
##'          lines=TRUE,col=c('blue','red','green')))
##'
##'
##'
wtrstr <- function(precipt,evapo,cws,soildepth,fieldc,wiltp,phi1=0.01,phi2 =10, wsFun = c("linear","logistic","exp","none")){

  wsFun <- match.arg(wsFun)
  ## precipitation data are entered in mm but need to convert to m
  precipM = precipt * 1e-3;
  ## precipitation in meters can now be added to avaliable water
  ## to indicate the new avaliable water
  aw = precipM + cws;

  ## If water exceeds field capacity it is considered to runoff
  if(aw > fieldc){ 
       runoff = aw - fieldc;
       aw = fieldc;
     }

  ## available water in 1 ha
  awha = aw * 1e4 * soildepth;
  ## Since evaporation is entered in Mg H2O ha-1
  ## This substraction can be made
  Newawha = awha - evapo;

  ## Need to convert back to new available water
  naw = Newawha * 1e-4 * (1/soildepth);
  ## I impose a limit of water which is the wilting point
  if(naw < 0) naw = 0;

  ## This is an empirical way of simulating water stress
  if(wsFun == "linear"){
    slp = 1/(fieldc - wiltp);
    intcpt = 1 - fieldc / (fieldc - wiltp);
    wsPhoto = slp * naw + intcpt ;
  }else
  if(wsFun == "logistic"){
    phi10 = (fieldc + wiltp)/2;
    wsPhoto = 1/(1 + exp((phi10 - naw)/phi1));
  }else
  if(wsFun == "exp"){
    slp = (1 - wiltp)/(fieldc - wiltp);
    intcpt = 1 - fieldc * slp;
    theta = slp * naw + intcpt ;
    wsPhoto = (1 - exp(-1 * (theta - wiltp)/(1 - wiltp))) / (1 - exp(-1));
  }else{
    wsPhoto = 1;
  }
    
  if(wsPhoto < 0) wsPhoto = 0;
  wsSpleaf = naw^phi2 * 1/fieldc^phi2;

  list(wsPhoto=wsPhoto,wsSpleaf=wsSpleaf,awc=naw)

}

wsRcoef <- function(aw,fieldc,wiltp,phi1,phi2, wsFun = c("linear","logistic","exp","none") ){

  wsFun <- match.arg(wsFun)

    if(wsFun == "linear"){
    slp = 1/(fieldc - wiltp);
    intcpt = 1 - fieldc / (fieldc - wiltp);
    wsPhoto = slp * aw + intcpt ;
  }else
  if(wsFun == "logistic"){
    phi10 = (fieldc + wiltp)/2;
    wsPhoto = 1/(1 + exp((phi10 - aw)/phi1));
  }else
  if(wsFun == "exp"){
    slp = (1 - wiltp)/(fieldc - wiltp);
    intcpt = 1 - fieldc * slp;
    theta = slp * aw + intcpt ;
    wsPhoto = (1 - exp(-2.5 * (theta - wiltp)/(1 - wiltp))) / (1 - exp(-2.5));
  }else{
    wsPhoto = 1;
  }

  wsPhoto[wsPhoto < 0] <- 0
  wsSpleaf <- aw^phi2 * 1/fieldc^phi2
  list(wsPhoto=wsPhoto,wsSpleaf=wsSpleaf)
}
