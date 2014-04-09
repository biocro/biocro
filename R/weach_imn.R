##  BioCro/R/weach_imn.R by Fernando Ezequiel Miguez  Copyright (C) 2011
#' Weather change Iowa Mesonet
#' 
#' Manipulates weather data in the format obtained from Iowa Mesonet (see link
#' below) and returns the format and units needed for most functions in this
#' package. This function should be used for one year at a time.  It takes and
#' returns hourly weather information only.
#' 
#' This function should be used to transform data from the Iowa Mesonet at
#' hourly intervals from here:
#' 
#' http://mesonet.agron.iastate.edu/agclimate/hist/hourlyRequest.php
#' 
#' When selecting to download variables: Air Temperature (Fahrenheit) Solar
#' Radiation (kilocalories per meter squared) Precipitation (inches) Relative
#' humidity (percent) Wind Speed (mph)
#' 
#' You can read the data directly as it is downloaded making sure you skip the
#' first 6 lines (This includes the title row).
#' 
#' The imported data frame should have 9 columns with:
#' 
#' \itemize{
#' 
#' \item{site ID} 
#' \item{site name} 
#' \item{date in format "year-month-day", e.g. '2010-3-25'} 
#' \item{hour in format "hour:minute", e.g. '15:00'} 
#' \item{temperature (Fahrenheit)} 
#' \item{solar radiation (kilocalories per meter squared)} 
#' \item{precipitation (inches)} 
#' \item{relative humidity (\%).} 
#' \item{wind speed (mph)}
#' } 
#' 
#' %% ~~ If necessary, more details than the description above ~~
#' 
#' @param data data as obtained from the Iowa Mesonet (see details) 
#' @param lati Latitude, not used at the moment 
#' @param ts Time step, not used at the moment 
#' @param temp.units Temperature units 
#' @param rh.units Relative humidity units 
#' @param ws.units wind speed units 
#' @param pp.units precipitation units 
#' @param \dots 
#' @return
#' 
#' It will return a data frame in the same format as the \code{\link{weach}}
#' function. 
#' @note %% ~~further notes~~
#' @author Fernando E. Miguez
#' @seealso \code{\link{weach}} %% ~~objects to See Also as \code{\link{help}},
#' ~~~
#' @references Iowa Mesonet http://mesonet.agron.iastate.edu/index.phtml %%
#' ~put references to the literature/web site here ~
#' @keywords datagen
#' @examples
#' 
#' 
#' ## Read an example data set from my website
#' url <- "http://www.agron.iastate.edu/miguezlab/teaching/CropSoilModel/ames_2010-iowamesonet.txt"
#' ames.wea <- read.table(url, skip = 6)
#' ames.wea2 <- weach_imn(ames.wea)
#' 
#' 
weach_imn <- function(data, lati, ts=1, temp.units=c("Fahrenheit","Celsius"),
                      rh.units=c("percent","fraction"),
                      ws.units=c("mph","mps"),
                      pp.units=c("in","mm"),...){


  ## if(missing(lati))
  ##   stop("latitude is missing")
  
  if((ts<1)||(24%%ts != 0))
    stop("ts should be a divisor of 24 (e.g. 1,2,3,4,6,etc.)")

  if(ts != 1) stop("This function only accepts hourly data at the moment")
  
  if(dim(data)[2] != 9)
    stop("X should have 9 columns")

  ## Need to get rid of the last row
  if(nrow(data) == 8761) data <- data[-8761,]

  MPHTOMPERSEC <- 0.447222222222222

  temp.units <- match.arg(temp.units)
  rh.units <- match.arg(rh.units)
  ws.units <- match.arg(ws.units)
  pp.units <- match.arg(pp.units)

  ## Collect the variables

  site <- data[,1]
  loc  <- data[,2]
  date <- data[,3]
  hour <- data[,4]
  temp <- data[,5]
  solarR <- data[,6]
  precip <- data[,7]
  RH <- data[,8]
  windS <- data[,9]

  ## Transform them in to needed input

  date <- as.Date(date)
  year <- as.numeric(format(date, "%Y"))
  doy <- as.numeric(format(date, "%j"))
  hour <- as.numeric(as.vector(sapply(as.character(hour),
                                      function(x) strsplit(x, ":")[[1]][1])))
  if(temp.units == "Fahrenheit"){
    temp <- (temp - 32) * (5/9)
  }
  ## the solar radiation is given in kilo calories per hour per meter squared
  ## To convert from kilocalories to joules
  ## 1 kilocalorie = 4184 joules
  ## To convert to Mega Joules
  solarR0 <- (solarR * 4184) * 1e-6 ## This is in MJ/hr/m2
  solarR <- solarR0 * 2.07 * 1e6 / 3600 ## Look for comments in the
                                        ## source code of the weach function
                                        ## For details
  if(pp.units == "in"){
    precip <- precip*2.54*10
  }

  if(rh.units == "percent"){
    RH <- RH / 100
  }
  
  if(ws.units == "mph"){
    windS <- windS * MPHTOMPERSEC
  }

  res <- data.frame(year = year, doy = doy, hour = hour, solarR = solarR,
                    temp = temp, RH = RH, windS = windS, precip = precip)

  res

}
