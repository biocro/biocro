##  BioCro/R/MCMCBioCro.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2010

##' Simulated annealing and MCMC  function
##' 
##' Simulated Annealing and Markov chain Monte Carlo for estimating parameters
##' for Biomass Growth
##'
##' This function atempts to implement the simulated annealing method for
##' estimating parameters of a generic C4 crop growth.
##'
##'
##' This function implements a hybrid algorithm where the first portion is
##' simulated annealing and the second portion is a Markov chain Monte Carlo.
##' The user controls the number of iterations in each portion of the chain
##' with niter and niter2.
##'
##' @aliases MCMCBioGro print.MCMCBioGro
##' @param niter number of iterations for the simulated annealing portion of
##' the optimization.
##' @param niter2 number of iterations for the Markov chain Monte Carlo portion
##' of the optimization.
##' @param phen Phenological stage being optimized.
##' @param iCoef initial coefficients for dry biomass partitioning.
##' @param saTemp simulated annealing temperature.
##' @param coolSamp number of cooling samples.
##' @param scale scale parameter to control the standard deviations.
##' @param WetDat weather data.
##' @param data observed data.
##' @param day1 first day of the growing season.
##' @param dayn last day of the growing season.
##' @param timestep Timestep see \code{\link{BioGro}}.
##' @param lat latitude.
##' @param iRhizome initial rhizome biomass.
##' @param irtl See \code{\link{BioGro}}.
##' @param canopyControl See \code{\link{canopyParms}}.
##' @param seneControl See \code{\link{seneParms}}.
##' @param photoControl See \code{\link{photoParms}}.
##' @param phenoControl See \code{\link{phenoParms}}.
##' @param soilControl See \code{\link{soilParms}}.
##' @param nitroControl See \code{\link{nitroParms}}.
##' @param centuryControl See \code{\link{centuryParms}}.
##' @param sd standard deviations for the parameters to be optimized. The first
##' (0.02) is for the positive dry biomass partitioning coefficients. The
##' second (1e-6) is for the negative dry biomass partitioning coefficients.
##' @author Fernando E. Miguez
##' @export
##' @return
##'
##' An object of class MCMCBioGro consisting of a list with 23 components.  The
##' easiest way of accessing the information is with the print and plot
##' methods.
##' @note The automatic method for guessing the last day of the growing season
##' differs slightly from that in \code{BioGro}. To prevent error due to a
##' shorter simulated growing season than the observed one the method in
##' \code{MCMCBioGro} adds one day to the last day of the growing season.
##' Although the upper limit is fixed at 330.
##' @author Fernando E. Miguez
##' @seealso See Also as \code{\link{BioGro}}, \code{\link{OpBioGro}} and
##' \code{\link{constrOpBioGro}}.
##' @keywords optimize
##' @examples
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
##' sel.rows <- seq(1,nrow(ans.dat),400)
##' simDat <- ans.dat[sel.rows,c('ThermalT','Stem','Leaf','Root','Rhizome','Grain','LAI')]
##' plot(ans,simDat)
##'
##' ## Residual sum of squares before the optimization
##'
##' ans0 <- BioGro(weather05)
##' RssBioGro(simDat,ans0)
##'
##'
##' op1.mc <- MCMCBioGro(phen=1, niter=200,niter2=200,
##'                      WetDat=weather05,
##'                      data=simDat)
##'
##'
##' plot(op1.mc)
##'
##' plot(op1.mc, plot.kind='trace', subset = nams %in%
##' \t\t\t\tc('kLeaf_1','kStem_1','kRoot_1'))
##'
##' }
##'
MCMCBioGro <- function(niter = 10, niter2=10, phen=6, iCoef=NULL,
                       saTemp=5,coolSamp=20,scale=0.5,
                       WetDat, data, day1=NULL, dayn=NULL,
                       timestep=1,
                       lat=40,iRhizome=7, iLeaf = iRhizome * 1e-4, iStem = iRhizome * 1e-3, iRoot = iRhizome * 1e-3,
                       canopyControl=list(),
                       seneControl=list(),
                       photoControl=list(),
                       phenoControl=list(),
                       soilControl=list(),
                       nitroControl=list(),
                       centuryControl=list(),                       
                       sd=c(2e-2,1e-6)){
                     
  if((niter < 1)|(niter2 < 1))
    stop("niter and niter2 must be 1 or greater")

  if(ncol(data) != 7 || nrow(data) < 2)
    stop("data should have 7 columns and at least 2 rows")

  dnames <- c("ThermalT","Stem","Leaf","Root","Rhizome","Grain","LAI")
  if(any(is.na(unlist(lapply(names(data),pmatch,dnames)))))
    warning("data names and/or order might be wrong")
  
  if(min(data,na.rm=TRUE) < 0)
    stop("negative values are not allowed")

  if( (timestep<1) || (24%%timestep != 0))
    stop("timestep should be a divisor of 24 (e.g. 1,2,3,4,6,etc.)")

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
          dayn <- min(WetDat1s[,1]) + 1
          if(dayn > 330) dayn <- 330
        }
      }

    if((day1<0) || (day1>365) || (dayn<0) || (dayn>365))
      stop("day1 and dayn should be between 0 and 365")

    if(day1 > dayn)
      stop("day1 should be smaller than dayn")
  
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

  data[is.na(data)] <- -1

  tint <- 24 / timestep
  ndat <- dim(data)[1]
  vecsize <- (dayn - (day1-1)) * tint + 1
  indes1 <- (day1-1) * tint;
  indesn <- (dayn) * tint;
  Temp <- WetDat[indes1:indesn,5];
  cTT <- cumsum(Temp/tint);
  tmp1 <- indfun(data[,1],cTT)

  if(length(tmp1) < nrow(data)){
    stop("Error with length of the growing season")
  }
  
  doy <- WetDat[indes1:indesn,2]
  hour <- WetDat[indes1:indesn,3]
  solar <- WetDat[indes1:indesn,4]
  rh <- WetDat[indes1:indesn,6]
  WindS <- WetDat[indes1:indesn,7]
  precip <- WetDat[indes1:indesn,8]

  if(max(rh) > 1)
    stop("Rel Hum. should be 0 < rh < 1")

  if(missing(iCoef)){
    iCoef <- valid_dbp(as.vector(unlist(phenoP)[7:31]))
   }
  
  TPcoefs <- as.vector(unlist(phenoP)[1:6])

  n1dat <- numeric(6)
  for(i in 1:6) n1dat[i] <- nrow(data[data[,1] <= TPcoefs[i],])

  SENcoefs <- as.vector(unlist(seneP))

  soilCoefs <- c(soilP$FieldC, soilP$WiltP, soilP$phi1, soilP$phi2, soilP$soilDepth, mean(soilP$iWatCont))
  wsFun <- soilP$wsFun
  soilType <- soilP$soilType
  
  centCoefs <- as.vector(unlist(centuryP)[1:23])
  
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
  upperT<-photoP$UPPERTEMP
  lowerT<-photoP$LOWERTEMP
  mResp <- canopyP$mResp
  kd <- canopyP$kd
  chi.l <- canopyP$chi.l
  Sp <- canopyP$Sp
  SpD <- canopyP$SpD
  heightF <- canopyP$heightF
  nlayers <- canopyP$nlayers
  leafwidth <- canopyP$leafwidth
  eteq <- canopyP$eteq
  thermal_base_temperature = 0
  initial_biomass = c(iRhizome, iStem, iLeaf, iRoot)
  
  res <- .Call(SABioGro,as.double(data[,1]), as.double(data[,2]),
               as.double(data[,3]), as.double(data[,4]),
               as.double(data[,5]), as.double(data[,6]),
               as.double(data[,7]), as.integer(doy),
               as.integer(hour), as.double(solar),
               as.double(Temp), as.double(rh),
               as.double(WindS), as.double(precip),
               as.double(iCoef), as.integer(vecsize),
               as.double(lat), as.integer(nlayers),
               as.double(initial_biomass), as.double(SENcoefs),
               as.integer(timestep), as.double(vmax),
               as.double(alpha), as.double(kparm),
               as.double(theta), as.double(beta),
               as.double(Rd), as.double(Catm),
               as.double(b0), as.double(b1),
               as.double(soilCoefs), as.integer(wsFun),
               as.integer(ws), as.double(nitroP$iLeafN),
               as.double(nitroP$kLN), as.double(nitroP$Vmax.b1),
               as.double(nitroP$alpha.b1), as.double(mResp),
               as.integer(soilType), as.double(centCoefs),
               as.double(centuryP$Ks), as.integer(centTimestep),
               as.double(kd), as.double(c(chi.l, heightF, leafwidth, eteq)),
               as.double(Sp), as.double(SpD), as.double(thermal_base_temperature),
               as.double(TPcoefs), as.integer(tmp1),
               as.integer(ndat), as.integer(n1dat),
               as.integer(niter), as.integer(niter2),
               as.double(saTemp), as.integer(coolSamp),
               as.double(scale), as.double(sd),
               as.integer(phen),
               as.integer(soilP$soilLayers), as.double(soilP$soilDepths),
               as.double(soilP$iWatCont), as.integer(soilP$hydrDist),
               as.double(c(soilP$rfl,soilP$rsec,soilP$rsdf)), as.double(c(nitroP$kpLN,nitroP$lnb0,nitroP$lnb1)),
               as.integer(nitroP$lnFun), as.double(upperT),
               as.double(lowerT))
  
  res$resMC <- t(res$resMC)
  colnames(res$resMC) <- c("kLeaf_1","kStem_1","kRoot_1","kRhizome_1",
                           "kLeaf_2","kStem_2","kRoot_2","kRhizome_2",
                           "kLeaf_3","kStem_3","kRoot_3","kRhizome_3",
                           "kLeaf_4","kStem_4","kRoot_4","kRhizome_4",
                           "kLeaf_5","kStem_5","kRoot_5","kRhizome_5",
                           "kLeaf_6","kStem_6","kRoot_6","kRhizome_6","kGrain_6")
  structure(res,class="MCMCBioGro")
}


## Auxiliary function to calculate the index and pick the
## correct elements
indfun <- function(obs,pred){

  nlen <- length(obs)
  colind <- numeric(nlen)
  
  for(i in 1:nlen){
    tmp1 <- abs(pred - obs[i])
    colind[i] <- which.min(tmp1)
  }
  colind <- unique(colind)
  colind
}

print.MCMCBioGro <- function(x,...){

  cfs <- c(x$coefs[1:4],NA,x$coefs[5:8],NA,
           x$coefs[9:12],NA,x$coefs[13:16],NA,
           x$coefs[17:20],NA,x$coefs[21:25])
  coefMat <- matrix(cfs,ncol=5,byrow=TRUE)
  colnames(coefMat) <- c("Leaf","Stem","Root","Rhizome","Grain")
  rownames(coefMat) <- c("1","2","3","4","5","6")
  print(coefMat,...)

  cat("accept1:",x$accept,"\n")
  cat("accept2:",x$accept2,"\n")
  cat("accept3:",x$accept3,"\n")
  cat("prop:",x$accept3/x$accept2,"\n")
  cat("RSS:",x$rss,"\n")
}

plot.MCMCBioGro <- function(x,x2=NULL,x3=NULL,plot.kind=c("rss","OF","RF","OFT","trace","density"),
                            type=c("l","p"),coef=1,cols=c("blue","green","red","magenta","black","purple"),...){

  run <- NULL
  plot.kind <- match.arg(plot.kind)
  type <- match.arg(type)
  
  if(plot.kind == "rss"){

    if(missing(x2) & missing(x3)){
      yy <- c(x$RssVec,x$RssVec2)
      plot1 <-  lattice::xyplot(yy  ~ c(1:length(yy)),xlab="Iterations",ylab="RSS",type=type,...)
      print(plot1)
    }else{
      if(missing(x3)){
        yy <- c(x$RssVec,x$RssVec2)
        yy2 <- c(x2$RssVec,x2$RssVec2)
        myl <- max(c(length(yy),length(yy2))) * 1.05
        minyl <- myl * 0.05
        myy <- max(c(yy,yy2),na.rm=TRUE) * 1.05           
        plot1 <-  lattice::xyplot(yy  ~ 1:length(yy),
                         xlab="Iterations",
                         ylab="RSS",type=type,
                         xlim=-minyl:myl,
                         ylim=-5:myy,
                         panel = function(x,y,...){
                           lattice::panel.xyplot(1:length(yy),yy,col=cols[1],...)
                           lattice::panel.xyplot(1:length(yy2),yy2,col=cols[2],...)
                         })
        print(plot1)
      }else{
        yy <- c(x$RssVec,x$RssVec2)
        yy2 <- c(x2$RssVec,x2$RssVec2)
        yy3 <- c(x3$RssVec,x3$RssVec2)
        myl <- max(c(length(yy),length(yy2),length(yy3))) * 1.05
        minyl <- myl * 0.05
        myy <- max(c(yy,yy2,yy3),na.rm=TRUE) * 1.05           
        plot1 <-  lattice::xyplot(yy  ~ 1:length(yy),
                         xlab="Iterations",
                         ylab="RSS",type=type,
                         xlim=-minyl:myl,
                         ylim=-5:myy,
                         panel = function(x,y,...){
                           lattice::panel.xyplot(1:length(yy),yy,col=cols[1],...)
                           lattice::panel.xyplot(1:length(yy2),yy2,col=cols[2],...)
                           lattice::panel.xyplot(1:length(yy3),yy3,col=cols[3],...)
                         })
        print(plot1)
      }
    }
    
  }

   
  if(coef == 1){
    if(plot.kind == "trace"){
      if(missing(x2) & missing(x3)){
        coefnames <- colnames(x$resMC)[c(1,2,3,5,6,7,9:24)]
      
        dat <- data.frame(coefs=c(x$resMC[,c(1,2,3,5,6,7,9:24)]),
                        nams=rep(coefnames,each=x$accept2),iter=1:x$accept2)
        plot1 <- lattice::xyplot(coefs ~ iter | nams , data = dat,xlab="Iterations",ylab="Prop",type=type,col=cols[1],...)
        print(plot1)
      }else{
        if(missing(x3)){
          coefnames <- colnames(x$resMC)[c(1,2,3,5,6,7,9:24)]
      
          dat1 <- data.frame(coefs=c(x$resMC[,c(1,2,3,5,6,7,9:24)]),
                        nams=rep(coefnames,each=x$accept2),iter=1:x$accept2,run="one")
          
          dat2 <- data.frame(coefs=c(x2$resMC[,c(1,2,3,5,6,7,9:24)]),
                        nams=rep(coefnames,each=x2$accept2),iter=1:x2$accept2,run="two")
          dat <- rbind(dat1,dat2)
          plot1 <- lattice::xyplot(coefs ~ iter | nams , groups=run, data = dat,xlab="Iterations",ylab="Prop",type=type,col=cols,...)
          print(plot1)
        }else{
          coefnames <- colnames(x$resMC)[c(1,2,3,5,6,7,9:24)]
      
          dat1 <- data.frame(coefs=c(x$resMC[,c(1,2,3,5,6,7,9:24)]),
                        nams=rep(coefnames,each=x$accept2),iter=1:x$accept2,run="one")
          
          dat2 <- data.frame(coefs=c(x2$resMC[,c(1,2,3,5,6,7,9:24)]),
                        nams=rep(coefnames,each=x2$accept2),iter=1:x2$accept2,run="two")

          dat3 <- data.frame(coefs=c(x3$resMC[,c(1,2,3,5,6,7,9:24)]),
                        nams=rep(coefnames,each=x3$accept2),iter=1:x3$accept2,run="three")
          
          dat <- rbind(dat1,dat2,dat3)
          plot1 <- lattice::xyplot(coefs ~ iter | nams , groups=run, data = dat,xlab="Iterations",ylab="Prop",type=type,col=cols,...)
          print(plot1)
        }
      }
    }
    if(plot.kind == "density"){
       coefnames <- colnames(x$resMC)[c(1,2,3,5,6,7,9:24)]
     if(missing(x2) & missing(x3)){
      
       dat <- data.frame(coefs=c(x$resMC[,c(1,2,3,5,6,7,9:24)])
                         ,nams=rep(coefnames,each=x$accept2))
       plot1 <- densityplot(~ coefs | nams , data = dat,col=cols[1],...)
       print(plot1)
     }else
     if(missing(x3)){

       dat1 <- data.frame(coefs=c(x$resMC[,c(1,2,3,5,6,7,9:24)])
                         ,nams=rep(coefnames,each=x$accept2),run="one")
       dat2 <- data.frame(coefs=c(x2$resMC[,c(1,2,3,5,6,7,9:24)])
                         ,nams=rep(coefnames,each=x2$accept2),run="two")
       dat <- rbind(dat1,dat2)
       plot1 <- densityplot(~ coefs | nams ,groups=run, data = dat,col=cols,...)
       print(plot1)       
     }else{
       dat1 <- data.frame(coefs=c(x$resMC[,c(1,2,3,5,6,7,9:24)])
                         ,nams=rep(coefnames,each=x$accept2),run="one")
       dat2 <- data.frame(coefs=c(x2$resMC[,c(1,2,3,5,6,7,9:24)])
                         ,nams=rep(coefnames,each=x2$accept2),run="two")
       dat3 <- data.frame(coefs=c(x3$resMC[,c(1,2,3,5,6,7,9:24)])
                         ,nams=rep(coefnames,each=x3$accept2),run="three")
       dat <- rbind(dat1,dat2,dat3)
       plot1 <- densityplot(~ coefs | nams ,groups=run, data = dat,col=cols,...)
       print(plot1)       
     }
    }
  }
    
    if(plot.kind == "OF"){

      obs <- c(x$obsStem,x$obsLeaf,x$obsRhiz,x$obsRoot,x$obsGrain)
      sim <- c(x$simStem,x$simLeaf,x$simRhiz,x$simRoot,x$simGrain)
      
    plot1 <- lattice::xyplot(obs ~ sim,xlab="Fitted",ylab="Observed",...,
                    panel = function(x,y,...){
                      lattice::panel.xyplot(x,y,pch=16,col="black",...)
                      lattice::panel.abline(0,1,...)
                    })
    print(plot1)

    }

    if(plot.kind == "RF"){

      obs <- c(x$obsStem,x$obsLeaf,x$obsRhiz,x$obsRoot,x$obsGrain)
      sim <- c(x$simStem,x$simLeaf,x$simRhiz,x$simRoot,x$simGrain)
      rsd <- obs - sim
      
    plot1 <- lattice::xyplot(rsd ~ sim,xlab="Fitted",ylab="Residuals",...,
                    panel = function(x,y,...){
                      lattice::panel.xyplot(x,y,pch=16,col="black",...)
                      lattice::panel.abline(h=0,...)
                    })
    print(plot1)

    }
  
    if(plot.kind == "OFT"){
    xso <- x
    plot1 <- lattice::xyplot(xso$simStem ~ xso$TTime,
                    xlab="Thermal time",
                    ylim=c(-2,I(max(c(xso$simStem,xso$obsStem),na.rm=TRUE)*1.1)),
                    ylab="Dry biomass (Mg/ha)",...,
                    panel = function(x,y,...){
                      lattice::panel.xyplot(xso$TTime,xso$obsStem,type="p",col=cols[1],...)
                      lattice::panel.xyplot(xso$TTime,xso$obsLeaf,type="p",col=cols[2],...)
                      lattice::panel.xyplot(xso$TTime,xso$obsRoot,type="p",col=cols[3],...)
                      lattice::panel.xyplot(xso$TTime,xso$obsRhiz,type="p",col=cols[4],...)
                      lattice::panel.xyplot(xso$TTime,xso$obsGrain,type="p",col=cols[5],...)
                      lattice::panel.xyplot(xso$TTime,xso$obsLAI,type="p",col=cols[6],...)


                      lattice::panel.xyplot(xso$TTime,xso$simStem,type="l",col=cols[1],...)
                      lattice::panel.xyplot(xso$TTime,xso$simLeaf,type="l",col=cols[2],...)
                      lattice::panel.xyplot(xso$TTime,xso$simRoot,type="l",col=cols[3],...)
                      lattice::panel.xyplot(xso$TTime,xso$simRhiz,type="l",col=cols[4],...)
                      lattice::panel.xyplot(xso$TTime,xso$simGrain,type="l",col=cols[5],...)
                      lattice::panel.xyplot(xso$TTime,xso$simLAI,type="l",col=cols[6],...)

                    },key = list(text = list(c("Stem", "Leaf", "Root", 
                         "Rhizome", "Grain", "LAI")), col = cols, lines = TRUE, points=TRUE,pch=21)
                    )
    print(plot1)

    }
}


  

