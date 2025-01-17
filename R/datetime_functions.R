setClass(
  '_biocro_time',
  slots = c(
    value = 'numeric',
    origin = 'integer'
    )
)

# USER CONSTRUCTOR
biocro_time <- function(time, origin=NULL){
    if (is.null(origin)) {

        origin <- attr(time, "origin")
        if (is.null(origin)) {
            stop("Must pass a non-null `origin` or `time` must have an `origin` attribute.")
        }
    }
    new(
        "_biocro_time",
        value = as.numeric(time),
        origin = as.integer(origin[[1]])
    )
}

setValidity("_biocro_time", function(object) {
  if (!is.numeric(object@value)) {
    return("@value must be numeric")
  }
  if (!is.integer(object@origin)) {
    return("@origin must be an integer")
  }
  if (length(object@origin) != 1) {
    return("@origin must be length == 1")
  }
  return(TRUE)
})



# ACCESSORS
setGeneric("get_origin", function(x) standardGeneric("get_origin"))
setMethod("get_origin", "_biocro_time", function(x) x@origin)

setGeneric("get_origin<-", function(x, value) standardGeneric("get_origin<-"))
setMethod("get_origin<-", "_biocro_time", function(x, value){x@origin <- value; x})

# R METHODS
print._biocro_time <- function(x){
    cat(
        "Hours since the year", get_origin(x), "begin:\n",
        sep = " "
    )
    print(x@value)
}

length._biocro_time <- function(x) {
    return(length(x@value))
}

setMethod("show", "_biocro_time", function(object) {
    print._biocro_time(object)
})


as.vector._biocro_time <- function(x, mode="any"){
    v <- as.vector(x@value, mode=mode)
    attr(v, 'origin') <- x@origin
    return(v)
}

as.data.frame._biocro_time <- function(x,
    row.names = NULL, optional=TRUE, ...)
{
    as.data.frame.vector(x, row.names = NULL, optional=TRUE, ...)
}


# CONVERSIONS TO TIME COMPONENTS
setGeneric("get_year", function(x) standardGeneric("get_year"))
setMethod("get_year", "_biocro_time", function(x) x@origin + x@value %/% 8765.82)

setGeneric("get_day_of_year", function(x) standardGeneric("get_day_of_year"))
setMethod("get_day_of_year", "_biocro_time",
    function(x) 1 + ((x@value %/% 24) %% days_in_year(get_year(x)))
)

setGeneric("get_hour_of_day", function(x) standardGeneric("get_hour_of_day"))
setMethod("get_hour_of_day", "_biocro_time", function(x) x@value %% 24 )

# HELPER FUNCTIONS
is_leap_year <- function(year){
    divisible_by_4 <- year %% 4 == 0
    divisible_by_100 <- year %% 100 == 0
    divisible_by_400 <- year %% 400 == 0
    divisible_by_4 & (!divisible_by_100 | divisible_by_400)
}

days_in_year <- function(year){
    return(365 + is_leap_year(year))
}


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
