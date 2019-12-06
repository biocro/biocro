get_growing_season_climate = function(climate, threshold_temperature=0) {
    doy_range = guess_growing_season_start_and_end_doy(climate, threshold_temperature)
    return(climate[with(climate, doy >= doy_range[1] & doy <= doy_range[2]), ])
}

guess_growing_season_start_and_end_doy = function(climate, threshold_temperature) {
    # Within the first half of the year, get the days on which the temperature was below 0.

    early_season_freezing_days = climate[with(climate, doy <= 183 & temp <= threshold_temperature), 'doy']
    day1 = max(c(early_season_freezing_days, 90))  # The first day of the season is the last freezing day, or day 90 at the earliest.

    # Within the second half of the year, get the days on which the temperature was below 0.
    late_season_freezing_days = climate[with(climate, doy > 183 & temp <= threshold_temperature), 'doy']
    dayn = min(c(late_season_freezing_days, 330))  # The last day of the season is the first freezing day, or day 330 at the latest.

    return(c(day1=day1, dayn=dayn))
}

