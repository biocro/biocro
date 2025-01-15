datetime_from_time <- function(year, time, tz=''){
    start_date <- get_start_date(year, tz=tz)
    return(start_date + as.difftime(time, units='hours'))
}

datetime_from_day_of_year_and_hour <- function(year, doy, hour, tz=''){
    start_date <- get_start_date(year, tz=tz)
    days_since_start <- as.difftime(doy - 1, units='days')
    hours_since_midnight <- as.difftime(hour, units='hours')
    return(start_date + days_since_start + hours_since_midnight)
}

time_from_datetime <- function(date){
    date <- as.POSIXct(date)
    tz = attr(date, 'tzone')
    year <- 1900 + as.POSIXlt(date[1])$year
    start_date <- get_start_date(year,tz=tz)
    difftime(date, start_date, units='hours') |> as.numeric()
}

day_of_year_and_hour_from_datetime <- function(date){
    num = length(date)
    date <- as.POSIXct(date)
    tz = attr(date, 'tzone')
    year <- 1900 + as.POSIXlt(date[1])$year
    start_date <- get_start_date(year,tz=tz)
    dt = difftime(date, start_date)
    days = as.difftime(dt, units='days') |> floor()
    list(
        year=rep(year, num),
        doy = 1 + days,
        hour =  as.difftime(dt, units='hours') - 24 * days
    )
}

get_day_of_year_from_date <- function(date){
    year <- 1900 + as.POSIXlt(date[1])$year
    start_date <- get_start_date(year)
    1 + difftime(date, start_date, units='days') |> as.numeric()
}

get_date_from_day_of_year <- function(year, doy){
    start_date <- get_start_date(year)
    start_date + as.difftime(doy - 1, units='days')
}

get_start_date <- function(year, tz=''){
    paste0(year, "-01-01") |> as.POSIXct(tz=tz)
}
