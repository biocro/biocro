# These tests just make sure that the solvers operate without crashing and produce reasonable results
# Here we use a harmonic oscillator as an example of a simple system

context("Test different solution methods using a harmonic oscillator.")

# Define parameters for running the tests
# Here the spring constant is chosen to be very small, ensuring slow oscillations and better performance from the Euler integrators

MAX_INDEX <- 100
SAMPLE_SIZE <- 5
TOLERANCE_FACTOR <- 0.01
VERBOSE = FALSE

initial_state <- list(
	position = 1,
	velocity = 0
)

parameters <- list(
	mass = 1,
	spring_constant = 0.0001,
	timestep = 1
)

varying_parameters <- list(
	doy = rep(0, MAX_INDEX),
	hour = seq(from = 0, by = 1, length = MAX_INDEX)
)

steady_state_module_names <- c("harmonic_energy")

derivative_module_names <- c("harmonic_oscillator")

solver <- list(
	type='',
	output_step_size = 1,
	adaptive_rel_error_tol = 1e-4,
	adaptive_abs_error_tol = 1e-4,
	adaptive_max_steps = 200
)

initial_energy <- 0.5 * parameters$spring_constant * initial_state$position^2 + 0.5 * parameters$mass * initial_state$velocity^2

all_solver_types <- get_all_solvers()

# Test each solver method
for (solver_type in all_solver_types) {
	# Update the solver parameters
	solver$type <- solver_type
	
	# Run the simulation
	result <- Gro_solver(initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, solver, VERBOSE)
	
	# Make the description string
	description_string <- paste("The ", solver_type, " method conserves energy reasonably well (within ", TOLERANCE_FACTOR*100, "%)", sep="")
	
	# Check a few random points to make sure the energy is properly conserved
	test_that(description_string, {
		sample <- sample(1:(MAX_INDEX/solver$output_step_size), SAMPLE_SIZE)
        for (index in sample) {
			expect_equal(result$total_energy[index], initial_energy, tolerance = initial_energy * TOLERANCE_FACTOR)
		}
	})
}