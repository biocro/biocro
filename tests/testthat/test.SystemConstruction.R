# This tests some basic properties of system construction

CROP <- miscanthus_x_giganteus
WEATHER <- get_growing_season_climate(weather$'2005')
MAX_INDEX <- 100
VERBOSE <- FALSE

test_that("drivers must not be empty", {
    expect_error(
        run_biocro(drivers = data.frame()),
        regexp = "The drivers cannot be empty"
    )

    expect_error(
        validate_dynamical_system_inputs(
            drivers = data.frame(),
            verbose = VERBOSE
        ),
        regexp = "The drivers cannot be empty"
    )
})

test_that("certain run_biocro inputs must be lists or data frames", {
    expect_error(
        with(CROP, {run_biocro(
            unlist(initial_values),
            unlist(parameters),
            unlist(WEATHER),
            direct_modules,
            differential_modules,
            unlist(ode_solver['type'])
        )}),
        regexp = paste0(
            "`initial_values` must be a list.\n",
            "  `parameters` must be a list.\n",
            "  `ode_solver` must be a list.\n",
            "  `drivers` must be a data frame.\n"
        )
    )
})

test_that("certain run_biocro inputs must not have empty elements", {
    expect_error(
        with(CROP, {run_biocro(
            within(initial_values, {bad_initial_value = numeric(0)}),
            within(parameters, {bad_parameter = numeric(0)}),
            WEATHER,
            direct_modules,
            differential_modules,
            within(ode_solver, {bad_ode_solver_setting = numeric(0)})
        )}),
        regexp = paste0(
            "The following `initial_values` members have lengths other than 1, but all members must have a length of exactly 1: bad_initial_value.\n",
            "  The following `parameters` members have lengths other than 1, but all members must have a length of exactly 1: bad_parameter.\n",
            "  The following `ode_solver` members have lengths other than 1, but all members must have a length of exactly 1: bad_ode_solver_setting.\n"
        )
    )
})

test_that("certain run_biocro inputs must not have elements with length > 1", {
    expect_error(
        with(CROP, {run_biocro(
            within(initial_values, {bad_initial_value = c(1,2)}),
            within(parameters, {bad_parameter = c(1,2)}),
            WEATHER,
            direct_modules,
            differential_modules,
            within(ode_solver, {bad_ode_solver_setting = c(1,2)})
        )}),
        regexp = paste0(
            "The following `initial_values` members have lengths other than 1, but all members must have a length of exactly 1: bad_initial_value.\n",
            "  The following `parameters` members have lengths other than 1, but all members must have a length of exactly 1: bad_parameter.\n",
            "  The following `ode_solver` members have lengths other than 1, but all members must have a length of exactly 1: bad_ode_solver_setting.\n"
        )
    )
})

test_that("certain run_biocro inputs must be strings", {
    expect_error(
        with(CROP, {run_biocro(
            initial_values,
            parameters,
            WEATHER,
            append(direct_modules, 1.23),
            append(differential_modules, 4.56),
            within(ode_solver, {type = 7.89})
        )}),
        regexp = paste0(
            "The following `direct_module_names` members are not strings, but all members must be strings: 1.23.\n",
            "  The following `differential_module_names` members are not strings, but all members must be strings: 4.56.\n",
            "  The following `ode_solver_type` members are not strings, but all members must be strings: 7.89.\n"
        )
    )
})

test_that("certain run_biocro inputs must be numeric", {
    expect_error(
        with(CROP, {run_biocro(
            within(initial_values, {bad_initial_value = "terrible"}),
            within(parameters, {bad_parameter = "awful"}),
            within(WEATHER, {bad_driver = "offensive"}),
            direct_modules,
            differential_modules,
            within(ode_solver, {bad_ode_solver_setting = "heinous"})
        )}),
        regexp = paste0(
            "The following `initial_values` members are not numeric or NA, but all members must be numeric or NA: bad_initial_value.\n",
            "  The following `parameters` members are not numeric or NA, but all members must be numeric or NA: bad_parameter.\n",
            "  The following `drivers` members are not numeric or NA, but all members must be numeric or NA: bad_driver.\n",
            "  The following `ode_solver_other_than_type` members are not numeric or NA, but all members must be numeric or NA: bad_ode_solver_setting.\n"
        )
    )
})

test_that("element names are not repeated", {
    expect_error(
        with(CROP, {run_biocro(
            c(initial_values, list(TTc = 1e3)),
            c(parameters, list(Catm = 2e3)),
            cbind(WEATHER, temp = WEATHER$temp + 3e3),
            c(direct_modules, list(canopy_photosynthesis = 'BioCro:c3_canopy')),
            c(differential_modules, list(thermal_time = 'BioCro:thermal_time_bilinear')),
            c(ode_solver, list(type = 'boost_euler'))
        )}),
        regexp = paste0(
            "`initial_values` contains multiple instances of some quantities:\n",
            "    `TTc` takes the following values:\n",
            "      0\n",
            "      1000\n",
            "  `parameters` contains multiple instances of some quantities:\n",
            "    `Catm` takes the following values:\n",
            "      400\n",
            "      2000\n",
            "  `drivers` contains multiple instances of some quantities:\n",
            "    `temp` takes the following values:\n",
            "      2.12, 1.72, 1.61, 1.13, 0.47, ...\n",
            "      3002.12, 3001.72, 3001.61, 3001.13, 3000.47, ...\n",
            "  `direct_module_names` contains multiple instances of some quantities:\n",
            "    `canopy_photosynthesis` takes the following values:\n",
            "      BioCro:c4_canopy\n",
            "      BioCro:c3_canopy\n",
            "  `differential_module_names` contains multiple instances of some quantities:\n",
            "    `thermal_time` takes the following values:\n",
            "      BioCro:thermal_time_linear\n",
            "      BioCro:thermal_time_bilinear\n",
            "  `ode_solver` contains multiple instances of some quantities:\n",
            "    `type` takes the following values:\n",
            "      homemade_euler\n",
            "      boost_euler\n"
        )
    )
})

test_that("All modules must exist", {
    # TO-DO: move this test to a module-library-specific testing file

    # This should throw an error
    expect_error(BioCro:::check_out_module("BioCro:module_that_does_not_exist"))

})

test_that("Duplicated quantities produce an error during validation", {
    # Set up some bad inputs for a harmonic oscillator

    initial_values <- list(
        position = 1,   # defined for the first time
        velocity = 0
    )

    parameters <- list(
        mass = 1,
        spring_constant = 1,
        timestep = 1,
        position = 1    # defined for the second time
    )

    drivers <- data.frame(
        time=seq(from=0, by=1, length=MAX_INDEX)
    )

    direct_module_names <- "BioCro:harmonic_energy"

    differential_module_names <- "BioCro:harmonic_oscillator"

    # Validation should return FALSE

    expect_false(validate_dynamical_system_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names,
        verbose=VERBOSE
    ))

})

test_that("Missing inputs produce an error during validation", {
    # Set up some bad inputs for a harmonic oscillator

    initial_values <- list(
        position = 1,
        velocity = 0
    )

    parameters <- list(
        mass = 1,
        #spring_constant = 1,   # not defined
        timestep = 1
    )

    drivers <- data.frame(
        time=seq(from=0, by=1, length=MAX_INDEX)
    )

    direct_module_names <- "BioCro:harmonic_energy"

    differential_module_names <- "BioCro:harmonic_oscillator"

    # Validation should return FALSE

    expect_false(validate_dynamical_system_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names,
        verbose=VERBOSE
    ))

})

test_that("Differential modules only supply derivatives for quantities in the initial values", {
    # Set up some bad inputs for a harmonic oscillator

    initial_values <- list(
        position = 1
    )

    parameters <- list(
        mass = 1,
        spring_constant = 1,
        timestep = 1,
        velocity = 0    # should be in the initial values instead
    )

    drivers <- data.frame(
        time=seq(from=0, by=1, length=MAX_INDEX)
    )

    direct_module_names <- "BioCro:harmonic_energy"

    differential_module_names <- "BioCro:harmonic_oscillator"

    # Validation should return FALSE

    expect_false(validate_dynamical_system_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names,
        verbose=VERBOSE
    ))

})

test_that("Direct modules are not required to be supplied in the correct order", {
    # Set up some bad inputs

    initial_values <- list(
        unused_initial_value = 0
    )

    parameters <- list(
        unused_parameter = 0,
        timestep = 1
    )

    drivers <- data.frame(
        unused_varying_parameter=rep(0, MAX_INDEX),
        time = seq(0, length=MAX_INDEX, by=parameters$timestep)
    )

    direct_module_names <- c("BioCro:Module_1", "BioCro:Module_2")

    differential_module_names <- c()

    # This should be valid

    expect_true(validate_dynamical_system_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names,
        verbose=VERBOSE
    ))

    # If we change the module order, it should still be valid

    direct_module_names <- c("BioCro:Module_2", "BioCro:Module_1")

    expect_true(validate_dynamical_system_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names,
        verbose=VERBOSE
    ))

})
