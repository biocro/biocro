##' Simple, Fast Daily to Hourly Climate Downscaling
##'
##' Based on weach family of functions but 5x faster than weachNEW,
##' and requiring metric units (temperature in celsius, windspeed in kph,
##' precip in mm, relative humidity as fraction)
##' @title weachDT
##' @param X data table with climate variables
##' @param lati latitude (for calculating solar radiation)
##' @export
##' @return weather file for input to BioGro and related crop growth functions
##' @author David LeBauer

.datatable.aware = NULL  # Needed for the ridiculous way that the data.table package works.

weach.data.table <- function(X, ...) {
	lati = list(...)$lati
	if (!requireNamespace('data.table')) stop('in weach.data.table: this function requires the data.tables package')
	if (!data.table::is.data.table(X)) stop('in weach.data.table: X must be a data table')
	assignInMyNamespace('.datatable.aware', TRUE)  # The ridiculous way to get data.table to work without depending on it.
	on.exit(assignInMyNamespace('.datatable.aware', FALSE))

    tint <- 24
    tseq <- 0:23
    ## Solar Radiation

    if("day" %in% colnames(X)) data.table::setnames(X, "day", "doy")
    data.table::setkeyv(X, c("year", "doy"))

#    solarR <-  X[,list(solarR = rep((0.12 * dswrf.MJ) * 2.07 * 10^6 / 3600, each = tint) ), by = c("year", "doy")]
    solarR <-  data.table:::`[.data.table`(X, , list(solarR = rep((0.12 * dswrf.MJ) * 2.07 * 10^6 / 3600, each = tint)), by = c("year", "doy"))
    
    light <- data.table:::`[.data.table`(X, , lightME(DOY = doy, t.d = tseq, lat = 40), by = c("year", "doy"))

    light$Itot <- data.table:::`[.data.table`(light, , list(I.dir + I.diff))
    resC2 <- data.table:::`[.data.table`(light, , list(resC2 = (Itot - min(Itot)) / max(Itot)), by = c("year", "doy")) 
    
    SolarR <- data.table:::`[.data.table`(cbind(resC2, solarR), , list(SolarR = solarR * resC2))

    ## Temperature
    Temp <- data.table:::`[.data.table`(X, , list(Temp = tmin + (sin(2*pi*(tseq-10)/tint) + 1)/2 * (tmax - tmin)), by = c("year", "doy"))
	Temp <- data.table:::`[.data.table`(Temp, , list(Temp))

    ## Relative Humidity
    rhscale <- (cos(2 * pi * (tseq - 10) / tint) + 1) / 2
    RH <- data.table:::`[.data.table`(X, , list(RH = rhmin + rhscale * (rhmax - rhmin)), by = c("year", "doy"))
    RH <- data.table:::`[.data.table`(RH, , list(RH))

    ## Wind Speed
    WS <- rep(X$wnd, each = tint)

    ## Precipitation
    precip <- rep(X$precip/tint, each = tint)

    ## Hour
    time <- data.table:::`[.data.table`(X, , list(hour = tseq), by = c("year", "doy"))
    
    ans <- cbind(time, SolarR, Temp, RH, WS, precip)
    return(ans)
}
    
