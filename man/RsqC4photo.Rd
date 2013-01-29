\name{RsqC4photo}
\alias{RsqC4photo}
\title{R-squared for C4 photosynthesis simulation (Collatz model)}
\usage{
  RsqC4photo(data, vmax = 39, alph = 0.04, kparm = 0.7,
    theta = 0.83, beta = 0.93, Rd = 0.8, Catm = 380,
    b0 = 0.08, b1 = 3, StomWS = 1,
    response = c("Assim", "StomCond"))
}
\arguments{
  \item{data}{observed assimilation data, which should be a
  data frame or matrix.  The first column should be
  observed net assimilation rate (\eqn{\mu} mol
  \eqn{m^{-2}} \eqn{s^{-1}}).  The second column should be
  the observed quantum flux (\eqn{\mu} mol \eqn{m^{-2}}
  \eqn{s^{-1}}).  The third column should be observed
  temperature of the leaf (Celsius).  The fourth column
  should be the observed relative humidity in proportion
  (e.g. 0.7).}

  \item{vmax}{Vcmax (default = 39); for more details see
  the \code{\link{c4photo}} function.}

  \item{alph}{alpha as in Collatz (default = 0.04); for
  more details see the \code{\link{c4photo}} function.}

  \item{kparm}{k as in Collatz (default = 0.70); for more
  details see the \code{\link{c4photo}} function.}

  \item{theta}{theta as in Collatz (default = 0.83); for
  more details see the \code{\link{c4photo}} function.}

  \item{beta}{beta as in Collatz (default = 0.93); for more
  details see the \code{\link{c4photo}} function.}

  \item{Rd}{Rd as in Collatz (default = 0.8); for more
  details see the \code{\link{c4photo}} function.}

  \item{StomWS}{StomWS as in Collatz (default = 1); for
  more details see the \code{\link{c4photo}} function.}

  \item{Catm}{Atmospheric CO2 in ppm (or
  \eqn{\mu}mol/mol).}

  \item{b0}{Intercept for the Ball-Berry model.}

  \item{b1}{Slope for the Ball-Berry model.}

  \item{response}{Use \code{'Assim'} if you want an
  \eqn{R^2} for assimilation data and use \code{'StomCond'}
  if you want an \eqn{R^2} for stomatal conductance data.}
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
  coefficients for the Collatz C4 photosynthesis model. The
  only coefficients being considered are Vcmax and alpha as
  described in the Collatz paper. At the moment it does not
  optimize k; this will be added soon.  Notice that to be
  able to optimize k A/Ci type data are needed.
}
\examples{
data(obsNaid)
## These data are from Naidu et al. (2003)
## in the correct format
res <- RsqC4photo(obsNaid)
## Other example using Beale, Bint and Long (1996)
data(obsBea)
resB <- RsqC4photo(obsBea)
}
\keyword{univar}

