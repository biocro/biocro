\docType{data}
\name{WD1979}
\alias{WD1979}
\title{Randomly picked dataset from the Illinois area from 1979.}
\format{A data frame with 365 observations of the following 9 variables.
    \describe{
        \item{list('year')}{Year.}
        \item{list('month')}{Month (not really needed).}
        \item{list('day')}{Day of the month (not really needed).} 
        \item{list('JD')}{Day of the year ([1, 365]).}
        \item{list('maxTemp')}{Maximum temperature (degrees Celsius).}
        \item{list('minTemp')}{Minimum temperature (degrees Celsius).}
        \item{list('SolarR')}{Solar radiation (MJ / m^-2).}
        \item{list('PotEv')}{Potential evaporation (kg / m^-2). Approx. mm.}
        \item{list('precip')}{Precipitation (kg / m^-2). Approx. mm.}
    }
}
\source{
    \url{http://www.noaa.gov/}
}
\description{
    Data from the Illinois area from one point from the 32km grid from NOAA
    from 1979.
}
\examples{

data(WD1979)
summary(WD1979)

}
\keyword{datasets}
