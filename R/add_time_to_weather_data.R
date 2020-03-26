add_time_to_weather_data <- function(weather_data)
{
	weather_data$doy_dbl <- weather_data$doy + weather_data$hour / 24.0
	return(weather_data)
}