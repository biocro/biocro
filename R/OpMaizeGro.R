##  BioCro/R/OpBioGro.R by Fernando Ezequiel Miguez  Copyright (C) 2012
##' Optimization of dry biomass partitioning coefficients.
##'
##' Optimizes dry biomass partitioning coefficients using constrained
##' optimization (see below).
##'
##'
##' The optimization is done over the \code{\link{MaizeGro}} function. The
##' \code{OpMaizeGro} function is a wrapper for \code{optim} and the
##' \code{constrOpBioGro} is a wrapper for \code{constrOptim}.
##'
##' @aliases OpMaizeGro
##' @param phen integer taking values 1 through 6 which indicate the
##' phenological stage being optimized. If all of the phenological stages need
##' to be optimized then use zero (0).
##' @param iCoef initial vector of size 24 for the dry biomass partitioning
##' coefficients.
##' @param cTT
##' @param WetDat Weather data.
##' @param data observed data.
##' @param plant.day
##' @param emerge.day
##' @param harvest.day
##' @param plant.density
##' @param canopyControl see \code{\link{MaizeGro}}
##' @param seneControl see \code{\link{MaizeGro}}
##' @param photoControl see \code{\link{MaizeGro}}
##' @param phenoControl see \code{\link{MaizeGro}}
##' @param soilControl see \code{\link{MaizeGro}}
##' @param nitroControl see \code{\link{MaizeGro}}
##' @param centuryControl see \code{\link{MaizeGro}}
##' @param op.method Optimization method. Whether to use optim or nlminb
##' @param verbose Displays additional information, originally used for
##' debugging.
##' @param \dots additional arguments passed to \code{\link{optim}} or
##' \code{\link{constrOptim}}.
##' @export OpMaizeGro
OpMaizeGro <- function(phen=1,iCoef=NULL,
                       cTT,
                       WetDat,
                       data,
                       plant.day = NULL, emerge.day = NULL, harvest.day = NULL,
                       plant.density = 7,
                       timestep=1,lat=40,
                       canopyControl = list(),
                       MaizeSeneControl = list(),
                       photoControl = list(),
                       MaizePhenoControl = list(),
                       MaizeCAllocControl = list(),
                       laiControl = list(),
                       soilControl = list(),
                       MaizeNitroControl = list(),
                       centuryControl = list(),
                       op.method=c("optim","nlminb"),
                       verbose=FALSE,...){

  op.method <- match.arg(op.method)

  if(op.method != "optim") stop("only optim is implemented at the moment")
  
  ## Sanity check for weather data
  if(any(is.na(WetDat))) stop("No missing data allowed")
  nrowdata <- nrow(WetDat)
  if(nrowdata < 8760){
    warning("The weather data does not have \n hourly data for a whole year")
    warning("NA will be introduced and the last day \n with data will be the harvest day")
    harvest.day <- max(WetDat[,2])
    doys <- rep(1:365, each = 24)
    hours <- rep(0:23, 365)
    dummy.wetdat <- data.frame(year = unique(WetDat[,1]), doy = doys, hour = hours,
                               solarR = 0, Temp = 0, RH = 0, WS = 0, precip = 0)
    dummy.wetdat[1:nrowdata,] <- WetDat
    WetDat <- dummy.wetdat
  }
    
  ## How should I guess the first day of growth?
  if(missing(plant.day))
    stop("planting date should be supplied")

  if(missing(emerge.day)) emerge.day <- -1

  ## If harvest day is missing it will be the day of the first frost
  ## in fall. The latest day allowed is 270 or Sept 27 th
  ## Completely arbitrary for now
  if(missing(harvest.day)){
    half <- as.integer(dim(WetDat)[1]/2)
    WetDat1 <- WetDat[half:dim(WetDat)[1],c(2,5)]
    if(min(WetDat1[,2]) > 0){
      harvest.day <- 270
    }else{
      WetDat1s <- WetDat1[which(WetDat1[,2]<0),]
      harvest.day <- min(WetDat1s[,1])
      if(harvest.day > 270) harvest.day <- 270
    }
  }
    
  if((plant.day<0) || (plant.day>365) || (harvest.day<0) || (harvest.day>365))
    stop("plant.day and harvest.day should be between 0 and 365")

  if(plant.day > harvest.day)
    stop("plant.day should be smaller than harvest.day")

  if( (timestep<1) || (24%%timestep != 0))
    stop("timestep should be a divisor of 24 (e.g. 1,2,3,4,6,etc.)")

  
##  Temp <- WetDat[emerge.day:harvest.day,5]
##  cTT <- cumsum(Temp/24)
  indTmp <- BioCro:::indfun(data[,1],cTT)

  doyTemp <- WetDat[emerge.day:harvest.day,2]
  doyTemp <- cbind(WetDat[,2],cTT) ## This is a matrix
  indDoy <- doyTemp[indTmp,]
  
  ## hour <- WetDat[,3]
  ## solar <- WetDat[,4]
  ## rh <- WetDat[,6]
  ## WindS <- WetDat[,6]

  ## Getting the Parameters
  ## Canopy parameters will stay the same
  canopyP <- canopyParms()
  canopyP[names(canopyControl)] <- canopyControl

  ## Soil and nitro parameters will stay the same
  soilP <- soilParms()
  soilP[names(soilControl)] <- soilControl

  nitroP <- nitroParms()
  nitroP[names(MaizeNitroControl)] <- MaizeNitroControl

  ## Need a specialized phenology control
  maizephenoP <- MaizePhenoParms()
  maizephenoP[names(MaizePhenoControl)] <- MaizePhenoControl
    
  ## Need special parameters for photosynthesis
  maizephotoP <- MaizePhotoParms()
  maizephotoP[names(photoControl)] <- photoControl

  ## sene P
  seneP <- MaizeSeneParms()
  seneP[names(MaizeSeneControl)] <- MaizeSeneControl

  ## Century
  centuryP <- centuryParms()
  centuryP[names(centuryControl)] <- centuryControl
  
  ## Need special parameters for photosynthesis
  laiP <- laiParms()
  laiP[names(laiControl)] <- laiControl

  ## C allocation is distinct from phenology
  mCallocP <- MaizeCAllocParms()
  mCallocP[names(MaizeCAllocControl)] <- MaizeCAllocControl

  if(missing(iCoef)){  
    iCoef <- as.vector(unlist(mCallocP))
  }

  ## This needs work because pheno stages are different in maize
  V6 <- 6 * maizephenoP$phyllochron1
  V12 <- 10 * maizephenoP$phyllochron1 + 2 * maizephenoP$phyllochron2
  R1 <- maizephenoP$R1
  R6 <- maizephenoP$R6
  ThermalP <- c(V6, V12, R1, R6) + 1 

  if(phen == 0) pheno <- TRUE
  else   pheno <- FALSE

  if(phen == 0) convs <- numeric(6)
  
  if((phen == 1) || pheno){

    iCoef <- c(BioCro:::alr(iCoef[1:3]),log(abs(iCoef[3:13])))

    dayn0 <- indDoy[indDoy[,2] <= ThermalP[1], , drop=FALSE]
    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    if(op.method == "optim"){
      opar <- optim(iCoef[1:2],BioCro:::objFunM, 
                    phenStage=1,
                    iCoefs=iCoef,
                    ThermalP=ThermalP,
                    data=data,
                    WetDat=WetDat,
                    plant.day=plant.day,
                    emerge.day=emerge.day,
                    harvest.day=dayn0,
                    plant.density=plant.density,
                    timestep=timestep,
                    lat=lat,
                    canopyP=canopyP,seneP=seneP,maizephotoP=maizephotoP,
                    mCallocP=mCallocP,
                    maizephenoP=maizephenoP,laiP=laiP,soilP=soilP,nitroP=nitroP,
                    verbose=verbose,...)
    }else{
      ## Not implemented yet
      ## opar <- nlminb(iCoef[c(1,2,4)],BioCro:::objFun, 
      ##                phenStage=1,
      ##                iCoefs=iCoef, ThermalP=ThermalP,
      ##                WetDat=WetDat,day1=day1,
      ##                dayn=dayn0,lat=lat,
      ##                irtl=irtl,iRhizome=iRhizome,timestep=timestep,
      ##                canopyP=canopyP,seneP=seneP,photoP=photoP,
      ##                phenoP=phenoP,soilP=soilP,nitroP=nitroP,
      ##                data=data,verbose=verbose,...)
      ## opar$value <- opar$objective
    }

    iCoef[1:3] <- BioCro:::ialr(opar$par[1:2])
    iCoef[4:13] <- exp(iCoef[4:13])

    if(pheno){
      convs[1] <- opar$convergence
      cat(" Stage 1. Converged: ",ifelse(convs[1] == 0, "YES... \n","NO... \n"))
    }
  }
  
  if((phen == 2) || pheno){

    iCoef <- c(log(abs(iCoef[1:3])),BioCro:::alr(iCoef[4:6]),log(abs(iCoef[6:13])))

    dayn0 <- indDoy[indDoy[,2] <= ThermalP[2],]
    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    if(op.method == "optim"){
      opar <- optim(iCoef[c(4,5,6)],BioCro:::objFunM, 
                    phenStage=2,
                    iCoefs=iCoef, 
                    ThermalP=ThermalP,
                    data=data,
                    WetDat=WetDat,
                    plant.day=plant.day,
                    emerge.day=emerge.day,
                    harvest.day=dayn0,
                    plant.density=plant.density,
                    timestep=timestep,
                    lat=lat,
                    canopyP=canopyP,seneP=seneP,maizephotoP=maizephotoP,
                    mCallocP=mCallocP,
                    maizephenoP=maizephenoP,laiP=laiP,soilP=soilP,nitroP=nitroP,
                    verbose=verbose,...)

    }else{
      ## not implemented yet
      ## opar <- nlminb(iCoef[c(5,6,8)],BioCro:::objFun, 
      ##               phenStage=2,
      ##               iCoefs=iCoef, ThermalP=ThermalP,
      ##               WetDat=WetDat,day1=day1,
      ##               dayn=dayn0,lat=lat,
      ##               irtl=irtl,iRhizome=iRhizome,timestep=timestep,
      ##               canopyP=canopyP,seneP=seneP,photoP=photoP,
      ##               phenoP=phenoP,soilP=soilP,nitroP=nitroP,
      ##               data=data,verbose=verbose,...)
      ## opar$value <- opar$objective
    }
 

    iCoef[1:3] <- exp(iCoef[1:3])
    iCoef[4:6] <- BioCro:::ialr(opar$par[1:2])
    iCoef[7:13] <- exp(iCoef[7:13])

    if(pheno){
      convs[2] <- opar$convergence
      cat(" Stage 2. Converged: ",ifelse(convs[2] == 0, "YES... \n","NO... \n"))
    }
  }
  
  if((phen == 3) || pheno){

    iCoef <- c(log(abs(iCoef[1:6])),BioCro:::alr(iCoef[7:9]),log(abs(iCoef[9:13])))
    
    dayn0 <- indDoy[indDoy[,2] <= ThermalP[3],]
    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    if(op.method == "optim"){
      opar <- optim(iCoef[c(7,8,9)],BioCro:::objFunM, 
                    phenStage=3,
                    iCoefs=iCoef, 
                    ThermalP=ThermalP,
                    data=data,
                    WetDat=WetDat,
                    plant.day=plant.day,
                    emerge.day=emerge.day,
                    harvest.day=dayn0,
                    plant.density=plant.density,
                    timestep=timestep,
                    lat=lat,
                    canopyP=canopyP,seneP=seneP,maizephotoP=maizephotoP,
                    mCallocP=mCallocP,
                    maizephenoP=maizephenoP,laiP=laiP,soilP=soilP,nitroP=nitroP,
                    verbose=verbose,...)
    }else{
      ## not implemented yet
      ## opar <- nlminb(iCoef[9:11],BioCro:::objFun,
      ##                phenStage=3,
      ##                iCoefs=iCoef,WetDat=WetDat,day1=day1,
      ##                dayn=dayn0,lat=lat, ThermalP = ThermalP,
      ##                irtl=irtl,iRhizome=iRhizome,timestep=timestep,
      ##                canopyP=canopyP,seneP=seneP,photoP=photoP,
      ##                phenoP=phenoP,soilP=soilP,nitroP=nitroP,
      ##                data=data,verbose=verbose,...)
      ## opar$value <- opar$objective
    }

    iCoef[1:6] <- exp(iCoef[1:6])
    iCoef[7:9] <- BioCro:::ialr(opar$par[1:2])
    iCoef[10:13] <- exp(iCoef[10:13])

    if(pheno){
      convs[3] <- opar$convergence
      cat(" Stage 3. Converged: ",ifelse(convs[3] == 0, "YES... \n","NO... \n"))
    }    
  }
  
  if((phen == 4) || pheno){

    iCoef <- c(log(abs(iCoef[1:9])),BioCro:::alr(iCoef[10:13]))
    
    dayn0 <- indDoy[indDoy[,2] <= ThermalP[4],]
    dayn0 <- dayn0[NROW(dayn0),1] + 1

    if(op.method == "optim"){
      opar <- optim(iCoef[c(7,8,9)],BioCro:::objFunM, 
                    phenStage=4,
                    iCoefs=iCoef, 
                    ThermalP=ThermalP,
                    data=data,
                    WetDat=WetDat,
                    plant.day=plant.day,
                    emerge.day=emerge.day,
                    harvest.day=dayn0,
                    plant.density=plant.density,
                    timestep=timestep,
                    lat=lat,
                    canopyP=canopyP,seneP=seneP,maizephotoP=maizephotoP,
                    mCallocP=mCallocP,
                    maizephenoP=maizephenoP,laiP=laiP,soilP=soilP,nitroP=nitroP,
                    verbose=verbose,...)
    }else{
      opar <- nlminb(iCoef[13:15],BioCro:::objFun,
                    phenStage=4,
                    iCoefs=iCoef,WetDat=WetDat,day1=day1,
                    dayn=dayn0,lat=lat,ThermalP = ThermalP,
                    irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                    canopyP=canopyP,seneP=seneP,photoP=photoP,
                    phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                    data=data,verbose=verbose,...)
      opar$value <- opar$objective
    }

    iCoef[1:9] <- exp(iCoef[1:9])
    iCoef[10:13] <- BioCro:::ialr(opar$par[1:3])

    if(pheno){
      convs[4] <- opar$convergence
      cat(" Stage 4. Converged: ",ifelse(convs[4] == 0, "YES... \n","NO... \n"))
    }    
  }
  ## if((phen == 5) || pheno){

  ##   iCoef <- c(log(abs(iCoef[1:16])),BioCro:::alr(iCoef[17:20]),log(abs(iCoef[20:25])))
    
  ##   dayn0 <- indDoy[indDoy[,2] <= ThermalP[5],]
  ##   dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

  ##   if(op.method == "optim"){
  ##     opar <- optim(iCoef[17:19],BioCro:::objFun,
  ##                   phenStage=5,
  ##                   iCoefs=iCoef,WetDat=WetDat,day1=day1,
  ##                   dayn=dayn0,lat=lat, ThermalP = ThermalP,
  ##                   irtl=irtl,iRhizome=iRhizome,timestep=timestep,
  ##                   canopyP=canopyP,seneP=seneP,photoP=photoP,
  ##                   phenoP=phenoP,soilP=soilP,nitroP=nitroP,
  ##                   data=data,verbose=verbose,...)
  ##   }else{
  ##     opar <- nlminb(iCoef[17:19],BioCro:::objFun,
  ##                    phenStage=5,
  ##                    iCoefs=iCoef,WetDat=WetDat,day1=day1,
  ##                    dayn=dayn0,lat=lat, ThermalP = ThermalP,
  ##                    irtl=irtl,iRhizome=iRhizome,timestep=timestep,
  ##                    canopyP=canopyP,seneP=seneP,photoP=photoP,
  ##                    phenoP=phenoP,soilP=soilP,nitroP=nitroP,
  ##                    data=data,verbose=verbose,...)
  ##     opar$value <- opar$objective
  ##   }

  ##   iCoef[c(1:3,5:7,9:16)] <- exp(iCoef[c(1:3,5:7,9:16)])
  ##   iCoef[c(4,8)] <- -exp(iCoef[c(4,8)])
  ##   iCoef[17:20] <- BioCro:::ialr(opar$par[1:3])
  ##   iCoef[21:25] <- exp(iCoef[21:25])

  ##   if(pheno){
  ##     convs[5] <- opar$convergence
  ##     cat(" Stage 5. Converged: ",ifelse(convs[5] == 0, "YES... \n","NO... \n"))
  ##   }    
  ## }

  if(pheno){
    phen <- 1:6
    opar$convergence <- convs
  }
  
  list1 <- list(TTTc=cTT,WetDat=WetDat,lat=lat,plant.day=plant.day,timestep=timestep,
                dayn=dayn0,indTmp=indTmp,canopyP=canopyP,
                seneP=seneP,photoP=maizephotoP,phenoP=maizephenoP,soilP=soilP,
                nitroP=nitroP, centuryP=centuryP)
structure(list(coefs=iCoef,data=data,opar=opar,phen=phen,list1=list1),class="OpMaizeGro")
}


objFunM <- function(coefficients,phenStage,iCoefs,ThermalP,data,
                    WetDat,
                    plant.day, emerge.day, harvest.day,
                    plant.density,timestep,lat,
                    canopyP,seneP,maizephotoP,maizephenoP,
                    mCallocP, laiP, soilP, nitroP,verbose,...){

  Coefs <- numeric(13)
  if(phenStage == 1){
    Coefs[1:3] <- BioCro:::ialr(coefficients[1:2])
    Coefs[4:13] <- exp(iCoefs[4:13])
    dat <- data[data[,1] <= ThermalP[1],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 2){
    Coefs[1:3] <- exp(coefficients[1:3])
    Coefs[4:6] <- BioCro:::ialr(coefficients[1:2])
    Coefs[7:13] <- exp(coefficients[7:13])
    dat <- data[data[,1] <= ThermalP[2],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 3){
    Coefs[1:6] <- exp(coefficients[1:6])
    Coefs[7:9] <- BioCro:::ialr(coefficients[1:2])
    Coefs[10:13] <- exp(coefficients[10:13])
    dat <- data[data[,1] <= ThermalP[3],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 4){
    Coefs[1:9] <- exp(coefficients[1:9])
    Coefs[10:13] <- BioCro:::ialr(coefficients[1:3])
    dat <- data[data[,1] <= ThermalP[4],]
    nrdat <- nrow(dat)
  }

  mCallocP[1:13] <- Coefs

  res <- MaizeGro(WetDat,
                  plant.day = plant.day, emerge.day = emerge.day,
                  harvest.day=harvest.day, plant.density = plant.density,
                  timestep,lat,
                  canopyControl=canopyP,
                  MaizeSeneControl=seneP,
                  photoControl=maizephotoP,
                  MaizePhenoControl=maizephenoP,
                  MaizeCAllocControl = mCallocP,
                  laiControl = laiP,
                  soilControl=soilP,
                  MaizeNitroControl=nitroP)

  rss <- RssMaizeGro(dat,res)
  if(verbose){
    cat("rss : ",rss,"\n")
    print(Coefs)
  }
  rss
}



## This is the S3 method for printing

print.OpMaizeGro <- function(x,...){

  cat("\n Optimization for stage:",x$phen,"\n")
  cat("\n Optimized coefficients\n")
  cfs <- numeric(13)
  cfs[c(1:4,6:9,11:14,16:19,21:24,26:30)] <- x$coefs
  coefMat <- matrix(cfs,ncol=5,nrow=4,byrow=TRUE)
  colnames(coefMat) <- c("Leaf","Stem","Root","Rhizome","Grain")
  rownames(coefMat) <- c("1","2","3","4")
  print(coefMat,...)

  cat("\n Residual Sum of Squares:",x$opar$value,"\n")
  
  cat("\n Convergence \n")
  if(length(x$phen) == 6){
    for(i in 1:6){
      if(x$opar$convergence[i] == 0) cat("  stage: ",i,"YES \n")
      else cat("  stage: ",i,"NO \n")
    }
  }else{
    if(x$opar$convergence == 0) cat("  YES \n")
    else cat("  NO \n")
  }
}


## TO do

## summary.OpBioGro <- function(object,...){

##   cfs <- object$coefs

##   phenoP <- object$list1$phenoP
##   phenoP[7:25] <- cfs[7:25-6]
                         
##   res <- BioGro(WetDat=object$list1$WetDat,
##                 day1=object$list1$day1,dayn=object$list1$dayn,
##                 lat=object$list1$lat,timestep=object$list1$timestep,
##                 iRhizome=object$list1$iRhizome, irtl=object$list1$irtl,
##                 canopyControl=object$list1$canopyP,
##                 seneControl= object$list1$seneP,
##                 photoControl= object$list1$photoP,
##                 phenoControl= phenoP,
##                 soilControl= object$list1$soilP,
##                 nitroControl= object$list1$nitroP,
##                 centuryControl= object$list1$centuryP)

##   dat <- object$data
## #print(dat)
##  # print(res)
##   rss <- RssBioGro(dat,res)

##   cat("Residual Sum of Squares: ",rss,"\n")

## }
