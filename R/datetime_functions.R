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
setMethod("[", "_biocro_time",  function(x, i, j, ..., drop=TRUE) x@value[i])
setMethod("[<-", "_biocro_time", function(x, i, j, ..., value) {x@value[i] <- value; x})



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

as.numeric._biocro_time <- function(x){
    x@value
}

as.vector._biocro_time <- function(x, mode="any"){
    v <- as.vector(x@value, mode=mode)
    attr(v, 'origin') <- x@origin
    return(v)
}

as.data.frame._biocro_time <- function (x,
    row.names = NULL, optional = FALSE, ..., nm = deparse1(substitute(x)))
{
    force(nm)
    x <- as.vector(x)
    nrows <- length(x)
    if (is.null(row.names)) {
        if (nrows == 0L)
            row.names <- character()
        else if (length(row.names <- names(x)) != nrows || anyDuplicated(row.names))
            row.names <- .set_row_names(nrows)
    }
    else if (!(is.character(row.names) || is.integer(row.names)) ||
        length(row.names) != nrows)
        stop(gettextf("'row.names' is not a character or integer vector of length %d",
            nrows), domain = NA)
    if (!is.null(names(x)))
        names(x) <- NULL
    value <- list(x)
    if (!optional)
        names(value) <- nm
    structure(value, row.names = row.names, class = "data.frame")
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

setGeneric("get_datetime", function(x, tz='') standardGeneric("get_datetime"))
setMethod("get_datetime", "_biocro_time",
    function(x, tz='') {
        as.POSIXct(paste0(x@origin, '-01-01'), tz=tz) + as.difftime(x@value, units='hours')
        })


as.POSIXct._biocro_time <- function(x, tz= '') get_datetime(x, tz)
