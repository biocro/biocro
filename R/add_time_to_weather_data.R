add_time_to_weather_data <- function(drivers)
{
    if ("doy" %in% names(drivers) &&
        "hour" %in% names(drivers) &&
        !"time" %in% names(drivers))
    {
        day_fraction <- drivers$hour
        # time = 0 is equivalent to Jan 1, 00:00:00
        days_per_hour <- 24
        drivers['time'] <- (drivers$doy-1)*days_per_hour  + day_fraction
    }

    drivers
}
