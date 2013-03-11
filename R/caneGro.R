##  BioCro/R/caneGro.R by Deepak Jaiswal

caneGro <- function(WetDat, day1=30, dayn=330,
                   timestep=1,
                   lat=23,plantingdensity=1,ileaf=0.001,
                   canopyControl=list(),
                   seneControl=list(),
                   photoControl=list(),
                   phenoControl=list(),
                   soilControl=list(),
                   nitroControl=list()
                   )
  {

    

    
#    if((day1<0) || (day1>365) || (dayn<0) || (dayn>365))
#      stop("day1 and dayn should be between 0 and 365")

#    if(day1 > dayn)
#      stop("day1 should be smaller than dayn")

    if( (timestep<1) || (24%%timestep != 0))
      stop("timestep should be a divisor of 24 (e.g. 1,2,3,4,6,etc.)")

    ## Getting the Parameters
    canopyP <- canecanopyParms()
    canopyP[names(canopyControl)] <- canopyControl
    
    soilP <- canesoilParms()
    soilP[names(soilControl)] <- soilControl

    nitroP <- canenitroParms()
    nitroP[names(nitroControl)] <- nitroControl

    phenoP <- canephenoParms()
    phenoP[names(phenoControl)] <- phenoControl

    photoP <- photoParms()
    photoP[names(photoControl)] <- photoControl

    seneP <- caneseneParms()
    seneP[names(seneControl)] <- seneControl

    

    tint <- 24 / timestep
    vecsize <- (dayn - (day1-1)) * tint
    indes1 <- (day1-1) * tint
    indesn <- (dayn) * tint
    
    doy <- WetDat[indes1:indesn,2]
    hr <- WetDat[indes1:indesn,3]
    solar <- WetDat[indes1:indesn,4]
    temp <- WetDat[indes1:indesn,5]
    rh <- WetDat[indes1:indesn,6]
    windspeed <- WetDat[indes1:indesn,7]
    precip <- WetDat[indes1:indesn,8]

    if(max(rh) > 1)
      stop("Rel Hum. should be 0 < rh < 1")
    if((min(hr) < 0) | (max(hr) > 23))
      stop("hr should be between 0 and 23")

    photocoeff<-unlist(photoP[1:12])
    soilcoeff<-unlist(soilP[1:17])
    cwsvec<-unlist(soilP$iWatCont)
    soildepths<-unlist(soilP$soilDepths)
    canopycoeff<-unlist(canopyP)
    phenocoeff<-unlist(phenoP)
    sencoeff<-unlist(seneP)
    nitrocoeff<-unlist(nitroP)
    
    res <- .Call(caneGro,
                 as.double(vecsize),
                 as.integer(timestep),
                 as.double(plantingdensity),
                 as.double(ileaf),
                 as.double(lat),
                 as.double(doy),
                 as.double(hr),
                 as.double(solar),
                 as.double(temp),
                 as.double(rh),
                 as.double(windspeed),
                 as.double(precip),
                 as.double(photocoeff),
                 as.double(canopycoeff),
                 as.double(phenocoeff),
                 as.double(nitrocoeff),
                 as.double(senecoeff),
                 as.double(soilcoeff),
                 as.double(cwsvec),
                 as.double(soildepths)
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
  
  list(FieldC=FieldC,WiltP=WiltP,phi1=phi1,phi2=phi2,soilDepth=soilDepth,
       soilType=soilType,soilLayers=soilLayers, wsFun=wsFun,
       scsf = scsf, transpRes = transpRes, leafPotTh = leafPotTh,
       hydrDist=hydrDist, rfl=rfl, rsec=rsec, rsdf=rsdf,optiontocalculaterootdepth=optiontocalculaterootdepth,rootfrontvelocity=rootfrontvelocity,iWatCont=iWatCont,soilDepths=soilDepths)
}

canecanopyParms <- function(Sp = 1.7, SpD = 0, nlayers = 10,
                        kd = 0.1, chi.l = 1,
                        Growthresp=0.25,Qleaf=1.58, Qstem=1.80, Qroot=1.80,mRespleaf=0.012,
                        mRespstem=0.004,mResproot=0.0088, heightFactor=3){

  if((nlayers < 1) || (nlayers > 50))
    stop("nlayers should be between 1 and 50")

  if(Sp <= 0)
    stop("Sp should be positive")

  if(heightFactor <= 0)
    stop("heightFactor should be positive")

  list(Sp=Sp,SpD=SpD,nlayers=nlayers,kd=kd,chi.l=chi.l,
      Growthresp=Growthresp,Qleaf=Qleaf, Qstem=Qstem, Qroot=Qroot,mRespleaf=mRespleaf,
      mRespstem=mRespstem,mResproot=mResproot, heightFactor=heightFactor)
}



canephenoParms <- function(TT0=200, TTseed=800, Tmaturity=6000,Rd=0.06,Alm=0.15,Arm=0.08,
	Clstem=0.04, Ilstem=7,Cestem=-0.05,Iestem=45, Clsuc=0.01,Ilsuc=25,Cesuc=-0.02,Iesuc=45,Tbase=12.0){

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
	Clstem=Clstem, Ilstem=Ilstem,Cestem=Cestem,Iestem=Iestem, Clsuc=Clsuc,Ilsuc=Ilsuc,Cesuc=Cesuc,Iesuc=Iesuc,Tbase=Tbase)
  
}

caneseneParms <- function(senLeaf=3000,senStem=3500,senRoot=4000,senSeedcane=4000,stemturnover=0.0,leafturnover=1.36,rootturnover=2.0,leafremobilizefraction=0.6,stemremobilizefraction=0.6,rootremobilizefraction=0.6,seedcaneremobilizefraction=0.6,option=c("thermal time based","percentage of standing biomass per day"),Tfrost.0percent=0.0,Tfrost.100percent=-5.6,minCanopyN=40){

  option<-match.arg(option)
  if(option=="thermal time based"){
    option <-0
  } else {
    option<-1
  }
  list(senLeaf=senLeaf,senStem=senStem,senRoot=senRoot,senSeedcane=senSeedcane,leafturnover=leafturnover,rootturnover=rootturnover,stemturnover=stemturnover,leafremobilizefraction=leafremobilizefraction,stemremobilizefraction=stemremobilizefraction,rootremobilizefraction=rootremobilizefraction,seedcaneremobilizefraction=seedcaneremobilizefraction,option=option,Tfrost.0percent=Tfrost.0percent,Tfrost.100percent=Tfrost.100percent,minCanopyN=minCanopyN)

}

canenitroParms <- function(iLeafN=2, kLN=0.5, Vmax.b1=0, Vmax.b0=0,alpha.b1=0,alpha.b0=0,
                       kpLN=0.2,lnb0 = -5, lnb1 = 18,lnFun=c("none","linear","cyclic"),maxln=90,minln=50,daymaxln=80){

  lnFun <- match.arg(lnFun)
  if(lnFun == "none"){
    lnFun <- 0
  }else
  if(lnFun=="linear"){
    lnFun<-1
    } else{
    lnFun <- 2
  }
  
  list(iLeafN=iLeafN, kLN=abs(kLN), Vmax.b1=Vmax.b1, Vmax.b0=Vmax.b0,alpha.b1=alpha.b1,alpha.b0=alpha.b0, kpLN=kpLN,
        lnb0=lnb0,lnb1=lnb1,lnFun=lnFun,maxln=maxln,minln=minln,daymaxln=daymaxln)

}
