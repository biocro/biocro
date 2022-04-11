# Makes sure the convenience functions for partial application are functioning
# properly

context("Test partial application")

# TESTING PARTIAL_RUN_BIOCRO

# Get the baseline simulation result, which is based on the 2005 Miscanthus
# simulation, but where we have changed the atmospheric CO2 concentration and
# all hourly air temperatures
weather <- get_growing_season_climate(weather2005)
new_catm <- 500
temp_offset <- 1

baseline_rb_result <- run_biocro(
    miscanthus_x_giganteus_initial_values,
    within(miscanthus_x_giganteus_parameters, {Catm = new_catm}),
    within(weather, {temp = temp + temp_offset}),
    miscanthus_x_giganteus_direct_modules,
    miscanthus_x_giganteus_differential_modules,
    miscanthus_x_giganteus_ode_solver
)

# Now use partial application
miscanthus_func <- partial_run_biocro(
    miscanthus_x_giganteus_initial_values,
    miscanthus_x_giganteus_parameters,
    weather,
    miscanthus_x_giganteus_direct_modules,
    miscanthus_x_giganteus_differential_modules,
    miscanthus_x_giganteus_ode_solver,
    c("Catm", "temp")
)

# Specify the values for Catm and temp in several ways, checking to make sure
# the results are the same as the baseline
rb_x_vals <- list(
    c(new_catm, weather$temp + temp_offset),
    list(Catm = new_catm, temp = weather$temp + temp_offset),
    list(temp = weather$temp + temp_offset, Catm = new_catm)
)

for (i in seq_along(rb_x_vals)) {
    msg <- paste("Testing `partial_run_biocro` with input case", i)
    test_that(paste("Testing `partial_run_biocro` with input case", i), {
        expect_equal(miscanthus_func(rb_x_vals[[i]])$Leaf, baseline_rb_result$Leaf)
    })
}

# Make sure errors are reported when expected
test_that("partial_run_biocro produces error messages when expected", {
    expect_error(
        miscanthus_func(list(Catm = new_catm, weather$temp + temp_offset))
    )

    expect_error(
        miscanthus_func(c(Catm = new_catm, temp = weather$temp + temp_offset))
    )

    expect_error(
        miscanthus_func(setNames(
            c(500, soybean_weather2002$temp + 2.0),
            c("Catm", rep("temp", length(soybean_weather2002$temp)))
        ))
    )
})

# TESTING PARTIAL_EVALUATE_MODULE
module <- 'BioCro:thermal_time_linear'

# Get the baseline result
tbase = 10
temp = 20
baseline_em_result <- evaluate_module(
    module,
    list(sowing_time = 0, tbase = tbase, temp = temp, time = 1)
)

# Now use partial application
ttc_func <- partial_evaluate_module(
    module,
    list(sowing_time = 0, time = 1),
    c("tbase", "temp")
)

# Specify the values for Catm and temp in several ways, checking to make sure
# the results are the same as the baseline
em_x_vals <- list(
    c(tbase, temp),
    c(tbase = tbase, temp = temp),
    c(temp = temp, tbase = tbase),
    list(tbase = tbase, temp = temp),
    list(temp = temp, tbase = tbase)
)

for (i in seq_along(em_x_vals)) {
    msg <- paste("Testing `partial_evaluate_module` with input case", i)
    test_that(paste("Testing `partial_evaluate_module` with input case", i), {
        expect_equal(ttc_func(em_x_vals[[i]])$outputs, baseline_em_result)
    })
}

# Make sure errors are reported when expected
test_that("partial_evaluate_module produces error messages when expected", {
    expect_error(
        ttc_func(c(tbase = tbase, temp))
    )
})
