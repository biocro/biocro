add_time_to_weather_data <- function(drivers)
{
    if ("doy" %in% names(drivers) &&
        "hour" %in% names(drivers) &&
        !"time" %in% names(drivers))
    {
        hours_per_day <- 24
        # time = 0 is equivalent to Jan 1, 00:00:00
        drivers[['time']] <- (drivers$doy - 1) * hours_per_day + drivers$hour
    }

    drivers
}
