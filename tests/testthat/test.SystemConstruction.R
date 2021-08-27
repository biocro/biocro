# This tests some basic properties of system construction

context("Test error conditions during system validation and construction")

MAX_INDEX <- 100
SILENT <- TRUE

test_that("certain run_biocro inputs must have the correct type", {
    expect_error(
        run_biocro(
            unlist(sorghum_initial_values),
            unlist(sorghum_parameters),
            unlist(get_growing_season_climate(weather05)),
            sorghum_steady_state_modules,
            sorghum_derivative_modules,
            unlist(sorghum_integrator['type'])
        ),
        regexp = paste0(
            "`initial_values` must be a list.\n",
            "  `parameters` must be a list.\n",
            "  `drivers` must be a data frame.\n",
            "  `integrator` must be a list.\n"
        )
    )
})

test_that("certain run_biocro inputs must not have empty elements", {
    expect_error(
        run_biocro(
            within(sorghum_initial_values, {bad_initial_value = numeric(0)}),
            within(sorghum_parameters, {bad_parameter = numeric(0)}),
            get_growing_season_climate(weather05),
            within(sorghum_steady_state_modules, {bad_steady_state_module = character(0)}),
            within(sorghum_derivative_modules, {bad_derivative_module = character(0)}),
            within(sorghum_integrator, {bad_integrator_setting = numeric(0)})
        ),
        regexp = paste0(
            "The following `initial_values` members have lengths other than 1, but all parameters must have a length of exactly 1: bad_initial_value.\n",
            "  The following `parameters` members have lengths other than 1, but all parameters must have a length of exactly 1: bad_parameter.\n",
            "  The following `steady_state_module_names` members have lengths other than 1, but all parameters must have a length of exactly 1: bad_steady_state_module.\n",
            "  The following `derivative_module_names` members have lengths other than 1, but all parameters must have a length of exactly 1: bad_derivative_module.\n",
            "  The following `integrator` members have lengths other than 1, but all parameters must have a length of exactly 1: bad_integrator_setting.\n"
        )
    )
})

test_that("certain run_biocro inputs must not have elements with length > 1", {
    expect_error(
        run_biocro(
            within(sorghum_initial_values, {bad_initial_value = c(1,2)}),
            within(sorghum_parameters, {bad_parameter = c(1,2)}),
            get_growing_season_climate(weather05),
            sorghum_steady_state_modules,
            sorghum_derivative_modules,
            within(sorghum_integrator, {bad_integrator_setting = c(1,2)})
        ),
        regexp = paste0(
            "The following `initial_values` members have lengths other than 1, but all parameters must have a length of exactly 1: bad_initial_value.\n",
            "  The following `parameters` members have lengths other than 1, but all parameters must have a length of exactly 1: bad_parameter.\n",
            "  The following `integrator` members have lengths other than 1, but all parameters must have a length of exactly 1: bad_integrator_setting.\n"
        )
    )
})

test_that("certain run_biocro inputs must be strings", {
    expect_error(
        run_biocro(
            sorghum_initial_values,
            sorghum_parameters,
            get_growing_season_climate(weather05),
            append(sorghum_steady_state_modules, 1.23),
            append(sorghum_derivative_modules, 4.56),
            within(sorghum_integrator, {type = 7.89})
        ),
        regexp = paste0(
            "The following `steady_state_module_names` members are not strings, but all members must be strings: 1.23.\n",
            "  The following `derivative_module_names` members are not strings, but all members must be strings: 4.56.\n",
            "  The following `integrator_type` members are not strings, but all members must be strings: 7.89.\n"
        )
    )
})

test_that("certain run_biocro inputs must be numeric", {
    expect_error(
        run_biocro(
            within(sorghum_initial_values, {bad_initial_value = "terrible"}),
            within(sorghum_parameters, {bad_parameter = "awful"}),
            within(get_growing_season_climate(weather05), {bad_driver = "offensive"}),
            sorghum_steady_state_modules,
            sorghum_derivative_modules,
            within(sorghum_integrator, {bad_integrator_setting = "heinous"})
        ),
        regexp = paste0(
            "The following `initial_values` members are not numeric or NA, but all members must be numeric or NA: bad_initial_value.\n",
            "  The following `parameters` members are not numeric or NA, but all members must be numeric or NA: bad_parameter.\n",
            "  The following `drivers` members are not numeric or NA, but all members must be numeric or NA: bad_driver.\n",
            "  The following `integrator_other_than_type` members are not numeric or NA, but all members must be numeric or NA: bad_integrator_setting.\n"
        )
    )
})

test_that("All modules must exist", {
	# Set up some bad inputs

	initial_values <- list(
		unused_state_variable = 0
	)

	parameters <- list(
		unused_parameter = 0
	)

	drivers <- list(
		unused_varying_parameter=rep(0, MAX_INDEX)
	)

	steady_state_module_names <- c("module_that_does_not_exist")

	derivative_module_names <- c()

	# This should throw an error

	expect_error(
        validate_system_inputs(initial_values, parameters, drivers, steady_state_module_names, derivative_module_names, silent=SILENT),
        regexp = '"module_that_does_not_exist" was given as a module name, but no module with that name could be found.'
    )

})

test_that("Duplicated quantities produce an error during validation", {
	# Set up some bad inputs for a harmonic oscillator

	initial_values <- list(
		position = 1,	# defined for the first time
		velocity = 0
	)

	parameters <- list(
		mass = 1,
		spring_constant = 1,
		timestep = 1,
		position = 1	# defined for the second time
	)

	drivers <- list(
		doy=rep(0, MAX_INDEX),
		hour=seq(from=0, by=1, length=MAX_INDEX)
	)

	steady_state_module_names <- c("harmonic_energy")

	derivative_module_names <- c("harmonic_oscillator")

	# Validation should return FALSE

	expect_false(validate_system_inputs(initial_values, parameters, drivers, steady_state_module_names, derivative_module_names, silent=SILENT))

})

test_that("Missing inputs produce an error during validation", {
	# Set up some bad inputs for a harmonic oscillator

	initial_values <- list(
		position = 1,
		velocity = 0
	)

	parameters <- list(
		mass = 1,
		#spring_constant = 1,	# not defined
		timestep = 1
	)

	drivers <- list(
		doy=rep(0, MAX_INDEX),
		hour=seq(from=0, by=1, length=MAX_INDEX)
	)

	steady_state_module_names <- c("harmonic_energy")

	derivative_module_names <- c("harmonic_oscillator")

	# Validation should return FALSE

	expect_false(validate_system_inputs(initial_values, parameters, drivers, steady_state_module_names, derivative_module_names, silent=SILENT))

})

test_that("Derivative modules only supply derivatives for quantities in the initial values", {
	# Set up some bad inputs for a harmonic oscillator

	initial_values <- list(
		position = 1
	)

	parameters <- list(
		mass = 1,
		spring_constant = 1,
		timestep = 1,
		velocity = 0	# should be in the initial values instead
	)

	drivers <- list(
		doy=rep(0, MAX_INDEX),
		hour=seq(from=0, by=1, length=MAX_INDEX)
	)

	steady_state_module_names <- c("harmonic_energy")

	derivative_module_names <- c("harmonic_oscillator")

	# Validation should return FALSE

	expect_false(validate_system_inputs(initial_values, parameters, drivers, steady_state_module_names, derivative_module_names, silent=SILENT))

})

test_that("Steady state modules are not required to be supplied in the correct order", {
	# Set up some bad inputs

	initial_values <- list(
		unused_state_variable = 0
	)

	parameters <- list(
		unused_parameter = 0
	)

	drivers <- list(
		unused_varying_parameter=rep(0, MAX_INDEX)
	)

	steady_state_module_names <- c("Module_1", "Module_2")

	derivative_module_names <- c()

	# This should be valid

	expect_true(validate_system_inputs(initial_values, parameters, drivers, steady_state_module_names, derivative_module_names, silent=SILENT))

	# If we change the module order, it should still be valid

	steady_state_module_names <- c("Module_2", "Module_1")

	expect_true(validate_system_inputs(initial_values, parameters, drivers, steady_state_module_names, derivative_module_names, silent=SILENT))

})
