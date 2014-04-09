\name{weach}
\alias{weach}
\title{Simulates the hourly conditions from daily}
\usage{
weach(X, lati, ts = 1, temp.units = c("Fahrenheit", "Celsius"),
  rh.units = c("percent", "fraction"), ws.units = c("mph", "mps"),
  pp.units = c("in", "mm"), ...)
}
\arguments{
  \item{X}{a matrix (or data frame) containing weather
  information.  The input format is strict but it is meant
  to be used with the data usually obtained from weather
  stations in Illinois. The data frame should have 11
  columns (see details).}

  \item{lati}{latitude at the specific location}

  \item{ts}{timestep for the simulation of sub-daily data
  from daily. For example a value of 3 would return data
  every 3 hours. Only divisors of 24 work (i.e. 1,2,3,4,
  etc.).}

  \item{temp.units}{Option to specify the units in which
  the temperature is entered. Default is Farenheit.}

  \item{rh.units}{Option to specify the units in which the
  relative humidity is entered. Default is percent.}

  \item{ws.units}{Option to specify the units in which the
  wind speed is entered. Default is miles per hour.}

  \item{pp.units}{Option to specify the units in which the
  precipitation is entered. Default is inches.}

  \item{list()}{additional arguments to be passed to
  \code{\link{lightME}}}
}
\value{
a \code{\link{matrix}} returning hourly (or sub-daily)
weather data. Dimensions 8760 (if hourly) by 8.
}
\description{
Manipulates weather data in the format obtained from WARM
(see link below) and returns the format and units needed
for most functions in this package. This function should be
used for one year at a time.  It returns hourly (or
sub-daily) weather information.
}
\details{
This function was originally used to transform daily data
to hourly data. Some flexibility has been added so that
other units can be used. The input data used originally
looked as follows. \itemize{

\item{year} \item{day of the year (1--365). Does not
consider leap years.} \item{total daily solar radiation
(MJ/m^2).} \item{maximum temperature (Fahrenheit).}
\item{minimum temperature (Fahrenheit).} \item{average
temperature (Fahrenheit).} \item{maximum relative humidity
(\%).} \item{minimum relative humidity (\%).} \item{average
relative humidity (\%).} \item{average wind speed (miles
per hour).} \item{precipitation (inches).} }

All the units above are the defaults but they can be
changed as part of the arguments.
}
\examples{
data(cmi0506)
tmp1 <- cmi0506[cmi0506$year == 2005,]
wet05 <- weach(tmp1,40)

# Return data every 3 hours
wet05.3 <- weach(tmp1,40,ts=3)
}
\keyword{datagen}

