\name{MCMCc4photo}
\alias{MCMCc4photo}
\alias{print.MCMCc4photo}
\title{Markov chain Monte Carlo for C4 photosynthesis parameters}
\usage{
  MCMCc4photo(data, niter = 20000, ivmax = 39,
    ialpha = 0.04, ikparm = 0.7, itheta = 0.83,
    ibeta = 0.93, iRd = 0.8, Catm = 380, b0 = 0.08, b1 = 3,
    StomWS = 1, ws = c("gs", "vmax"), scale = 1,
    sds = c(1, 0.005), prior = c(39, 10, 0.04, 0.02))
}
\arguments{
  \item{data}{observed assimilation data, which should be a
  data frame or matrix.  The first column should be
  observed net assimilation rate (\eqn{\mu mol \; m^{-2} \;
  }{micro mol per meter squared per second}\eqn{
  s^{-1}}{micro mol per meter squared per second}).  The
  second column should be the observed quantum flux
  (\eqn{\mu mol \; m^{-2} \; }{micro mol per meter squared
  per second}\eqn{ s^{-1}}{micro mol per meter squared per
  second}).  The third column should be observed
  temperature of the leaf (Celsius).  The fourth column
  should be the observed relative humidity in proportion
  (e.g. 0.7).}

  \item{niter}{number of iterations to run the chain for
  (default = 20000).}

  \item{ivmax}{initial value for Vcmax (default = 39).}

  \item{ialpha}{initial value for alpha (default = 0.04).}

  \item{ikparm}{initial value for kparm (default = 0.7).
  Not optimized at the moment.}

  \item{itheta}{initial value for theta (default = 0.83).
  Not optimized at the moment.}

  \item{ibeta}{initial value for beta (default = 0.93). Not
  optimized at the moment.}

  \item{iRd}{initial value for dark respiration (default =
  0.8).}

  \item{Catm}{see \code{\link{c4photo}} function.}

  \item{b0}{see \code{\link{c4photo}} function.}

  \item{b1}{see \code{\link{c4photo}} function.}

  \item{StomWS}{see \code{\link{c4photo}} function.}

  \item{ws}{see \code{\link{c4photo}} function.}

  \item{scale}{This scale parameter controls the size of
  the standard deviations which generate the moves in the
  chain.}

  \item{sds}{Finer control for the standard deviations of
  the prior normals. The first element is for vmax and the
  second for alpha.}

  \item{prior}{Vector of length 4 with first element prior
  mean for vmax, second element prior standard deviation
  for vmax, third element prior mean for alpha and fourth
  element prior standard deviation for alpha.}
}
\value{
  an object of class \code{MCMCc4photo} with components
}
\description{
  This function implement Markov chain Monte Carlo methods
  for the C4 photosynthesis model of Collatz et al.  The
  chain is constructed using a Gaussian random walk. This
  is definitely a beta version of this function and more
  testing and improvements are needed. The value of this
  function is in the possibility of examining the empirical
  posterior distribution (i.e. vectors) of the vmax and
  alpha parameters.  Notice that you will get different
  results each time you run it.
}
\examples{
\dontrun{
## Using Beale, Bint and Long (1996)
data(obsBea)

## Different starting values
resB1 <- MCMCc4photo(obsBea, 100000, scale=1.5)
resB2 <- MCMCc4photo(obsBea, 100000, ivmax=25, ialpha=0.1, scale=1.5)
resB3 <- MCMCc4photo(obsBea, 100000, ivmax=45, ialpha=0.02, scale=1.5)

## Use the plot function to examine results
plot(resB1,resB2,resB3)
plot(resB1,resB2,resB3,plot.kind="density",burnin=1e4)

}
}
\references{
  Brooks, Stephen. (1998). Markov chain Monte Carlo and its
  application. The Statistician. 47, Part 1, pp. 69-100.
}
\keyword{optimize}

