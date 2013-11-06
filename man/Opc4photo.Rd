\name{Opc4photo}
\alias{Opc4photo}
\alias{plot.Opc4photo}
\alias{plotAQ}
\alias{predict.Opc4photo}
\alias{print.Opc4photo}
\alias{print.summary.Opc4photo}
\alias{summary.Opc4photo}
\title{Optimization of C4 photosynthesis parameters}
\usage{
  Opc4photo(data, ivmax = 39, ialpha = 0.04, iRd = 0.8,
    ikparm = 0.7, itheta = 0.83, ibeta = 0.93, Catm = 380,
    ib0 = 0.08, ib1 = 3, iStomWS = 1, ws = c("gs", "vmax"),
    iupperT = 37.5, ilowerT = 3,
    response = c("Assim", "StomCond"),
    curve.kind = c("Q", "Ci"), op.level = 1, level = 0.95,
    hessian = TRUE, op.ci = FALSE, ...)
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
  (e.g. 0.7). An optional fifth column can contain
  intercellular CO2. The reference level of CO2 should be
  supplied to the function using the \code{Catm} argument.}

  \item{ivmax}{initial value for Vcmax (default = 39).}

  \item{ialpha}{initial value for alpha (default = 0.04).}

  \item{iRd}{initial value for dark respiration (default =
  0.8).}

  \item{ikparm}{initial value for k (default = 0.7).}

  \item{itheta}{initial value for theta (default = 0.83).}

  \item{ibeta}{initial value for beta (default = 0.93).}

  \item{Catm}{Atmospheric CO2 in ppm (or
  \eqn{\mu}mol/mol).}

  \item{ib0}{initial value for the Ball-Berry intercept.}

  \item{ib1}{initial value for the Ball-Berry slope.}

  \item{iStomWS}{initial value for the stomata water stress
  factor.}

  \item{ws}{\code{ws} flag. See \code{\link{c4photo}}.}

  \item{response}{Use \code{'Assim'} if you want to
  optimize assimilation data and use \code{'StomCond'} if
  you want to optimize stomatal conductance data. The
  parameters optimized will be different.}

  \item{curve.kind}{If \code{'Q'} a type of response which
  mainly depends on light will be assumed. Typically used
  to optimized light response curves or diurnals. Use
  \code{'Ci'} for A/Ci curves (stomatal conductance could
  also be optimized).}

  \item{op.level}{optimization level. If equal to 1
  \code{vmax} and \code{alpha} will be optimized. If 2,
  \code{vmax}, \code{alpha} and \code{Rd} will be
  optimized. If 3, \code{vmax}, \code{alpha}, \code{theta}
  and \code{Rd} will be optimized.}

  \item{level}{level for the confidence intervals.}

  \item{hessian}{Whether the hessian matrix should be
  computed. See \code{\link{optim}}.}

  \item{op.ci}{Whether to optimize intercellular CO2.
  Default is FALSE as 'fast-measured' light curves do not
  provide reliable values of Ci.}

  \item{list()}{Additional arguments passed to the
  \code{\link{optim}} in particular if a lower or upper
  bound is desired this could be achieved by adding
  \code{lower=c(0,0)} this will impose a lower bound on
  \code{vmax} and \code{alpha} of zero so preventing
  negative values from being returned. When the lower
  argument is added the optimization method changes from
  Nelder-Mead to BFGS.}
}
\value{
  An object of class \code{Opc4photo} a \code{\link{list}}
  with components

  If \code{op.level} 2 \code{bestRd} will also be supplied.
  If \code{op.level} 3 \code{theta} and \code{bestRd} will
  also be supplied.

  If \code{op.level} 2 \code{ciRd} will also be supplied.
  If \code{op.level} 3 \code{ciTheta} and \code{ciRd} will
  also be supplied.
}
\description{
  Optimization method for the Collatz C4 photosynthesis
  model.  At the moment Vcmax and alpha are optimized only.
}
\examples{
data(aq)
## Select data for a single AQ optimization
aqd <- data.frame(aq[aq[,1] == 6,-c(1:2)],Catm=400)
res <- Opc4photo(aqd, Catm=aqd$Catm)
res

plot(res, plot.kind = 'OandF', type='o')
}
\seealso{
  \code{\link{c4photo}} \code{\link{optim}}
}
\keyword{optimize}

