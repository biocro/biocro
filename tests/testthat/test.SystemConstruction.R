# This tests some basic properties of system construction

context("Test error conditions during system validation and construction")

MAX_INDEX <- 100
SILENT <- TRUE

test_that("All modules must exist", {
	# Set up some bad inputs
	
	initial_state <- list(
		unused_state_variable = 0
	)
	
	parameters <- list(
		unused_parameter = 0
	)
	
	varying_parameters <- list(
		unused_varying_parameter=rep(0, MAX_INDEX)
	)
	
	steady_state_module_names <- c("module_that_does_not_exist")
	
	derivative_module_names <- c()
	
	# This should throw an error
	
	expect_error(validate_system_inputs(initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, silent=SILENT))
	
})

test_that("Duplicated quantities produce an error during validation", {
	# Set up some bad inputs for a harmonic oscillator
	
	initial_state <- list(
		position = 1,	# defined for the first time
		velocity = 0
	)
	
	parameters <- list(
		mass = 1,
		spring_constant = 1,
		timestep = 1,
		position = 1	# defined for the second time
	)
	
	varying_parameters <- list(
		doy=rep(0, MAX_INDEX),
		hour=seq(from=0, by=1, length=MAX_INDEX)
	)
	
	steady_state_module_names <- c("harmonic_energy")
	
	derivative_module_names <- c("harmonic_oscillator")
	
	# Validation should return FALSE
	
	expect_false(validate_system_inputs(initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, silent=SILENT))
	
})

test_that("Missing inputs produce an error during validation", {
	# Set up some bad inputs for a harmonic oscillator
	
	initial_state <- list(
		position = 1,
		velocity = 0
	)
	
	parameters <- list(
		mass = 1,
		#spring_constant = 1,	# not defined
		timestep = 1
	)
	
	varying_parameters <- list(
		doy=rep(0, MAX_INDEX),
		hour=seq(from=0, by=1, length=MAX_INDEX)
	)
	
	steady_state_module_names <- c("harmonic_energy")
	
	derivative_module_names <- c("harmonic_oscillator")
	
	# Validation should return FALSE
	
	expect_false(validate_system_inputs(initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, silent=SILENT))
	
})

test_that("Derivative modules only supply derivatives for quantities in the initial state", {
	# Set up some bad inputs for a harmonic oscillator
	
	initial_state <- list(
		position = 1
	)
	
	parameters <- list(
		mass = 1,
		spring_constant = 1,
		timestep = 1,
		velocity = 0	# should be in the initial state instead
	)
	
	varying_parameters <- list(
		doy=rep(0, MAX_INDEX),
		hour=seq(from=0, by=1, length=MAX_INDEX)
	)
	
	steady_state_module_names <- c("harmonic_energy")
	
	derivative_module_names <- c("harmonic_oscillator")
	
	# Validation should return FALSE
	
	expect_false(validate_system_inputs(initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, silent=SILENT))
	
})

test_that("Steady state modules are not required to be supplied in the correct order", {
	# Set up some bad inputs
	
	initial_state <- list(
		unused_state_variable = 0
	)
	
	parameters <- list(
		unused_parameter = 0
	)
	
	varying_parameters <- list(
		unused_varying_parameter=rep(0, MAX_INDEX)
	)
	
	steady_state_module_names <- c("Module_1", "Module_2")
	
	derivative_module_names <- c()
	
	# This should be valid
	
	expect_true(validate_system_inputs(initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, silent=SILENT))
	
	# If we change the module order, it should still be valid
	
	steady_state_module_names <- c("Module_2", "Module_1")
	
	expect_true(validate_system_inputs(initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, silent=SILENT))
	
})