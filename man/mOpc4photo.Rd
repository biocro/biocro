\name{mOpc4photo}
\alias{mOpc4photo}
\alias{plot.mOpc4photo}
\alias{print.mOpc4photo}
\title{Multiple optimization of C4 photosynthesis.}
\usage{
  mOpc4photo(data, ID = NULL, ivmax = 39, ialpha = 0.04,
    iRd = 0.8, ikparm = 0.7, itheta = 0.83, ibeta = 0.93,
    curve.kind = c("Q", "Ci"), op.level = 1, op.ci = FALSE,
    verbose = FALSE, ...)
}
\arguments{
  \item{data}{observed assimilation data, which should be a
  data frame or matrix. The first column should contain a
  run or id.  The second column should be observed net
  assimilation rate (\eqn{\mu} mol \eqn{m^{-2}}
  \eqn{s^{-1}}).  The third column should be the observed
  quantum flux (\eqn{\mu} mol \eqn{m^{-2}} \eqn{s^{-1}}).
  The fourth column should be observed temperature of the
  leaf (Celsius).  The fifth column should be the observed
  relative humidity in proportion (e.g. 0.7). An optional
  sixth column can contain atmospheric CO2.}

  \item{ID}{Optional vector with an alternative ID tothe
  one used in data for runs. The length shoudl be equal to
  the number of runs.}

  \item{ivmax}{Initial value for vmax. It can be a single
  value or a vector of length equal to the number of runs.}

  \item{ialpha}{Initial value for alpha. It can be a single
  value or a vector of length equal to the number of runs.}

  \item{iRd}{Initial value for vmax. It can be a single
  value or a vector of length equal to the number of runs.}

  \item{ikparm}{Initial value for vmax. It can be a single
  value or a vector of length equal to the number of runs.}

  \item{itheta}{Initial value for vmax. It can be a single
  value or a vector of length equal to the number of runs.}

  \item{ibeta}{Initial value for vmax. It can be a single
  value or a vector of length equal to the number of runs.}

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

  \item{op.ci}{Whether to optimize intercellular CO2.
  Default is FALSE as 'fast-measured' light curves do not
  provide reliable values of Ci.}

  \item{verbose}{Whether to display output about
  convergence of each run.}

  \item{\dots}{Used to supply additional arguments to
  \code{Opc4photo}.}
}
\value{
  An object of class \code{mOpc4photo} %% ~Describe the
  value returned %% If it is a LIST, use

  %% ...
}
\description{
  Wrapper function that allows for optimization of multiple
  A/Ci or A/Q curves.
}
\details{
  There are printing and plotting methods for the object
  created by this function. The plotting function has an
  argument that it is used to dsiplay either vmax or alpha
  (i.e. \code{parm=c('vmax','alpha')}). In both cases the
  optimized value plus confidence intervals will be
  displayed for each run.
}
\examples{
data(simAssim)
}
\author{
  Fernando E. miguez
}
\seealso{
  \code{\link{Opc4photo}} \code{\link{c4photo}}
  \code{\link{optim}} %% ~~objects to See Also as
  \code{\link{help}}, ~~~
}
\keyword{optimize}

