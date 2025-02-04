test_that('Years in weather data are correct', {
    # Check the CMI weather
    years_from_names <- as.numeric(names(weather))

    years_from_df <- sapply(weather, function(x) {
        as.numeric(x[1, 'year'])
    })

    names(years_from_df) <- NULL

    expect_equal(
        years_from_names,
        years_from_df
    )

    # Check the soybean weather
    soybean_years_from_names <- as.numeric(names(soybean_weather))

    soybean_years_from_df <- sapply(soybean_weather, function(x) {
        as.numeric(x[1, 'year'])
    })

    names(soybean_years_from_df) <- NULL

    expect_equal(
        soybean_years_from_names,
        soybean_years_from_df
    )
})

test_that('Years of weather data are all different', {
    # Check the CMI weather
    weather_names <- names(weather)

    weather_combinations <- combn(weather_names, 2, simplify = FALSE)

    weather_identical <- sapply(weather_combinations, function(x) {
        year1 <- x[1]
        year2 <- x[2]

        weather1 <- weather[[year1]]
        weather2 <- weather[[year2]]

        if (all(dim(weather1) == dim(weather2))) {
            all(weather1 == weather2)
        } else {
            FALSE
        }
    })

    expect_true(
        all(!weather_identical)
    )

    # Check the soybean weather
    soybean_weather_names <- names(soybean_weather)

    soybean_weather_combinations <- combn(soybean_weather_names, 2, simplify = FALSE)

    soybean_weather_identical <- sapply(soybean_weather_combinations, function(x) {
        year1 <- x[1]
        year2 <- x[2]

        soybean_weather1 <- soybean_weather[[year1]]
        soybean_weather2 <- soybean_weather[[year2]]

        if (all(dim(soybean_weather1) == dim(soybean_weather2))) {
            all(soybean_weather1 == soybean_weather2)
        } else {
            FALSE
        }
    })

    expect_true(
        all(!soybean_weather_identical)
    )
})
