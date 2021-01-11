\docType{data}
\name{EngWea94rf}
\alias{EngWea94rf}
\title{Weather data corresponding to a paper by Clive Beale (see source).}
\format{
  A data frame with 8760 observations on the following 8 variables.
  \describe{
    \item{\code{year}}{A numeric vector.}
    \item{\code{doy}}{A numeric vector.}
    \item{\code{hour}}{A numeric vector.}
    \item{\code{solar}}{A numeric vector.}
    \item{\code{temp}}{A numeric vector.}
    \item{\code{RH}}{A numeric vector.}
    \item{\code{WindSpeed}}{A numeric vector.}
    \item{\code{precip}}{A numeric vector.}
  }
}
\source{
}
\description{
    Weather data with the precipitation column giving precipitation without
    irrigation.
}
\details{
}
\examples{

data(EngWea94rf)
## maybe str(EngWea94rf) ; plot(EngWea94rf) ...

}
\references{
}
\keyword{datasets}
