##
##  BioCro/R/Opc4photo.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2009
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 or 3 of the License
##  (at your option).
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
##  A copy of the GNU General Public License is available at
##  http://www.r-project.org/Licenses/
##
##

## This is the Opc4photo and all of its realted functions

Opc4photo <- function(data,ivmax=39,ialpha=0.04,iRd=0.8,ikparm=0.7,
                      itheta=0.83, ibeta=0.93,
                      Catm=380,ib0=0.08,ib1=3,iStomWS=1, ws=c("gs","vmax"),
                      response=c("Assim","StomCond"),
                      curve.kind=c("Q","Ci"),
                      op.level=1,
                      level=0.95,hessian=TRUE,
                      op.ci=FALSE,...){

  ncol.data <- ncol(data)
  
## I might need some kind of sanity check here
  stopifnot(op.level == 1 || op.level == 2 || op.level == 3)
  
  response <- match.arg(response)
  curve.kind <- match.arg(curve.kind)
  if(response == "Assim"){
    if(curve.kind == "Q"){
      if(op.level == 1){
        cfs <- c(ivmax,ialpha)
      }else
      if(op.level == 2){
        cfs <- c(ivmax,ialpha,iRd)
      }else{
        cfs <- c(ivmax,ialpha,itheta,iRd)
      }
    }else{
      if(op.level == 1){
        cfs <- c(ivmax,ikparm)
      }else
      if(op.level == 2){
        cfs <- c(ivmax,ikparm,iRd)
      }else{
        cfs <- c(ivmax,ikparm,ibeta,iRd)
      }
    }
  }else{
    cfs <- c(ib0,ib1)
  }

  obsvec <- as.matrix(data[,1])
  xparms <- list(Rd=iRd, kparm=ikparm, alpha=ialpha,
                 theta=itheta, beta=ibeta,
                 Catm=Catm, b0=ib0, b1=ib1,
                 StomWS=iStomWS,ws=ws)

  
  RSS <- function(coefs){
      if(response == "Assim"){
        if(max(data[,1]) < 1)
          warning("Units of Assim might be wrong:
                   should be micro mol m-2 s-1\n")

        if(curve.kind == "Q"){
          if(op.level == 1){
            vec1 <- c4photo(data[,2],data[,3],data[,4],
                            coefs[1],coefs[2],ikparm,itheta,ibeta,
                            iRd,Catm,ib0,ib1,iStomWS,ws=ws)
          }else
          if(op.level == 2){
            vec1 <- c4photo(data[,2],data[,3],data[,4],
                            coefs[1],coefs[2],ikparm,itheta,ibeta,
                            coefs[3],Catm,ib0,ib1,iStomWS,ws=ws)
          }else{
            vec1 <- c4photo(data[,2],data[,3],data[,4],
                            coefs[1],coefs[2],ikparm,coefs[3],ibeta,
                            coefs[4],Catm,ib0,ib1,iStomWS,ws=ws)
          }
        }else{
          if(op.level == 1){
            vec1 <- c4photo(data[,2],data[,3],data[,4],
                            coefs[1],ialpha,coefs[2],itheta,ibeta,
                            iRd,Catm,ib0,ib1,iStomWS,ws=ws)
          }else
          if(op.level == 2){
            vec1 <- c4photo(data[,2],data[,3],data[,4],
                            coefs[1],ialpha,coefs[2],itheta,ibeta,
                            coefs[3],Catm,ib0,ib1,iStomWS,ws=ws)
          }else{
            vec1 <- c4photo(data[,2],data[,3],data[,4],
                            coefs[1],ialpha,coefs[2],itheta,coefs[3],
                            coefs[4],Catm,ib0,ib1,iStomWS,ws=ws)
          }
        }
      }else
      if(response == "StomCond"){
        if(max(data[,1]) < 1)
          warning("Units of StomCond might be wrong:
                   should be mmol m-2 s-1\n")
         vec1 <- c4photo(data[,2],data[,3],data[,4],
                    ivmax,ialpha,ikparm,itheta,ibeta,
                         iRd,Catm,coefs[1],coefs[2],iStomWS,ws)$Gs
      }

      if(response == "Assim"){
        rs1 <- obsvec - vec1$Assim
        rss1 <- sum(rs1^2)
      }else{
        stop("not implemented yet")
      }
      if(curve.kind == "Q"){
        if(op.ci){
          rs2 <- data[,5] - vec1$Ci
          rss2 <- sum(rs2^2)
        }else{
          rss2 <- 0
        }
      }else{
        if(op.ci){
          rs2 <- data[,5] - vec1$Ci
          rss2 <- sum(rs2^2)
        }else{
          rss2 <- 0
        }
      }
      rss <- rss1 + rss2
      rss
  }

  resp <- optim(cfs,RSS,hessian=hessian,...)

  bestParms <- resp$par
  ReSumS <- resp$value
  conv <- resp$convergence
  if((conv == 0) && hessian){
    HessMat <- resp$hessian
    iHess <- solve(HessMat)
  }else{
    iHess <- matrix(ncol=I(1+op.level),nrow=I(1+op.level))
  }
  def <- nrow(data)-length(coef)
  sigm <- ReSumS/def
  varcov <- sigm * iHess
  corVA <- varcov[1,2]/sqrt(varcov[1,1]*varcov[2,2])
## Calculating confidence intervals
  alp <- (1 - level)/2
  
  if(response == "Assim"){
    if(curve.kind == "Q"){
      if(op.level == 1){
        ## Vcmax
        lcVmax <- bestParms[1] + qt(alp,def)*sqrt(varcov[1,1])
        ucVmax <- bestParms[1] + qt(1-alp,def)*sqrt(varcov[1,1])
        ## alpha
        lcAlpha <- bestParms[2] + qt(alp,def)*sqrt(varcov[2,2])
        ucAlpha <- bestParms[2] + qt(1-alp,def)*sqrt(varcov[2,2])
        ret <- structure(list(bestVmax=bestParms[1],
                              bestAlpha=bestParms[2],
                              ReSumS=as.numeric(ReSumS),
                              Convergence=conv,
                              VarCov=varcov,df=def,
                              ciVmax=c(lcVmax,ucVmax),
                              ciAlpha=c(lcAlpha,ucAlpha),
                              corVA=corVA,
                              level=level,data=data,
                              xparms=xparms,ncold=ncol.data,
                              op.level=op.level,
                              response="Assim",
                              curve.kind=curve.kind), class = "Opc4photo")
      }else
      if(op.level == 2){
        ## Vcmax
        lcVmax <- bestParms[1] + qt(alp,def)*sqrt(varcov[1,1])
        ucVmax <- bestParms[1] + qt(1-alp,def)*sqrt(varcov[1,1])
        ## alpha
        lcAlpha <- bestParms[2] + qt(alp,def)*sqrt(varcov[2,2])
        ucAlpha <- bestParms[2] + qt(1-alp,def)*sqrt(varcov[2,2])
        ## Rd
        lcRd <- bestParms[3] + qt(alp,def)*sqrt(varcov[3,3])
        ucRd <- bestParms[3] + qt(1-alp,def)*sqrt(varcov[3,3])      
        ret <- structure(list(bestVmax=bestParms[1],
                              bestAlpha=bestParms[2],
                              bestRd=bestParms[3],
                              ReSumS=as.numeric(ReSumS),
                              Convergence=conv,
                              VarCov=varcov,df=def,
                              ciVmax=c(lcVmax,ucVmax),
                              ciAlpha=c(lcAlpha,ucAlpha),
                              ciRd=c(lcRd,ucRd),
                              corVA=corVA,
                              level=level,data=data,
                              xparms=xparms,ncold=ncol.data,
                              op.level=op.level,
                              response="Assim",
                              curve.kind=curve.kind), class = "Opc4photo")
      }else{
        ## Vcmax
        lcVmax <- bestParms[1] + qt(alp,def)*sqrt(varcov[1,1])
        ucVmax <- bestParms[1] + qt(1-alp,def)*sqrt(varcov[1,1])
        ## alpha
        lcAlpha <- bestParms[2] + qt(alp,def)*sqrt(varcov[2,2])
        ucAlpha <- bestParms[2] + qt(1-alp,def)*sqrt(varcov[2,2])
        ## theta
        lcTheta <- bestParms[3] + qt(alp,def)*sqrt(varcov[3,3])
        ucTheta <- bestParms[3] + qt(1-alp,def)*sqrt(varcov[3,3])      
        ## Rd
        lcRd <- bestParms[4] + qt(alp,def)*sqrt(varcov[4,4])
        ucRd <- bestParms[4] + qt(1-alp,def)*sqrt(varcov[4,4])      
        ret <- structure(list(bestVmax=bestParms[1],
                              bestAlpha=bestParms[2],
                              bestTheta=bestParms[3],
                              bestRd=bestParms[4],
                              ReSumS=as.numeric(ReSumS),
                              Convergence=conv,
                              VarCov=varcov,df=def,
                              ciVmax=c(lcVmax,ucVmax),
                              ciAlpha=c(lcAlpha,ucAlpha),
                              ciTheta=c(lcTheta,ucTheta),
                              ciRd=c(lcRd,ucRd),
                              corVA=corVA,
                              level=level,data=data,
                              xparms=xparms,ncold=ncol.data,
                              op.level=op.level,
                              response="Assim",
                              curve.kind=curve.kind), class = "Opc4photo")    
      }
    }else{
      ## Section for Aci
      if(op.level == 1){
        ## Vcmax
        lcVmax <- bestParms[1] + qt(alp,def)*sqrt(varcov[1,1])
        ucVmax <- bestParms[1] + qt(1-alp,def)*sqrt(varcov[1,1])
        ## kparm
        lcKparm <- bestParms[2] + qt(alp,def)*sqrt(varcov[2,2])
        ucKparm <- bestParms[2] + qt(1-alp,def)*sqrt(varcov[2,2])
        ret <- structure(list(bestVmax=bestParms[1],
                              bestKparm=bestParms[2],
                              ReSumS=as.numeric(ReSumS),
                              Convergence=conv,
                              VarCov=varcov,df=def,
                              ciVmax=c(lcVmax,ucVmax),
                              ciKparm=c(lcKparm,ucKparm),
                              corVA=corVA,
                              level=level,data=data,
                              xparms=xparms,ncold=ncol.data,
                              op.level=op.level,
                              response="Assim",
                              curve.kind=curve.kind), class = "Opc4photo")
      }else
      if(op.level == 2){
        ## Vcmax
        lcVmax <- bestParms[1] + qt(alp,def)*sqrt(varcov[1,1])
        ucVmax <- bestParms[1] + qt(1-alp,def)*sqrt(varcov[1,1])
        ## kparm
        lcKparm <- bestParms[2] + qt(alp,def)*sqrt(varcov[2,2])
        ucKparm <- bestParms[2] + qt(1-alp,def)*sqrt(varcov[2,2])
        ## Rd
        lcRd <- bestParms[3] + qt(alp,def)*sqrt(varcov[3,3])
        ucRd <- bestParms[3] + qt(1-alp,def)*sqrt(varcov[3,3])      
        ret <- structure(list(bestVmax=bestParms[1],
                              bestKparm=bestParms[2],
                              bestRd=bestParms[3],
                              ReSumS=as.numeric(ReSumS),
                              Convergence=conv,
                              VarCov=varcov,df=def,
                              ciVmax=c(lcVmax,ucVmax),
                              ciKparm=c(lcKparm,ucKparm),
                              ciRd=c(lcRd,ucRd),
                              corVA=corVA,
                              level=level,data=data,
                              xparms=xparms,ncold=ncol.data,
                              op.level=op.level,
                              response="Assim",
                              curve.kind=curve.kind), class = "Opc4photo")
      }else{
        ## Vcmax
        lcVmax <- bestParms[1] + qt(alp,def)*sqrt(varcov[1,1])
        ucVmax <- bestParms[1] + qt(1-alp,def)*sqrt(varcov[1,1])
        ## kparm
        lcKparm <- bestParms[2] + qt(alp,def)*sqrt(varcov[2,2])
        ucKparm <- bestParms[2] + qt(1-alp,def)*sqrt(varcov[2,2])
        ## beta
        lcBeta <- bestParms[3] + qt(alp,def)*sqrt(varcov[3,3])
        ucBeta <- bestParms[3] + qt(1-alp,def)*sqrt(varcov[3,3])      
        ## Rd
        lcRd <- bestParms[4] + qt(alp,def)*sqrt(varcov[4,4])
        ucRd <- bestParms[4] + qt(1-alp,def)*sqrt(varcov[4,4])      
        ret <- structure(list(bestVmax=bestParms[1],
                              bestKparm=bestParms[2],
                              bestBeta=bestParms[3],
                              bestRd=bestParms[4],
                              ReSumS=as.numeric(ReSumS),
                              Convergence=conv,
                              VarCov=varcov,df=def,
                              ciVmax=c(lcVmax,ucVmax),
                              ciKparm=c(lcKparm,ucKparm),
                              ciBeta=c(lcBeta,ucBeta),
                              ciRd=c(lcRd,ucRd),
                              corVA=corVA,
                              level=level,data=data,
                              xparms=xparms,ncold=ncol.data,
                              op.level=op.level,
                              response="Assim",
                              curve.kind=curve.kind), class = "Opc4photo")    
      }
    }
  }else{
    ## Beta 0
    lcb0 <- bestParms[1] + qt(alp,def)*sqrt(varcov[1,1])
    ucb0 <- bestParms[1] + qt(1-alp,def)*sqrt(varcov[1,1])
    ## Beta 1
    lcb1 <- bestParms[2] + qt(alp,def)*sqrt(varcov[2,2])
    ucb1 <- bestParms[2] + qt(1-alp,def)*sqrt(varcov[2,2]) 
    structure(list(bestb0=bestParms[1],
                   bestb1=bestParms[2],
                   ReSumS=as.numeric(ReSumS),
                   Convergence=conv,
                   VarCov=varcov,df=def,
                   cib0=c(lcb0,ucb0),
                   cib1=c(lcb1,ucb1),
                   corVA=corVA,
                   level=level,data=data,response="StomCond"),
            class = "Opc4photo")
  }
  ret 
}

RsqC4photo <- function(data, vmax=39,alph=0.04,
                           kparm=0.7,theta=0.83,beta=0.93,
                       Rd=0.8,Catm=380,b0=0.08,b1=3,StomWS=1,
                           response=c("Assim","StomCond")){
  response <- match.arg(response)
  if(response == "Assim"){
    if(max(data[,1]) < 1)
      warning("Units of Assim might be wrong:
               should be micro mol m-2 s-1\n")
     vec1 <- c4photo(data[,2],data[,3],data[,4],
                    vmax,alph,kparm,theta,beta,Rd,Catm,b0,b1,StomWS)$Assim
   }
  if(response == "StomCond"){
    if(max(data[,1]) < 1)
      warning("Units of StomCond might be wrong:
               should be mmol m-2 s-1\n")
     vec1 <- c4photo(data[,2],data[,3],data[,4],
                    vmax,alph,kparm,theta,beta,Rd,Catm,b0,b1,StomWS)$Gs
  }
  obsvec <- as.matrix(data[,1])
  SST <- sum(obsvec^2)
  SSE <- sum((obsvec - vec1)^2)
  Rsquare <- 1 - SSE/SST
  if(Rsquare < 0)
    stop("negative Rsq\n")
  Rsquare
}

## Display methods for Opc4photo and OpEC4photo
print.Opc4photo <- function(x,digits=2,...){

  opl <- x$op.level
  c.kind <- x$curve.kind
  cat("\nOptimization of C4 photosynthesis\n")
  cat("\n\t\t",x$level*100,"%   Conf Int\n")

  if(x$response == "Assim"){
    if(x$curve.kind == "Q"){
      if(opl == 1){
        mat <- matrix(nrow=2,ncol=3)
        dimnames(mat) <- list(c("Vmax","alpha"),c("best","lower","upper"))
        mat[,1] <- c(x$bestVmax,x$bestAlpha)
        mat[1,2:3] <- x$ciVmax
        mat[2,2:3] <- x$ciAlpha
      }else
      if(opl == 2){
        mat <- matrix(nrow=3,ncol=3)
        dimnames(mat) <- list(c("Vmax","alpha","Rd"),c("best","lower","upper"))
        mat[,1] <- c(x$bestVmax,x$bestAlpha,x$bestRd)
        mat[1,2:3] <- x$ciVmax
        mat[2,2:3] <- x$ciAlpha
        mat[3,2:3] <- x$ciRd
      }else{
        mat <- matrix(nrow=4,ncol=3)
        dimnames(mat) <- list(c("Vmax","alpha","theta","Rd"),c("best","lower","upper"))
        mat[,1] <- c(x$bestVmax,x$bestAlpha,x$bestTheta,x$bestRd)
        mat[1,2:3] <- x$ciVmax
        mat[2,2:3] <- x$ciAlpha
        mat[3,2:3] <- x$ciTheta
        mat[4,2:3] <- x$ciRd
      }
    }else{
      if(opl == 1){
        mat <- matrix(nrow=2,ncol=3)
        dimnames(mat) <- list(c("Vmax","kparm"),c("best","lower","upper"))
        mat[,1] <- c(x$bestVmax,x$bestKparm)
        mat[1,2:3] <- x$ciVmax
        mat[2,2:3] <- x$ciKparm
      }else
      if(opl == 2){
        mat <- matrix(nrow=3,ncol=3)
        dimnames(mat) <- list(c("Vmax","kparm","Rd"),c("best","lower","upper"))
        mat[,1] <- c(x$bestVmax,x$bestKparm,x$bestRd)
        mat[1,2:3] <- x$ciVmax
        mat[2,2:3] <- x$ciKparm
        mat[3,2:3] <- x$ciRd
      }else{
        mat <- matrix(nrow=4,ncol=3)
        dimnames(mat) <- list(c("Vmax","kparm","beta","Rd"),c("best","lower","upper"))
        mat[,1] <- c(x$bestVmax,x$bestKparm,x$bestBeta,x$bestRd)
        mat[1,2:3] <- x$ciVmax
        mat[2,2:3] <- x$ciKparm
        mat[3,2:3] <- x$ciBeta
        mat[4,2:3] <- x$ciRd
      }
    }
  }else{
      mat <- matrix(nrow=2,ncol=3)
      dimnames(mat) <- list(c("b0","b1"),c("best","lower","upper"))
      mat[,1] <- c(x$bestb0,x$bestb1)
      mat[1,2:3] <- x$cib0
      mat[2,2:3] <- x$cib1
  }

  print.default(mat,digits=digits,print.gap=3)

  if(c.kind == "Q")
    cat("\n Corr  Vmax and alpha:",x$corVA,"\n")
  else
    cat("\n Corr  Vmax and kparm:",x$corVA,"\n")
  
  cat("\n Resid Sums Sq:",x$ReSumS,"\n")
  cat("\nConvergence:")
  if(x$Convergence == 0) cat("YES\n")
  else cat("NO\n")

  invisible(x)

}

## Predict method
predict.Opc4photo <- function(object, newdata,...){

  x <- object
  dat <- x$data

  if(x$response == "Assim"){
    if(x$curve.kind == "Q"){
      if(x$op.level == 1){
        vmax <- x$bestVmax
        alpha <- x$bestAlpha
        theta <- x$xparms$theta
        Rd <- x$xparms$Rd
      }else
      if(x$op.level == 2){
        vmax <- x$bestVmax
        alpha <- x$bestAlpha
        theta <- x$xparms$theta
        Rd <- x$bestRd
      }else{
        vmax <- x$bestVmax
        alpha <- x$bestAlpha
        theta <- x$bestTheta
        Rd <- x$bestRd
      }
    }else{
      if(x$op.level == 1){
        vmax <- x$bestVmax
        kparm <- x$bestKparm
        beta <- x$xparms$beta
        Rd <- x$xparms$Rd
      }else
      if(x$op.level == 2){
        vmax <- x$bestVmax
        kparm <- x$bestKparm
        beta <- x$xparms$beta
        Rd <- x$bestRd
      }else{
        vmax <- x$bestVmax
        kparm <- x$bestKparm
        beta <- x$bestBeta
        Rd <- x$bestRd
      }
    }
  }else{
    stop("Stomatal conductance no implemented yet")
  }

  if(x$curve.kind == "Q"){
    if(missing(newdata) || is.null(newdata)){
      fittd <- c4photo(dat[,2],dat[,3],dat[,4], vmax = vmax,
                       alpha = alpha, kparm = x$xparms$kparm,
                       theta = theta, beta = x$xparms$beta,
                       Rd = Rd, Catm = x$xparms$Catm,
                       b0 = x$xparms$b0, b1 = x$xparms$b1,
                       StomWS = x$xparms$StomWS, ws = x$xparms$ws)
    }else{
      fittd <- c4photo(newdata[,2],newdata[,3],newdata[,4], vmax = vmax,
                       alpha = alpha, kparm = x$xparms$kparm,
                       theta = theta, beta = x$xparms$beta,
                       Rd = Rd, Catm = x$xparms$Catm,
                       b0 = x$xparms$b0, b1 = x$xparms$b1,
                       StomWS = x$xparms$StomWS, ws = x$xparms$ws)

    }
  }else{
    if(missing(newdata) || is.null(newdata)){
      fittd <- c4photo(dat[,2],dat[,3],dat[,4], vmax = vmax,
                       alpha = x$xparms$alpha, kparm = kparm,
                       theta = x$xparms$theta, beta = beta,
                       Rd = Rd, Catm = x$xparms$Catm,
                       b0 = x$xparms$b0, b1 = x$xparms$b1,
                       StomWS = x$xparms$StomWS, ws = x$xparms$ws)
    }else{
      fittd <- c4photo(newdata[,2],newdata[,3],newdata[,4], vmax = vmax,
                       alpha = x$xparms$alpha, kparm = kparm,
                       theta = x$xparms$theta, beta = beta,
                       Rd = Rd, Catm = x$xparms$Catm,
                       b0 = x$xparms$b0, b1 = x$xparms$b1,
                       StomWS = x$xparms$StomWS, ws = x$xparms$ws)

    }    
  }
    fittd
}


## This function will implement simple calculations
## of predicted and residuals for the Opc4photo function

summary.Opc4photo <- function(object,...){

  dat <- object$data
  obsvec <- as.vector(dat[,1])
  
  fittd <- predict(object)
  
  rsd <- obsvec - fittd$Assim
  rss <- object$ReSumS
  ## Some measures of agreement
  ## Index of agreement
  IAN <- t(rsd)%*%rsd
  IAD1 <- abs(rsd) + abs(scale(obsvec,scale=FALSE))
  IAD <- t(IAD1)%*%IAD1
  IA <- 1 - IAN/IAD
  ## Rsquared 1
  Rsq1 <- as.numeric(1 - rss / t(obsvec)%*%obsvec)
  ## Rsquared 2
  Rsq2 <- as.numeric(cor(fittd$Assim,obsvec)^2)
  ## Mean bias
  meanBias <- mean(rsd)
  ## AIC and BIC
  n1 <- length(rsd)
  AIC <- n1 * log(rss/n1) + 2
  BIC <- n1 * log(rss/n1) + 2 * log(n1)

  sigma <- sqrt(rss/(n1-2))
  stdresid <- rsd/sigma
  outli <- which(abs(stdresid) > 2)
  
  structure(list(fitted=fittd$Assim,resid=rsd,
                 stdresid=stdresid,
                 IA=IA,Rsq1=Rsq1,Rsq2=Rsq2,
                 meanBias=meanBias,
                 AIC=AIC,BIC=BIC,
                 outli=outli,
       sigma=sigma),class="summary.Opc4photo")
}

print.summary.Opc4photo <- function(x,...){

  cat("\n Diagnostic measures\n")
  cat("\n Index of Agreement:",x$IA)
  cat("\n Rsquared 1:",x$Rsq1)
  cat("\n Rsquared 2:",x$Rsq2)
  cat("\n Mean Bias:",x$meanBias)
  cat("\n AIC:",x$AIC)
  cat("\n BIC:",x$BIC,"\n")
}

plot.Opc4photo <- function(x,plot.kind=c("RvsF","OandF","OvsF"),resid=c("std","raw"),...){

  dat <- x$data
  obsvec <- as.vector(dat[,1])

  if(x$response == "Assim") {
    fittd <- predict(x)
  }else{
    fittd <- predict(x)$Gs
  }

  fttA <- fittd$Assim
  fttCi <- fittd$Ci
  
  rsd <- obsvec - fttA
  rss <- x$ReSumS
  n1 <- length(rsd)
  
  sigma <- sqrt(rss/(n1-2))
  stdresid <- rsd/sigma
  outid <- which(abs(stdresid) > 2)

  resid <- match.arg(resid)
  plot.kind <- match.arg(plot.kind)

  if(plot.kind == "RvsF"){
    if(resid == "std"){
      plot1 <- xyplot(stdresid ~ fttA,...,
                      xlab="fitted",
                      ylab="standardized resduals",
                      panel = function(x,y,...){
                        panel.xyplot(x,y,...)
                        panel.abline(h=0,...)
                        ltext(x[outid],y[outid],labels=outid,adj=-1,...)
                      }
                      )
      print(plot1)
    }
    if(resid == "raw"){
            plot1 <- xyplot(rsd ~ fttA,...,
                      xlab="fitted",
                      ylab="resduals",
                      panel = function(x,y,...){
                        panel.xyplot(x,y,...)
                        panel.abline(h=0,...)
                      }
                      )
      print(plot1)
    }
  }
  if(plot.kind == "OandF"){
    if(x$curve.kind == "Q"){
      plot1 <- xyplot(obsvec + fttA ~ dat[,2],...,
                      auto.key=TRUE,
                      ylab = "CO2 uptake",
                      xlab = "Quantum flux")
      print(plot1)
    }else{
      plot1 <- xyplot(obsvec ~ dat[,5],...,
                      ylab = "CO2 uptake",
                      xlab = "Ci",
                      panel = function(x,y,...){
                        panel.xyplot(x,y,col="blue",...)
                        panel.xyplot(dat[,5],fttA,col="green",...)
                      },
                      key=list(text=list(c("obs","sim")),
                        col=c("blue","green"),lines=TRUE,space="top"))
      print(plot1)
    }

  }
  if(plot.kind == "OvsF"){
    plot1 <- xyplot(obsvec ~ fttA,...,
                    xlab="fitted",
                    ylab="observed",
                    panel = function(x,y,...){
                      panel.xyplot(x,y,...)
                      panel.abline(0,1,...)
                    })
    plot(plot1)
  }
}

mOpc4photo <- function(data,ID=NULL,
                       ivmax=39,ialpha=0.04,
                       iRd=0.8,ikparm=0.7,
                       itheta=0.83,ibeta=0.93,
                       curve.kind=c("Q","Ci"),
                       op.level=1,op.ci=FALSE,
                       verbose=FALSE,...){

## Some sanity check

  uruns <- unique(data[,1])
  nruns <- length(unique(data[,1]))

  curve.kind <- match.arg(curve.kind)

  if(curve.kind == "Q"){
    if(length(ivmax) == 1){
      miVmax <- rep(ivmax,nruns)
    }else{
      if(length(ivmax) != nruns)
        stop("length of ivmax should be either 1 or equal to the number or runs") 
      miVmax <- ivmax
    }

    if(length(ialpha) == 1){
      miAlpha <- rep(ialpha,nruns)
    }else{
      if(length(ialpha) != nruns)
        stop("length of ialpha should be either 1 or equal to the number or runs")
      miAlpha <- ialpha
    }
    
    if(op.level == 2 || op.level == 3){
      if(length(iRd) == 1){
        miRd <- rep(iRd,nruns)
      }else{
        if(length(iRd) != nruns)
          stop("length of iRd should be either 1 or equal to the number or runs")
        miRd <- iRd
      }
    }
    
    if(op.level == 3){
      if(length(itheta) == 1){
        miTheta <- rep(itheta,nruns)
      }else{
        if(length(itheta) != nruns)
          stop("length of itheta should be either 1 or equal to the number or runs")
        miTheta <- itheta
      }
    }
    
    mat <- matrix(ncol=I(3+op.level),nrow=nruns)
    ciVmax <- matrix(ncol=3,nrow=nruns)
    ciAlpha <- matrix(ncol=3,nrow=nruns)
    
    for(i in seq_len(nruns)){

      if(op.level == 1){
        op <- try(Opc4photo(data[data[,1] == uruns[i],2:5],Catm=data[data[,1] == uruns[i],6],
                            ivmax=miVmax[i],ialpha=miAlpha[i],
                            op.level=1,op.ci=op.ci,...),TRUE)

        if(class(op) == "try-error"){
          mat[i,1:4] <- c(i,rep(NA,2),1)
        }else{
          mat[i,1:4] <- c(i,op$bestVmax,op$bestAlpha,op$Convergence)
          ciVmax[i,1:3] <- c(i,op$ciVmax)
          ciAlpha[i,1:3] <- c(i,op$ciAlpha)
        }
        colnames(mat) <- c("run","vmax","alpha","conv")
      }else
      if(op.level == 2){
        op <- try(Opc4photo(data[data[,1] == uruns[i],2:5],Catm=data[data[,1] == uruns[i],6],
                            ivmax=miVmax[i],ialpha=miAlpha[i],
                            iRd=miRd[i],op.level=2,op.ci=op.ci,...),TRUE)

        if(class(op) == "try-error"){
          mat[i,1:5] <- c(i,rep(NA,3),1)
        }else{
          mat[i,1:5] <- c(i,op$bestVmax,op$bestAlpha,op$bestRd,op$Convergence)
        }
        colnames(mat) <- c("run","vmax","alpha","Rd","conv")
      }else
      if(op.level == 3){
        op <- try(Opc4photo(data[data[,1] == uruns[i],2:5],Catm=data[data[,1] == uruns[i],6],
                            ivmax=miVmax[i],ialpha=miAlpha[i],
                            iRd=miRd[i],itheta=miTheta[i],
                            op.level=3,op.ci=op.ci,...),TRUE)

        if(class(op) == "try-error"){
          mat[i,1:6] <- c(i,rep(NA,4),1)
        }else{
          mat[i,1:6] <- c(i,op$bestVmax,op$bestAlpha,op$bestTheta,op$bestRd,op$Convergence)
        }
        colnames(mat) <- c("run","vmax","alpha","theta","Rd","conv")
      }

      if(verbose){
        cat("Run:",i,"... Converged",ifelse(mat[i,ncol(mat)]==0,"YES","NO"),"\n")
      }

    }
    if(!missing(ID)){
      if(length(ID) != nruns)
        stop("Length of ID should be equal to the number of runs")
      
      mat <- as.data.frame(mat)
      mat$ID <- ID
    }

    ans <- structure(list(mat=mat, op.level=op.level,
                          ciVmax=ciVmax, ciAlpha=ciAlpha,
                          curve.kind = curve.kind), class = "mOpc4photo")
  }else{
    if(length(ivmax) == 1){
      miVmax <- rep(ivmax,nruns)
    }else{
      if(length(ivmax) != nruns)
        stop("length of ivmax should be either 1 or equal to the number or runs") 
      miVmax <- ivmax
    }

    if(length(ikparm) == 1){
      miKparm <- rep(ikparm,nruns)
    }else{
      if(length(ikparm) != nruns)
        stop("length of ikparm should be either 1 or equal to the number or runs")
      miKparm <- ikparm
    }
  
    if(op.level == 2 || op.level == 3){
      if(length(iRd) == 1){
        miRd <- rep(iRd,nruns)
      }else{
        if(length(iRd) != nruns)
          stop("length of iRd should be either 1 or equal to the number or runs")
        miRd <- iRd
      }
    }
  
    if(op.level == 3){
      if(length(ibeta) == 1){
        miBeta <- rep(ibeta,nruns)
      }else{
        if(length(ibeta) != nruns)
          stop("length of ibeta should be either 1 or equal to the number or runs")
        miBeta <- ibeta
      }
    }
  
  mat <- matrix(ncol=I(3+op.level),nrow=nruns)
  ciVmax <- matrix(ncol=3,nrow=nruns)
  ciKparm <- matrix(ncol=3,nrow=nruns)
  
  for(i in seq_len(nruns)){

    if(op.level == 1){
      op <- try(Opc4photo(data[data[,1] == uruns[i],2:6],Catm=data[data[,1] == uruns[i],7],
                          ivmax=miVmax[i],ikparm=miKparm[i],
                          op.level=1,curve.kind=curve.kind,
                          op.ci=op.ci,...),TRUE)

      if(class(op) == "try-error"){
        mat[i,1:4] <- c(i,rep(NA,2),1)
      }else{
        mat[i,1:4] <- c(i,op$bestVmax,op$bestKparm,op$Convergence)
        ciVmax[i,1:3] <- c(i,op$ciVmax)
        ciKparm[i,1:3] <- c(i,op$ciKparm)
      }
      colnames(mat) <- c("ID","vmax","kparm","conv")
    }else
    if(op.level == 2){
      op <- try(Opc4photo(data[data[,1] == uruns[i],2:6],Catm=data[data[,1] == uruns[i],7],
                          ivmax=miVmax[i],ikparm=miKparm[i],
                          iRd=miRd[i],op.level=2,
                          curve.kind=curve.kind,op.ci=op.ci,...),TRUE)

      if(class(op) == "try-error"){
        mat[i,1:5] <- c(i,rep(NA,3),1)
      }else{
        mat[i,1:5] <- c(i,op$bestVmax,op$bestKparm,op$bestRd,op$Convergence)
      }
      colnames(mat) <- c("ID","vmax","kparm","Rd","conv")
    }else
    if(op.level == 3){
      op <- try(Opc4photo(data[data[,1] == uruns[i],2:6],Catm=data[data[,1] == uruns[i],7],
                          ivmax=miVmax[i],ikparm=miKparm[i],
                          iRd=miRd[i],ibeta=miBeta[i],
                          op.level=3,
                          curve.kind=curve.kind,op.ci=op.ci,...),TRUE)

      if(class(op) == "try-error"){
        mat[i,1:6] <- c(i,rep(NA,4),1)
      }else{
        mat[i,1:6] <- c(i,op$bestVmax,op$bestKparm,op$bestBeta,op$bestRd,op$Convergence)
      }
      colnames(mat) <- c("ID","vmax","kparm","beta","Rd","conv")
    }
  
    if(verbose){
      cat("Run:",i,"... Converged",ifelse(mat[i,ncol(mat)]==0,"YES","NO"),"\n")
    }
  }
      
    if(!missing(ID)){
      if(length(ID) != nruns)
        stop("Length of ID should be equal to the number of runs")
      
      mat <- as.data.frame(mat)
      mat$ID <- ID
    }
    
    ans <- structure(list(mat=mat, op.level=op.level,
                          ciVmax=ciVmax, ciKparm=ciKparm,
                        curve.kind=curve.kind), class = "mOpc4photo")
  }
  ans 
}


## Printing method
print.mOpc4photo <- function(x,...){

  ncolm <- ncol(unclass(x)$mat)
  ma <- x$mat
  cat("Number of runs:",nrow(ma),"\n")
  cat("Number converged:",length(ma[ma[,ncolm] == 0,ncolm]),"\n\n")

  if(x$op.level == 1){
    mat <- matrix(ncol=3,nrow=2)
    if(x$curve.kind == "Q"){
      dimnames(mat) <- list(c("vmax","alpha"),c("mean","min","max"))
    }else{
      dimnames(mat) <- list(c("vmax","kparm"),c("mean","min","max"))
    }
    mat[1,1] <- mean(ma[,2],na.rm=TRUE)
    mat[2,1] <- mean(ma[,3],na.rm=TRUE)
    mat[1,2] <- min(ma[,2],na.rm=TRUE)
    mat[2,2] <- min(ma[,3],na.rm=TRUE)
    mat[1,3] <- max(ma[,2],na.rm=TRUE)
    mat[2,3] <- max(ma[,3],na.rm=TRUE)
    print.default(mat,print.gap=3)
  }else
  if(x$op.level == 2){
    mat <- matrix(ncol=3,nrow=3)
    if(x$curve.kind == "Q"){
      dimnames(mat) <- list(c("vmax","alpha","Rd"),c("mean","min","max"))
    }else{
      dimnames(mat) <- list(c("vmax","kparm","Rd"),c("mean","min","max"))
    }
    mat[1,1] <- mean(ma[,2],na.rm=TRUE)
    mat[2,1] <- mean(ma[,3],na.rm=TRUE)
    mat[3,1] <- mean(ma[,4],na.rm=TRUE)
    mat[1,2] <- min(ma[,2],na.rm=TRUE)
    mat[2,2] <- min(ma[,3],na.rm=TRUE)
    mat[3,2] <- min(ma[,4],na.rm=TRUE)
    mat[1,3] <- max(ma[,2],na.rm=TRUE)
    mat[2,3] <- max(ma[,3],na.rm=TRUE)
    mat[3,3] <- max(ma[,4],na.rm=TRUE)
    print.default(mat,print.gap=3)
  }else
  if(x$op.level == 3){
    mat <- matrix(ncol=3,nrow=4)
    if(x$curve.kind == "Q"){
      dimnames(mat) <- list(c("vmax","alpha","theta","Rd"),c("mean","min","max"))
    }else{
      dimnames(mat) <- list(c("vmax","kparm","beta","Rd"),c("mean","min","max"))
    }
    mat[1,1] <- mean(ma[,2],na.rm=TRUE)
    mat[2,1] <- mean(ma[,3],na.rm=TRUE)
    mat[3,1] <- mean(ma[,4],na.rm=TRUE)
    mat[4,1] <- mean(ma[,5],na.rm=TRUE)
    mat[1,2] <- min(ma[,2],na.rm=TRUE)
    mat[2,2] <- min(ma[,3],na.rm=TRUE)
    mat[3,2] <- min(ma[,4],na.rm=TRUE)
    mat[4,2] <- min(ma[,5],na.rm=TRUE)
    mat[1,3] <- max(ma[,2],na.rm=TRUE)
    mat[2,3] <- max(ma[,3],na.rm=TRUE)
    mat[3,3] <- max(ma[,4],na.rm=TRUE)
    mat[4,3] <- max(ma[,5],na.rm=TRUE)
    print.default(mat,print.gap=3)
  }
}



## Plotting method
plot.mOpc4photo <- function(x, parm = c("vmax","alpha"), ...){

  parm <- match.arg(parm)
  res <- x

  if(parm == "vmax"){
    civmax <- x$ciVmax
    id <- factor(res$mat[,1])
    ymax <- max(civmax[,3],na.rm=TRUE) * 1.05
    ymin <- min(civmax[,2],na.rm=TRUE) * 0.95
    xyplot(civmax[,3] ~ id, ylim = c(ymin, ymax),
           ylab = "Vmax",
           xlab = "ID",
           panel = function(x,y,...){
             panel.xyplot(x,y,pch="-",cex=3,...)
             panel.xyplot(id,res$mat[,2],pch=1,cex=1.5,...)
             panel.xyplot(id,civmax[,2],pch="-",cex=3,...)
           }
           )
  }else
  if(parm == "alpha"){
    cialpha <- x$ciAlpha
    id <- factor(res$mat[,1])
    ymax <- max(cialpha[,3],na.rm=TRUE) * 1.05
    ymin <- min(cialpha[,2],na.rm=TRUE) * 0.95
    xyplot(cialpha[,3] ~ id, ylim = c(ymin, ymax),
           ylab = "alpha",
           xlab = "ID",
           panel = function(x,y,...){
             panel.xyplot(x,y,pch="-",cex=3,...)
             panel.xyplot(id,res$mat[,3],pch=1,cex=1.5,...)
             panel.xyplot(id,cialpha[,2],pch="-",cex=3,...)
           }
           )
  }
  
###   stop("not implemented yet")
###   if(x$curve.kind == "Q"){
###     plotAQ(x, fittd)
###   }else{
###     stop("A/Ci not implemented yet")
###   }

}

