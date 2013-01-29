\name{RsqEC4photo}
\alias{RsqEC4photo}
\title{R-squared for C4 photosynthesis simulation (von Caemmerer model)}
\usage{
  RsqEC4photo(obsDat, iVcmax = 60, iVpmax = 120, iVpr = 80,
    iJmax = 400, co2 = 380, o2 = 210,
    type = c("Assim", "StomCond"))
}
\arguments{
  \item{obsDat}{observed assimilation data, which should be
  a data frame or matrix.  The first column should be
  observed net assimilation rate (\eqn{\mu} mol
  \eqn{m^{-2}} \eqn{s^{-1}}).  The second column should be
  the observed quantum flux (\eqn{\mu} mol \eqn{m^{-2}}
  \eqn{s^{-1}}).  The third column should be observed
  temperature of the leaf (Celsius).  The fourth column
  should be the observed relative humidity in proportion
  (e.g. 0.7).}

  \item{iVcmax}{Maximum rubisco activity (\eqn{\mu} mol
  \eqn{m^{-2}} \eqn{s^{-1}}).}

  \item{iVpmax}{Maximum PEP carboxylase activity (\eqn{\mu}
  mol \eqn{m^{-2}} \eqn{s^{-1}}).}

  \item{iVpr}{PEP regeneration rate (\eqn{\mu} mol
  \eqn{m^{-2}} \eqn{s^{-1}}).}

  \item{iJmax}{Maximal electron transport rate
  (\eqn{\mu}mol electrons \eqn{m^{-2}} \eqn{s^{-1}}).}

  \item{co2}{atmospheric carbon dioxide concentration (ppm
  or \eqn{\mu}bar) (default = 380).}

  \item{o2}{atmospheric oxygen concentration (mbar)
  (default = 210).}

  \item{type}{Use \code{'Assim'} if you want an \eqn{R^2}
  for assimilation data and use \code{'StomCond'} if you
  want an \eqn{R^2} for stomatal conductance data.}
}
\value{
  a \code{\link{numeric}} object

  It simply returns the \eqn{R^2} value for the given data
  and coefficients.
}
\description{
  This is an auxiliary function which is made available in
  case it is useful. It calculates the R-squared based on
  observed assimilation (or stomatal conductance) data and
  coefficients for the von Caemmerer C4 photosynthesis
  model. The only coefficients being considered are Vcmax,
  Vpmax, Vpr and Jmax.
}
\examples{
data(obsNaid)
obs <- obsNaid
## These data are from Naidu et al. (2003)
## in the correct format
res <- RsqEC4photo(obs)
## Other example using Beale, Bint and Long (1996)
data(obsBea)
obsD <- obsBea
resB <- RsqEC4photo(obsD)
}
\keyword{univar}

