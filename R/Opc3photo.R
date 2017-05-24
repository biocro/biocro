##  R/Opc3photo.R by Fernando Ezequiel Miguez  Copyright (C) 2009
## This is the Opc4photo and all of its realted functions
##' Optimize parameters of the C3 photosynthesis model.
##'
##' Applies the \code{optim} function to C3 photosynthesis.
##'
##'
##' @aliases Opc3photo plot.Opc3photo print.Opc3photo predict.Opc3photo
##' @param data should be a \code{data.frame} or \code{matrix} with x columns
##'
##' col 1: measured assimilation (CO2 uptake) col 2: Incomming PAR
##' (photosynthetic active radiation) col 3: Leaf temperature col 4: Relative
##' humidity col 5: Intercellular CO2 (for A/Ci curves) col 6: Reference CO2
##' level
##' @param ivcmax Initial value for \code{vcmax}.
##' @param ijmax Initial value for \code{jmax}.
##' @param iRd Initial value for \code{Rd}.
##' @param Catm Reference CO2.
##' @param O2 Reference level of O2.
##' @param ib0 Initial value for the intercept to the Ball-Berry model.
##' @param ib1 Initial value for the slope to the Ball-Berry model.
##' @param itheta Initial value for \code{theta}.
##' @param op.level Level 1 will optimize \code{Vcmax} and \code{Jmax} and
##' level 2 will optimize \code{Vcmax}, \code{Jmax} and \code{Rd}.
##' @param op.method optimization method. At the moment only optim is
##' implemented.
##' @param response \code{'Assim'} for assimilation and \code{'StomCond'} for
##' stomatal conductance.
##' @param level Confidence interval level.
##' @param hessian Whether the hessian should be computed
##' @param curve.kind Whether an A/Ci curve is being optimized or an A/Q curve.
##' @param op.ci whether to optimize intercellular CO2.
##' @param \dots Additioanl arguments to be passed to \code{\link{optim}}.
##' @export
##' @return
##'
##' An object of class \code{Opc3photo}.
##'
##' The following components can be extracted:
##' \itemize{
##' \item bestVmax optimized \code{vmax}.
##' \item bestJmax optimized \code{jmax}.
##' \item ReSumS Residual Sum of Squares.
##' \item Convergence Convergence status.
##' \item VarCov Variance-covariance matrix.
##' \item df degress of freedom.
##' \item ciVmax Confidence interval for \code{vmax}.
##' \item ciJmax Confidence interval for \code{jmax}.
##' \item corVJ correlation between \code{vmax} and \code{jmax}.
##' \item level Confidence interval level.
##' \item data Original data.
##' \item xparms Additional parameters.
##' \item curve.kind A/Ci or A/Q curve.
##' \item op.level Level 1 means \code{vcmax} and \code{jmax} were
##' optimized and level 2 \code{vcmax}, \code{jmax} and \code{Rd}.
##' \item response \code{'Assim'} or \code{'StomCond'}.
##' }
##' @note ~~further notes~~ Additional notes about the assumptions.
##'
##' @author Fernando E. Miguez
##' @seealso See Also \code{\link{mOpc3photo}}
##' @keywords optimize
##' @examples
##'
##' ## Load fabricated data
##' data(simA100)
##' ## Look at it
##' head(simA100)
##'
##' op <- Opc3photo(simA100[,1:5],Catm=simA100[,9], op.level = 2)
##'
##' ## If faced with a difficult problem
##' ## This can give starting values
##' op100 <- Opc3photo(simA100[,1:5],Catm=simA100[,9],
##'                    op.level = 2, method='SANN',
##'                    hessian=FALSE)
##'
##' op100 <- Opc3photo(simA100[,1:5],Catm = simA100[,9],
##'                    op.level = 2,
##'                    ivcmax = op100$bestVmax,
##'                    ijmax = op100$bestJmax,
##'                    iRd = op100$bestRd)
##' op100
##'
Opc3photo <- function(data,ivcmax=100,ijmax=180,iRd=1.1,
                      Catm=380,O2=210, ib0=0.08,ib1=9.58,
                      itheta=0.7,
                      op.level=1,
                      op.method=c("optim","nlminb"),
                      response=c("Assim","StomCond"),level=0.95,hessian=TRUE,
                      curve.kind=c("Ci","Q"),
                      op.ci=FALSE,...){

  stopifnot(op.level == 1 || op.level == 2 || op.level == 3)
  
  response <- match.arg(response)
  op.method <- match.arg(op.method)
  curve.kind <- match.arg(curve.kind)

  if(curve.kind == "Q"){
    stop("Not implemented yet")
  }
  
  if(response == "Assim"){
    if(curve.kind == "Q"){ 
      if(op.level == 1){
        cfs <- c(ivcmax,ijmax)
      }else
      if(op.level == 2){
        cfs <- c(ivcmax, ijmax, iRd)
      }else{
        cfs <- c(ivcmax, ijmax, itheta, iRd)
      }
    }else{
      if(op.level == 1){
        cfs <- c(ivcmax,ijmax)
      }else
      if(op.level == 2){
        cfs <- c(ivcmax, ijmax, iRd)
      }else{
        stop("No op.level 3 for A/Ci curves")
      }
    }
  }else{
    cfs <- c(ib0,ib1)
  }

  obsvec <- as.matrix(data[,1])
  ## Extra parameters
  xparms <- list(Catm=Catm, O2=O2, b0=ib0, b1=ib1, theta=itheta,
                 Rd = iRd)
  
  RSS <- function(coefs){
      if(response == "Assim"){
        if(max(data[,1]) < 1)
          warning("Units of Assim might be wrong:
                   should be micro mol m-2 s-1\n")
        if(curve.kind =="Ci"){
          if(op.level == 1){        
            vec1 <- c3photo(data[,2],data[,3],data[,4],
                            vcmax=coefs[1],jmax=coefs[2],Rd=iRd,
                            Catm,O2,ib0,ib1,itheta)
          }else{
            vec1 <- c3photo(data[,2],data[,3],data[,4],
                            vcmax=coefs[1],jmax=coefs[2],Rd=coefs[3],
                            Catm,O2,ib0,ib1,itheta)
          }
        }else{
          stop("Not imlemented yet")
        }
      }else
      if(response == "StomCond"){
        stop("not implemented yet")
        if(max(data[,1]) < 1)
          warning("Units of StomCond might be wrong:
                   should be mmol m-2 s-1\n")
         ## vec1 <- c3photo(data[,2],data[,3],data[,4],
         ##            ivcmax,ialph,ikparm,iTheta,ibeta,
         ##                 iRd,Catm,coefs[1],coefs[2])$Gs
      }
      if(response == "Assim"){
        rs1 <- obsvec - vec1$Assim
        rss1 <- sum(rs1^2)
      }else{
        stop("Not implemented yet")
      }
      if(op.ci){
        rs2 <- data[,5] - vec1$Ci
        rss2 <- sum(rs2^2)
      }else{
        rss2 <- 0
      }
      rss <- rss1 + rss2
      rss
  }

  if(op.method == "optim"){
    if(response == "Assim"){
      resp <- optim(cfs,RSS,hessian=hessian,...)
    }else{
      stop("Not implemented yet")
      resp <- optim(cfs,RSS,hessian=hessian,...)
    }
  }else{
    if(response == "Assim"){
      resp <- nlminb(cfs,RSS)
      resp$value <- resp$objective
     }else{
      resp <- nlminb(cfs,RSS)
    }
  }

  bestParms <- resp$par
  ReSumS <- resp$value
  conv <- resp$convergence
  if((op.method == "optim") && (conv == 0) && hessian){
    HessMat <- resp$hessian
    iHess <- solve(HessMat)
  }else{
    iHess <- matrix(ncol=3,nrow=3)
  }
  def <- nrow(data)-length(coef)
  sigm <- ReSumS/def
  varcov <- sigm * iHess
  corVJ <- varcov[1,2]/sqrt(varcov[1,1]*varcov[2,2])
## Calculating confidence intervals
  alp <- (1 - level)/2
  if(response == "Assim"){
    if(op.level == 1){
      ## Vcmax
      lcVmax <- bestParms[1] + qt(alp,def)*sqrt(varcov[1,1])
      ucVmax <- bestParms[1] + qt(1-alp,def)*sqrt(varcov[1,1])
      ## Jmax
      lcJmax <- bestParms[2] + qt(alp,def)*sqrt(varcov[2,2])
      ucJmax <- bestParms[2] + qt(1-alp,def)*sqrt(varcov[2,2])

      structure(list(bestVmax=bestParms[1],
                     bestJmax=bestParms[2],
                     ReSumS=as.numeric(ReSumS),
                     Convergence=conv,
                     VarCov=varcov,df=def,
                     ciVmax=c(lcVmax,ucVmax),
                     ciJmax=c(lcJmax,ucJmax),
                     corVJ=corVJ,
                     level=level,data=data,
                     xparms=xparms,
                     curve.kind = curve.kind,
                     op.level = op.level,
                     response="Assim"),
                class = "Opc3photo")
    }else
    if(op.level == 2){
      ## Vcmax
      lcVmax <- bestParms[1] + qt(alp,def)*sqrt(varcov[1,1])
      ucVmax <- bestParms[1] + qt(1-alp,def)*sqrt(varcov[1,1])
      ## Jmax
      lcJmax <- bestParms[2] + qt(alp,def)*sqrt(varcov[2,2])
      ucJmax <- bestParms[2] + qt(1-alp,def)*sqrt(varcov[2,2])
      ## Rd
      lcRd <- bestParms[3] + qt(alp,def)*sqrt(varcov[3,3])
      ucRd <- bestParms[3] + qt(1-alp,def)*sqrt(varcov[3,3]) 
      structure(list(bestVmax=bestParms[1],
                     bestJmax=bestParms[2],
                     bestRd=bestParms[3],
                     ReSumS=as.numeric(ReSumS),
                     Convergence=conv,
                     VarCov=varcov,df=def,
                     ciVmax=c(lcVmax,ucVmax),
                     ciJmax=c(lcJmax,ucJmax),
                     ciRd=c(lcRd,ucRd),
                     corVJ=corVJ,
                     level=level,data=data,
                     xparms=xparms,
                     curve.kind = curve.kind,
                     op.level=op.level,
                     response="Assim"),
                class = "Opc3photo")
    }
  }else{
    stop("not implemented yet")
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
#                 corVA=corVA,
                 level=level,data=data,response="StomCond"),
            class = "Opc3photo")
  }
    
}

## Display methods for Opc4photo and OpEC4photo
##' Display methods for Opc4photo and OpEC4photo
##' @export
##' @S3method print Opc3photo
print.Opc3photo <- function(x,digits=2,...){

  cat("\nOptimization of C3 photosynthesis\n")
  cat("\n\t\t",x$level*100,"%   Conf Int\n")

  if(x$response == "Assim"){
    if(x$curve.kind == "Ci") 
      if(x$op.level == 1){
        mat <- matrix(nrow=2,ncol=3)
        dimnames(mat) <- list(c("Vmax","Jmax"),c("best","lower","upper"))
        mat[,1] <- c(x$bestVmax,x$bestJmax)
        mat[1,2:3] <- x$ciVmax
        mat[2,2:3] <- x$ciJmax
      }else{
        mat <- matrix(nrow=3,ncol=3)
        dimnames(mat) <- list(c("Vmax","Jmax","Rd"),c("best","lower","upper"))
        mat[,1] <- c(x$bestVmax,x$bestJmax,x$bestRd)
        mat[1,2:3] <- x$ciVmax
        mat[2,2:3] <- x$ciJmax
        mat[3,2:3] <- x$ciRd
      }
  }else{
    stop("not implemented yet")
      mat <- matrix(nrow=2,ncol=3)
      dimnames(mat) <- list(c("b0","b1"),c("best","lower","upper"))
      mat[,1] <- c(x$bestb0,x$bestb1)
      mat[1,2:3] <- x$cib0
      mat[2,2:3] <- x$cib1
  }

  print.default(mat,digits=digits,print.gap=3)

  if(x$response == "Assim"){
    cat("\n Corr  Vmax and Jmax:",x$corVJ,"\n")
  }else{
    stop("not implemented yet")
    cat("\n Corr  b0 and b1:",x$corVA,"\n")
  }
  cat("\n Resid Sums Sq:",x$ReSumS,"\n")
  cat("\nConvergence:")
  if(x$Convergence == 0) cat("YES\n")
  else cat("NO\n")

  invisible(x)

}

## Predict method
##' Predict method
##' @export
##' @S3method predict Opc3photo
predict.Opc3photo <- function(object,newdata,...){

  x <- object
  dat <- x$data

  if(x$response == "Assim"){
    if(x$curve.kind == "Q"){
      if(x$op.level == 1){
        vcmax <- x$bestVmax
        jmax <- x$bestJmax
        theta <- x$xparms$theta
        Rd <- x$xparms$Rd
      }else
      if(x$op.level == 2){
        vcmax <- x$bestVmax
        jmax <- x$bestJmax
        theta <- x$xparms$theta
        Rd <- x$bestRd
      }else{
        vcmax <- x$bestVmax
        jmax <- x$bestJmax
        theta <- x$bestTheta
        Rd <- x$bestRd
      }
    }else{
      theta <- x$xparms$theta
      if(x$op.level == 1){
        vcmax <- x$bestVmax
        jmax <- x$bestJmax
        Rd <- x$xparms$Rd
      }else
      if(x$op.level == 2){
        vcmax <- x$bestVmax
        jmax <- x$bestJmax
        Rd <- x$bestRd
      }else{
        stop("no level 3 for curve.kind = Ci")
      }
    }
  }else{
    stop("Stomatal conductance no implemented yet")
  }

  if(x$curve.kind == "Q"){
    if(missing(newdata) || is.null(newdata)){
      fittd <- c3photo(dat[,2],dat[,3],dat[,4], vcmax = vcmax,
                       jmax = jmax, 
                       theta = theta, 
                       Rd = Rd, Catm = x$xparms$Catm,
                       b0 = x$xparms$b0, b1 = x$xparms$b1,
                       O2 = x$xparms$O2)
    }else{
      fittd <- c3photo(newdata[,2],newdata[,3],newdata[,4], vcmax = vcmax,
                       jmax = jmax, 
                       theta = theta, 
                       Rd = Rd, Catm = x$xparms$Catm,
                       b0 = x$xparms$b0, b1 = x$xparms$b1,
                       O2 = x$xparms$O2)

    }
  }else{
    if(missing(newdata) || is.null(newdata)){
      fittd <- c3photo(dat[,2],dat[,3],dat[,4], vcmax = vcmax,
                       jmax = jmax, 
                       theta = theta, 
                       Rd = Rd, Catm = x$xparms$Catm,
                       b0 = x$xparms$b0, b1 = x$xparms$b1,
                       O2 = x$xparms$O2)
    }else{
      fittd <- c3photo(newdata[,2],newdata[,3],newdata[,4], vcmax = vcmax,
                       jmax = jmax, 
                       theta = theta, 
                       Rd = Rd, Catm = x$xparms$Catm,
                       b0 = x$xparms$b0, b1 = x$xparms$b1,
                       O2 = x$xparms$O2)


    }    
  }
    fittd
}


## This function will implement simple calculations
## of predicted and residuals for the Opc4photo function

## summary.Opc3photo <- function(object,...){

##   dat <- object$data
##   obsvec <- as.vector(dat[,1])
  
##   fittd <- c4photo(dat[,2],dat[,3],dat[,4],object$bestVmax,object$bestAlpha)
## ## Warning here I'm not taking into account different values of Rd, kparm, theta and beta
  
##   rsd <- obsvec - fittd$Assim
##   rss <- object$ReSumS
##   ## Some measures of agreement
##   ## Index of agreement
##   IAN <- t(rsd)%*%rsd
##   IAD1 <- abs(rsd) + abs(scale(obsvec,scale=FALSE))
##   IAD <- t(IAD1)%*%IAD1
##   IA <- 1 - IAN/IAD
##   ## Rsquared 1
##   Rsq1 <- as.numeric(1 - rss / t(obsvec)%*%obsvec)
##   ## Rsquared 2
##   Rsq2 <- as.numeric(cor(fittd$Assim,obsvec)^2)
##   ## Mean bias
##   meanBias <- mean(rsd)
##   ## AIC and BIC
##   n1 <- length(rsd)
##   AIC <- n1 * log(rss/n1) + 2
##   BIC <- n1 * log(rss/n1) + 2 * log(n1)

##   sigma <- sqrt(rss/(n1-2))
##   stdresid <- rsd/sigma
##   outli <- which(abs(stdresid) > 2)
  
##   structure(list(fitted=fittd$Assim,resid=rsd,
##                  stdresid=stdresid,
##                  IA=IA,Rsq1=Rsq1,Rsq2=Rsq2,
##                  meanBias=meanBias,
##                  AIC=AIC,BIC=BIC,
##                  outli=outli,
##        sigma=sigma),class="summary.Opc4photo")
## }

## print.summary.Opc4photo <- function(x,...){

##   cat("\n Diagnostic measures\n")
##   cat("\n Index of Agreement:",x$IA)
##   cat("\n Rsquared 1:",x$Rsq1)
##   cat("\n Rsquared 2:",x$Rsq2)
##   cat("\n Mean Bias:",x$meanBias)
##   cat("\n AIC:",x$AIC)
##   cat("\n BIC:",x$BIC,"\n")
## }

##' @export
##' @S3method plot Opc3photo
plot.Opc3photo <- function(x,plot.kind=c("RvsF","OvsF","OandF"),resid=c("std","raw"),...){

  dat <- x$data
  obsvec <- as.vector(dat[,1])
  plot.kind <- match.arg(plot.kind)
  
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
      plot1 <- lattice::xyplot(stdresid ~ fttA,...,
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
         plot1 <-   lattice::xyplot(rsd ~ fttA,...,
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
  if(plot.kind == "OvsF"){
    plot1 <- lattice::xyplot(obsvec ~ fttA,...,
                    xlab="fitted",
                    ylab="observed",
                    panel = function(x,y,...){
                      panel.xyplot(x,y,...)
                      panel.abline(0,1,...)
                    })
    print(plot1)
  }
  if(plot.kind == "OandF"){
    if(x$curve.kind == "Q"){
      plot1 <- lattice::xyplot(obsvec + fttA ~ dat[,2],...,
                      auto.key=TRUE,
                      ylab = "CO2 uptake",
                      xlab = "Quantum flux")
      print(plot1)
    }else{
      plot1 <- lattice::xyplot(obsvec ~ dat[,5],...,
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
}

## Wrapper function for multiple A/Ci sets
## Wrapper function for multiple A/Ci sets
##' Multiple optimization of assimilation (or stomatal conductance) curves.
##'
##' It is a wrapper for Opc3photo which allows for optimization of multiple
##' runs of curves (A/Q or A/Ci).
##'
##' Include more details about the data.
##'
##' @param data should be a \code{data.frame} or \code{matrix} with x columns
##'
##' col 1: should be an ID for the different runs col 2: measured assimilation
##' (CO2 uptake) col 3: Incomming PAR (photosynthetic active radiation) col 4:
##' Leaf temperature col 5: Relative humidity col 6: Intercellular CO2 (for
##' A/Ci curves) col 7: Reference CO2 level
##' @param ID optional argument to include ids. should be of length equal to
##' the number of runs.
##' @param iVcmax Single value or vector of length equal to number of runs to
##' supply starting values for the optimization of \code{vcmax}.
##' @param iJmax Single value or vector of length equal to number of runs to
##' supply starting values for the optimization of \code{jmax}.
##' @param iRd Single value or vector of length equal to number of runs to
##' supply starting values for the optimization of \code{Rd}.
##' @param op.level Level 1 will optimize \code{Vcmax} and \code{Jmax} and
##' level 2 will optimize \code{Vcmax}, \code{Jmax} and \code{Rd}.
##' @param curve.kind Whether an A/Ci curve is being optimized or an A/Q curve.
##' @param verbose Whether to print information about progress.
##' @param \dots Additional arguments to be passed to \code{\link{Opc3photo}}
##' @export
##' @return an object of class 'mOpc3photo'
##'
##' if op.level equals 1 best Vcmax, Jmax and convergence
##'
##' if op.level equals 2 best Vcmax, Jmax, Rd and convergence

##' @author Fernando E. Miguez
##' @seealso See also \code{\link{Opc3photo}} 
##' @keywords optimize
##' @examples
##'
##' data(simAssim)
##' simAssim <- cbind(simAssim[,1:6],Catm=simAssim[,10])
##' simAssim <- simAssim[simAssim[,1] < 11,]
##'
##' plotAC(simAssim, trt.col=1)
##'
##' op.all <- mOpc3photo(simAssim, op.level=1,
##'                       verbose=TRUE)
##'
##' plot(op.all)
##' plot(op.all, parm='jmax')
##'
##'
##'
##'
mOpc3photo <- function(data,ID=NULL,iVcmax=100,iJmax=180,iRd=1.1,
                       op.level=1, curve.kind=c("Ci","Q"),verbose=FALSE,...){

  curve.kind <- match.arg(curve.kind)
  uruns <- unique(data[,1])
  nruns <- length(unique(data[,1]))
  
  if(curve.kind == "Q"){
    if(ncol(data) < 5)
      stop("data should have at least 5 columns")
  }else{
    if(ncol(data) != 7)
      stop("data should have 7 columns")
  }
  
  if(length(iVcmax) == 1){
    miVcmax <- rep(iVcmax,nruns)
  }else{
    if(length(iVcmax) != nruns)
      stop("length of iVcmax should be either 1 or equal to the number or runs") 
    miVcmax <- iVcmax
  }

  if(length(iJmax) == 1){
    miJmax <- rep(iJmax,nruns)
  }else{
    if(length(iJmax) != nruns)
      stop("length of iJmax should be either 1 or equal to the number or runs")
    miJmax <- iJmax
  }

  if(length(iRd) == 1){
    miRd <- rep(iRd,nruns)
  }else{
    if(length(iRd) != nruns)
      stop("length of iRd should be either 1 or equal to the number or runs")
    miRd <- iRd
  }
  
  mat <- matrix(ncol=I(3+op.level),nrow=nruns)
  ciVcmax <- matrix(ncol=3,nrow=nruns)
  ciJmax <- matrix(ncol=3,nrow=nruns)
  ciRd <- matrix(ncol=3,nrow=nruns)
  
  for(i in seq_len(nruns)){

    if(op.level == 1){
    
      op <- try(Opc3photo(data[data[,1] == uruns[i],2:6],Catm=data[data[,1] == uruns[i],7],
                        ivcmax=miVcmax[i],ijmax=miJmax[i],iRd=miRd[i],
                        curve.kind=curve.kind, op.level=op.level,...),TRUE)

      if(class(op) == "try-error"){
        mat[i,1:4] <- c(i,rep(NA,2),1)
      }else{
        mat[i,1:4] <- c(i,op$bestVmax,op$bestJmax,op$Convergence)
        ciVcmax[i,1:3] <- c(i,op$ciVmax)
        ciJmax[i,1:3] <- c(i,op$ciJmax)
      }

      if(verbose){
        cat("Run:",i,"... Converged",ifelse(mat[i,4]==0,"YES","NO"),"\n")
      }
    }else
    if(op.level == 2){
    
      op <- try(Opc3photo(data[data[,1] == uruns[i],2:6],Catm=data[data[,1] == uruns[i],7],
                        ivcmax=miVcmax[i],ijmax=miJmax[i],iRd=miRd[i],
                        curve.kind=curve.kind, op.level=op.level,...),TRUE)

      if(class(op) == "try-error"){
        mat[i,1:5] <- c(i,rep(NA,3),1)
      }else{
        mat[i,1:5] <- c(i,op$bestVmax,op$bestJmax,op$bestRd,op$Convergence)
        ciVcmax[i,1:3] <- c(i,op$ciVmax)
        ciJmax[i,1:3] <- c(i,op$ciJmax)
        ciRd[i,1:3] <- c(i,op$ciRd)
      }

      if(verbose){
        cat("Run:",i,"... Converged",ifelse(mat[i,5]==0,"YES","NO"),"\n")
      }
    }
  }
  if(op.level == 1){
    colnames(mat) <- c("ID","Vcmax","Jmax","Conv")
  }else{
    colnames(mat) <- c("ID","Vcmax","Jmax","Rd","Conv")
  }

  if(!missing(ID)){
    if(length(ID) != nruns)
      stop("Length of ID should be equal to the number of runs")

    mat <- as.data.frame(mat)
    mat$ID <- ID
  }

  ans <- structure(list(mat=mat, op.level=op.level,
                        ciVcmax=ciVcmax, ciJmax=ciJmax,
                        ciRd=ciRd,
                        curve.kind=curve.kind), class = "mOpc3photo")
  ans
  
}


## Printing method
print.mOpc3photo <- function(x,...){

  ncolm <- ncol(unclass(x)$mat)
  ma <- x$mat
  cat("Number of runs:",nrow(ma),"\n")
  cat("Number converged:",length(ma[ma[,ncolm] == 0,ncolm]),"\n\n")

  if(x$op.level == 1){
    mat <- matrix(ncol=3,nrow=2)
    if(x$curve.kind == "Ci"){
      dimnames(mat) <- list(c("vmax","jmax"),c("mean","min","max"))
    }else{
      dimnames(mat) <- list(c("vmax","jmax"),c("mean","min","max"))
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
    if(x$curve.kind == "Ci"){
      dimnames(mat) <- list(c("vmax","jmax","Rd"),c("mean","min","max"))
    }else{
      dimnames(mat) <- list(c("vmax","jmax","Rd"),c("mean","min","max"))
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
  }
}



## Plotting method
##' Plotting method
##' @export
##' @S3method plot mOpc3photo
plot.mOpc3photo <- function(x, parm = c("vcmax","jmax"), ...){

  parm <- match.arg(parm)
  res <- x

  if(parm == "vcmax"){
    civmax <- x$ciVcmax
    id <- factor(res$mat[,1])
    ymax <- max(civmax[,3],na.rm=TRUE) * 1.05
    ymin <- min(civmax[,2],na.rm=TRUE) * 0.95
    lattice::xyplot(civmax[,3] ~ id, ylim = c(ymin, ymax),
           ylab = "Vcmax",
           xlab = "ID",
           panel = function(x,y,...){
             panel.xyplot(x,y,pch="-",cex=3,...)
             panel.xyplot(id,res$mat[,2],pch=1,cex=1.5,...)
             panel.xyplot(id,civmax[,2],pch="-",cex=3,...)
           }
           )
  }else
  if(parm == "jmax"){
    cijmax <- x$ciJmax
    id <- factor(res$mat[,1])
    ymax <- max(cijmax[,3],na.rm=TRUE) * 1.05
    ymin <- min(cijmax[,2],na.rm=TRUE) * 0.95
    lattice::xyplot(cijmax[,3] ~ id, ylim = c(ymin, ymax),
           ylab = "jmax",
           xlab = "ID",
           panel = function(x,y,...){
             panel.xyplot(x,y,pch="-",cex=3,...)
             panel.xyplot(id,res$mat[,3],pch=1,cex=1.5,...)
             panel.xyplot(id,cijmax[,2],pch="-",cex=3,...)
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
