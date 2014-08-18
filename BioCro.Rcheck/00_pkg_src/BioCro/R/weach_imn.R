##  BioCro/R/weach_imn.R by Fernando Ezequiel Miguez  Copyright (C) 2011

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
