\name{weach_imn}
\alias{weach_imn}
\title{Weather change Iowa Mesonet}
\usage{
weach_imn(data, lati, ts = 1, temp.units = c("Fahrenheit", "Celsius"),
  rh.units = c("percent", "fraction"), ws.units = c("mph", "mps"),
  pp.units = c("in", "mm"), ...)
}
\arguments{
  \item{data}{data as obtained from the Iowa Mesonet (see
  details)}

  \item{lati}{Latitude, not used at the moment}

  \item{ts}{Time step, not used at the moment}

  \item{temp.units}{Temperature units}

  \item{rh.units}{Relative humidity units}

  \item{ws.units}{wind speed units}

  \item{pp.units}{precipitation units}

  \item{\dots}{}
}
\value{
It will return a data frame in the same format as the
\code{\link{weach}} function.
}
\description{
Manipulates weather data in the format obtained from Iowa
Mesonet (see link below) and returns the format and units
needed for most functions in this package. This function
should be used for one year at a time.  It takes and
returns hourly weather information only.
}
\details{
This function should be used to transform data from the
Iowa Mesonet at hourly intervals from here:

http://mesonet.agron.iastate.edu/agclimate/hist/hourlyRequest.php

When selecting to download variables: Air Temperature
(Fahrenheit) Solar Radiation (kilocalories per meter
squared) Precipitation (inches) Relative humidity (percent)
Wind Speed (mph)

You can read the data directly as it is downloaded making
sure you skip the first 6 lines (This includes the title
row).

The imported data frame should have 9 columns with:

\itemize{

\item{site ID} \item{site name} \item{date in format
"year-month-day", e.g. '2010-3-25'} \item{hour in format
"hour:minute", e.g. '15:00'} \item{temperature
(Fahrenheit)} \item{solar radiation (kilocalories per meter
squared)} \item{precipitation (inches)} \item{relative
humidity (\%).} \item{wind speed (mph)} }

%% ~~ If necessary, more details than the description above
~~
}
\note{
%% ~~further notes~~
}
\examples{
## Read an example data set from my website
url <- "http://www.agron.iastate.edu/miguezlab/teaching/CropSoilModel/ames_2010-iowamesonet.txt"
ames.wea <- read.table(url, skip = 6)
ames.wea2 <- weach_imn(ames.wea)
}
\author{
Fernando E. Miguez
}
\references{
Iowa Mesonet http://mesonet.agron.iastate.edu/index.phtml
%% ~put references to the literature/web site here ~
}
\seealso{
\code{\link{weach}} %% ~~objects to See Also as
\code{\link{help}}, ~~~
}
\keyword{datagen}

