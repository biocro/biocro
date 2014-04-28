\docType{data}
\name{WD1979}
\alias{WD1979}
\title{Randomly picked dataset from the Illinois area from 1979}
\format{A data frame with 365 observations on the following 9 variables.
\describe{ \item{list('year')}{year} \item{list('month')}{month (not really
needed)} \item{list('day')}{day of the month (not really needed)}
\item{list('JD')}{day of the year (1-365)} \item{list('maxTemp')}{maximum
temperature (Celsius)} \item{list('minTemp')}{minimum temperature
(Celsius)} \item{list('SolarR')}{solar radiation (MJ/m2)}
\item{list('PotEv')}{potential evaporation (kg/m2). Approx. mm.}
\item{list('precip')}{precipitation (kg/m2). Approx. mm.} }}
\source{
\url{http://www.noaa.gov/}
}
\description{
Data from the Illinois area from one point from the 32km
grid from NOAA from 1979. the purpuse is to illustrate the
\code{\link{Rmiscanmod}} function.
}
\examples{
data(WD1979)
summary(WD1979)
}
\keyword{datasets}

