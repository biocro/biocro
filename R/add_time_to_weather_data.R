add_time_to_weather_data <- function(drivers)
{
    if ("doy" %in% names(drivers) &&
        "hour" %in% names(drivers) &&
        !"time" %in% names(drivers))
    {
        drivers$time <- 24 * (drivers$doy - 1) + drivers$hour
        drivers$doy  <- NULL
        drivers$hour <- NULL
    }

    drivers
}
