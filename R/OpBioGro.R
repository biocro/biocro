##  BioCro/R/OpBioGro.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2008


## This will be hopefully a more clever way of approaching the
## Optimization of Biomass Growth
## The design idea is to optimize one phenological stage at a time
## It will be mostly written in R as I don't see big problems and most
## of the simulation time goes in running the BioGro function which
## is almost completely written in C
##' Optimization of dry biomass partitioning coefficients.
##'
##' Optimizes dry biomass partitioning coefficients using constrained
##' optimization (see below).
##'
##'
##' The optimization is done over the \code{\link{BioGro}} function. The
##' \code{OpBioGro} function is a wrapper for \code{optim} and the
##' \code{constrOpBioGro} is a wrapper for \code{constrOptim}.
##'
##' @aliases OpBioGro constrOpBioGro summary.OpBioGro
##' @param phen integer taking values 1 through 6 which indicate the
##' phenological stage being optimized. If all of the phenological stages need
##' to be optimized then use zero (0).
##' @param iCoef initial vector of size 24 for the dry biomass partitioning
##' coefficients.
##' @param WetDat Weather data.
##' @param data observed data.
##' @param day1 first day of the growing season.
##' @param dayn last day of the growing season.
##' @param timestep see \code{\link{BioGro}}
##' @param lat see \code{\link{BioGro}}
##' @param iRhizome see \code{\link{BioGro}}
##' @param irtl see \code{\link{BioGro}}
##' @param canopyControl see \code{\link{BioGro}}
##' @param seneControl see \code{\link{BioGro}}
##' @param photoControl see \code{\link{BioGro}}
##' @param phenoControl see \code{\link{BioGro}}
##' @param soilControl see \code{\link{BioGro}}
##' @param nitroControl see \code{\link{BioGro}}
##' @param centuryControl see \code{\link{BioGro}}
##' @param op.method Optimization method. Whether to use optim or nlminb
##' @param verbose Displays additional information, originally used for
##' debugging.
##' @param \dots additional arguments passed to \code{\link{optim}} or
##' \code{\link{constrOptim}}.
##' @export
##' @return
##'
##' \code{\link{list}} of class \code{OpBioGro} with components
##' \itemize{
##' \item coefs Optimized coefficients.
##' \item data It passes the data for subsequent plotting and printing.
##' \item opar Results from the optimization function.
##' \item phen Indicates the phenological stage being optimized.
##' \item list1 a \code{\link{list}} with several components.
##' }
##' @section Warning:
##'
##' This function has not had enough testing.
##'
##' @seealso \code{\link{BioGro}} \code{\link{constrOptim}} \code{\link{optim}}
##' \code{\link{c4photo}}
##' @references no references yet.
##' @keywords optimize
##' @examples
##'
##'
##' \dontrun{
##'
##' data(weather05)
##'
##' ## Some coefficients
##' pheno.ll <- phenoParms(kLeaf1=0.48,kStem1=0.47,kRoot1=0.05,kRhizome1=-1e-4,
##'                        kLeaf2=0.14,kStem2=0.65,kRoot2=0.21, kRhizome2=-1e-4,
##'                        kLeaf3=0.01, kStem3=0.56, kRoot3=0.13, kRhizome3=0.3,
##'                        kLeaf4=0.01, kStem4=0.56, kRoot4=0.13, kRhizome4=0.3,
##'                        kLeaf5=0.01, kStem5=0.56, kRoot5=0.13, kRhizome5=0.3,
##'                        kLeaf6=0.01, kStem6=0.56, kRoot6=0.13, kRhizome6=0.3)
##'
##' system.time(ans <- BioGro(weather05, phenoControl = pheno.ll))
##'
##' ans.dat <- as.data.frame(unclass(ans)[1:11])
##' sel.rows <- seq(1,nrow(ans.dat),length.out=8)
##' simDat <- ans.dat[sel.rows,c('ThermalT','Stem','Leaf','Root','Rhizome','Grain','LAI')]
##' plot(ans,simDat)
##'
##' ## Residual sum of squares before the optimization
##'
##' ans0 <- BioGro(weather05)
##' RssBioGro(simDat,ans0)
##'
##' ## This will optimize only the first phenological stage
##' idb <- valid_dbp(idbp(simDat))
##' op1 <- OpBioGro(phen=0, WetDat=weather05, data = simDat, iCoef=idb)
##' ## or
##' cop1 <- constrOpBioGro(phen=0, WetDat=weather05, data = simDat)
##'
##' }
##'
##'

OpBioGro <- function(phen=1,iCoef=NULL,WetDat,
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
                     centuryControl = list(),
                     op.method=c("optim","nlminb"),
                     verbose=FALSE,...){

  ## This seems like repeated code from BioGro but it is needed
  ## in order to automatically pick the right elements from the data
  op.method <- match.arg(op.method)
  
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
  doyTemp <- cbind(doy,cTT) ## This is a matrix
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
  }

  ThermalP <- as.vector(unlist(phenoP)[1:6]) + 1

  if(phen == 0) pheno <- TRUE
  else   pheno <- FALSE

  if(phen == 0) convs <- numeric(6)
  
  if((phen == 1) || pheno){

    iCoef <- c(BioCro:::alr(iCoef[1:3]),log(abs(iCoef[3:25])))

    dayn0 <- indDoy[indDoy[,2] <= ThermalP[1], , drop=FALSE]
    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    if(op.method == "optim"){
      opar <- optim(iCoef[c(1,2,4)],BioCro:::objFun, 
                    phenStage=1,
                    iCoefs=iCoef, ThermalP=ThermalP,
                    WetDat=WetDat,day1=day1,
                    dayn=dayn0,lat=lat,
                    irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                    canopyP=canopyP,seneP=seneP,photoP=photoP,
                    phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                    data=data,verbose=verbose,...)
    }else{
      opar <- nlminb(iCoef[c(1,2,4)],BioCro:::objFun, 
                     phenStage=1,
                     iCoefs=iCoef, ThermalP=ThermalP,
                     WetDat=WetDat,day1=day1,
                     dayn=dayn0,lat=lat,
                     irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                     canopyP=canopyP,seneP=seneP,photoP=photoP,
                     phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                     data=data,verbose=verbose,...)
      opar$value <- opar$objective
    }

    iCoef[1:3] <- BioCro:::ialr(opar$par[1:2])
    iCoef[4] <- -exp(opar$par[3])
    iCoef[5:7] <- exp(iCoef[5:7])
    iCoef[8] <- -exp(iCoef[8])
    iCoef[9:25] <- exp(iCoef[9:25])

    if(pheno){
      convs[1] <- opar$convergence
      cat(" Stage 1. Converged: ",ifelse(convs[1] == 0, "YES... \n","NO... \n"))
    }
  }
  if((phen == 2) || pheno){

    iCoef <- c(log(abs(iCoef[1:4])),BioCro:::alr(iCoef[5:7]),log(abs(iCoef[7:25])))

    dayn0 <- indDoy[indDoy[,2] <= ThermalP[2],]
    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    if(op.method == "optim"){
      opar <- optim(iCoef[c(5,6,8)],BioCro:::objFun, 
                    phenStage=2,
                    iCoefs=iCoef, ThermalP=ThermalP,
                    WetDat=WetDat,day1=day1,
                    dayn=dayn0,lat=lat,
                    irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                    canopyP=canopyP,seneP=seneP,photoP=photoP,
                    phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                    data=data,verbose=verbose,...)
    }else{
      opar <- nlminb(iCoef[c(5,6,8)],BioCro:::objFun, 
                    phenStage=2,
                    iCoefs=iCoef, ThermalP=ThermalP,
                    WetDat=WetDat,day1=day1,
                    dayn=dayn0,lat=lat,
                    irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                    canopyP=canopyP,seneP=seneP,photoP=photoP,
                    phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                    data=data,verbose=verbose,...)
      opar$value <- opar$objective
    }
 

    iCoef[1:3] <- exp(iCoef[1:3])
    iCoef[4] <- -exp(iCoef[4])
    iCoef[5:7] <- BioCro:::ialr(opar$par[1:2])
    iCoef[8] <- -exp(opar$par[3])
    iCoef[9:25] <- exp(iCoef[9:25])

    if(pheno){
      convs[2] <- opar$convergence
      cat(" Stage 2. Converged: ",ifelse(convs[2] == 0, "YES... \n","NO... \n"))
    }
  }
  if((phen == 3) || pheno){

    iCoef <- c(log(abs(iCoef[1:8])),BioCro:::alr(iCoef[9:12]),log(abs(iCoef[12:25])))
    
    dayn0 <- indDoy[indDoy[,2] <= ThermalP[3],]
    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    if(op.method == "optim"){
      opar <- optim(iCoef[9:11],BioCro:::objFun,
                    phenStage=3,
                    iCoefs=iCoef,WetDat=WetDat,day1=day1,
                    dayn=dayn0,lat=lat, ThermalP = ThermalP,
                    irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                    canopyP=canopyP,seneP=seneP,photoP=photoP,
                    phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                    data=data,verbose=verbose,...)
    }else{
      opar <- nlminb(iCoef[9:11],BioCro:::objFun,
                     phenStage=3,
                     iCoefs=iCoef,WetDat=WetDat,day1=day1,
                     dayn=dayn0,lat=lat, ThermalP = ThermalP,
                     irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                     canopyP=canopyP,seneP=seneP,photoP=photoP,
                     phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                     data=data,verbose=verbose,...)
      opar$value <- opar$objective
    }

    iCoef[c(1:3,5:7)] <- exp(iCoef[c(1:3,5:7)])
    iCoef[c(4,8)] <- -exp(iCoef[c(4,8)])
    iCoef[9:12] <- BioCro:::ialr(opar$par[1:3])
    iCoef[13:25] <- exp(iCoef[13:25])

    if(pheno){
      convs[3] <- opar$convergence
      cat(" Stage 3. Converged: ",ifelse(convs[3] == 0, "YES... \n","NO... \n"))
    }    
  }
  if((phen == 4) || pheno){

    iCoef <- c(log(abs(iCoef[1:12])),BioCro:::alr(iCoef[13:16]),log(abs(iCoef[16:25])))
    
    dayn0 <- indDoy[indDoy[,2] <= ThermalP[4],]
    dayn0 <- dayn0[NROW(dayn0),1] + 1

    if(op.method == "optim"){
      opar <- optim(iCoef[13:15],BioCro:::objFun,
                    phenStage=4,
                    iCoefs=iCoef,WetDat=WetDat,day1=day1,
                    dayn=dayn0,lat=lat,ThermalP = ThermalP,
                    irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                    canopyP=canopyP,seneP=seneP,photoP=photoP,
                    phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                    data=data,verbose=verbose,...)
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

    iCoef[c(1:3,5:7,9:12)] <- exp(iCoef[c(1:3,5:7,9:12)])
    iCoef[c(4,8)] <- -exp(iCoef[c(4,8)])
    iCoef[13:16] <- BioCro:::ialr(opar$par[1:3])
    iCoef[17:25] <- exp(iCoef[17:25])

    if(pheno){
      convs[4] <- opar$convergence
      cat(" Stage 4. Converged: ",ifelse(convs[4] == 0, "YES... \n","NO... \n"))
    }    
  }
  if((phen == 5) || pheno){

    iCoef <- c(log(abs(iCoef[1:16])),BioCro:::alr(iCoef[17:20]),log(abs(iCoef[20:25])))
    
    dayn0 <- indDoy[indDoy[,2] <= ThermalP[5],]
    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    if(op.method == "optim"){
      opar <- optim(iCoef[17:19],BioCro:::objFun,
                    phenStage=5,
                    iCoefs=iCoef,WetDat=WetDat,day1=day1,
                    dayn=dayn0,lat=lat, ThermalP = ThermalP,
                    irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                    canopyP=canopyP,seneP=seneP,photoP=photoP,
                    phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                    data=data,verbose=verbose,...)
    }else{
      opar <- nlminb(iCoef[17:19],BioCro:::objFun,
                     phenStage=5,
                     iCoefs=iCoef,WetDat=WetDat,day1=day1,
                     dayn=dayn0,lat=lat, ThermalP = ThermalP,
                     irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                     canopyP=canopyP,seneP=seneP,photoP=photoP,
                     phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                     data=data,verbose=verbose,...)
      opar$value <- opar$objective
    }

    iCoef[c(1:3,5:7,9:16)] <- exp(iCoef[c(1:3,5:7,9:16)])
    iCoef[c(4,8)] <- -exp(iCoef[c(4,8)])
    iCoef[17:20] <- BioCro:::ialr(opar$par[1:3])
    iCoef[21:25] <- exp(iCoef[21:25])

    if(pheno){
      convs[5] <- opar$convergence
      cat(" Stage 5. Converged: ",ifelse(convs[5] == 0, "YES... \n","NO... \n"))
    }    
  }
  if((phen == 6) | pheno){

    if(iCoef[25] < 1e-40) iCoef[25] <- 1e-6
    
    iCoef <- c(log(abs(iCoef[1:20])),BioCro:::alr(iCoef[21:25]),log(iCoef[25]))

    dayn0 <- indDoy[indDoy[,2] <= ThermalP[6],]
    dayn0 <- as.vector(dayn0[NROW(dayn0),1]) + 1

    if(op.method == "optim"){
      opar <- optim(iCoef[21:24],BioCro:::objFun,
                    phenStage=6,
                    iCoefs=iCoef,WetDat=WetDat,day1=day1,
                    dayn=dayn0,lat=lat, ThermalP = ThermalP,
                    irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                    canopyP=canopyP,seneP=seneP,photoP=photoP,
                    phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                    data=data,verbose=verbose,...)
    }else{
      opar <- nlminb(iCoef[21:24],BioCro:::objFun,
                     phenStage=6,
                     iCoefs=iCoef,WetDat=WetDat,day1=day1,
                     dayn=dayn0,lat=lat, ThermalP = ThermalP,
                     irtl=irtl,iRhizome=iRhizome,timestep=timestep,
                     canopyP=canopyP,seneP=seneP,photoP=photoP,
                     phenoP=phenoP,soilP=soilP,nitroP=nitroP,
                     data=data,verbose=verbose,...)
      opar$value <- opar$objective
    }

    iCoef[c(1:3,5:7,9:20)] <- exp(iCoef[c(1:3,5:7,9:20)])
    iCoef[c(4,8)] <- -exp(iCoef[c(4,8)])
    iCoef[21:25] <- BioCro:::ialr(opar$par[1:4])

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


objFun <- function(coefficients,phenStage,iCoefs,ThermalP,data,
                   WetDat,day1,dayn,timestep,lat,iRhizome,irtl,
                   canopyP,seneP,photoP,phenoP,soilP,nitroP,verbose,...){

  Coefs <- numeric(24)
  if(phenStage == 1){
    Coefs[1:3] <- BioCro:::ialr(coefficients[1:2])
    Coefs[4] <- -exp(coefficients[3])
    Coefs[c(5:7,9:25)] <- exp(iCoefs[c(5:7,9:25)])
    Coefs[8] <- -exp(iCoefs[8])
    dat <- data[data[,1] <= ThermalP[1],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 2){
    Coefs[1:3] <- exp(iCoefs[1:3])
    Coefs[4] <- -exp(iCoefs[4])
    Coefs[5:7] <- BioCro:::ialr(coefficients[1:2])
    Coefs[8] <- -exp(coefficients[3])
    Coefs[9:25] <- exp(iCoefs[9:25])
    dat <- data[data[,1] <= ThermalP[2],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 3){
    Coefs[c(1:3,5:7)] <- exp(iCoefs[c(1:3,5:7)])
    Coefs[c(4,8)] <- -exp(iCoefs[c(4,8)])
    Coefs[9:12] <- BioCro:::ialr(coefficients[1:3])
    Coefs[13:25] <- exp(iCoefs[13:25])
    dat <- data[data[,1] <= ThermalP[3],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 4){
    Coefs[c(1:3,5:7,9:12)] <- exp(iCoefs[c(1:3,5:7,9:12)])
    Coefs[c(4,8)] <- -exp(iCoefs[c(4,8)])
    Coefs[13:16] <- BioCro:::ialr(coefficients[1:3])
    Coefs[17:25] <- exp(iCoefs[17:25])
    dat <- data[data[,1] <= ThermalP[4],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 5){
    Coefs[c(1:3,5:7,9:16)] <- exp(iCoefs[c(1:3,5:7,9:16)])
    Coefs[c(4,8)] <- -exp(iCoefs[c(4,8)])
    Coefs[17:20] <- BioCro:::ialr(coefficients[1:3]);
    Coefs[21:25] <- exp(iCoefs[21:25])
    dat <- data[data[,1] <= ThermalP[5],]
    nrdat <- nrow(dat)
  }else
  if(phenStage == 6){
    Coefs[c(1:3,5:7,9:20)] <- exp(iCoefs[c(1:3,5:7,9:20)])
    Coefs[c(4,8)] <- -exp(iCoefs[c(4,8)])
    Coefs[9:20] <- exp(iCoefs[9:20])
    Coefs[21:25] <- BioCro:::ialr(coefficients[1:4])
    dat <- data[data[,1] <= ThermalP[6],]
    nrdat <- nrow(dat)
  }

  phenoP[7:31] <- Coefs

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
    print(Coefs)
  }
  rss
}

## Additive log ratio transformations from
## Statistical Analysis and Interpretation of Discrete Compositional Data
alr <- function(props){
  n <- length(props)
  ans <- log(props[-n]/props[n])
  ans
}

ialr <- function(lprops){
  tmp <- c(exp(lprops),1)
  ans <- tmp/sum(tmp)
  ans
}

## This is the S3 method for printing

print.OpBioGro <- function(x,...){

  cat("\n Optimization for stage:",x$phen,"\n")
  cat("\n Optimized coefficients\n")
  cfs <- numeric(30)
  cfs[c(1:4,6:9,11:14,16:19,21:24,26:30)] <- x$coefs
  coefMat <- matrix(cfs,ncol=5,nrow=6,byrow=TRUE)
  colnames(coefMat) <- c("Leaf","Stem","Root","Rhizome","Grain")
  rownames(coefMat) <- c("1","2","3","4","5","6")
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



summary.OpBioGro <- function(object,...){

  cfs <- object$coefs

  phenoP <- object$list1$phenoP
  phenoP[7:25] <- cfs[7:25-6]
                         
  res <- BioGro(WetDat=object$list1$WetDat,
                day1=object$list1$day1,dayn=object$list1$dayn,
                lat=object$list1$lat,timestep=object$list1$timestep,
                iRhizome=object$list1$iRhizome, irtl=object$list1$irtl,
                canopyControl=object$list1$canopyP,
                seneControl= object$list1$seneP,
                photoControl= object$list1$photoP,
                phenoControl= phenoP,
                soilControl= object$list1$soilP,
                nitroControl= object$list1$nitroP,
                centuryControl= object$list1$centuryP)

  dat <- object$data
#print(dat)
 # print(res)
  rss <- RssBioGro(dat,res)

  cat("Residual Sum of Squares: ",rss,"\n")

}
