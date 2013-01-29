## File containing the OpEC4photo and all it auxiliary functions

OpEC4photo <- function(obsDat,iVcmax=60,iVpmax=120,
                       iVpr=80,iJmax=400,co2=380,o2=210,level=0.95){

  if(iVpr != 80)
    warning("\n iVpr is not optimized at the moment\n")
  
  coef <- c(iVcmax,iVpmax,iJmax)

  obsvec <- as.vector(obsDat[,1])
  SST <- t(obsvec)%*%(obsvec)
  RSS <- function(coefs){

  vec1 <- eC4photo(obsDat[,2],obsDat[,3],obsDat[,4],
                   co2,o2,coefs[1],coefs[2],iVpr,
                   coefs[3])$Assim

  rss <- t(obsvec - vec1)%*%(obsvec - vec1)
  rss
}
  resp <- optim(coef,RSS,method="L-BFGS-B",
                lower=c(5,5,5),
                hessian=TRUE)
  bestParms <- resp$par
  ReSumS <- resp$value
  conv <- resp$convergence
  HessMat <- resp$hessian
  iHess <- solve(HessMat)
  def <- nrow(obsDat)-length(coef)
  sigm <- ReSumS/def
  varcov <- sigm * iHess
  ## Need to add the correlation matrix
  ## Calculating confidence intervals
  alp <- (1 - level)/2
  ## Vcmax
  lcVcmax <- bestParms[1] + qt(alp,def)*sqrt(varcov[1,1])
  ucVcmax <- bestParms[1] + qt(1-alp,def)*sqrt(varcov[1,1])
  ## Vpmax
  lcVpmax <- bestParms[2] + qt(alp,def)*sqrt(varcov[2,2])
  ucVpmax <- bestParms[2] + qt(1-alp,def)*sqrt(varcov[2,2])
  ## Jmax
  lcJmax <- bestParms[3] + qt(alp,def)*sqrt(varcov[3,3])
  ucJmax <- bestParms[3] + qt(1-alp,def)*sqrt(varcov[3,3])
  

  
  structure(list(bestVcmax=bestParms[1],
                 bestVpmax=bestParms[2],
                 bestJmax=bestParms[3],
                 ReSumS=as.numeric(ReSumS),
                 Convergence=conv,
                 VarCov=varcov,df=def,
                 ciVcmax=c(lcVcmax,ucVcmax),
                 ciVpmax=c(lcVpmax,ucVpmax),
                 ciJmax=c(lcJmax,ucJmax),
                 level=level,data=obsDat)
            , class = "OpEC4photo")
}

RsqEC4photo <- function(obsDat, iVcmax=60,iVpmax=120,
                        iVpr=80,iJmax=400,co2=380,o2=210,
                           type=c("Assim","StomCond")){
  coef <- c(iVcmax,iVpmax,iVpr,iJmax)
  type <- match.arg(type)
  if(type == "Assim"){
    if(max(obsDat[,1]) < 1)
      warning("Units of Assim might be wrong:
               should be micro mol m-2 s-1\n")
    vec1 <- eC4photo(obsDat[,2],obsDat[,3],obsDat[,4],
                     co2,o2,coef[1],coef[2],coef[3],
                     coef[4])$Assim
   }
  if(type == "StomCond"){
    if(max(obsDat[,1]) < 1)
      warning("Units of StomCond might be wrong:
               should be mmol m-2 s-1\n")
        vec1 <- eC4photo(obsDat[,2],obsDat[,3],obsDat[,4],
                     co2,o2,coef[1],coef[2],coef[3],
                     coef[4])$Assim
  }
  obsvec <- as.matrix(obsDat[,1])
  SST <- t(obsvec)%*%(obsvec)
  SSE <- t(obsvec - vec1)%*%(obsvec - vec1)
  Rsquare <- 1 - SSE/SST
  if(Rsquare < 0)
    stop("negative Rsq\n")
  Rsquare
}


print.OpEC4photo <- function(x,...){

  cat("\nOptimization of C4 photosynthesis\n")
  cat("\nbest Vcmax:",x$bestVcmax,"\n")
  cat("\n",x$level*100,"% Conf Interval Vcmax")
  cat("\n",x$ciVcmax,"\n")
  
  cat("\nbest Vpmax:",x$bestVpmax,"\n")
  cat("\n",x$level*100,"% Conf Interval Vpmax")
  cat("\n",x$ciVpmax,"\n")

  cat("\nbest Jmax:",x$bestJmax,"\n")
  cat("\n",x$level*100,"% Conf Interval Jmax")
  cat("\n",x$ciJmax,"\n")

  cat("\nConvergence:")
  if(x$Convergence == 0) cat("YES\n")
  else cat("NO\n")

  invisible(x)

}


summary.OpEC4photo <- function(object,...){

  dat <- object$data
  obsvec <- as.vector(dat[,1])
  
    fittd <- eC4photo(dat[,2],dat[,3],dat[,4],ca=380,oa=210,
                      object$bestVcmax,object$bestVpmax,80,object$bestJmax)

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
       sigma=sigma),class="summary.OpEC4photo")
}


## Need to add a plotting method
