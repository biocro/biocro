add_time_to_weather_data <- function(drivers, direct_modules = NULL)
{
    if ("doy" %in% names(drivers) &&
        "hour" %in% names(drivers) &&
        !"time" %in% names(drivers))
    {
        drivers$time <- 24 * (drivers$doy - 1) + drivers$hour
        drivers$doy  <- NULL
        drivers$hour <- NULL

        if (!is.null(direct_modules) &&
            !"BioCro:format_time" %in% direct_modules)
        {
            warning(
                "`doy` and `hour` have been removed by ",
                "`add_time_to_weather_data`. It is recommended to add ",
                "`BioCro:format_time` to the direct modules."
            )
        }
    }

    drivers
}
