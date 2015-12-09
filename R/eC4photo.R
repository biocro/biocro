##  BioCro/R/eC4photo.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2008

## Start of the functions related to the c4
## photosynthesis model proposed by von Caemmerer


## This file will contain the functions in BioCro for
## and alternative photosynthesis model proposed by
## von Caemmerer

## This is the C4photo function based on von Caemmerer (2000)
##' C4 photosynthesis simulation (von Caemmerer model)
##'
##'
##' Simulation of C4 photosynthesis based on the equations proposed by von
##' Caemmerer (2000).  At this point assimilation and stomatal conductance are
##' not coupled and although, for example a lower relative humidity will lower
##' stomatal conductance it will not affect assimilation.  Hopefully, this will
##' be improved in the future.
##'
##' The equations are taken from von Caemmerer (2000) for the assimilation part
##' and stomatal conductance is based on FORTRAN code by Joe Berry (translated
##' to C).
##'
##' @param Qp quantum flux (\eqn{\mu}{micro} mol \eqn{m^{-2}}{m-2}
##' \eqn{s^{-1}}{s-1}).
##' @param airtemp air temperature (Celsius).
##' @param rh relative humidity in proportion (e.g. 0.7).
##' @param ca atmospheric carbon dioxide concentration (ppm or
##' \eqn{\mu}{micro}bar) (e.g. 380).
##' @param oa atmospheric oxygen concentration (mbar) (e.g. 210).
##' @param vcmax Maximum rubisco activity (\eqn{\mu}{micro} mol
##' \eqn{m^{-2}}{m-2} \eqn{s^{-1}}{s-1}).
##' @param vpmax Maximum PEP carboxylase activity (\eqn{\mu}{micro} mol
##' \eqn{m^{-2}}{m-2} \eqn{s^{-1}}{s-1}).
##' @param vpr PEP regeneration rate (\eqn{\mu}{micro} mol \eqn{m^{-2}}{m-2}
##' \eqn{s^{-1}}{s-1}).
##' @param jmax Maximal electron transport rate (\eqn{\mu}{micro}mol electrons
##' \eqn{m^{-2}}{m-2} \eqn{s^{-1}}{s-1}).
##' @export
##' @return results of call to C function eC4photo_sym
##'
##' a \code{\link{list}} structure with components
##' \itemize{
##' \item Assim net assimilation rate (\eqn{\mu}{micro} mol
##' \eqn{m^{-2}}{m-2} \eqn{s^{-1}}{s-1}).
##' \item Gs stomatal conductance rate (\eqn{\mu}{micro} mol
##' \eqn{m^{-2}}{m-2} \eqn{s^{-1}}{s-1}).
##' \item Ci CO2 concentration in the bundle-sheath
##' (\eqn{\mu}{micro}bar).
##' \item Os oxygen evolution (mbar).
##' }
##' @references Susanne von Caemmerer (2000) Biochemical Models of Leaf
##' Photosynthesis. CSIRO Publishing. (In particular chapter 4).
##' @keywords models
##' @examples
##'
##' \dontrun{
##' ## A simple example for the use of eC4photo
##' ## This is the model based on von Caemmerer
##' ## First we can compare the effect of varying
##' ## Jmax. Notice that this is different from
##' ## varying alpha in the Collatz model
##'
##' Qps <- seq(0,2000,10)
##' Tls <- seq(0,55,5)
##' rhs <- c(0.7)
##' dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs))
##' res1 <- eC4photo(dat1$Qp,dat1$Tl,dat1$RH)
##' res2 <- eC4photo(dat1$Qp,dat1$Tl,dat1$RH,jmax=700)
##'
##' ## Plot comparing Jmax 400 vs. 700 for a range of conditions
##' lattice::xyplot(res1$Assim + res2$Assim ~ Qp | factor(Tl) , data = dat1,
##'             type='l',col=c('blue','green'),lwd=2,
##'             ylab=expression(paste('Assimilation (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'              xlab=expression(paste('Quantum flux (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'             key=list(text=list(c('Jmax 400','Jmax 700')),
##'               lines=TRUE,col=c('blue','green'),lwd=2))
##'
##' ## Second example is the effect of varying Vcmax
##'
##' Qps <- seq(0,2000,10)
##' Tls <- seq(0,35,5)
##' rhs <- 0.7
##' vcmax <- seq(0,40,5)
##' dat1 <- data.frame(expand.grid(Qp=Qps,Tl=Tls,RH=rhs,vcmax=vcmax))
##' res1 <- numeric(nrow(dat1))
##' for(i in 1:nrow(dat1)){
##' res1[i] <- eC4photo(dat1$Qp[i],dat1$Tl[i],dat1$RH[i],vcmax=dat1$vcmax[i])$Assim
##' }
##'
##' ## Plot comparing different Vcmax
##' cols <- rev(heat.colors(9))
##' lattice::xyplot(res1 ~ Qp | factor(Tl) , data = dat1,col=cols,
##'             groups=vcmax,
##'             type='l',lwd=2,
##'             ylab=expression(paste('Assimilation (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'              xlab=expression(paste('Quantum flux (',
##'                  mu,mol,' ',m^-2,' ',s^-1,')')),
##'             key=list(text=list(as.character(vcmax)),
##'               lines=TRUE,col=cols,lwd=2))
##'
##'
##' }
##'
eC4photo <- function(Qp,airtemp,rh,ca=380,oa=210,vcmax=60,
                     vpmax=120,vpr=80,jmax=400){
  nqp = length(Qp); nat =length(airtemp);
  nrh = length(rh);
  if((nqp != nat) || (nat != nrh)){
    stop("Qp, airtemp and rh sould be of equal length")
  }
  if(length(c(ca,oa,vcmax,vpmax,vpr,jmax)) != 6){
    stop("ca, oa, vcmax, vpmax, vpr, jmax should all be of length 1")
  }

  res <- .Call(eC4photo_sym,as.double(Qp),as.double(airtemp),
               as.double(rh),as.double(ca),as.double(oa),
               as.double(vcmax),as.double(vpmax),
               as.double(vpr),as.double(jmax))
  res
}

## Here I will add the Canopy Assimilation C routine
##' Simulates canopy assimilation (von Caemmerer model)
##'
##' It represents an integration of the photosynthesis function
##' \code{\link{eC4photo}}, canopy evapo/transpiration and the multilayer
##' canopy model \code{\link{sunML}}.
##'
##'
##' @param LAI leaf area index.
##' @param doy day of the year, (1--365).
##' @param hour hour of the day, (0--23).
##' @param solarR solar radiation (\eqn{\mu mol \; m^{-2} \; s^{-1}}{micro mol
##' /m2/s}).
##' @param AirTemp temperature (Celsius).
##' @param RH relative humidity (0--1).
##' @param WindS wind speed (\eqn{m \; s^{-1}}{m/s}).
##' @param Vcmax Maximum rubisco activity (\eqn{\mu mol \; m^{-2} \;
##' s^{-1}}{micro mol /m2/s}).
##' @param Vpmax Maximum PEP carboxylase activity (\eqn{\mu mol \; m^{-2} \;
##' s^{-1}}{micro mol /m2/s}).
##' @param Vpr PEP regeneration rate (\eqn{\mu mol \; m^{-2} \; s^{-1}}{micro
##' mol /m2/s}).
##' @param Jmax Maximal electron transport rate (\eqn{\mu}{micro}mol electrons
##' \eqn{m^{-2}}{m-2} \eqn{s^{-1}}{s-1}).
##' @param Ca atmospheric carbon dioxide concentration (ppm or
##' \eqn{\mu}{micro}bar) (e.g. 380).
##' @param Oa atmospheric oxygen concentration (mbar) (e.g. 210).
##' @param StomataWS Effect of water stress on assimilation.
##' @export
##' @return
##'
##' \code{\link{numeric}}
##'
##' returns a single value which is hourly canopy assimilation (mol
##' \eqn{m^{-2}}{m-2} ground \eqn{hr^{-1}}{s-1})
##' @keywords models
##' @examples
##'
##' \dontrun{
##' data(doy124)
##' tmp1 <- numeric(24)
##' for(i in 1:24){
##'   lai <- doy124[i,1]
##'   doy <- doy124[i,3]
##'   hr  <- doy124[i,4]
##'   solar <- doy124[i,5]
##'   temp <- doy124[i,6]
##'   rh <- doy124[i,7]/100
##'   ws <- doy124[i,8]
##'
##'   tmp1[i] <- CanA(lai,doy,hr,solar,temp,rh,ws)
##' }
##'
##' plot(c(0:23),tmp1,
##'      type='l',lwd=2,
##'      xlab='Hour',
##'      ylab=expression(paste('Canopy assimilation (mol  ',
##'      m^-2,' ',s^-1,')')))
##' }
##'

eCanA <- function(LAI,doy,hour,solarR,AirTemp,RH,WindS,
                  Vcmax,Vpmax,Vpr,Jmax,Ca=380,Oa=210,StomataWS=1){
  
  inputs <- c(LAI,doy,hour,solarR,AirTemp,RH,WindS,
            WindS,Vcmax,Vpmax,Vpr,Jmax)
  if(length(inputs) != 12)
    stop("The inputs should all be of length 1")
  
  res <- .Call(eCanA_sym,as.double(LAI),as.integer(doy),as.integer(hour),
               as.double(solarR),as.double(AirTemp),as.double(RH),
               as.double(WindS),as.double(Ca),as.double(Oa),
               as.double(Vcmax),as.double(Vpmax),
               as.double(Vpr),as.double(Jmax),as.double(StomataWS))
  res
}
##' Markov chain Monte Carlo for C4 photosynthesis parameters
##'
##'
##' This function attempts to implement Markov chain Monte Carlo methods for
##' models with no likelihoods. In this case it is done for the von Caemmerer
##' C4 photosynthesis model.  The method implemented is based on a paper by
##' Marjoram et al. (2003).  The method is described in Miguez (2007). The
##' chain is constructed using a Gaussian random walk. This is definitely a
##' beta version of this function and more testing and improvements are needed.
##' The value of this function is in the possibility of examining the empirical
##' posterior distribution (i.e. vectors) of the Vcmax and alpha parameters.
##' Notice that you will get different results each time you run it.
##'
##'
##' @param obsDat observed assimilation data, which should be a data frame or
##' matrix.  The first column should be observed net assimilation rate
##' (\eqn{\mu} mol \eqn{m^{-2}} \eqn{s^{-1}}).  The second column should be the
##' observed quantum flux (\eqn{\mu} mol \eqn{m^{-2}} \eqn{s^{-1}}).  The third
##' column should be observed temperature of the leaf (Celsius).  The fourth
##' column should be the observed relative humidity in proportion (e.g. 0.7).
##' @param niter number of iterations to run the chain for (default = 30000).
##' @param iCa CO2 atmospheric concentration (ppm or \eqn{\mu}bar).
##' @param iOa O2 atmospheric concentration (mbar).
##' @param iVcmax initial value for Vcmax (default = 60).
##' @param iVpmax initial value for Vpmax (default = 120).
##' @param iVpr initial value for Vpr (default = 80).
##' @param iJmax initial value for Jmax (default = 400).
##' @param thresh this is a threshold that determines the ``convergence'' of
##' the initial burn-in period. The convergence of the whole chain can be
##' evaluated by running the model with different starting values for Vcmax and
##' alpha. The chain should convergence, but for this, runs with similar
##' thresholds should be compared. This threshold reflects the fact that for
##' any given data the model will not be able to simulate it perfectly so it
##' represents a compromise between computability and fit.
##' @param scale This scale parameter controls the size of the standard
##' deviations which generate the moves in the chain. Decrease it to increase
##' the acceptance rate and viceversa.
##' @export
##' @return a \code{\link{list}} structure with components
##' \itemize{
##' \item RsqBI This is the \eqn{R^2} for the ``burn-in'' period.  This
##' value becomes the cut off value for the acceptance in the chain.
##' \item CoefBI parameter estimates after the burn-in period.  These are
##' not optimal as in the case of the optimization routine but are starting
##' values for the chain.
##' \item accept1 number of iterations for the initial burn-in period.
##' \item resuBI matrix of dimensions 5 by \code{accept1} containing the
##' values for Vcmax and alpha and the \eqn{R^2} in each iteration of the
##' burn-in period.
##' \item resuMC matrix of dimensions 5 by \code{accept2} containing the
##' values for Vcmax and alpha and the \eqn{R^2} in each iteration of the chain
##' period.
##' \item accept2 number of accepted samples or length of the chain.
##' \item accept3 number of accepted moves in the chain.
##' }
##' @references P. Marjoram, J. Molitor, V. Plagnol, S. Tavare, Markov chain
##' monte carlo without likelihoods, PNAS 100 (26) (2003) 15324--15328.
##'
##' Miguez (2007) Miscanthus x giganteus production: meta-analysis, field study
##' and mathematical modeling. PhD Thesis. University of Illinois at
##' Urbana-Champaign.
##' @keywords optimize
##' @examples
##'
##' \dontrun{
##' ## This is an example for the MCMCEc4photo
##' ## evaluating the convergence of the chain
##' ## Notice that if a parameter does not seem
##' ## to converge this does not mean that the method
##' ## doesn't work. Careful examination is needed
##' ## in order to evaluate the validity of the results
##' data(obsNaid)
##' res1 <- MCMCEc4photo(obsNaid,100000,thresh=0.007)
##' res1
##'
##' ## Run it a few more times
##' ## and test the stability of the method
##' res2 <- MCMCEc4photo(obsNaid,100000,thresh=0.007)
##' res3 <- MCMCEc4photo(obsNaid,100000,thresh=0.007)
##'
##' ## Now plot it
##' plot(res1,res2,res3)
##' plot(res1,res2,res3,type='density')
##' }
##'
##'

MCMCEc4photo <- function(obsDat, niter = 30000, iCa=380, iOa=210,
                        iVcmax = 60, iVpmax=120,
                        iVpr=80, iJmax=400, thresh=0.01, scale = 1){
  assim <- obsDat[,1]
  qp <- obsDat[,2]
  temp <- obsDat[,3]
  rh <- obsDat[,4]

if(iVpr != 80)
  warning("\n Vpr is not optimized at the moment \n")
  
  res <- .Call(McMCEc4photo,as.double(assim),
               as.double(qp),as.double(temp),
               as.double(rh),as.integer(niter),
               as.double(iCa),as.double(iOa),
               as.double(iVcmax),as.double(iVpmax),
               as.double(iJmax), as.double(thresh),
               as.double(scale))
  res$resuBI <- res$resuBI[,1:res$accept1]
  res$resuMC <- res$resuMC[,1:res$accept2]
  res$niter <- niter
  if(I(res$accept2/res$niter) < 0.05)
    stop("\nRun the chain again. Try increasing thresh.\n")
  structure(res , class ="MCMCEc4photo")
}
##' Print an MCMCEc4photo object
##'
##' This functions doesn't just print the object components, but it also
##' computes quantiles according to the \code{level} argument below and a
##' correlation matrix as well as a summary for each parameter.
##'
##' The Highest Posterior Density region is calculated using the
##' \code{\link{quantile}} function.  The correlation matrix is computed using
##' the \code{\link{cor}} function.  The summaries for each parameter are
##' computed using the \code{\link{summary}} function.
##'
##' @param x \code{\link{MCMCEc4photo}} object
##' @param level specified \code{level} for the Highest Posterior Density
##' region.
##' @param \dots Optional arguments
##' @seealso \code{\link{MCMCEc4photo}}
##' @keywords optimize
##' @export
##' @S3method print MCMCEc4photo

print.MCMCEc4photo <- function(x,level=0.95,...){

  ul <- 1 - (1-level)/2
  ll <- (1 - level)/2
  xMat <- t(x$resuMC[1:4,])
  colnames(xMat) <- c("Vcmax","Vpmax","Vpr","Jmax")
  cat("\n Markov chain Monte Carlo for the von Caemmerer c4 photosynthesis model")
  cat("\n Burn in period:")
  cat("\n R-squared:",x$RsqBI)
  cat("\n Coefficients:",x$CoefBI)
  cat("\n Iterations:",x$accept1,"\n")

  cat("\n Markov chain")
  cat("\n Length of the chain:",x$accept2)
  cat("\n Moves:",x$accept3,"Prop:",x$accept3/x$niter,"\n")
  cat("\n Summaries \n  Vcmax:\n")
  print(summary(x$resuMC[1,]),...)
  cat("\n Vpmax:\n")
  print(summary(x$resuMC[2,]),...)
  cat("\n Jmax:\n")
  print(summary(x$resuMC[3,]),...)
  cat("\n HPD region \n Vcmax:\n")
  print(quantile(x$resuMC[1,],c(ll,ul)),...)
  cat("\n Vpmax:\n")
  print(quantile(x$resuMC[2,],c(ll,ul)),...)
  cat("\n Jmax:\n")
  print(quantile(x$resuMC[3,],c(ll,ul)),...)
  cat("\n correlation matrix:\n")
  print(cor(xMat),...)
  cat("\n R-squared range:",range(x$resuMC[4,]),"\n")
  invisible(x)
}
##' Plottin function for MCMCEc4photo objects
##'
##' By default it prints the trace of the four parameters being estimated by
##' the \code{\link{MCMCEc4photo}} function. As an option the density can be
##' plotted.
##'
##' This function uses internally \code{\link[lattice]{xyplot}} and
##' \code{\link[lattice]{densityplot}} both in the 'lattice' package.
##'
##' @param x \code{\link{MCMCEc4photo}} object.
##' @param x2 optional additional \code{link{MCMCEc4photo}} object.
##' @param x3 optional additional \code{link{MCMCEc4photo}} object.
##' @param type 'trace' plots the iteration history and 'density' plots the
##' kernel density.
##' @param \dots Optional arguments.
##' @seealso \code{\link{MCMCEc4photo}}
##' @keywords hplot
##' @export
##' @S3method plot MCMCEc4photo

           
plot.MCMCEc4photo <- function(x,x2=NULL,x3=NULL,
                             type=c("trace","density"),...){

  type <- match.arg(type)
## This first code is to plot the first object only
  ## Ploting the trace
  if(missing(x2) && missing(x3)){
    if(type == "trace"){
    plot1 <-  lattice::xyplot(x$resuMC[1,] ~ 1:x$accept2 ,
                     xlab = "Iterations", type = "l",
                     ylab = expression(paste("Vcmax (",mu,mol," ",m^-2," ",s^-1,")")),
                     ...)
    plot2 <-  lattice::xyplot(x$resuMC[2,] ~ 1:x$accept2 ,
                     xlab = "Iterations", type = "l",
                     ylab = expression(paste("Vpmax (",mu,mol," ",m^-2," ",s^-1,")")),
                     ...)
    plot3 <-  lattice::xyplot(x$resuMC[3,] ~ 1:x$accept2 ,
                     xlab = "Iterations", type = "l",
                     ylab = expression(paste("Jmax (",mu,mol," ",m^-2," ",s^-1,")")),
                     ...)
    print(plot1,position=c(0,0,1,0.333),more=TRUE)
    print(plot2,position=c(0,0.333,1,0.666),more=TRUE)
    print(plot3,position=c(0,0.666,1,1))
  } else
  ## Ploting the density
    if(type == "density"){
      plot1 <-  densityplot(~x$resuMC[1,],xlab="Vcmax",
                            ...)
      plot2 <-  densityplot(~x$resuMC[2,],xlab="Vpmax",
                            ...)
      plot3 <-  densityplot(~x$resuMC[3,],xlab="Jmax",
                            ...)
    print(plot1,position=c(0,0,1,0.333),more=TRUE)
    print(plot2,position=c(0,0.333,1,0.666),more=TRUE)
    print(plot3,position=c(0,0.666,1,1))
    }
  } else
  ## This part of the code is to plot objects x and x2
  ## Ploting the trace
  if(missing(x3)){
    n1 <- x$accept2
    n2 <- x2$accept2
    minchainLength <- min(n1,n2)
    tmpvec11 <- x$resuMC[1,1:minchainLength]
    tmpvec12 <- x2$resuMC[1,1:minchainLength]
    tmpvec21 <- x$resuMC[2,1:minchainLength]
    tmpvec22 <- x2$resuMC[2,1:minchainLength]
    tmpvec31 <- x$resuMC[3,1:minchainLength]
    tmpvec32 <- x2$resuMC[3,1:minchainLength]
    if(type == "trace"){
      plot1 <-  lattice::xyplot(tmpvec11 + tmpvec12 ~ 1:minchainLength ,
                       xlab = "Iterations", type = "l",
                       ylab = expression(paste("Vcmax (",mu,mol," ",m^-2," ",s^-1,")")),
                       ...)
      plot2 <-  lattice::xyplot(tmpvec21 + tmpvec22 ~ 1:minchainLength ,
                       xlab = "Iterations", type = "l",
                       ylab = expression(paste("Vpmax (",mu,mol," ",m^-2," ",s^-1,")")),
                       ...)
      plot3 <-  lattice::xyplot(tmpvec31 + tmpvec32 ~ 1:minchainLength ,
                       xlab = "Iterations", type = "l",
                       ylab = expression(paste("Jmax (",mu,mol," ",m^-2," ",s^-1,")")),
                       ...)
    print(plot1,position=c(0,0,1,0.333),more=TRUE)
    print(plot2,position=c(0,0.333,1,0.666),more=TRUE)
    print(plot3,position=c(0,0.666,1,1))
    } else
  ## ploting the density
    if(type == "density"){
      plot1 <-  densityplot(~ tmpvec11 + tmpvec12, xlab="Vmax",
                             plot.points=FALSE,...)
      plot2 <-  densityplot(~ tmpvec21 + tmpvec22 ,xlab="Vpmax",
                             plot.points=FALSE,...)
      plot3 <-  densityplot(~ tmpvec31 + tmpvec32 ,xlab="Jmax",
                             plot.points=FALSE,...)
    print(plot1,position=c(0,0,1,0.25),more=TRUE)
    print(plot2,position=c(0,0.25,1,0.5),more=TRUE)
    print(plot3,position=c(0,0.75,1,1))
    }
  }else
  {
    n1 <- x$accept2
    n2 <- x2$accept2
    n3 <- x3$accept2
    minchainLength <- min(n1,n2,n3)
    tmpvec11 <- x$resuMC[1,1:minchainLength]
    tmpvec12 <- x2$resuMC[1,1:minchainLength]
    tmpvec13 <- x3$resuMC[1,1:minchainLength]
    tmpvec21 <- x$resuMC[2,1:minchainLength]
    tmpvec22 <- x2$resuMC[2,1:minchainLength]
    tmpvec23 <- x3$resuMC[2,1:minchainLength]
    tmpvec31 <- x$resuMC[3,1:minchainLength]
    tmpvec32 <- x2$resuMC[3,1:minchainLength]
    tmpvec33 <- x3$resuMC[3,1:minchainLength]

  if(type == "trace"){
     plot1 <-  lattice::xyplot(tmpvec11 + tmpvec12 + tmpvec13
                      ~ 1:minchainLength ,
                      xlab = "Iterations", type = "l",
                      ylab = expression(paste("Vmax (",mu,mol," ",m^-2," ",s^-1,")")),
                      ...)
     plot2 <-  lattice::xyplot(tmpvec21 + tmpvec22 + tmpvec23
                      ~ 1:minchainLength ,
                      xlab = "Iterations", type = "l",
                      ylab = expression(paste("Vpmax (",mu,mol," ",m^-2," ",s^-1,")")),
                      ...)
     plot3 <-  lattice::xyplot(tmpvec31 + tmpvec32 + tmpvec33
                      ~ 1:minchainLength ,
                      xlab = "Iterations", type = "l",
                      ylab = expression(paste("Jmax (",mu,mol," ",m^-2," ",s^-1,")")),
                      ...)

     print(plot1,position=c(0,0,1,0.333),more=TRUE)
     print(plot2,position=c(0,0.333,1,0.666),more=TRUE)
     print(plot3,position=c(0,0.666,1,1))

   } else
  if(type == "density"){
    plot1 <-  densityplot(~tmpvec11 + tmpvec12 + tmpvec13
                          ,xlab="Vmax", plot.points=FALSE,...)
    plot2 <-  densityplot(~tmpvec21 + tmpvec22 + tmpvec23
                          ,xlab="Vpmax", plot.points=FALSE,...)
    plot3 <-  densityplot(~tmpvec31 + tmpvec32 + tmpvec33
                          ,xlab="Jmax", plot.points=FALSE,...)

     print(plot1,position=c(0,0,1,0.333),more=TRUE)
     print(plot2,position=c(0,0.333,1,0.666),more=TRUE)
     print(plot3,position=c(0,0.666,1,1))
  }
}
}
