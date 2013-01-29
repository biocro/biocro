\name{EngWea94i}
\alias{EngWea94i}
\docType{data}
\title{ Weather data corresponding to a paper by Clive Beale (see source).}
\description{
  Weather data with the precipitation column giving precipitation plus irrigation.
}
\usage{data(EngWea94i)}
\format{
  A data frame with 8760 observations on the following 8 variables.
  \describe{
    \item{\code{year}}{a numeric vector}
    \item{\code{doy}}{a numeric vector}
    \item{\code{hour}}{a numeric vector}
    \item{\code{solarR}}{a numeric vector}
    \item{\code{DailyTemp.C}}{a numeric vector}
    \item{\code{RH}}{a numeric vector}
    \item{\code{WindSpeed}}{a numeric vector}
    \item{\code{precip}}{a numeric vector}
  }
}
\details{
  ~~ If necessary, more details than the above ~~
}
\source{
  ~~ reference to a publication or URL from which the data were obtained ~~
}
\references{
  ~~ possibly secondary sources and usages ~~
}
\examples{
data(EngWea94i)
## maybe str(EngWea94i) ; plot(EngWea94i) ...
}
\keyword{datasets}
