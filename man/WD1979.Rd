\name{WD1979}
\alias{WD1979}
\docType{data}
\title{ Randomly picked dataset from the Illinois area from 1979 }
\description{
  Data from the Illinois area from one point from
  the 32km grid from NOAA from 1979. the purpuse is to illustrate the
  \code{\link{Rmiscanmod}} function. 
}
\usage{data(WD1979)}
\format{
  A data frame with 365 observations on the following 9 variables.
  \describe{
    \item{\code{year}}{year}
    \item{\code{month}}{month (not really needed)}
    \item{\code{day}}{day of the month (not really needed)}
    \item{\code{JD}}{day of the year (1-365)}
    \item{\code{maxTemp}}{maximum temperature (Celsius)}
    \item{\code{minTemp}}{minimum temperature (Celsius)}
    \item{\code{SolarR}}{solar radiation (MJ/m2)}
    \item{\code{PotEv}}{potential evaporation (kg/m2). Approx. mm.}
    \item{\code{precip}}{precipitation (kg/m2). Approx. mm.}
  }
}

\source{
  \url{http://www.noaa.gov/}
}

\examples{
data(WD1979)
summary(WD1979)
}
\keyword{datasets}
