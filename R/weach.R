##  BioCro/R/weach.R by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
##
##
##
##' Downscales daily weather to hourly
##'
##' Manipulates weather data in the format obtained from WARM (see link below)
##' and returns the format and units needed for most functions in this package.
##' This function should be used for one year at a time.
##' It returns hourly (or sub-daily) weather information.
##'
##'
##' This function was originally used to transform daily data to hourly data.
##' Some flexibility has been added so that other units can be used. The input
##' data used originally looked as follows.
##' \itemize{
##' \item col 1 year
##' \item col 2 day of the year (1--365). Does not consider leap
##' years.
##' \item col 3 total daily solar radiation (MJ/m^2).
##' \item col 4 maximum temperature (Fahrenheit).
##' \item col 5 minimum temperature (Fahrenheit).
##' \item col 6 average temperature (Fahrenheit).
##' \item col 7 maximum relative humidity (\%).
##' \item col 8 minimum relative humidity (\%).
##' \item col 9 average relative humidity (\%).
##' \item col 10 average wind speed (miles per hour).
##' \item col 11 precipitation (inches). 
##' }
##'
##' All the units above are the defaults but they can be changed as part of the
##' arguments.
##'
##' @param X a matrix (or data frame) containing weather information.  The
##' input format is strict but it is meant to be used with the data usually
##' obtained from weather stations in Illinois. The data frame should have 11
##' columns (see details).
##' @param latitude latitude at the specific location
##' @param ts timestep for the simulation of sub-daily data from daily. For
##' example a value of 3 would return data every 3 hours. Only divisors of 24
##' work (i.e. 1, 2, 3, 4, etc.).
##' @param temp.units Option to specify the units in which the temperature is
##' entered. Default is Farenheit.
##' @param rh.units Option to specify the units in which the relative humidity
##' is entered. Default is percent.
##' @param ws.units Option to specify the units in which the wind speed is
##' entered. Default is miles per hour.
##' @param pp.units Option to specify the units in which the precipitation is
##' entered. Default is inches.
##' @param list() additional arguments to be passed to \code{\link{lightME}}
##' @return a \code{\link{matrix}} returning hourly (or sub-daily) weather
##' data. Dimensions 8760 (if hourly) by 8.
##' \itemize{
##' \item year Year.
##' \item doy Day of the year.
##' \item hour Hour of the day (0--23, depending on the timestep).
##' \item solar Direct solar radiation (\eqn{\mu mol \; m^{-2} \;
##' }{micro mol per meter squared per second}\eqn{ s^{-1}}{micro mol per meter
##' squared per second}).
##' \item temp Air temperature (degrees Celsius).
##' \item rh Relative humidity (0--1).
##' \item windspeed Average wind speed (\eqn{m \;s^{-1}}{meter per second}).
##' \item precip Precipitation (\eqn{mm}{mm})
##' }
##' @keywords datagen
##' @examples
##'
##' data(cmi0506)
##' tmp1 <- cmi0506[cmi0506$year == 2005, ]
##' wet05 <- weach(tmp1, 40)
##'
##' # Return data every 3 hours
##' wet05.3 <- weach(tmp1, 40, ts=3)
##'

weach = function(X, ...) UseMethod('weach')

weach.default <- function(X, latitude, ts=1, ...){

  if (missing(latitude))
    stop("latitude is missing")
  
  if ((ts < 1) || (24 %% ts != 0))
    stop("ts should be a divisor of 24 (e.g. 1, 2, 3, 4, 6, etc.)")
  
  if (dim(X)[2] != 11)
    stop("X should have 11 columns")

  year <- X[, 1]
  DOYm <- X[, 2]
  insolation <- X[, 3]
  maxTemp <- X[, 4]
  minTemp <- X[, 5]
  avgTemp <- X[, 6]
  maxRH <- X[, 7]
  minRH <- X[, 8]
  avgRH <- X[, 9]
  WindSpeed <- X[, 10]
  precip <- X[, 11]

  tint <- 24 / ts
  tseq <- seq(0, 23, ts)

  ## Solar radiation
  solarR <- (0.12 * insolation) * 2.07 * 1e6 / 3600
  ## This last line needs some explanation
  ## There is no easy straight way to convert MJ / m^2 to micromoles photons / m^2 / s (PAR)
  ## The above conversion is based on the following reasoning.
  ## 0.12 is about how much of the total radiation is expected to occur during the hour of maximum insolation (it is a guesstimate).
  ## 2.07 is a coefficient which converts from MJ to mol photons (it is approximate and it is taken from ...
  ## Campbell and Norman (1998). Introduction to Environmental Biophysics. pg 151 'the energy content of solar radiation in the PAR waveband is 2.35 x 10^5 J / mol'
  ## See also the chapter radiation basics (10)
  ## Here the input is the total solar radiation so to obtain in the PAR spectrum need to multiply by 0.486.
  ## This last value 0.486 is based on the approximation that PAR is 0.45-0.50 of the total radiation.
  ## This means that 1e6 / 2.35e5 * 0.486 = 2.07.
  ## 1e6 converts from moles to micromoles.
  ## 1 / 3600 divides the values in hours to seconds.
  solarR <- rep(solarR, each=tint)

  ltseq <- length(tseq)
  resC2 <- numeric(ltseq*nrow(X))
  for (i in seq_len(nrow(X)))
    {
      res <- lightME(DOY=i, t.d=tseq, lat=latitude, ...)
      Itot <- res$I.dir + res$I.diff
      indx <- seq_len(ltseq) + (i - 1) * ltseq 
      resC2[indx] <- (Itot - min(Itot)) / max(Itot)
    }

  solar <- solarR * resC2

  ## Temperature
  minTemp <- rep(minTemp, each = tint)
  maxTemp <- rep(maxTemp, each = tint)
  rangeTemp <- maxTemp - minTemp

  xx <- rep(tseq, nrow(X))
  temp1 <- sin(2 * pi * (xx - 10) / tint)
  temp1 <- (temp1 + 1) / 2
  temp <- minTemp + temp1 * rangeTemp

  ## Relative humidity
  minRH <- rep(minRH, each=tint)
  maxRH <- rep(maxRH, each=tint)

  temp2 <- cos(2 * pi * (xx - 10) / tint)
  temp2 <- (temp2 + 1) / 2

  rh <- minRH + temp2 * (maxRH - minRH)

  ## Wind Speed
  windspeed <- rep(WindSpeed, each=tint)

  ## Precipitation
  precip <- rep(I(precip / tint), each=tint)
  
  hour <- rep(tseq, nrow(X))
  doy <- rep(DOYm, each=tint)

  data.frame(year, doy, hour, solar, temp, rh, windspeed, precip)
}
