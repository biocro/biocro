context("Test to make sure the simultaneous equation solvers function as expected")

# Use the "golden ratio hyperbola" to test the solver
run_trial_se <- function(initial_value, lower_bounds, upper_bounds, abs_error_tols, rel_error_tols, solver_type, trial_description) {

	# Set up inputs
	direct_modules <- "BioCro:golden_ratio_hyperbola"
	known_quantities <- list()
	unknown_quantities <- list(x = initial_value)

	# test for any errors
	# regexp = NA indicates that no error should be encountered
	test_that(trial_description, {
		expect_error(solve_simultaneous_equations(
						direct_modules,
						known_quantities,
						unknown_quantities,
						lower_bounds,
						upper_bounds,
						abs_error_tols,
						rel_error_tols,
						max_it,
						solver_type,
						silent = TRUE),
					regexp = NA)
	})
}

# Define defaults for the lower and upper bounds
lower_bounds <- list(x = -100)
upper_bounds <- list(x = +100)

# Define defaults for the error tolerances and max iterations
abs_error_tols <- list(x = 1e-3)
rel_error_tols <- list(x = 1e-3)
max_it <- 50

# Define an initial guess
initial_guess <- 1

# Test each solver method
all_solver_types <- get_all_se_solvers()
for (solver_type in all_solver_types) {
	description <- paste("Solving the 'Golden ratio hyperbola' using the ", solver_type, " method", sep="")
	run_trial_se(initial_guess, lower_bounds, upper_bounds, abs_error_tols, rel_error_tols, solver_type, description)
}
