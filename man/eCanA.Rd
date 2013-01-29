\name{eCanA}
\alias{eCanA}
\title{Simulates canopy assimilation (von Caemmerer model)}
\usage{
  eCanA(LAI, doy, hour, solarR, AirTemp, RH, WindS, Vcmax,
    Vpmax, Vpr, Jmax, Ca = 380, Oa = 210, StomataWS = 1)
}
\arguments{
  \item{LAI}{leaf area index.}

  \item{doy}{day of the year, (1--365).}

  \item{hour}{hour of the day, (0--23).}

  \item{solarR}{solar radiation (\eqn{\mu mol \; m^{-2} \;
  s^{-1}}{micro mol /m2/s}).}

  \item{AirTemp}{temperature (Celsius).}

  \item{RH}{relative humidity (0--1).}

  \item{WindS}{wind speed (\eqn{m \; s^{-1}}{m/s}).}

  \item{Vcmax}{Maximum rubisco activity (\eqn{\mu mol \;
  m^{-2} \; s^{-1}}{micro mol /m2/s}).}

  \item{Vpmax}{Maximum PEP carboxylase activity (\eqn{\mu
  mol \; m^{-2} \; s^{-1}}{micro mol /m2/s}).}

  \item{Vpr}{PEP regeneration rate (\eqn{\mu mol \; m^{-2}
  \; s^{-1}}{micro mol /m2/s}).}

  \item{Jmax}{Maximal electron transport rate
  (\eqn{\mu}{micro}mol electrons \eqn{m^{-2}}{m-2}
  \eqn{s^{-1}}{s-1}).}

  \item{Ca}{atmospheric carbon dioxide concentration (ppm
  or \eqn{\mu}{micro}bar) (e.g. 380).}

  \item{Oa}{atmospheric oxygen concentration (mbar) (e.g.
  210).}

  \item{StomataWS}{Effect of water stress on assimilation.}
}
\value{
  \code{\link{numeric}}

  returns a single value which is hourly canopy
  assimilation (mol \eqn{m^{-2}}{m-2} ground
  \eqn{hr^{-1}}{s-1})
}
\description{
  It represents an integration of the photosynthesis
  function \code{\link{eC4photo}}, canopy
  evapo/transpiration and the multilayer canopy model
  \code{\link{sunML}}.
}
\examples{
data(doy124)
tmp1 <- numeric(24)
for(i in 1:24){
  lai <- doy124[i,1]
  doy <- doy124[i,3]
  hr  <- doy124[i,4]
  solar <- doy124[i,5]
  temp <- doy124[i,6]
  rh <- doy124[i,7]/100
  ws <- doy124[i,8]

  tmp1[i] <- CanA(lai,doy,hr,solar,temp,rh,ws)
}

plot(c(0:23),tmp1,
     type="l",lwd=2,
     xlab="Hour",
     ylab=expression(paste("Canopy assimilation (mol  ",
     m^-2," ",s^-1,")")))
}
\keyword{models}

