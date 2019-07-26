\docType{data}
\name{EngWea94i}
\alias{EngWea94i}
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
    Weather data with the precipitation column giving precipitation plus
    irrigation.
}
\details{
}
\examples{

data(EngWea94i)
## maybe str(EngWea94i) ; plot(EngWea94i) ...

}
\references{
}
\keyword{datasets}
