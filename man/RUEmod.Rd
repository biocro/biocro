\name{RUEmod}
\alias{RUEmod}
\title{Radiation use efficiency based model}
\usage{
  RUEmod(Rad, T.a, doy.s = 91, doy.f = 227, lai.c = 0.0102,
    rue = 2.4, T.b = 10, k = 0.68)
}
\arguments{
  \item{Rad}{Daily solar radiation (MJ \eqn{ha^{-2}}).}

  \item{T.a}{Daily average temperature (Fahrenheit).}

  \item{doy.s}{first day of the growing season, default
  91.}

  \item{doy.f}{last day of the growing season, default
  227.}

  \item{lai.c}{linear relationship between growing degree
  days and leaf area index.}

  \item{rue}{radiation use efficiency, default 2.4.}

  \item{T.b}{base temperature for calculating growing
  degree days, default 10.}

  \item{k}{light extinction coefficient, default 0.68.}
}
\value{
  a \code{\link{list}} structure with components
}
\description{
  Simulates leaf area index, biomass and light
  interception.  Based on the Monteith (1973) equations,
  adapted for Miscanthus by Clifton Brown et al.(2001) (see
  references).
}
\note{
  This empirical model is useful but it has limitations.
}
\examples{
## See RUEmodMY
}
\references{
  Monteith (1973) \emph{Principles of Environmental
  Physics}. Edward Arnold, London. UK.

  Clifton-Brown, J.C., Long, S.P. and Jorgensen, U. with
  contributions from S.A. Humphries, Schwarz, K.-U. and
  Schwarz, H. (2001) \emph{Miscanthus Productivity}. Ch 4.
  In: Miscanthus for Energy and Fibre. Edited by: Jones,
  Michael B. and Walsh, Mary. James & James (Science
  Publishers), London, UK.
}
\seealso{
  \code{\link{RUEmodMY}}
}
\keyword{models}

