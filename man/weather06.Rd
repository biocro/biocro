\docType{data}
\name{weather06}
\alias{weather06}
\title{Weather data}
\format{A data frame with 8760 observations on the following 8 variables.
    \describe{
        \item{list('year')}{A numeric vector.}
        \item{list('doy')}{A numeric vector.}
        \item{list('hour')}{A numeric vector.} 
        \item{list('SolarR')}{A numeric vector.}
        \item{list('Temp')}{A numeric vector.}
        \item{list('RH')}{A numeric vector.}
        \item{list('WS')}{A numeric vector.}
        \item{list('precip')}{A numeric vector.} }}
\source{
}
\description{
    Weather data as produced by the \code{\link{weach}} function.  These are for 2006.
}
\examples{

data(weather06)
## maybe str(weather06) ; plot(weather06) ...

}
\keyword{datasets}
