short_weather <- soybean_weather$'2002'[seq_len(36), ]

sho_model <- list(
    initial_values = list(position = 1, velocity = 0),
    parameters = list(spring_constant = 1, mass = 1, timestep=1),
    direct_module_names = list(),
    differential_module_names = list("BioCro:harmonic_oscillator"),
    ode_solver = soybean$ode_solver
)

test_that('doy and hour are unchanged with default settings', {
    default_result <- with(soybean, {run_biocro(
      initial_values,
      parameters,
      short_weather,
      direct_modules,
      differential_modules,
      ode_solver
    )})

    # Make sure time columns are included in the output
    expect_true('time' %in% colnames(default_result))
    expect_true('doy'  %in% colnames(default_result))
    expect_true('hour' %in% colnames(default_result))

    # Make sure `doy` and `hour` values are the same. Here we need to remove
    # row names from the data frame subsets or the comparison will fail.
    tcol <- c('doy', 'hour')

    driver_time <- short_weather[, tcol]
    result_time <- default_result[, tcol]

    rownames(driver_time) <- NULL
    rownames(result_time) <- NULL

    expect_equal(
        result_time,
        driver_time
    )
})

test_that('doy is always an integer', {
    half_hour_result <- with(soybean, {run_biocro(
      initial_values,
      parameters,
      short_weather,
      direct_modules,
      differential_modules,
      within(ode_solver, {output_step_size = 0.5})
    )})

    expect_equal(
        unique(half_hour_result$doy),
        c(152, 153)
    )

    expect_equal(
        unique(half_hour_result$hour),
        seq(0, 23.999, by = 0.5)
    )
})

test_that('doy and hour are not gratuitously added to run_biocro results', {
    sho_result <- with(sho_model, {run_biocro(
        initial_values,
        parameters,
        drivers = data.frame(time = seq(0, 10, 1)),
        direct_module_names,
        differential_module_names,
        ode_solver
    )})

    expect_false('doy'  %in% colnames(sho_result))
    expect_false('hour' %in% colnames(sho_result))
})

test_that('The BioCro:format_time module is added when necessary', {
    direct_modules <- soybean$direct_modules
    direct_modules <- direct_modules[direct_modules != 'BioCro:format_time']

    res <- expect_silent(
        with(soybean, {run_biocro(
          initial_values,
          parameters,
          short_weather,
          direct_modules,
          differential_modules,
          ode_solver
        )})
    )

    expect_true('doy'  %in% colnames(res))
    expect_true('hour' %in% colnames(res))
})
