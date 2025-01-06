get_growing_season_climate <- function(climate, threshold_temperature = 0) {
    doy_range <-
        guess_growing_season_start_and_end_doy(climate, threshold_temperature)

    out <- climate[with(climate, doy >= doy_range[1] & doy <= doy_range[2]), ]
    out <- add_time_to_weather_data(out)
    return(out)
}

# This is a helper function for `get_growing_season_climate`.
#
# Within the first half of the year, get the days on which the temperature was
# below (or equal to) the threshold temperature. The first day of the season is
# the last day meeting that criterion, or day 90 at the earliest.
#
# Within the second half of the year, get the days on which the temperature was
# below (or equal to) the threshold temperature. The last day of the season is
# the first day meeting that criterion, or day 330 at the latest.
#
# This function returns the first and last days as a two-element numeric vector.
guess_growing_season_start_and_end_doy <- function(
    climate,
    threshold_temperature
)
{
    early_season_cold_days <-
        climate[with(climate, doy <= 183 & temp <= threshold_temperature), 'doy']

    day1 = max(c(early_season_cold_days, 90))

    late_season_cold_days <-
        climate[with(climate, doy > 183 & temp <= threshold_temperature), 'doy']

    dayn = min(c(late_season_cold_days, 330))

    return(c(day1, dayn))
}
