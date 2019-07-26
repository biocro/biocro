##' Weather data
##'
##' Selected weather data corresponding to the Champaign weather station (IL,
##' U.S.). It has two years: 2005 and 2006. Dimensions: 730 by 11. The columns
##' correspond to the input necessary for the \code{\link{weach}} function.
##'
##'
##' @name cmi0506
##' @docType data
##' @format Data frame of dimensions 730 by 11.
##' @source \url{http://www.sws.uiuc.edu/warm/}
##' @keywords datasets
NULL


##' Weather data
##'
##' Example for a given day of the year to illustrate the \code{\link{CanA}}
##' function.
##'
##' LAI: Leaf area index.
##'
##' year: Year.
##'
##' doy: 124 in this case.
##'
##' hour: Hour of the day (0--23).
##'
##' solarR: Direct solar radiation.
##'
##' DailyTemp.C: Hourly air temperature (Celsius).
##'
##' RH: Relative humidity (0--1).
##'
##' WindSpeed: 4.1 m s\eqn{^{-1}}{-1} average daily value in this case.
##'
##'
##' @name doy124
##' @docType data
##' @format Data frame of dimensions 24 by 8.
##' @source Simulated.
##' @keywords datasets
NULL


##' Weather data corresponding to a paper by Clive Beale (see source).
##'
##' Weather data with the precipitation column giving precipitation plus
##' irrigation.
##'
##' ~~ If necessary, more details than the above ~~
##'
##' @name EngWea94i
##' @docType data
##' @format A data frame with 8760 observations on the following 8 variables.
##' \describe{ \item{list('year')}{A numeric vector.} \item{list('doy')}{A
##' numeric vector.} \item{list('hour')}{A numeric vector.}
##' \item{list('solarR')}{A numeric vector.} \item{list('DailyTemp.C')}{A
##' numeric vector.} \item{list('RH')}{A numeric vector.}
##' \item{list('WindSpeed')}{A numeric vector.} \item{list('precip')}{A numeric
##' vector.} }
##' @references ~~ possibly secondary sources and usages ~~
##' @source ~~ reference to a publication or URL from which the data were
##' obtained ~~
##' @keywords datasets
##' @examples
##'
##' data(EngWea94i)
##' ## maybe str(EngWea94i) ; plot(EngWea94i) ...
##'
NULL





##' Weather data corresponding to a paper by Clive Beale (see source).
##'
##' Weather data with the precipitation column giving precipitation without
##' irrigation.
##'
##' ~~ If necessary, more details than the description above ~~
##'
##' @name EngWea94rf
##' @docType data
##' @format A data frame with 8760 observations on the following 8 variables.
##' \describe{ \item{list('year')}{A numeric vector.} \item{list('doy')}{A
##' numeric vector.} \item{list('hour')}{A numeric vector.}
##' \item{list('solarR')}{A numeric vector.} \item{list('DailyTemp.C')}{A
##' numeric vector.} \item{list('RH')}{A numeric vector.}
##' \item{list('WindSpeed')}{A numeric vector.} \item{list('precip')}{A numeric
##' vector.} }
##' @references ~~ possibly secondary sources and usages ~~
##' @source ~~ reference to a publication or URL from which the data were
##' obtained ~~
##' @keywords datasets
##' @examples
##'
##' data(EngWea94rf)
##' ## maybe str(EngWea94rf) ; plot(EngWea94rf) ...
##'
NULL





##' Weather data
##'
##' Layer data for evapo/transpiration. Simulated data to show the result of
##' the EvapoTrans function.
##'
##' lfClass: Leaf class, `sun' or `shade'.
##'
##' layer: Layer in the canopy, 1 to 8.
##'
##' hour: Hour of the day (0--23).
##'
##' Rad: Direct light.
##'
##' Itot: Total radiation.
##'
##' Temp: Air temperature (Celsius).
##'
##' RH: Relative humidity (0--1).
##'
##' WindSpeed: Wind speed (m s\eqn{^{-1}}{-1}).
##'
##' LAI: Leaf area index.
##'
##'
##' @name LayET
##' @docType data
##' @format Data frame of dimensions 384 by 9.
##' @source Simulated.
##' @keywords datasets
NULL





##' Complete Miscanthus assimilation field data
##'

##'
##' Assimilation and stomatal conductance in Miscanthus as measured in Beale, Bint, and Long
##' 1996.  (Missing data are also included.)  The first column is
##' the date, the second the hour. Columns 3 and 4 are assimilation and
##' stomatal conductance respectively.
##'
##' The third column is the observed net assimilation rate (\eqn{\mu} mol
##' m\eqn{^{-2}}{-2} s\eqn{^{-1}}{-1}).
##'
##' The fifth column is the observed quantum flux (\eqn{\mu} mol m\eqn{^{-2}}{-2}
##' s\eqn{^{-1}}{-1}).
##'
##' The sixth column is the temperature (Celsius).
##'
##'
##' @name obsBeaC
##' @docType data
##' @format Data frame of dimensions 35 by 6.
##' @source C. V. Beale, D. A. Bint, S. P. Long. 1996. Leaf photosynthesis in the
##' C4-grass Miscanthus x giganteus, growing in the cool temperate climate of
##' southern England. \emph{J. Exp. Bot.} 47 (2): 267--273.
##' @keywords datasets
NULL





##' Miscanthus assimilation field data
##'
##' Assimilation in Miscanthus as measured in Beale, Bint, and Long 1996.  The
##' first column is the observed net assimilation rate (\eqn{\mu} mol
##' m\eqn{^{-2}}{-2} s\eqn{^{-1}}{-1}).  The second column is the observed quantum flux
##' (\eqn{\mu} mol m\eqn{^{-2}}{-2} s\eqn{^{-1}}{-1}). The third column is the
##' temperature (Celsius).  Relative humidity was not reported and thus was
##' assumed to be 0.7.
##'
##'
##' @name obsBea
##' @docType data
##' @format Data frame of dimensions 27 by 4.
##' @source C. V. Beale, D. A. Bint, S. P. Long. 1996. Leaf photosynthesis in the
##' C4-grass Miscanthus x giganteus, growing in the cool temperate climate of
##' southern England. \emph{J. Exp. Bot.} 47 (2): 267--273.
##' @keywords datasets
NULL





##' Miscanthus assimilation data
##'
##' Assimilation in Miscanthus as measured in Naidu et al. (2003). The first
##' column is the observed net assimilation rate (\eqn{\mu} mol m\eqn{^{-2}}{-2}
##' s\eqn{^{-1}}{-1}).  The second column is the observed quantum flux (\eqn{\mu} mol
##' m\eqn{^{-2}}{-2} s\eqn{^{-1}}{-1}).  The third column is the temperature (Celsius).
##' The fourth column is the observed relative humidity in proportion (e.g.
##' 0.7).
##'
##'
##' @name obsNaid
##' @docType data
##' @format Data frame of dimensions 16 by 4.
##' @source S. L. Naidu, S. P. Moose, A. K. AL-Shoaibi, C. A. Raines, S. P.
##' Long.  2003. Cold Tolerance of C4 photosynthesis in Miscanthus x giganteus:
##' Adaptation in Amounts and Sequence of C4 Photosynthetic Enzymes.
##' \emph{Plant Physiol.} 132 (3): 1688--1697.
##' @keywords datasets
NULL





##' Simulated biomass data.
##'
##' Simulated data produced by \code{\link{BioGro}}.
##'
##' ~~ If necessary, more details than the description above ~~
##'
##' @name simDat2
##' @docType data
##' @format A data frame with 5 observations on the following 6 variables.
##' \describe{ \item{list('TT')}{A numeric vector.} \item{list('Stem')}{A
##' numeric vector.} \item{list('Leaf')}{A numeric vector.} \item{list('Root')}{A
##' numeric vector.} \item{list('Rhiz')}{A numeric vector.} \item{list('LAI')}{A
##' numeric vector.} }
##' @references ~~ possibly secondary sources and usages ~~
##' @source ~~ reference to a publication or URL from which the data were
##' obtained ~~
##' @keywords datasets
##' @examples
##'
##' data(simDat2)
##' ## maybe str(simDat2) ; plot(simDat2) ...
##'
NULL





##' Randomly picked dataset from the Illinois area from 1979
##'
##' Data from the Illinois area from one point from the 32km grid from NOAA
##' from 1979. The purpuse is to illustrate the \code{\link{Rmiscanmod}}
##' function.
##'
##'
##' @name WD1979
##' @docType data
##' @format A data frame with 365 observations on the following 9 variables.
##' \describe{ \item{list('year')}{Year.} \item{list('month')}{Month (not really
##' needed).} \item{list('day')}{Day of the month (not really needed).}
##' \item{list('JD')}{Day of the year (1--365).} \item{list('maxTemp')}{Maximum
##' temperature (Celsius).} \item{list('minTemp')}{Minimum temperature
##' (Celsius).} \item{list('SolarR')}{Solar radiation (MJ/m\eqn{{}^2}{2}).}
##' \item{list('PotEv')}{Potential evaporation (kg/m\eqn{{}^2}{2}). Approx. mm..}
##' \item{list('precip')}{Precipitation (kg/m\eqn{{}^2}{2}). Approx. mm.} }
##' @source \url{http://www.noaa.gov/}
##' @keywords datasets
##' @examples
##'
##' data(WD1979)
##' summary(WD1979)
##'
NULL





##' Weather data
##'
##' Weather data as produced by the \code{\link{weach}} function.  These are
##' for 2004 and 2005.
##'
##'
##' @name weather05
##' @aliases weather05 weather04
##' @docType data
##' @format Data frame of dimensions 8760 by 7.
##' @source Simulated (based on Champaign, Illinois conditions).
##' @keywords datasets
NULL





##' Weather data
##'
##' Weather data as produced by the \code{\link{weach}} function.  These are
##' for 2006.
##'
##' @name weather06
##' @docType data
##' @format A data frame with 8760 observations on the following 8 variables.
##' \describe{ \item{list('year')}{A numeric vector.} \item{list('doy')}{A
##' numeric vector.} \item{list('hour')}{A numeric vector.}
##' \item{list('SolarR')}{A numeric vector.} \item{list('Temp')}{A numeric
##' vector.} \item{list('RH')}{A numeric vector.} \item{list('WS')}{A numeric
##' vector.} \item{list('precip')}{A numeric vector.} }
##' @source %% ~~ reference to a publication or URL from which the data were
##' obtained ~~
##' @keywords datasets
##' @examples
##'
##' data(weather06)
##' ## maybe str(weather06) ; plot(weather06) ...
##'
NULL 
