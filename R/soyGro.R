####################################################################################################################
####################################################################################################################
soyGro <- function(WetDat,day1=180,dayn=300,
                   timestep=1,iRhizome=1.0,
                   lat=40,iPlant=1,irtl=1e-4,
                   canopyControl=list(Sp=3.57,SpD=0,nlayers=10,kd=0.37,chi.l=1,heightFactor=1,
                                      mResp=c(0.02,0.03)),
                   seneControl=list(senLeaf=1200,senStem=2000,senRoot=2000,senRhizome=9999,Tfrosthigh=5,
                                    Tfrostlow=0,leafdeathrate=5),
                   photoControl=list(vmax=100,Rd=0.8,b0=0.008,b1=3,Catm=550),
                   phenoControl=list(Tbase=0),
                   phenoParms=list(Tbase=0,
                                   tp1=0,tp2=100,tp3=500,tp4=1000,tp5=2000,tp6=3000,
                                   kleaf1=0.3,kstem1=0.5,kroot1=0.2,krhizome1=0.0,
                                   kleaf2=0.3,kstem2=0.5,kroot2=0.2,krhizome2=0.0,
                                   kleaf3=0.3,kstem3=0.5,kroot3=0.2,krhizome3=0.0,
                                   kleaf4=0.3,kstem4=0.6,kroot4=0.1,krhizome4=0.0,
                                   kleaf5=0.0,kstem5=0.0,kroot5=0.0,krhizome5=0.0,
                                   kleaf6=0.0,kstem6=0.0,kroot6=0.0,krhizome6=0.0,kgrain6=1.0),
                   soilControl=list(FieldC=0.32,WiltP=0.19,soilDepth=2,wsFun=3,scsf=1,transpRes=5e+06,
                                    leafPotTh=800,hydrDist=0,rfl=0.2,rsec=0.2,rsdf=0.44),                   
                   nitroControl=list(),
                   iPlantControl=list(iRhizome=0,iStem=0.25,iLeaf=0.25,iRoot=0.25,ifrRhizome=0,
                                      ifrStem=0),
                   centuryControl=list()){
  ##################################################################################################################
  # Write weather data into text file
  # browser()
  WeatherFileName <- file.path(tempdir(),"WeatherData.txt")
  
  IntegerVariables <- as.data.frame(lapply(WetDat[,1:3],as.integer))
  FloatVariables <- as.data.frame(lapply(WetDat[,4:8],function(X) sprintf(fmt="%10.5f",X)))
  WetDat <- cbind(IntegerVariables,FloatVariables)
  write.table(WetDat,WeatherFileName,sep="\t",row.names=FALSE,col.names=FALSE,quote=FALSE)
  ##################################################################################################################
  # Write parameter data into file
  WeatherParameters=list(Year=2002,StartDay=day1,EndDay=dayn,DeltaT=timestep,Lattitude=lat)
  CanopyParameters=list(SLAI=canopyControl$Sp,SLAIDecline=canopyControl$SpD,Layers=canopyControl$nlayers,
                        KDiffuse=canopyControl$kd,Chi=canopyControl$chi.l,Clump=1,
                        HeightFactor=canopyControl$heightFactor)
  PhotosynthesisParameters=list(Vcmax25=photoControl$vmax,Jmax25=180,Theta=0.76,
                                Rd25=photoControl$Rd,BallBerryIntercept=photoControl$b0,
                                BallBerrySlope=photoControl$b1,CO2=photoControl$Catm,
                                O2=210,WaterStessFunction=soilControl$wsFun,
                                WaterStressFactor=1)
  InitialBiomassParameters=list(Leaf=iPlantControl$iLeaf,Stem=iPlantControl$iStem,Flower=0,Grain=0,
                                Root=iPlantControl$iRoot,Rhizome=iPlantControl$iRhizome,Nodule=0)
  SenescenceParameters=list(Leaf=seneControl$senLeaf,Stem=seneControl$senStem,Flower=9999,Grain=9999,
                            Root=2000,Rhizome=seneControl$senRhizome,Nodule=9999)
  PhenologyParameters=list(TBase=phenoControl$Tbase,Stages=6,
                           TT1=phenoParms$tp1,Leaf1=phenoParms$kleaf1,Stem1=phenoParms$kstem1,Flower1=0.0,
                           Grain1=0.0,Root1=phenoParms$kroot1,Rhizome1=phenoParms$krhizome1,Nodule1=0,
                           TT2=phenoParms$tp2,Leaf2=phenoParms$kleaf2,Stem2=phenoParms$kstem2,Flower2=0.0,
                           Grain2=0.0,Root2=phenoParms$kroot2,Rhizome2=phenoParms$krhizome2,Nodule2=0,
                           TT3=phenoParms$tp3,Leaf3=phenoParms$kleaf3,Stem3=phenoParms$kstem3,Flower3=0.0,
                           Grain3=0.0,Root3=phenoParms$kroot3,Rhizome3=phenoParms$krhizome3,Nodule3=0,
                           TT4=phenoParms$tp4,Leaf4=phenoParms$kleaf4,Stem4=phenoParms$kstem4,Flower4=0.0,
                           Grain4=0.0,Root4=phenoParms$kroot4,Rhizome4=phenoParms$krhizome4,Nodule4=0,
                           TT5=phenoParms$tp5,Leaf5=phenoParms$kleaf5,Stem5=phenoParms$kstem5,Flower5=0.0,
                           Grain5=1.0,Root5=phenoParms$kroot5,Rhizome5=phenoParms$krhizome5,Nodule5=0,
                           TT6=phenoParms$tp6,Leaf6=phenoParms$kleaf6,Stem6=phenoParms$kstem6,Flower6=0.0,
                           Grain6=phenoParms$kgrain6,Root6=phenoParms$kroot6,Rhizome6=phenoParms$krhizome6,
                           Nodule6=0)
  RespirationParameters=list(GLeaf=0.3,GStem=0.3,GFlower=0.3,GGrain=0.3,GRoot=0.3,GRhizome=0.3,GNodule=0.3,
                             QLeaf=2.0,QStem=2.0,QFlower=2.0,QGrain=2.0,QRoot=2.0,QRhizome=2.0,QNodule=2.0,
                             MLeaf=0.0120,MStem=0.0016,MFlower=0.0000,MGrain=0.0000,MRoot=0.0036,
                             MRhizome=0.0000,MNodule=0.0000)
  ParameterFileName <- file.path(tempdir(),"ParameterData.txt")
  Parameters <- cbind("*",as.data.frame(WeatherParameters[1:3]),
                      as.data.frame(lapply(WeatherParameters[4:5],function(X) sprintf(fmt="%10.5f",X))))
  write.table(Parameters,ParameterFileName, sep="\t",row.names=FALSE,col.names=FALSE,quote=FALSE)
  Parameters <- cbind("*",as.data.frame(lapply(CanopyParameters[1:2],function(X) sprintf(fmt="%10.5f",X))),
                      as.data.frame(CanopyParameters[3]),
                      as.data.frame(lapply(CanopyParameters[4:6],function(X) sprintf(fmt="%10.5f",X))),
                      as.data.frame(CanopyParameters[7]))
  write.table(Parameters,ParameterFileName, sep="\t",row.names=FALSE,col.names=FALSE,quote=FALSE,append=TRUE)
  Parameters <- cbind("*",as.data.frame(lapply(PhotosynthesisParameters[1:8],function(X) sprintf(fmt="%10.5f",X))),
                      as.data.frame(PhotosynthesisParameters[9]),
                      as.data.frame(lapply(PhotosynthesisParameters[10],function(X) sprintf(fmt="%10.5f",X))))
  write.table(Parameters,ParameterFileName, sep="\t",row.names=FALSE,col.names=FALSE,quote=FALSE,append=TRUE)
  Parameters <- cbind("*",as.data.frame(lapply(InitialBiomassParameters[1:7],function(X) sprintf(fmt="%10.5f",X))))
  write.table(Parameters,ParameterFileName, sep="\t",row.names=FALSE,col.names=FALSE,quote=FALSE,append=TRUE)
  Parameters <- cbind("*",as.data.frame(lapply(SenescenceParameters[1:7],function(X) sprintf(fmt="%10.5f",X))))
  write.table(Parameters,ParameterFileName, sep="\t",row.names=FALSE,col.names=FALSE,quote=FALSE,append=TRUE)
  Parameters <- cbind("*",as.data.frame(lapply(PhenologyParameters[1],function(X) sprintf(fmt="%10.5f",X))),
                      as.data.frame(PhenologyParameters[2]))
  write.table(Parameters,ParameterFileName, sep="\t",row.names=FALSE,col.names=FALSE,quote=FALSE,append=TRUE)
  for(Count in 1:PhenologyParameters$Stages){
    Parameters <- cbind("*",as.data.frame(lapply(PhenologyParameters[(3+(Count-1)*8):(2+(Count*8))],function(X)
                        sprintf(fmt="%10.5f",X))))
    write.table(Parameters,ParameterFileName, sep="\t",row.names=FALSE,col.names=FALSE,quote=FALSE,append=TRUE)}
  Parameters <- cbind("*",as.data.frame(lapply(RespirationParameters[1:7],function(X) sprintf(fmt="%10.5f",X))))
  write.table(Parameters,ParameterFileName, sep="\t",row.names=FALSE,col.names=FALSE,quote=FALSE,append=TRUE)
  Parameters <- cbind("*",as.data.frame(lapply(RespirationParameters[8:14],function(X) sprintf(fmt="%10.5f",X))))
  write.table(Parameters,ParameterFileName, sep="\t",row.names=FALSE,col.names=FALSE,quote=FALSE,append=TRUE)
  Parameters <- cbind("*",as.data.frame(lapply(RespirationParameters[15:21],function(X) sprintf(fmt="%10.5f",X))))
  write.table(Parameters,ParameterFileName, sep="\t",row.names=FALSE,col.names=FALSE,quote=FALSE,append=TRUE)
  # Parameters <- list(WeatherParameters=WeatherParameters,CanopyParameters=CanopyParameters,
  #                    PhotosynthesisParameters=PhotosynthesisParameters,
  #                    InitialBiomassParameters=InitialBiomassParameters,SenescenceParameters=SenescenceParameters,
  #                    PhenologyParameters=PhenologyParameters,RespirationParameters=RespirationParameters)
  # XMLParameters <- listToXml(Parameters,'constants')
  # ParameterFileName <- file.path(tempdir(),"ParameterData.xml")
  # print(ParameterFileName)
  # saveXML(doc=XMLParameters,file=ParameterFileName,indent=TRUE,prefix=NULL)
  ##################################################################################################################
  # Specify folder name for output
  OutputFolderName <- file.path(tempdir())
  ##################################################################################################################
  # Call soyGro function
  # print(ParameterFileName)
  # print(WeatherFileName)
  # print(OutputFolderName)
  Void <-.Call("RWrapper",ParameterFileName,WeatherFileName,OutputFolderName)
  ##################################################################################################################
  # Import output data into R
  Sys.sleep(1)
  OutputFileName <- file.path(tempdir(),"soyGroOutput.txt")
  Result <- read.table(OutputFileName)
  colnames(Result) <- c("DayofYear","Hour","ThermalT","LAI","Leaf","Stem","Flower","Grain","Root",
                        "Rhizome","Nodule")
  Result <- structure(Result, class = "BioGro")
  return(Result)}
####################################################################################################################
####################################################################################################################