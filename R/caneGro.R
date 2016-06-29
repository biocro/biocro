## BioCro/R/caneGro.R by Deepak Jaiswal 

caneGro <- function(WetDat, day1=5, dayn=360,
                   timestep=1,
                   lat=40,iRhizome=3,irtl=1e-2,
                   canopyControl=list(),
                   seneControl=list(),
                   photoControl=list(),
                   phenoControl=list(),
                   soilControl=list(),
                   nitroControl=list(),
                   canephenoControl=list(),
                   centuryControl=list(),
                   managementControl=list(),
                   frostControl=list()
)
  {

    
## Trying to guess the first and last day of the growing season from weather data
    
    if(missing(day1)){
        half <- as.integer(dim(WetDat)[1]/2)
        WetDat1 <- WetDat[1:half,c(2,5)]
        if(min(WetDat1[,2]) > 0){
          day1 <- 90
        }else{
        WetDat1s <- WetDat1[which(WetDat1[,2]<0),]
        day1 <- max(WetDat1s[,1])
        if(day1 < 90) day1 <- 90
      }
     }
    if(missing(dayn)){
        half <- as.integer(dim(WetDat)[1]/2)
        WetDat1 <- WetDat[half:dim(WetDat)[1],c(2,5)]
        if(min(WetDat1[,2]) > 0){
          dayn <- 330
        }else{
          WetDat1s <- WetDat1[which(WetDat1[,2]<0),]
          dayn <- min(WetDat1s[,1])
          if(dayn > 330) dayn <- 330
        }
      }
    
#    if((day1<0) || (day1>365) || (dayn<0) || (dayn>365))
#      stop("day1 and dayn should be between 0 and 365")

#    if(day1 > dayn)
#      stop("day1 should be smaller than dayn")

    if( (timestep<1) || (24%%timestep != 0))
      stop("timestep should be a divisor of 24 (e.g. 1,2,3,4,6,etc.)")

    ## Getting the Parameters
    canopyP <- canopyParms(Sp=1.4,SpD=0.0,chi.l=1.0)
    canopyP[names(canopyControl)] <- canopyControl
    
    soilP <- canesoilParms()
    soilP[names(soilControl)] <- soilControl

    nitroP <- canenitroParms()
    nitroP[names(nitroControl)] <- nitroControl
    nnitroP<-as.vector(unlist(nitroP))
    
   
    phenoP <- phenoParms(tp1=350,tp2=475,tp3=800,tp4=1800, tp5=2800, tp6=10000,
                     kStem1=0.38, kLeaf1=0.39, kRoot1=0.23, kRhizome1=-8e-4, 
                     kStem2=0.01, kLeaf2=0.76, kRoot2=0.23, kRhizome2=0, 
                     kStem3=0.38, kLeaf3=0.39, kRoot3=0.23, kRhizome3=0, 
                     kStem4=0.63, kLeaf4=0.25, kRoot4=0.12, kRhizome4=0,
                     kStem5=0.63, kLeaf5=0.25, kRoot5=0.12, kRhizome5=0,
                     kStem6=0.63, kLeaf6=0.25, kRoot6=0.12, kRhizome6=0)
    phenoP[names(phenoControl)] <- phenoControl

    sugarphenoP<-canephenoParms()
    sugarphenoP[names(canephenoControl)]<-canephenoControl

    photoP <- photoParms(b0=0.03, b1=12,Catm=380, uppertemp=39.8, lowertemp=9.0)
    photoP[names(photoControl)] <- photoControl

    seneP <- caneseneParms()
    seneP[names(seneControl)] <- seneControl

    centuryP <- centuryParms()
    centuryP[names(centuryControl)] <- centuryControl
    
    manage<-managementParms()
    manage[names(managementControl)] <-managementControl
    irrigationfactor<-as.vector(unlist(manage))
    
    frostP<-frostParms()
    frostP[names(frostControl)]<-frostControl
    frostP<-as.vector(unlist(frostP))
    
    tint <- 24 / timestep
    vecsize <- (dayn - (day1-1)) * tint + 1
    indes1 <- (day1-1) * tint
    indesn <- (dayn) * tint
    
    doy <- WetDat[indes1:indesn,2]
    hr <- WetDat[indes1:indesn,3]
    solar <- WetDat[indes1:indesn,4]
    temp <- WetDat[indes1:indesn,5]
    rh <- WetDat[indes1:indesn,6]
    windspeed <- WetDat[indes1:indesn,7]
    precip <- WetDat[indes1:indesn,8]

    if(max(rh) > 1) stop("Rel Hum. should be 0 < rh < 1")
    if((min(hr) < 0) | (max(hr) > 23)) stop("hr should be between 0 and 23")
    
    DBPcoefs <- valid_dbp(as.vector(unlist(phenoP)[7:31]))

    TPcoefs <- as.vector(unlist(phenoP)[1:6])

    sugarcoefs <-as.vector(unlist(sugarphenoP))

    SENcoefs <- as.vector(unlist(seneP))

    soilCoefs <- c(unlist(soilP[1:5]),mean(soilP$iWatCont),soilP$scsf, soilP$transpRes, soilP$leafPotTh)
    wsFun <- soilP$wsFun
    soilType <- soilP$soilType
    rootfrontvelocity=soilP$rootfrontvelocity;
    optiontocalculaterootdepth=soilP$optiontocalculaterootdepth;
    

    centCoefs <- as.vector(unlist(centuryP)[1:24])

    if(centuryP$timestep == "year"){
      stop("Not developed yet")
      centTimestep <- dayn - day1 ## This is really the growing season
    }
    if(centuryP$timestep == "week") centTimestep <- 7
    if(centuryP$timestep == "day") centTimestep <- 1
    
    vmax <- photoP$vmax
    alpha <- photoP$alpha
    kparm <- photoP$kparm
    theta <- photoP$theta
    beta <- photoP$beta
    Rd <- photoP$Rd
    Catm <- photoP$Catm
    b0 <- photoP$b0
    b1 <- photoP$b1
    ws <- photoP$ws
    upperT<-photoP$uppertemp
    lowerT<-photoP$lowertemp

    mResp <- canopyP$mResp
    kd <- canopyP$kd
    chi.l <- canopyP$chi.l
    Sp <- canopyP$Sp
    SpD <- canopyP$SpD
    heightF <- canopyP$heightFactor
    nlayers <- canopyP$nlayers
	thermal_base_temp = 9
    
    res <- .Call("caneGro",
                 as.double(lat),
                 as.integer(doy),
                 as.integer(hr),
                 as.double(solar),
                 as.double(temp),
                 as.double(rh),
                 as.double(windspeed),
                 as.double(precip),
                 as.double(kd),
                 as.double(c(chi.l,heightF)),
                 as.integer(nlayers),
                 as.double(iRhizome),
                 as.double(irtl),
                 as.double(SENcoefs),
                 as.integer(timestep),
                 as.integer(vecsize),
                 as.double(Sp),
                 as.double(SpD),
                 as.double(DBPcoefs),
                 as.double(TPcoefs),
				 as.double(thermal_base_temp),
                 as.double(vmax),
                 as.double(alpha),
                 as.double(kparm),
                 as.double(theta),
                 as.double(beta),
                 as.double(Rd),
                 as.double(Catm),
                 as.double(b0),
                 as.double(b1),
                 as.integer(ws),
                 as.double(soilCoefs),
                 as.double(nitroP$iLeafN),
                 as.double(nitroP$kLN), 
                 as.double(nitroP$Vmax.b1),
                 as.double(nitroP$alpha.b1),
                 as.double(mResp),
                 as.integer(soilType),
                 as.integer(wsFun),
                 as.double(centCoefs),
                 as.integer(centTimestep),
                 as.double(centuryP$Ks),
                 as.integer(soilP$soilLayers),
                 as.double(soilP$soilDepths),
                 as.double(soilP$iWatCont),
                 as.integer(soilP$hydrDist),
                 as.double(c(soilP$rfl,soilP$rsec,soilP$rsdf)),
                 as.double(nitroP$kpLN),
                 as.double(nitroP$lnb0),
                 as.double(nitroP$lnb1),
                 as.integer(nitroP$lnFun),
                 as.double(sugarcoefs),
                 as.double(upperT),
                 as.double(lowerT),
                 as.double(nitroP$maxln),
                 as.double(nitroP$minln),
                 as.double(nitroP$daymaxln),
                 as.double(seneP$leafturnover),
                 as.double(seneP$rootturnover),
                 as.double(seneP$leafremobilizefraction),
                 as.integer(optiontocalculaterootdepth),
                 as.double(rootfrontvelocity),
                 as.double(nnitroP),
                 as.double(irrigationfactor),
                 as.double(frostP)
                 )
    
    res$cwsMat <- t(res$cwsMat)
    colnames(res$cwsMat) <- soilP$soilDepths[-1]
    res$rdMat <- t(res$rdMat)
    colnames(res$rdMat) <- soilP$soilDepths[-1]
    res$psimMat <- t(res$psimMat)
    colnames(res$psimMat) <- soilP$soilDepths[-1]
    structure(res,class="caneGro")
  }

canesoilParms <- function(FieldC=NULL,WiltP=NULL,phi1=0.01,phi2=10,soilDepth=1,iWatCont=NULL,
                      soilType=6, soilLayers=1, soilDepths=NULL, hydrDist=0,
                      wsFun=c("linear","logistic","exp","none","lwp"),
                      scsf = 1, transpRes = 5e6, leafPotTh = -800,
                      rfl=0.2, rsec=0.2, rsdf=0.44,optiontocalculaterootdepth=1,rootfrontvelocity=0.5){

  if(soilLayers < 1 || soilLayers > 50)
    stop("soilLayers must be an integer larger than 0 and smaller than 50")

  if(missing(iWatCont)){
    if(missing(FieldC))
      iWatCont <- rep(SoilType(soilType)$fieldc,soilLayers)
    else
      iWatCont <- rep(FieldC,soilLayers)
  }else{
    if(length(iWatCont) == 1)
      iWatCont <- rep(iWatCont,soilLayers)
  }

  if(length(iWatCont) != soilLayers){
    stop("iWatCont should be NULL, of length 1 or length == soilLayers")
  }

  if(missing(soilDepths)){
    soilDepths <- seq(0,soilDepth,length.out=I(soilLayers+1))
  }else{
    if(length(soilDepths) != I(soilLayers+1)) stop("soilDepths should be of length == soilLayers + 1")
  }

  if(missing(FieldC)) FieldC <- -1

  if(missing(WiltP))  WiltP <- -1
  
  wsFun <- match.arg(wsFun)
  if(wsFun == "linear")  wsFun <- 0
  else if(wsFun == "logistic") wsFun <- 1
  else if(wsFun =="exp") wsFun <- 2 
  else if(wsFun == "none") wsFun <- 3
  else if(wsFun == "lwp") wsFun <- 4
  
  list(FieldC=FieldC,WiltP=WiltP,phi1=phi1,phi2=phi2,soilDepth=soilDepth,iWatCont=iWatCont,
       soilType=soilType,soilLayers=soilLayers,soilDepths=soilDepths, wsFun=wsFun,
       scsf = scsf, transpRes = transpRes, leafPotTh = leafPotTh,
       hydrDist=hydrDist, rfl=rfl, rsec=rsec, rsdf=rsdf,optiontocalculaterootdepth=optiontocalculaterootdepth,rootfrontvelocity=rootfrontvelocity)
}


canephenoParms <- function(TT0=400, TTseed=800, Tmaturity=3000,Rd=0.85,Alm=0.1,Arm=0.2,
	Clstem=0.008, Ilstem=2.0,Cestem=-0.08,Iestem=6, Clsuc=0.006,Ilsuc=1.2,Cesuc=-0.01,Iesuc=20){

  # Think about Error conditions in parameter values
  # TT0= End of germination phase
  # TTseed = seed cane stops providing nutrients/C for plant parts
  # Tmaturity = Thermal time required to reach the maturity for the crop
  # Rd= decline coefficients for root allocation
  # Alm = minimum fraction to leaf
  # minimum fraction to root
  # Clstem and Ilstem togetehr determines when the linear phase of stem allocation ends
  # Cestem and Iestem togetger determines when the log phase of stem allocation ends
  # Clsuc and Ilsuc determines when the linear phase of sugar fraction ends
  # Cesuc and Iesuc determines when the log phase of sugar fraction ends

  if(Clsuc>0.01)
    stop("Clsuc should be lower than 0.01") 
  list(TT0=TT0,TTseed=TTseed, Tmaturity=Tmaturity,Rd=Rd,Alm=Alm,Arm=Arm,
	Clstem=Clstem, Ilstem=Ilstem,Cestem=Cestem,Iestem=Iestem, Clsuc=Clsuc,Ilsuc=Ilsuc,Cesuc=Cesuc,Iesuc=Iesuc)
  
}



caneseneParms <- function(senLeaf=0,senStem=10000,senRoot=0,senRhizome=4000,leafturnover=1.36,rootturnover=0.2,leafremobilizefraction=0.6){

  list(senLeaf=senLeaf,senStem=senStem,senRoot=senRoot,senRhizome=senRhizome,leafturnover=leafturnover,rootturnover=rootturnover,leafremobilizefraction=leafremobilizefraction)

}


canenitroParms <- function(iLeafN=85, kLN=0.5, Vmax.b1=0.6938, Vmax.b0=-16.25,alpha.b1=0.000488,alpha.b0=0.02367,Rd.b1=0.1247,Rd.b0=-4.5917,
                       kpLN=0.17, lnb0 = -5, lnb1 = 18, lnFun=c("linear"),maxln=85,minln=57,daymaxln=60){

  lnFun <- match.arg(lnFun)
  if(lnFun == "none"){
    lnFun <- 0
  }else{
    lnFun <- 1
  }
  
  list(iLeafN=iLeafN, kLN=abs(kLN), Vmax.b1=Vmax.b1, Vmax.b0=Vmax.b0,alpha.b1=alpha.b1, alpha.b0=alpha.b0,Rd.b1=Rd.b1,Rd.b0=Rd.b0,kpLN=kpLN,
       lnb0 = lnb0, lnb1 = lnb1, lnFun = lnFun,maxln=maxln,minln=minln,daymaxln=daymaxln)

}


managementParms <-function (irrigationFactor=0.0){
  list(irrigationFactor=irrigationFactor)
}

frostParms <-function (leafT0=0.0,leafT100=-5.6,stemT0=-272,stemT100=-273){
  list(leafT0=leafT0,leafT100=leafT100,stemT0=stemT0,stemT100=stemT100)
}
