\name{weach_imn}
\alias{weach_imn}
%- Also NEED an '\alias' for EACH other topic documented here.
\title{
  Weather change Iowa Mesonet

}
\description{
  Manipulates weather data in the format obtained from Iowa Mesonet
  (see link below)
  and returns the format and units needed for most functions
  in this package. This function should be used for one year at a time.
  It takes and returns hourly weather information only.
}
\usage{
weach_imn(data, lati, ts = 1, temp.units = c("Fahrenheit", "Celsius"), rh.units = c("percent", "fraction"), ws.units = c("mph", "mps"), pp.units = c("in", "mm"), ...)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{data}{
    data as obtained from the Iowa Mesonet (see details)
%%     ~~Describe \code{data} here~~
}
\item{lati}{
  Latitude, not used at the moment
%%     ~~Describe \code{lati} here~~
}
\item{ts}{
  Time step, not used at the moment
%%     ~~Describe \code{ts} here~~
}
\item{temp.units}{
  Temperature units.
%%     ~~Describe \code{temp.units} here~~
}
\item{rh.units}{
  Relative humidity units
%%     ~~Describe \code{rh.units} here~~
}
\item{ws.units}{
  wind speed units
%%     ~~Describe \code{ws.units} here~~
}
\item{pp.units}{
  precipitation units
%%     ~~Describe \code{pp.units} here~~
}
  \item{\dots}{
%%     ~~Describe \code{\dots} here~~
}
}
\details{
    This function should be used to transform data from the Iowa Mesonet
    at hourly intervals from here:

    http://mesonet.agron.iastate.edu/agclimate/hist/hourlyRequest.php

    When selecting to download variables:
    Air Temperature (Fahrenheit)
    Solar Radiation (kilocalories per meter squared)
    Precipitation (inches)
    Relative humidity (percent)
    Wind Speed (mph)

    You can read the data directly as it is downloaded making sure you
    skip the first 6 lines (This includes the title row).

    The imported data frame should have 9 columns with:
    
\enumerate{

\item{col 1}{ site ID}
\item{col 2}{ site name }
\item{col 3}{ date in format "year-month-day", e.g. '2010-3-25' }
\item{col 4}{ hour in format "hour:minute", e.g. '15:00'}
\item{col 5}{ temperature (Fahrenheit) }
\item{col 6}{ solar radiation (kilocalories per meter squared }
\item{col 7}{ precipitation (inches) }
\item{col 8}{ relative humidity (\%).}
\item{col 9}{ wind speed (mph) }

}
%%  ~~ If necessary, more details than the description above ~~
}
\value{

  It will return a data frame in the same format as the
  \code{\link{weach}} function.
%%  ~Describe the value returned
%%  If it is a LIST, use
%%  \item{comp1 }{Description of 'comp1'}
%%  \item{comp2 }{Description of 'comp2'}
%% ...
}
\references{
  Iowa Mesonet
  http://mesonet.agron.iastate.edu/index.phtml
%% ~put references to the literature/web site here ~
}
\author{
Fernando E. Miguez
}
\note{
%%  ~~further notes~~
}

%% ~Make other sections like Warning with \section{Warning }{....} ~

\seealso{
  \code{\link{weach}}
%% ~~objects to See Also as \code{\link{help}}, ~~~
}
\examples{

## Read an example data set from my website
url <- "http://www.agron.iastate.edu/miguezlab/teaching/CropSoilModel/ames_2010-iowamesonet.txt"
ames.wea <- read.table(url, skip = 6)
ames.wea2 <- weach_imn(ames.wea)

}
% Add one or more standard keywords, see file 'KEYWORDS' in the
% R documentation directory.
\keyword{ datagen }

