\name{OpEC4photo}
\alias{OpEC4photo}
\alias{summary.OpEC4photo}
\title{Optimization of C4 photosynthesis parameters (von Caemmerer model)}
\usage{
  OpEC4photo(obsDat, iVcmax = 60, iVpmax = 120, iVpr = 80,
    iJmax = 400, co2 = 380, o2 = 210, level = 0.95)
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

  \item{iVcmax}{initial value for Vcmax (default = 60).}

  \item{iVpmax}{initial value for Vpmax (default = 120).}

  \item{iVpr}{initial value for Vpr (default = 80).}

  \item{iJmax}{initial value for Jmax (default = 400).}

  \item{co2}{atmospheric CO2 concentration (ppm), default =
  380.}

  \item{o2}{atmospheric O2 concentration (mmol/mol),
  default = 210.}

  \item{level}{level for the confidence intervals.}
}
\value{
  an object of class \code{\link{OpEC4photo}}. Notice that
  these are the new-style S4 classes.
}
\description{
  Optimization method for the von Caemmerer C4
  photosynthesis model.
}
\examples{
data(obsNaid)
## These data are from Naidu et al. (2003)
## in the correct format
res <- OpEC4photo(obsNaid)
## Other example using Beale, Bint and Long (1996)
data(obsBea)
resB <- OpEC4photo(obsBea)
}
\keyword{optimize}

