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
##' not coupled and although, for example, a lower relative humidity will lower
##' stomatal conductance, it will not affect assimilation.  Hopefully, this will
##' be improved in the future.
##'
##' The equations are taken from von Caemmerer (2000) for the assimilation part
##' and stomatal conductance is based on FORTRAN code by Joe Berry (translated
##' to C).
##'
##' @param Qp Quantum flux (\eqn{\mu}mol \eqn{\mathrm{m}^{-2}}{m-2}
##' \eqn{\mathrm{s}^{-1}}{s-1}).
##' @param airtemp Air temperature (Celsius).
##' @param rh Relative humidity in proportion (e.g. 0.7).
##' @param ca Atmospheric carbon dioxide concentration (ppm or
##' \eqn{\mu}bar) (e.g. 380).
##' @param oa Atmospheric oxygen concentration (mbar) (e.g. 210).
##' @param vcmax Maximum rubisco activity (\eqn{\mu}mol
##' \eqn{\mathrm{m}^{-2}}{m-2} \eqn{\mathrm{s}^{-1}}{s-1}).
##' @param vpmax Maximum PEP carboxylase activity (\eqn{\mu}mol
##' \eqn{\mathrm{m}^{-2}}{m-2} \eqn{\mathrm{s}^{-1}}{s-1}).
##' @param vpr PEP regeneration rate (\eqn{\mu}mol \eqn{\mathrm{m}^{-2}}{m-2}
##' \eqn{\mathrm{s}^{-1}}{s-1}).
##' @param jmax Maximal electron transport rate (\eqn{\mu}mol electrons
##' \eqn{\mathrm{m}^{-2}}{m-2} \eqn{\mathrm{s}^{-1}}{s-1}).
##' @export
##' @return Results of call to C function eC4photo_sym,
##' a \code{\link{list}} structure with components
##'
##' \item{Assim}{Net assimilation rate (\eqn{\mu}mol
##' \eqn{\mathrm{m}^{-2}}{m-2} \eqn{\mathrm{s}^{-1}}{s-1}).}
##' \item{Gs}{Stomatal conductance rate (\eqn{\mu}mol
##' \eqn{\mathrm{m}^{-2}}{m-2} \eqn{\mathrm{s}^{-1}}{s-1}).}
##' \item{Ci}{CO2 concentration in the bundle-sheath
##' (\eqn{\mu}bar).}
##' \item{Os}{Oxygen evolution (mbar).}
##'
##' @references Susanne von Caemmerer. 2000.  \emph{Biochemical Models of Leaf
##' Photosynthesis.} CSIRO Publishing. (In particular chapter 4).
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
##' \code{\link{eC4photo}}, canopy evapo/transpiration, and the multilayer
##' canopy model \code{\link{sunML}}.
##'
##'
##' @param LAI Leaf area index.
##' @param doy Day of the year (1--365).
##' @param hour Hour of the day (0--23).
##' @param solarR Solar radiation (\eqn{\mu\mathrm{mol} \; \mathrm{m}^{-2} \; \mathrm{s}^{-1}}{\mu mol
##' /m2/s}).
##' @param AirTemp Temperature (Celsius).
##' @param RH Relative humidity (0--1).
##' @param WindS Wind speed (\eqn{\mathrm{m} \; \mathrm{s}^{-1}}{m/s}).
##' @param Vcmax Maximum rubisco activity (\eqn{\mu\mathrm{mol} \; \mathrm{m}^{-2} \;
##' \mathrm{s}^{-1}}{\mu mol /m2/s}).
##' @param Vpmax Maximum PEP carboxylase activity (\eqn{\mu\mathrm{mol} \; \mathrm{m}^{-2} \;
##' \mathrm{s}^{-1}}{\mu mol /m2/s}).
##' @param Vpr PEP regeneration rate (\eqn{\mu\mathrm{mol} \; \mathrm{m}^{-2} \;
##' \mathrm{s}^{-1}}{\mu mol /m2/s}).
##' @param Jmax Maximal electron transport rate (\eqn{\mu}mol electrons
##' \eqn{\mathrm{m}^{-2}}{m-2} \eqn{\mathrm{s}^{-1}}{s-1}).
##' @param Ca Atmospheric carbon dioxide concentration (ppm or
##' \eqn{\mu}bar) (e.g. 380).
##' @param Oa Atmospheric oxygen concentration (mbar) (e.g. 210).
##' @param StomataWS Effect of water stress on assimilation.
##' @export
##' @return
##'
##' \code{\link{numeric}}
##'
##' Returns a single value which is hourly canopy assimilation (mol
##' \eqn{\mathrm{m}^{-2}}{m-2} ground \eqn{\mathrm{hr}^{-1}}{hr-1})
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

