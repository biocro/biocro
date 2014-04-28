\name{MCMCEc4photo}
\alias{MCMCEc4photo}
\title{Markov chain Monte Carlo for C4 photosynthesis parameters}
\usage{
MCMCEc4photo(obsDat, niter = 30000, iCa = 380, iOa = 210, iVcmax = 60,
  iVpmax = 120, iVpr = 80, iJmax = 400, thresh = 0.01, scale = 1)
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

  \item{niter}{number of iterations to run the chain for
  (default = 30000).}

  \item{iCa}{CO2 atmospheric concentration (ppm or
  \eqn{\mu}bar).}

  \item{iOa}{O2 atmospheric concentration (mbar).}

  \item{iVcmax}{initial value for Vcmax (default = 60).}

  \item{iVpmax}{initial value for Vpmax (default = 120).}

  \item{iVpr}{initial value for Vpr (default = 80).}

  \item{iJmax}{initial value for Jmax (default = 400).}

  \item{thresh}{this is a threshold that determines the
  ``convergence'' of the initial burn-in period. The
  convergence of the whole chain can be evaluated by
  running the model with different starting values for
  Vcmax and alpha. The chain should convergence, but for
  this, runs with similar thresholds should be compared.
  This threshold reflects the fact that for any given data
  the model will not be able to simulate it perfectly so it
  represents a compromise between computability and fit.}

  \item{scale}{This scale parameter controls the size of
  the standard deviations which generate the moves in the
  chain. Decrease it to increase the acceptance rate and
  viceversa.}
}
\value{
a \code{\link{list}} structure with components
}
\description{
This function attempts to implement Markov chain Monte
Carlo methods for models with no likelihoods. In this case
it is done for the von Caemmerer C4 photosynthesis model.
The method implemented is based on a paper by Marjoram et
al. (2003).  The method is described in Miguez (2007). The
chain is constructed using a Gaussian random walk. This is
definitely a beta version of this function and more testing
and improvements are needed. The value of this function is
in the possibility of examining the empirical posterior
distribution (i.e. vectors) of the Vcmax and alpha
parameters. Notice that you will get different results each
time you run it.
}
\examples{
\dontrun{
## This is an example for the MCMCEc4photo
## evaluating the convergence of the chain
## Notice that if a parameter does not seem
## to converge this does not mean that the method
## doesn't work. Careful examination is needed
## in order to evaluate the validity of the results
data(obsNaid)
res1 <- MCMCEc4photo(obsNaid,100000,thresh=0.007)
res1

## Run it a few more times
## and test the stability of the method
res2 <- MCMCEc4photo(obsNaid,100000,thresh=0.007)
res3 <- MCMCEc4photo(obsNaid,100000,thresh=0.007)

## Now plot it
plot(res1,res2,res3)
plot(res1,res2,res3,type='density')
}
}
\references{
P. Marjoram, J. Molitor, V. Plagnol, S. Tavare, Markov
chain monte carlo without likelihoods, PNAS 100 (26) (2003)
15324--15328.

Miguez (2007) Miscanthus x giganteus production:
meta-analysis, field study and mathematical modeling. PhD
Thesis. University of Illinois at Urbana-Champaign.
}
\keyword{optimize}

