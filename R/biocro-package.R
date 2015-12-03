

##' A/Ci curves
##'
##' Four A/Ci (assimilation vs. intercellular CO2) curves.
##'
##' Measurements taken on Miscanthus x giganteus.
##'
##' @name aci
##' @docType data
##' @format A data frame with 32 observations on the following 7 variables.
##' \describe{ \item{list('ID')}{Identification for each curve. a numeric
##' vector} \item{list('Photo')}{Assimilation. a numeric vector}
##' \item{list('PARi')}{Incident Photosynthetic Active Radiation. a numeric
##' vector} \item{list('Tleaf')}{Temperature of the leaf. a numeric vector}
##' \item{list('RH_S')}{Realtive humidity. a numeric vector}
##' \item{list('Ci')}{Intercellular CO2. a numeric vector}
##' \item{list('CO2_R')}{Reference CO2. a numeric vector} }
##' @references Dandan Wang
##' @source Measurements taken by Dandan Wang.
##' @keywords datasets
##' @examples
##'
##' data(aci)
##' plotAC(aci)
##'
NULL





##' Miscanthus dry biomass data.
##'
##' The first column is the thermal time. The second, third, fourth and fifth
##' columns are miscanthus stem, leaf, root and rhizome dry biomass in Mg
##' ha\eqn{^{-1}} (root is missing). The sixth column is the leaf area index.
##' The \code{annualDB.c} version is altered so that root biomass is not
##' missing and LAI is smaller. The purpose of this last modification is for
##' testing some functions.
##'
##'
##' @name annualDB
##' @aliases annualDB annualDB2
##' @docType data
##' @format data frame of dimensions 5 by 6.
##' @source Clive Beale and Stephen Long. (1997) Seasonal dynamics of nutrient
##' accumulation and partitioning in the perennial C4 grasses Miscanthus x
##' giganteus and Spartina cynosuroides. Biomass and Bioenergy. 419-428.
##' @keywords datasets
NULL





##' A/Q curves
##'
##' Example of A/Q curves which serves as a template for using the
##' \code{\link{Opc4photo}} and \code{\link{mOpc4photo}} functions.
##'

##'
##' \code{swg} stand for switchgrass (Panicum virgatum) \code{mxg} stands for
##' miscanthus (Miscanthus x gignateus)
##'
##' %% ~~ If necessary, more details than the __description__ above ~~
##'
##' @name aq
##' @docType data
##' @format A data frame with 64 observations on the following 6 variables.
##' \describe{ \item{list('ID')}{a numeric vector} \item{list('trt')}{a factor
##' with levels \code{mxg} \code{swg}} \item{list('A')}{a numeric vector.
##' Assimilation} \item{list('PARi')}{a numeric vector. Photosynthetic Active
##' Radiation (incident).} \item{list('Tleaf')}{a numeric vector. Temperature
##' of the leaf.} \item{list('RH_S')}{a numeric vector. Relative humidity
##' (fraction).} }
##' @references Dandan Wang %% ~~ possibly secondary sources and usages ~~
##' @source Data based on measurements made by Dandan Wang. %% ~~ reference to
##' a publication or URL from which the data were obtained ~~
##' @keywords datasets
##' @examples
##'
##' data(aq)
##' plotAQ(aq)
##'
NULL




##' Weather data
##'
##' selected weather data corresponding to the Champaign weather station (IL,
##' U.S.). It has two years: 2005 and 2006. Dimensions: 730 by 11. The columns
##' correspond to the input necessary for the \code{\link{weach}} function.
##'
##'
##' @name cmi0506
##' @docType data
##' @format data frame of dimensions 730 by 11.
##' @source \url{http://www.sws.uiuc.edu/warm/}
##' @keywords datasets
NULL





##' Weather data
##'
##' Layer data for evapo/transpiration. Simulated data to show the result of
##' the EvapoTrans function.
##'
##' lfClass: leaf class, 'sun' or 'shade'.
##'
##' layer: layer in the canopy, 1 to 8.
##'
##' hour: hour of the day, (0--23).
##'
##' Rad: direct light.
##'
##' Itot: total radiation.
##'
##' Temp: air temperature, (Celsius).
##'
##' RH: relative humidity, (0--1).
##'
##' WindSpeed: wind speed, (m \eqn{s^{-1}}).
##'
##' LAI: leaf area index.
##'
##'
##' @name cmiWet
##' @docType data
##' @format data frame of dimensions 384 by 9.
##' @source simulated
##' @keywords datasets
NULL





##' Weather data
##'
##' Example for a given day of the year to illustrate the \code{\link{CanA}}
##' function.
##'
##' LAI: leaf area index.
##'
##' year: year.
##'
##' doy: 124 in this case.
##'
##' hour: hour of the day, (0--23).
##'
##' solarR: direct solar radiation.
##'
##' DailyTemp.C: hourly air temperature (Celsius).
##'
##' RH: relative humidity, (0--1).
##'
##' WindSpeed: 4.1 m \eqn{s^{-1}} average daily value in this case.
##'
##'
##' @name doy124
##' @docType data
##' @format data frame of dimensions 24 by 8.
##' @source simulated
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
##' \describe{ \item{list('year')}{a numeric vector} \item{list('doy')}{a
##' numeric vector} \item{list('hour')}{a numeric vector}
##' \item{list('solarR')}{a numeric vector} \item{list('DailyTemp.C')}{a
##' numeric vector} \item{list('RH')}{a numeric vector}
##' \item{list('WindSpeed')}{a numeric vector} \item{list('precip')}{a numeric
##' vector} }
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
##' \describe{ \item{list('year')}{a numeric vector} \item{list('doy')}{a
##' numeric vector} \item{list('hour')}{a numeric vector}
##' \item{list('solarR')}{a numeric vector} \item{list('DailyTemp.C')}{a
##' numeric vector} \item{list('RH')}{a numeric vector}
##' \item{list('WindSpeed')}{a numeric vector} \item{list('precip')}{a numeric
##' vector} }
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
##' lfClass: leaf class, 'sun' or 'shade'.
##'
##' layer: layer in the canopy, 1 to 8.
##'
##' hour: hour of the day, (0--23).
##'
##' Rad: direct light.
##'
##' Itot: total radiation.
##'
##' Temp: air temperature, (Celsius).
##'
##' RH: relative humidity, (0--1).
##'
##' WindSpeed: wind speed, (m \eqn{s^{-1}}).
##'
##' LAI: leaf area index.
##'
##'
##' @name LayET
##' @docType data
##' @format data frame of dimensions 384 by 9.
##' @source simulated
##' @keywords datasets
NULL





##' Complete Miscanthus assimilation field data
##'

##'
##' Assimilation and stomatal conductance as measured in Beale, Bint and Long
##' (1996) in Miscanthus (missing data are also included).  The first column is
##' the date, the second the hour. Columns 3 and 4 are assimilation and
##' stomatal conductance respectively.
##'
##' The third column is the observed net assimilation rate (\eqn{\mu} mol
##' \eqn{m^{-2}} \eqn{s^{-1}}).
##'
##' The fifth column is the observed quantum flux (\eqn{\mu} mol \eqn{m^{-2}}
##' \eqn{s^{-1}}).
##'
##' The sixth column is the temperature (Celsius).
##'
##'
##' @name obsBeaC
##' @docType data
##' @format data frame of dimensions 35 by 6.
##' @source C. V. Beale, D. A. Bint, S. P. Long, Leaf photosynthesis in the
##' C4-grass miscanthus x giganteus, growing in the cool temperate climate of
##' southern England, \emph{J. Exp. Bot.} 47 (2) (1996) 267--273.
##' @keywords datasets
NULL





##' Miscanthus assimilation field data
##'
##' assimilation as measured in Beale, Bint and Long (1996) in Miscanthus.  The
##' first column is the observed net assimilation rate (\eqn{\mu} mol
##' \eqn{m^{-2}} \eqn{s^{-1}}).  The second column is the observed quantum flux
##' (\eqn{\mu} mol \eqn{m^{-2}} \eqn{s^{-1}}). The third column is the
##' temperature (Celsius).Relative humidity was not reported and thus was
##' assumed to be 0.7.
##'
##'
##' @name obsBea
##' @docType data
##' @format data frame of dimensions 27 by 4.
##' @source C. V. Beale, D. A. Bint, S. P. Long, Leaf photosynthesis in the
##' C4-grass miscanthus x giganteus, growing in the cool temperate climate of
##' southern England, \emph{J. Exp. Bot.} 47 (2) (1996) 267--273.
##' @keywords datasets
NULL





##' Miscanthus assimilation data
##'
##' assimilation as measured in Naidu et al. (2003) in Miscanthus. The first
##' column is the observed net assimilation rate (\eqn{\mu} mol \eqn{m^{-2}}
##' \eqn{s^{-1}}) The second column is the observed quantum flux (\eqn{\mu} mol
##' \eqn{m^{-2}} \eqn{s^{-1}}) The third column is the temperature (Celsius).
##' The fourth column is the observed relative humidity in proportion (e.g.
##' 0.7).
##'
##'
##' @name obsNaid
##' @docType data
##' @format data frame of dimensions 16 by 4.
##' @source S. L. Naidu, S. P. Moose, A. K. AL-Shoaibi, C. A. Raines, S. P.
##' Long, Cold Tolerance of C4 photosynthesis in Miscanthus x giganteus:
##' Adaptation in Amounts and Sequence of C4 Photosynthetic Enzymes,
##' \emph{Plant Physiol.} 132 (3) (2003) 1688--1697.
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
##' \describe{ \item{list('TT')}{a numeric vector} \item{list('Stem')}{a
##' numeric vector} \item{list('Leaf')}{a numeric vector} \item{list('Root')}{a
##' numeric vector} \item{list('Rhiz')}{a numeric vector} \item{list('LAI')}{a
##' numeric vector} }
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
##' from 1979. the purpuse is to illustrate the \code{\link{Rmiscanmod}}
##' function.
##'
##'
##' @name WD1979
##' @docType data
##' @format A data frame with 365 observations on the following 9 variables.
##' \describe{ \item{list('year')}{year} \item{list('month')}{month (not really
##' needed)} \item{list('day')}{day of the month (not really needed)}
##' \item{list('JD')}{day of the year (1-365)} \item{list('maxTemp')}{maximum
##' temperature (Celsius)} \item{list('minTemp')}{minimum temperature
##' (Celsius)} \item{list('SolarR')}{solar radiation (MJ/m2)}
##' \item{list('PotEv')}{potential evaporation (kg/m2). Approx. mm.}
##' \item{list('precip')}{precipitation (kg/m2). Approx. mm.} }
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
##' @format data frame of dimensions 8760 by 7.
##' @source simulated (based on Champaign, Illinois conditions).
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
##' \describe{ \item{list('year')}{a numeric vector} \item{list('doy')}{a
##' numeric vector} \item{list('hour')}{a numeric vector}
##' \item{list('SolarR')}{a numeric vector} \item{list('Temp')}{a numeric
##' vector} \item{list('RH')}{a numeric vector} \item{list('WS')}{a numeric
##' vector} \item{list('precip')}{a numeric vector} }
##' @source %% ~~ reference to a publication or URL from which the data were
##' obtained ~~
##' @keywords datasets
##' @examples
##'
##' data(weather06)
##' ## maybe str(weather06) ; plot(weather06) ...
##'
NULL 
