#' Add a time component to input
#'
#' @description Ensure, if possible, that input data that varies over
#'     time has a "time" component.
#'
#' @param varying_parameters A list or dataframe representing known
#'     system parameters that vary over time, such as weather data.
#'
#' @return If varying_parameters has no time component, then one is
#'     added, provided it _does_ have components for doy and
#'     hour. (The time values will be equal to the doy values plus
#'     the fractional portion of a day represented by the hour
#'     values.)  Otherwise varying_parameters is returned as is.
#'
#' @note Preconditions: If varying_parameters is a list, the values
#'     should be vectors of equal length.  Moreover, if it already
#'     contains a time component, then it shouldn't contain either
#'     a doy or an hour component unless it contains both of them and
#'     the values are mutually consistent.  Furthermore, the time
#'     represented by (doy, hour), if given, or by time, if given,
#'     should increase as the vector or row index increases.
#' @md
add_time_to_weather_data <- function(varying_parameters)
{
    if ("doy" %in% names(varying_parameters) &&
        "hour" %in% names(varying_parameters) &&
        !"time" %in% names(varying_parameters))
    {
        day_fraction <- varying_parameters$hour / 24.0
        varying_parameters$time <- varying_parameters$doy + day_fraction
    }

    varying_parameters
}
