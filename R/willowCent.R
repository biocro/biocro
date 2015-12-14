##' @export
willowCent <- function(WetDat, day1=120, dayn=300,
                      timestep=1,iRhizome=1.0,
                      lat=40,iPlant=1,irtl=1e-4,
                      canopyControl=list(),
                      seneControl=list(),
                      photoControl=list(),
                      willowphenoControl=list(),
                      soilControl=list(),
                      nitroControl=list(),
                      iPlantControl=list(),
                      centuryControl=list())
{
  
  ## Getting the Parameters
  
  iPlant <-iwillowParms()
  iPlant[names(iPlantControl)]<-iPlantControl
  
  canopyP <- willowcanopyParms()
  canopyP[names(canopyControl)] <- canopyControl
  
  soilP <- soilParms()
  soilP[names(soilControl)] <- soilControl
  
  nitroP <- nitroParms()
  nitroP[names(nitroControl)] <- nitroControl
  
  willowphenoP <- willowphenoParms()
  willowphenoP[names(willowphenoControl)] <- willowphenoControl
  willowphenoP <- c(unlist(willowphenoP))
  
  photoP <- willowphotoParms()
  photoP[names(photoControl)] <- photoControl
  
  seneP <- willowseneParms()
  seneP[names(seneControl)] <- seneControl
  
  centuryP <- centuryParms()
  centuryP[names(centuryControl)] <- centuryControl
  
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
  
  if(max(rh) > 1) stop("Rel Hum. should be 0 < rh < 1")
  if((min(hr) < 0) | (max(hr) > 23))  stop("hr should be between 0 and 23")
  iPlant<-as.vector(unlist(iPlant))
  
  DBPcoefs <- valid_dbp(as.vector(unlist(willowphenoP)[7:31]))
  
  TPcoefs <- as.vector(unlist(willowphenoP)[1:6])
  
  Tbase<-as.vector(unlist(willowphenoP)[32])
  
  SENcoefs <- as.vector(unlist(seneP))
  
  soilCoefs <- c(unlist(soilP[1:5]),mean(soilP$iWatCont),soilP$scsf, soilP$transpRes, soilP$leafPotTh)
  wsFun <- soilP$wsFun
  soilType <- soilP$soilType
  
  centCoefs <- as.vector(unlist(centuryP)[1:24])
  
  if(centuryP$timestep == "year"){
    stop("Not developed yet")
    centTimestep <- dayn - day1 ## This is really the growing season
  }
  if(centuryP$timestep == "week") centTimestep <- 7
  if(centuryP$timestep == "day") centTimestep <- 1
  
  vmax <- photoP$vmax
  jmax <- photoP$jmax
  jmaxb1<-0.0; ## This needs to be changed
  alpha <- 0
  #alpha <- photoP$alpha
  theta <- photoP$theta
  #beta <- photoP$beta
  Rd <- photoP$Rd
  Catm <- photoP$Catm
  b0 <- photoP$b0
  b1 <- photoP$b1
  ws <- photoP$ws
  StomWS <-photoP$StomWS
  beta <- 0
  #ws <- photoP$ws
  o2 <- photoP$O2
  #upperT<-photoP$UPPERTEMP
  #lowerT<-photoP$LOWERTEMP
  upperT<-0
  lowerT<- 0
  mResp <- canopyP$mResp
  kd <- canopyP$kd
  chi.l <- 1.0
  Sp <- canopyP$Sp
  SpD <- canopyP$SpD
  heightF <- canopyP$heightFactor
  nlayers <- canopyP$nlayers
  GrowthRespFraction <- canopyP$GrowthRespFraction

  res <- .Call("willowCent",
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
               as.double(iPlant),
               as.double(irtl),
               as.double(SENcoefs),
               as.integer(timestep),
               as.integer(vecsize),
               as.double(Sp),
               as.double(SpD),
               as.double(DBPcoefs),
               as.double(TPcoefs),
               as.double(Tbase),
               as.double(vmax),
               as.double(alpha),
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
               as.double(upperT),
               as.double(lowerT),
               as.double(jmax),
               as.double(jmaxb1),
               as.double(o2),
               as.double(GrowthRespFraction),
               as.double(StomWS)
  )
  
  res$cwsMat <- t(res$cwsMat)
  colnames(res$cwsMat) <- soilP$soilDepths[-1]
  res$rdMat <- t(res$rdMat)
  colnames(res$rdMat) <- soilP$soilDepths[-1]
  res$psimMat <- t(res$psimMat)
  colnames(res$psimMat) <- soilP$soilDepths[-1]
  res <- structure(res, class="BioGro")
  
}
