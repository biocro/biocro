context("Test to make sure the simultaneous equation solvers function as expected")

# Use the "golden ratio hyperbola" to test the solver
run_trial_se <- function(initial_value, lower_bounds, upper_bounds, solver, trial_description) {
	
	# Set up inputs
	steady_state_module_names <- c("golden_ratio_hyperbola")
	known_quantities <- list()
	unknown_quantities <- list(x = initial_value)
	
	# test for any errors
	# regexp = NA indicates that no error should be encountered
	test_that(trial_description, {
		expect_error(solve_simultaneous_equations(
						steady_state_module_names,
						known_quantities,
						unknown_quantities,
						lower_bounds,
						upper_bounds,
						solver,
						silent = TRUE),
					regexp = NA)
	})
}

# Define the default solver properties
default_solver <- list(
	rel_error_tol=1e-4,
	abs_error_tol=1e-4,
	max_it=50
)

# Define defaults for the lower and upper bounds
lower_bounds <- list(x = -100)
upper_bounds <- list(x = +100)

# Define an initial guess
initial_guess <- 1

# Test each solver method
all_solver_types <- get_all_se_solvers()
for (solver_type in all_solver_types) {
	solver <- default_solver
	solver$type <- solver_type
	description <- paste("Solving the 'Golden ratio hyperbola' using the ", solver_type, " method", sep="")
	run_trial_se(initial_guess, lower_bounds, upper_bounds, solver, description)
}