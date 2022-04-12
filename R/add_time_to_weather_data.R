add_time_to_weather_data <- function(drivers)
{
    if ("doy" %in% names(drivers) &&
        "hour" %in% names(drivers) &&
        !"time" %in% names(drivers))
    {
        day_fraction <- drivers$hour / 24.0
        drivers$time <- drivers$doy + day_fraction
    }

    drivers
}
