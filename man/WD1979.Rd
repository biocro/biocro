\docType{data}
\name{WD1979}
\alias{WD1979}
\title{Randomly picked dataset from the Illinois area from 1979.}
\format{A data frame with 365 observations of the following 9 variables.
    \describe{
        \item{\code{year}}{Year (numeric; always 1979)}
        \item{\code{month}}{Month (not available; can be computed from JD)}
        \item{\code{day}}{Day of the month  (not available; can be computed from year and JD)}
        \item{\code{JD}}{Day of the year ([1, 365]; numeric)}
        \item{\code{maxTemp}}{Maximum temperature (degrees Celsius; numeric).}
        \item{\code{minTemp}}{Minimum temperature (degrees Celsius; numeric).}
        \item{\code{SolarR}}{Solar radiation (MJ / m^2; numeric).}
        \item{\code{PotEv}}{Potential evaporation (kg / m^2 ~ mm; numeric)}
        \item{\code{precip}}{Precipitation (kg / m^2 ~ mm; numeric)}
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
