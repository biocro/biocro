\docType{data}
\name{EngWea94rf}
\alias{EngWea94rf}
\title{Weather data corresponding to a paper by Clive Beale (see source).}
\format{A data frame with 8760 observations on the following 8 variables.
    \describe{
        \item{list('year')}{A numeric vector.}
        \item{list('doy')}{A numeric vector.}
        \item{list('hour')}{A numeric vector.}
        \item{list('solarR')}{A numeric vector.}
        \item{list('DailyTemp.C')}{A numeric vector.}
        \item{list('RH')}{A numeric vector.}
        \item{list('WindSpeed')}{A numeric vector.}
        \item{list('precip')}{A numeric vector.} }}
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
