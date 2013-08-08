## I'm trying to figure out ways of manipulating netCDF data

args <- commandArgs(TRUE)

if(length(args) == 0){
  warning("args is missing")
}else{
  k <- as.numeric(args)
}


year <- as.integer(k)

library(RNetCDF)

uslay <- read.table("USlayer.txt",header=TRUE)

## Loading the soil data
ussoil <- read.table("./soil/ussoil-comp-pp.txt")
names(ussoil) <- c("mukey","cokey","Loc","Lat","Lon",
                   "dist","centX","centY","cont","awc",
                   "mdep","hrb","elev","corn_yield","corn_yield_high","comppct")

setwd("/home/miguez/USsim/data")  
## Loading radiation
##shortR <- open.nc(paste("uUSshortR.",k,".nc",sep="")) ## radiation will come form NLDAS
shortR <- read.csv(paste("/home/miguez/USsim/nldas/nldas-proc-rad-",k,"-pp.csv",sep=""))
## Loading temperature
airT <- open.nc(paste("uUSairT.",k,".nc",sep=""))
## Loading relative humidity
rhum <- open.nc(paste("uUSrhum.",k,".nc",sep=""))
## Loading windspeed
winds <- open.nc(paste("uUSwind.",k,".nc",sep=""))
## Loading precip
precip <- open.nc(paste("uUSprecip.",k,".nc",sep=""))
## vegetation
veg <- open.nc(paste("uUSveg.",k,".nc",sep=""))
## Soil moisture
soilm <- open.nc(paste("uUSsoilm.",k,".nc",sep=""))


## Getting the variables
##Rad <- var.get.nc(shortR,"dswrf")
Temp <- var.get.nc(airT,"air")
RH <- var.get.nc(rhum,"rhum")
windspeed <- var.get.nc(winds,"uwnd")
Precip <- var.get.nc(precip,"apcp")
Veg <- var.get.nc(veg,"veg")              
Soilm <- var.get.nc(soilm,"soilm")              

lat <- var.get.nc(airT,"lat")
lon <- var.get.nc(airT,"lon")
time  <- var.get.nc(airT,"time")

## Creating the hours vectors
timet <- utcal.nc("hours since 1800-01-01 00:00:00",time)
hr <- timet[,4]
day <- rep(1:365,each=8)

setwd("/home/djaiswal/Simulations/USA/willow")

for(i in 1:dim(lat)[1]){

  for(j in 1:dim(lat)[2]){

    lati <- lat[i,j]
    long <- lon[i,j]
     loc<-i*100 + j
 csoil <- ussoil[ussoil$Loc == loc,]
 sRad <- as.vector(shortR[,paste("X",loc,sep="")])

 if(length(sRad) > 366){
      stop("sRad should be length 365 or 366")
    }
    if(length(sRad) < 365) sRad <- c(sRad, rep(10, 365 - length(sRad)))
    if(length(sRad) < 365) stop("sRad should be length 365")

    # Fetching and unit conversion temperature
    # From Kelvin to Celsius
    sTemp <- Temp[i,j,] - 273.15
    # Fetcing and unit conversion relative humidity
    # From (%) to fraction
    sRH <- RH[i,j,] * 1e-2 - 0.001 
    # Fetcing and unit conversion wind speed
    # Just calculating absolute value
    sWindS <- abs(windspeed[i,j,])
    # Fetcing and unit conversion precipitation
    # The units of kg/m2 are approx. equivalent to mm
    sPrecip <- abs(Precip[i,j,])
    # Fetcing and unit conversion precipitation
    # The units of kg/m2 are approx. equivalent to mm
    sVeg <- Veg[i,j,]
    # Soil moisture
    # It represents the soil moisture of the first 2m
    # I can get what I need by dividing by 2000
    sSoilm <- Soilm[i,j,] / 2000
    
    ## Fixing the missing value problem
#    sRad <- ifelse(sRad > 32765 , 100,sRad)
    sTemp <- ifelse(sTemp > 32765 , 5,sTemp)
    sRH <- ifelse(sRH > 32765*1e-2 , 0.7,sRH)
    sWindS <- ifelse(sWindS > 32765 , 2,sWindS)
    sPrecip <- ifelse(sPrecip > 32765 , 0,sPrecip)
    sVeg <- ifelse(sVeg > 32765 , NA,sVeg)
    sSoilm <- ifelse(sSoilm > 30000,NA,sSoilm)

    ## Addressing the leap year problem
    if(length(sTemp) > 2920){
            sRad <- sRad[1:365]
            sTemp <- sTemp[1:2920]
            sRH <- sRH[1:2920]
            sWindS <- sWindS[1:2920]
            sPrecip <- sPrecip[1:2920]
            hr <- hr[1:2920]
          }

    if(any(is.na(sVeg)) || !(loc %in% uslay$Loc)){
      resS <- data.frame(loc=loc,Lat=lati,Lon=long,
                         Stem1=NA, Leaf1=NA,Root1=NA,Rhiz1=NA,
                         Stem2=NA, Leaf2=NA,Root2=NA,Rhiz2=NA,
                         Stem3=NA, Leaf3=NA,Root3=NA,Rhiz3=NA,
                         Stem4=NA, Leaf4=NA,Root4=NA,Rhiz4=NA,
                         Stem5=NA, Leaf5=NA, Root5=NA, Rhiz5=NA,
                         avgStem=NA,
                         avgTemp=NA,TotPrecip=NA,avgRad=NA,awc=NA,
                         mdep=NA,year = NA)
    }
	
    else{

      ## Interpolating
      dat <- data.frame(year=year,doy=day,hour=hr,
                        Temp=sTemp,RH=sRH,WS=sWindS,precip=sPrecip)

      MaxTemp <- aggregate(dat$Temp,by=list(dat$doy),max)$x ## In Celsius
      MinTemp <- aggregate(dat$Temp,by=list(dat$doy),min)$x ## In Celsius
      MeanTemp <- aggregate(dat$Temp,by=list(dat$doy),mean)$x ## In Celsius

      MaxRH <- aggregate(dat$RH,by=list(dat$doy),max)$x  ## Fraction
      MinRH <- aggregate(dat$RH,by=list(dat$doy),min)$x  ## Fraction
      MeanRH <- aggregate(dat$RH,by=list(dat$doy),mean)$x ## Fraction

      MeanWS <- aggregate(dat$WS,by=list(dat$doy),mean)$x ## in m/s

      TotPrecip <- aggregate(dat$precip,by=list(dat$doy),sum)$x ## in mm

##      TotRad <- aggregate(dat$solarR,by=list(dat$doy),sum)$x ## in MJ/m^2 

      dat <- weach(cbind(year,1:365,sRad,MaxTemp,MinTemp,MeanTemp,
                         MaxRH,MinRH,MeanRH,MeanWS,TotPrecip),lati=lati,temp.units="Celsius",
                   rh.units="fraction",ws.units="mps",pp.units="mm")
      
      dat <- as.data.frame(dat)

##     Parameters for simulating Willow in the US
       day1=120
       dayn=330
       canwillow <- willowcanopyParms(Sp=1.1,kd=0.37,GrowthRespFraction=0.4)
       senwillow <- willowseneParms(senLeaf=1000,Tfrosthigh=5,Tfrostlow=0,leafdeathrate=5)
       ## phewillow <- willowphenoParms(tp1=250, tp2=500, tp3=900, tp4=1200, tp5=3939, tp6=7000,
       ##                        kStem1=0.01, kLeaf1=0.98, kRoot1=0.01, kRhizome1=-8e-4, 
       ##                        kStem2=0.01, kLeaf2=0.98, kRoot2=0.005, kRhizome2=0.005, 
       ##                        kStem3=0.5, kLeaf3=0.15, kRoot3=0.175, kRhizome3=0.175, 
       ##                        kStem4=0.7, kLeaf4=0.15, kRoot4=0.075, kRhizome4=0.075, 
       ##                        kStem5=0.7, kLeaf5=0.00001, kRoot5=0.15, kRhizome5=0.15, 
       ##                        kStem6=0.7, kLeaf6=0.000001, kRoot6=0.15, kRhizome6=0.15)
    
      
      ## Growing season 1
      Stem <- numeric(nrow(csoil))
      Leaf <- numeric(nrow(csoil))
      Rhizome <- numeric(nrow(csoil))
      Root <- numeric(nrow(csoil))
      qfinal <- max(1, nrow(csoil))
      for(q in 1:qfinal){
        if(nrow(csoil) == 0){
          fieldc <- 0.15 + 0.17
          wiltp <- 0.15
          mdep <- 1.5
          elev <- NA
          hrb <- NA
          cornyd <- NA
          cornyd_h <- NA
        }else{
          awc <- csoil$awc[q]
          if(is.na(awc)) awc <- 0.174
          if(awc > 0.35) awc <- 0.35
          mdep <- csoil$mdep[q]/100
          if(is.na(mdep)) mdep <- 1.5
          if(mdep > 2.5) mdep <- 2.5
          if(mdep < 1e-2) mdep <- 1.5
          wiltp <- 0.15
          if(awc < 1e-2) awc <- 0.01
          fieldc <- wiltp + awc
        }

        soil.ll <- soilParms(iWatCont=sSoilm[day1],
                               FieldC=fieldc,WiltP=wiltp,
                               soilDepth=mdep, wsFun="linear", phi2=5, soilLayers=1)
       iplant <-iwillowParms(iRhizome=1.0,iStem=1.0,iLeaf=0.0,iRoot=0.0,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
       res1 <- willowGro(dat,day1=day1,dayn=dayn,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,iPlantControl=iplant,soilControl=soil.ll)
       Leaf[q]<-res1$Leaf[length(res1$Leaf)]
       Stem[q]<-res1$Stem[length(res2$Stem)]
       Root[q]<-res1$Root[length(res1$Root)]
       Rhizome[q]<-res1$Rhizome[length(res1$Rhizome)]

      }
      weights <- csoil$comppct/100
      stem1<-sum(Stem*weights,na.rm=TRUE)
      leaf1<-sum(Leaf*weights,na.rm=TRUE)
      root1<-sum(Root*weights,na.rm=TRUE)
      rhiz1<-sum(Rhizome*weights,na.rm=TRUE)
      
      ## Growing season 2
      Stem <- numeric(nrow(csoil))
      Leaf <- numeric(nrow(csoil))
      Rhizome <- numeric(nrow(csoil))
      Root <- numeric(nrow(csoil))
      qfinal <- max(1, nrow(csoil))
      for(q in 1:qfinal){
        if(nrow(csoil) == 0){
          fieldc <- 0.15 + 0.17
          wiltp <- 0.15
          mdep <- 1.5
          elev <- NA
          hrb <- NA
          cornyd <- NA
          cornyd_h <- NA
        }else{
          awc <- csoil$awc[q]
          if(is.na(awc)) awc <- 0.174
          if(awc > 0.35) awc <- 0.35
          mdep <- csoil$mdep[q]/100
          if(is.na(mdep)) mdep <- 1.5
          if(mdep > 2.5) mdep <- 2.5
          if(mdep < 1e-2) mdep <- 1.5
          wiltp <- 0.15
          if(awc < 1e-2) awc <- 0.01
          fieldc <- wiltp + awc
        }

        soil.ll <- soilParms(iWatCont=sSoilm[day1],
                               FieldC=fieldc,WiltP=wiltp,
                               soilDepth=mdep, wsFun="linear", phi2=5, soilLayers=1)
        iStem=stem1*0.05
        iRoot=root1
        iRhizome=rhiz1
        iplant <-iwillowParms(iRhizome=iRhizome,iStem=iStem,iLeaf=0.0,iRoot=iRoot,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
        res2 <- willowGro(dat,day1=day1,dayn=dayn,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,iPlantControl=iplant,soilControl=soil.ll)
        Leaf[q]<-res2$Leaf[length(res2$Leaf)]
        Stem[q]<-res2$Stem[length(res2$Stem)]
        Root[q]<-res2$Root[length(res2$Root)]
        Rhizome[q]<-res2$Rhizome[length(res2$Rhizome)]

      }
      weights <- csoil$comppct/100
      stem2<-sum(Stem*weights,na.rm=TRUE)
      leaf2<-sum(Leaf*weights,na.rm=TRUE)
      root2<-sum(Root*weights,na.rm=TRUE)
      rhiz2<-sum(Rhizome*weights,na.rm=TRUE)

       ## Growing season 3
      Stem <- numeric(nrow(csoil))
      Leaf <- numeric(nrow(csoil))
      Rhizome <- numeric(nrow(csoil))
      Root <- numeric(nrow(csoil))
      qfinal <- max(1, nrow(csoil))
      for(q in 1:qfinal){
        if(nrow(csoil) == 0){
          fieldc <- 0.15 + 0.17
          wiltp <- 0.15
          mdep <- 1.5
          elev <- NA
          hrb <- NA
          cornyd <- NA
          cornyd_h <- NA
        }else{
          awc <- csoil$awc[q]
          if(is.na(awc)) awc <- 0.174
          if(awc > 0.35) awc <- 0.35
          mdep <- csoil$mdep[q]/100
          if(is.na(mdep)) mdep <- 1.5
          if(mdep > 2.5) mdep <- 2.5
          if(mdep < 1e-2) mdep <- 1.5
          wiltp <- 0.15
          if(awc < 1e-2) awc <- 0.01
          fieldc <- wiltp + awc
        }

        soil.ll <- soilParms(iWatCont=sSoilm[day1],
                               FieldC=fieldc,WiltP=wiltp,
                               soilDepth=mdep, wsFun="linear", phi2=5, soilLayers=1)
        iStem=stem2
        iRoot=root2
        iRhizome=rhiz2
        iplant <-iwillowParms(iRhizome=iRhizome,iStem=iStem,iLeaf=0.0,iRoot=iRoot,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
        res3 <- willowGro(dat,day1=day1,dayn=dayn,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,iPlantControl=iplant,soilControl=soil.ll)
        Leaf[q]<-res3$Leaf[length(res3$Leaf)]
        Stem[q]<-res3$Stem[length(res3$Stem)]
        Root[q]<-res3$Root[length(res3$Root)]
        Rhizome[q]<-res3$Rhizome[length(res3$Rhizome)]

      }
      weights <- csoil$comppct/100
      stem3<-sum(Stem*weights,na.rm=TRUE)
      leaf3<-sum(Leaf*weights,na.rm=TRUE)
      root3<-sum(Root*weights,na.rm=TRUE)
      rhiz3<-sum(Rhizome*weights,na.rm=TRUE)

       ## Growing season 4
      Stem <- numeric(nrow(csoil))
      Leaf <- numeric(nrow(csoil))
      Rhizome <- numeric(nrow(csoil))
      Root <- numeric(nrow(csoil))
      qfinal <- max(1, nrow(csoil))
      for(q in 1:qfinal){
        if(nrow(csoil) == 0){
          fieldc <- 0.15 + 0.17
          wiltp <- 0.15
          mdep <- 1.5
          elev <- NA
          hrb <- NA
          cornyd <- NA
          cornyd_h <- NA
        }else{
          awc <- csoil$awc[q]
          if(is.na(awc)) awc <- 0.174
          if(awc > 0.35) awc <- 0.35
          mdep <- csoil$mdep[q]/100
          if(is.na(mdep)) mdep <- 1.5
          if(mdep > 2.5) mdep <- 2.5
          if(mdep < 1e-2) mdep <- 1.5
          wiltp <- 0.15
          if(awc < 1e-2) awc <- 0.01
          fieldc <- wiltp + awc
        }

        soil.ll <- soilParms(iWatCont=sSoilm[day1],
                               FieldC=fieldc,WiltP=wiltp,
                               soilDepth=mdep, wsFun="linear", phi2=5, soilLayers=1)
        iStem=stem3
        iRoot=root3
        iRhizome=rhiz3
        iplant <-iwillowParms(iRhizome=iRhizome,iStem=iStem,iLeaf=0.0,iRoot=iRoot,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
        res4 <- willowGro(dat,day1=day1,dayn=dayn,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,iPlantControl=iplant,soilControl=soil.ll)
        Leaf[q]<-res4$Leaf[length(res4$Leaf)]
        Stem[q]<-res4$Stem[length(res4$Stem)]
        Root[q]<-res4$Root[length(res4$Root)]
        Rhizome[q]<-res4$Rhizome[length(res4$Rhizome)]

      }
      weights <- csoil$comppct/100
      stem4<-sum(Stem*weights,na.rm=TRUE)
      leaf4<-sum(Leaf*weights,na.rm=TRUE)
      root4<-sum(Root*weights,na.rm=TRUE)
      rhiz4<-sum(Rhizome*weights,na.rm=TRUE)

        ## Growing season 5
      Stem <- numeric(nrow(csoil))
      Leaf <- numeric(nrow(csoil))
      Rhizome <- numeric(nrow(csoil))
      Root <- numeric(nrow(csoil))
      qfinal <- max(1, nrow(csoil))
      for(q in 1:qfinal){
        if(nrow(csoil) == 0){
          fieldc <- 0.15 + 0.17
          wiltp <- 0.15
          mdep <- 1.5
          elev <- NA
          hrb <- NA
          cornyd <- NA
          cornyd_h <- NA
        }else{
          awc <- csoil$awc[q]
          if(is.na(awc)) awc <- 0.174
          if(awc > 0.35) awc <- 0.35
          mdep <- csoil$mdep[q]/100
          if(is.na(mdep)) mdep <- 1.5
          if(mdep > 2.5) mdep <- 2.5
          if(mdep < 1e-2) mdep <- 1.5
          wiltp <- 0.15
          if(awc < 1e-2) awc <- 0.01
          fieldc <- wiltp + awc
        }

        soil.ll <- soilParms(iWatCont=sSoilm[day1],
                               FieldC=fieldc,WiltP=wiltp,
                               soilDepth=mdep, wsFun="linear", phi2=5, soilLayers=1)
        iStem=stem4
        iRoot=root4
        iRhizome=rhiz4
        iplant <-iwillowParms(iRhizome=iRhizome,iStem=iStem,iLeaf=0.0,iRoot=iRoot,ifrRhizome=0.01,ifrStem=0.01,ifrLeaf=0.0,ifrRoot=0.0)
        res5 <- willowGro(dat,day1=day1,dayn=dayn,canopyControl=canwillow,willowphenoControl=phewillow,seneControl=senwillow,iPlantControl=iplant,soilControl=soil.ll)
        Leaf[q]<-res5$Leaf[length(res5$Leaf)]
        Stem[q]<-res5$Stem[length(res5$Stem)]
        Root[q]<-res5$Root[length(res5$Root)]
        Rhizome[q]<-res5$Rhizome[length(res5$Rhizome)]
      }
      weights <- csoil$comppct/100
      stem5<-sum(Stem*weights,na.rm=TRUE)
      leaf5<-sum(Leaf*weights,na.rm=TRUE)
      root5<-sum(Root*weights,na.rm=TRUE)
      rhiz5<-sum(Rhizome*weights,na.rm=TRUE)

      avgStem<-(stem5)*0.95*(1/4)  # 0.95 is fraction of stem removed via coppicing and (1/5) is convert to avergae annual yield based on 5 yr cycle
    

      
      resS <- data.frame(loc=loc,Lat=lati,Lon=long,
                         Stem1=stem1, Leaf1=leaf1,Root1=root1,Rhiz1=rhiz1,
                         Stem2=stem2, Leaf2=leaf2,Root2=root2,Rhiz2=rhiz2,
                         Stem3=stem3, Leaf3=leaf3,Root3=root3,Rhiz3=rhiz3,
                         Stem4=stem4, Leaf4=leaf4,Root4=root4,Rhiz4=rhiz4,
                         Stem5=stem5, Leaf5=leaf5, Root5=root5, Rhiz5=rhiz5,
                         avgStem=avgStem,
                         avgTemp=mean(sTemp),TotPrecip=sum(sPrecip),
                         avgRad=mean(sRad, na.rm=TRUE),
                         awc=mean(csoil$awc, na.rm=TRUE),
                         mdep=mean(csoil$mdep/100, na.rm=TRUE),year = year)
      
    }

    write.table(resS,paste("./Results/res",k,".txt",sep=""),append=TRUE,row.names=FALSE,col.names=FALSE)
    
  }
}


close.nc(airT)
close.nc(rhum)
close.nc(winds)
close.nc(precip)
close.nc(soilm)

              

