##  BioCro/R/constrOpBioGro.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2009


## This will be hopefully a more clever way of approaching the
## Optimization of Biomass Growth
## The design idea is to optimize one phenological stage at a time
## It will be mostly written in R as I don't see big problems and most
## of the simulation time goes in running the BioGro function which
## is almost completely written in C


constrOpBioGro <- function(phen=1,iCoef=NULL,WetDat,
                     data,
                     day1=NULL, dayn=NULL,
                     timestep=1,lat=40,iRhizome=7,
                     irtl=1e-4,
                     canopyControl = list(),
                     seneControl = list(),
                     photoControl = list(),
                     phenoControl = list(),
                     soilControl = list(),
                     nitroControl = list(),
                     centuryControl = list(),verbose=FALSE,...){

  ## This seems like repeated code from BioGro but it is needed
  ## in order to automatically pick the right elements from the data
  
  if(missing(day1)){
    half <- as.integer(dim(WetDat)[1]/2)
    WetDat1 <- WetDat[1:half,c(2,5)]
    WetDat1s <- WetDat1[which(WetDat1[,2]<0),]
    day1 <- max(WetDat1s[,1])
    if(day1 < 90) day1 <- 90
  }
  if(missing(dayn)){
    half <- as.integer(dim(WetDat)[1]/2)
    WetDat1 <- WetDat[half:dim(WetDat)[1],c(2,5)]
    WetDat1s <- WetDat1[which(WetDat1[,2]<0),]
    dayn <- min(WetDat1s[,1])
    if(dayn > 330) dayn <- 330
  }
    
  if((day1<0) || (day1>365) || (dayn<0) || (dayn>365))
    stop("day1 and dayn should be between 0 and 365")

  if(day1 > dayn)
    stop("day1 should be smaller than dayn")

  if( (timestep<1) || (24%%timestep != 0))
      stop("timestep should be a divisor of 24 (e.g. 1,2,3,4,6,etc.)")
  
  vecsize <- (dayn - (day1-1)) * 24 + 1
  indes1 <- (day1-1) * 24
  indesn <- (dayn) * 24
  Temp <- WetDat[indes1:indesn,5]
  cTT <- cumsum(Temp/24)
  indTmp <- BioCro:::indfun(data[,1],cTT)

  doy <- WetDat[indes1:indesn,2]
  doyTemp <- cbind(doy,cTT)
  indDoy <- doyTemp[indTmp,]
  
  hour <- WetDat[indes1:indesn,3]
  solar <- WetDat[indes1:indesn,4]
  rh <- WetDat[indes1:indesn,6]
  WindS <- WetDat[indes1:indesn,6]

  ## Getting the Parameters
  canopyP <- canopyParms()
  canopyP[names(canopyControl)] <- canopyControl
  
  soilP <- soilParms()
  soilP[names(soilControl)] <- soilControl

  nitroP <- nitroParms()
  nitroP[names(nitroControl)] <- nitroControl

  phenoP <- phenoParms()
  phenoP[names(phenoControl)] <- phenoControl

  photoP <- photoParms()
  photoP[names(photoControl)] <- photoControl

  seneP <- seneParms()
  seneP[names(seneControl)] <- seneControl
  
  centuryP <- centuryParms()
  centuryP[names(centuryControl)] <- centuryControl

  if(missing(iCoef)){  
    iCoef <- as.vector(unlist(phenoP[7:31]))
  }else{
    if(length(iCoef) != 25)
      stop("iCoef should be of length 25")
  }

  ThermalP <- as.vector(unlist(phenoP)[1:6]) + 1

  if(phen == 0) pheno <- TRUE
  else   pheno <- FALSE

  if(phen == 0) convs <- numeric(6)
  
  if((phen == 1) || pheno){

    dayn0 <- indDoy[indDoy[,2] <= ThermalP[1],]
    
    if(class(dayn0) == "numeric"){
        dayn0 <- matrix(dayn0,nrow=1,ncol=2)
      }

    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    ui <- matrix(c(-1,-1,0,
                   0,0,-1,
                   1,0,0,
                   0,1,0),ncol=3,byrow=TRUE)
               
    ci <- c(-1,0,0,0)
    
    opar <- constrOptim(iCoef[c(1:2,4)],BioCro:::objFun2, 
                  phenStage=1,grad=NULL,ui=ui,ci=ci,
                  iCoefs=iCoef, ThermalP=ThermalP,
                  WetDat=WetDat,day1=day1,
                  dayn=dayn0,lat=lat,
                  irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                  canopyP=canopyP,seneP=seneP,photoP=photoP,
                  phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                  data=data,verbose=verbose,...)

    iCoef[c(1:2,4)] <- opar$par[1:3]
    iCoef[3] <- 1 - sum(iCoef[1:2])
    iCoef[5:25] <- iCoef[5:25]
 
    if(pheno){
      convs[1] <- opar$convergence
      cat(" Stage 1. Converged: ",ifelse(convs[1] == 0, "YES... \n","NO... \n"))
    }
  }
  if((phen == 2) || pheno){

    dayn0 <- indDoy[indDoy[,2] <= ThermalP[2],]
    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    ui <- matrix(c(-1,-1,0,
                   0,0,-1,
                   1,0,0,
                   0,1,0),ncol=3,byrow=TRUE)
               
    ci <- c(-1,0,0,0)
    
    
    opar <- constrOptim(iCoef[c(5,6,8)],BioCro:::objFun2, 
                        phenStage=2,grad=NULL,ui=ui,ci=ci,
                        iCoefs=iCoef, ThermalP=ThermalP,
                        WetDat=WetDat,day1=day1,
                        dayn=dayn0,lat=lat,
                        irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                        canopyP=canopyP,seneP=seneP,photoP=photoP,
                        phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                        data=data,verbose=verbose,...)
 

    iCoef[1:4] <- iCoef[1:4]
    iCoef[c(5,6,8)] <- opar$par[1:3]
    iCoef[7] <- 1 - sum(opar$par[1:2])
    iCoef[9:25] <- iCoef[9:25]

    if(pheno){
      convs[2] <- opar$convergence
      cat(" Stage 2. Converged: ",ifelse(convs[2] == 0, "YES... \n","NO... \n"))
    }
  }
  if((phen == 3) || pheno){

    dayn0 <- indDoy[indDoy[,2] <= ThermalP[3],]
    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    ui <- matrix(c(-1,-1,-1,                   
                    1,0,0,
                    0,1,0,
                    0,0,1),ncol=3,byrow=TRUE)
               
    ci <- c(-1,0,0,0)
    
    opar <- constrOptim(iCoef[9:11],BioCro:::objFun2,
                        grad=NULL,ui=ui,ci=ci,phenStage=3,
                        iCoefs=iCoef,WetDat=WetDat,day1=day1,
                        dayn=dayn0,lat=lat, ThermalP = ThermalP,
                        irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                        canopyP=canopyP,seneP=seneP,photoP=photoP,
                        phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                        data=data,verbose=verbose,...)

    iCoef[1:8] <- iCoef[1:8]
    iCoef[9:11] <- opar$par[1:3]
    iCoef[12] <- 1 - sum(opar$par[1:3])
    iCoef[13:25] <- iCoef[13:25]

    if(pheno){
      convs[3] <- opar$convergence
      cat(" Stage 3. Converged: ",ifelse(convs[3] == 0, "YES... \n","NO... \n"))
    }    
  }
  if((phen == 4) || pheno){

    dayn0 <- indDoy[indDoy[,2] <= ThermalP[4],]
    dayn0 <- dayn0[NROW(dayn0),1] + 1

    ui <- matrix(c(-1,-1,-1,                   
                    1,0,0,
                    0,1,0,
                    0,0,1),ncol=3,byrow=TRUE)
               
    ci <- c(-1,0,0,0)
    
    opar <- constrOptim(iCoef[13:15],BioCro:::objFun2,
                        grad=NULL,ui=ui,ci=ci,phenStage=4,
                        iCoefs=iCoef,WetDat=WetDat,day1=day1,
                        dayn=dayn0,lat=lat,ThermalP = ThermalP,
                        irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                        canopyP=canopyP,seneP=seneP,photoP=photoP,
                        phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                        data=data,verbose=verbose,...)

    iCoef[1:12] <- iCoef[1:12]
    iCoef[13:15] <- opar$par[1:3]
    iCoef[16] <- 1 - sum(iCoef[13:15])
    iCoef[17:25] <- iCoef[17:25]

    if(pheno){
      convs[4] <- opar$convergence
      cat(" Stage 4. Converged: ",ifelse(convs[4] == 0, "YES... \n","NO... \n"))
    }    
  }
  if((phen == 5) || pheno){

    dayn0 <- indDoy[indDoy[,2] <= ThermalP[5],]
    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    ui <- matrix(c(-1,-1,-1,                   
                    1,0,0,
                    0,1,0,
                    0,0,1),ncol=3,byrow=TRUE)
               
    ci <- c(-1,0,0,0)
    
    opar <- constrOptim(iCoef[17:19],BioCro:::objFun2,
                  grad=NULL,ui=ui,ci=ci,phenStage=5,
                  iCoefs=iCoef,WetDat=WetDat,day1=day1,
                  dayn=dayn0,lat=lat, ThermalP = ThermalP,
                  irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                  canopyP=canopyP,seneP=seneP,photoP=photoP,
                  phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                  data=data,verbose=verbose,...)

    iCoef[1:16] <- iCoef[1:16]
    iCoef[17:19] <- opar$par[1:3]
    iCoef[20] <- 1 - sum(iCoef[17:19])
    iCoef[21:25] <- iCoef[21:25]

    if(pheno){
      convs[5] <- opar$convergence
      cat(" Stage 5. Converged: ",ifelse(convs[5] == 0, "YES... \n","NO... \n"))
    }    
  }
  if((phen == 6) || pheno){

    dayn0 <- indDoy[indDoy[,2] <= ThermalP[6],]
    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    ui <- matrix(c(-1,-1,-1,-1,                   
                    1,0,0,0,
                    0,1,0,0,
                    0,0,1,0,
                    0,0,0,1),ncol=4,byrow=TRUE)
    
    ci <- c(-1 + -1e-7,0,0,0,0)

    opar <- constrOptim(iCoef[21:24],BioCro:::objFun2,
                        grad=NULL,ui=ui,ci=ci,phenStage=6,
                        iCoefs=iCoef,WetDat=WetDat,day1=day1,
                        dayn=dayn0,lat=lat, ThermalP = ThermalP,
                        irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                        canopyP=canopyP,seneP=seneP,photoP=photoP,
                        phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                        data=data,verbose=verbose,...)

    iCoef[1:20] <- iCoef[1:20]
    iCoef[21:24] <- opar$par[1:4]
    iCoef[25] <- 1 - sum(iCoef[21:24])

    if(pheno){
      convs[6] <- opar$convergence
      cat(" Stage 6. Converged: ",ifelse(convs[6] == 0, "YES. \n","NO. \n"))
    }    
  }

  if(pheno){
    phen <- 1:6
    opar$convergence <- convs
  }
  
  list1 <- list(ThermalP=ThermalP,WetDat=WetDat,lat=lat,day1=day1,timestep=timestep,
                dayn=dayn,iRhizome=iRhizome,irtl=irtl,indTmp=indTmp,canopyP=canopyP,
                seneP=seneP,photoP=photoP,phenoP=phenoP,soilP=soilP,
                nitroP=nitroP, centuryP=centuryP)
structure(list(coefs=iCoef,data=data,opar=opar,phen=phen,list1=list1),class="OpBioGro")
}
    


objFun2 <- function(coefficients,phenStage,iCoefs,ThermalP,data,
                   WetDat,day1,dayn,timestep,lat,iRhizome,irtl,
                   canopyP,seneP,photoP,phenoP,soilP,nitroP,verbose,...){

  Coefs <- numeric(25)
  if(phenStage == 1){
    Coefs[c(1,2,4)] <- coefficients[1:3]
    Coefs[3] <- 1 - sum(coefficients[1:2])
    Coefs[5:25] <- iCoefs[5:25]
    dat <- data[data[,1] <= ThermalP[1],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 2){
    Coefs[1:4] <- iCoefs[1:4]
    Coefs[c(5,6,8)] <- coefficients[1:3]
    Coefs[7] <- 1 - sum(coefficients[1:2])
    Coefs[9:25] <- iCoefs[9:25]
    dat <- data[data[,1] <= ThermalP[2],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 3){
    Coefs[1:12] <- iCoefs[1:12]
    Coefs[9:11] <- coefficients[1:3]
    Coefs[12] <- 1 - sum(coefficients[1:3])
    Coefs[13:25] <- iCoefs[13:25]
    dat <- data[data[,1] <= ThermalP[3],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 4){
    Coefs[1:12] <- iCoefs[1:12]
    Coefs[13:15] <- coefficients[1:3]
    Coefs[16] <- 1 - sum(coefficients[1:3])
    Coefs[17:25] <- iCoefs[17:25]
    dat <- data[data[,1] <= ThermalP[4],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 5){
    Coefs[1:16] <- iCoefs[1:16]
    Coefs[17:19] <- coefficients[1:3]
    Coefs[20] <- 1 - sum(coefficients[1:3])
    Coefs[21:25] <- iCoefs[21:25]
    dat <- data[data[,1] <= ThermalP[5],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 6){
    Coefs[1:20] <- iCoefs[1:20]
    Coefs[21:24] <- coefficients[1:4]
    Coefs[25] <- 1 - sum(coefficients[1:4])
    dat <- data[data[,1] <= ThermalP[6],]
    nrdat <- nrow(dat)
  }

    phenoP[7:31] <- Coefs[7:31-6]

  res <- BioGro(WetDat,day1,dayn,timestep,lat,
                iRhizome,irtl,
                canopyControl=canopyP,
                seneControl=seneP,
                photoControl=photoP,
                phenoControl=phenoP,
                soilControl=soilP,
                nitroControl=nitroP)

  rss <- RssBioGro(dat,res)
  if(verbose){
    cat("rss : ",rss,"\n")
    print(matrix(Coefs,ncol=4,byrow=TRUE))
  }
  rss
}


