## Bugs: The tests may fail for large values of ω (that is, when the spring
## constant is very large compared to the mass).  This might be remedied by
## screening out these cases as not suitable for the model or possibly by
## adjusting the error tolerance in such cases.

context("Test basic system building and solving using the harmonic oscillator modules.")

DEBUG_TEST <- FALSE    # Change this to TRUE to get useful output for debugging these tests.

NUMBER_OF_TRIALS <- 10 # number of different sets of parameters and initial conditions to test
MAX_INDEX <- 100       # how long to run the simulation
SAMPLE_SIZE <- 5       # number of time points to test in each simulation result
TOLERANCE_FACTOR <- 0.01


## Equations for the position x and velocity v of an undamped oscillating mass are
##
##   x =  A sin(ωt + φ)
##   v = ωA cos(ωt + φ)
##
## where A is the amplitude, ω is the angular frequency, and φ is the phase.
##
## A, φ, and ω may in turn be computed from the mass m, the spring constant k, the
## initial position x0, and the initial velocity v0 using the equations
##
##   A = sqrt(x0^2 + m/k * v0^2)
##   φ = arctan(x0 * ω / v0)      (if v0 > 0)
##     = arctan(x0 * ω / v0) + π  (if v0 < 0)
##     = π/2                      (if v0 = 0 and x0 > 0)
##     = -π/2                     (if v0 = 0 and x0 < 0)
##   ω = sqrt(k/m)
##
## Note that φ is undefined if both x0 and v0 are zero, but in this case A will
## also be zero, so the value of φ is immaterial and may be arbitrarily assigned
## a value of zero.  m and k are assumed to be non-zero.
##
## The total energy of the system is constant over time and is given by
##
##   E = 1/2 * kA^2
##
##
## A note on units
##
## In the equations above, we assume a consistent set of units so that no
## conversion factors are required.  Moreover, φ is assumed to be in radians and
## ω in radians per unit time.
##
## Although it is a goal of BioCro to work exclusively in SI units, currently,
## the base time unit for BioCro is always an hour.  This means that even if we
## choose meters for the unit of length and kilogram for the unit of mass, the
## derived units in which the quantities v (velocity), k (the spring constant),
## and E (the system energy) will not be expressed in SI units.  v, for example,
## will be in meters per hour, and k will be in kilograms per hour squared, or
## in other words, a unit equivalent to 7.716 x 10^-8 newtons per meter.  E will
## be in units of kilogram-meters squared per hour squared, a unit equivalent to
## 7.716 x 10^-8 joules.


## helper functions:

position <- function(time, amplitude, angular_frequency, phase) {
    amplitude * sin(angular_frequency * time + phase)
}

velocity <- function(time, amplitude, angular_frequency, phase) {
    angular_frequency * amplitude * cos(angular_frequency * time + phase)
}

debug_print <- function(ob) {
    if (DEBUG_TEST) print(ob)
}

debug_view <- function(ob) {
    if (DEBUG_TEST) View(ob)
}



derivative_modules <- c("harmonic_oscillator")
steady_state_modules <- c("harmonic_energy")
varying_parameters <- list(doy=rep(0, MAX_INDEX), hour=seq(from=0, by=1, length=MAX_INDEX))
default_solver <- list(type='Gro', output_step_size=1, adaptive_rel_error_tol=1e-4, adaptive_abs_error_tol=1e-4, adaptive_max_steps=200)

## Given system parameters and initial conditions, run a simulation of harmonic
## motion and test that the values from the simulation match those predicted
## from harmonic motion equations.
run_trial <- function(initial_position, initial_velocity, mass, spring_constant, solver, trial_description) {
    initial_state <- list(position=initial_position, velocity=initial_velocity)
    invariant_parameters <- list(mass=mass, spring_constant=spring_constant, timestep=1)

    
    debug_print(initial_state)
    debug_print(invariant_parameters)

    
    ## compute equation of motion parameters:
    amplitude <- sqrt(initial_position^2 + initial_velocity^2 * mass / spring_constant)
    angular_frequency <- sqrt(spring_constant / mass)
    # arbitrarily set the phase to zero if the mass isn't moving:
    phase <- if (initial_velocity == 0) {
                 if (initial_position > 0) pi/2
                 else if (initial_position < 0) -pi/2
                 else 0
             }
             else atan(initial_position * angular_frequency / initial_velocity) + (if (initial_velocity < 0) pi else 0)

    ## compute the total energy, which doesn't depend on time:
    total_energy <- 0.5 * spring_constant * amplitude^2

    
    debug_print(list(amplitude = amplitude, phase = phase, angular_frequency = angular_frequency))

    
    ## try out the solver
    result <- Gro_solver(initial_state, invariant_parameters, varying_parameters, steady_state_modules, derivative_modules, solver)

    ## add useful columns to the resulting data frame:    
    result$time <- result$doy_dbl * 24 # time is in hours
    result$expected_position <- position(result$time, amplitude, angular_frequency, phase)
    result$expected_velocity <- velocity(result$time, amplitude, angular_frequency, phase)

    ## select and order the columns we wish to keep (for debug display):
    result <- result[, c("time", "expected_position", "position", "expected_velocity", "velocity", "kinetic_energy", "spring_energy", "total_energy")]

    debug_view(result)

    if (DEBUG_TEST) {
        ## something is seriously wrong if the initial state doesn't conform to the equations of motion:
        if (abs(result$position[1] - result$expected_position[1]) > .01) break
        if (abs(result$velocity[1] - result$expected_velocity[1]) > .01) break
    }

    
	overall_description <- paste("Harmonic oscillator position and velocity values match the expected values (", trial_description, ")", sep="")
	
    test_that(overall_description, {
        expect_true(class(result) == "data.frame") # sanity check

        sample <- sample(1:MAX_INDEX, SAMPLE_SIZE) # randomly choose a number of points in the evolution of the system to test

        for (index in sample) {
            time <- result$time[index] # for convenience

            ## in these tests, we set the tolerance based on the maximum posible value for each quantity:
            expect_equal(result$position[index], result$expected_position[index], tolerance = amplitude * TOLERANCE_FACTOR)
            expect_equal(result$velocity[index], result$expected_velocity[index], tolerance = angular_frequency * amplitude * TOLERANCE_FACTOR)
            expect_equal(result$total_energy[index], total_energy, tolerance = total_energy * TOLERANCE_FACTOR)
        }
    })
}

## some special cases

run_trial(initial_position = 0, initial_velocity = 26.18, mass = 14.59, spring_constant = 1, default_solver, "initial position 0, amplitude 100, and period 24")

run_trial(initial_position = 0, initial_velocity = 0, mass = 100, spring_constant = 100, default_solver, "a mass at rest")

run_trial(initial_position = 10, initial_velocity = 0, mass = 100, spring_constant = 100, default_solver, "a mass with no initial velocity with initial positive displacement")

run_trial(initial_position = -10, initial_velocity = 0, mass = 100, spring_constant = 100, default_solver, "a mass with no initial velocity and with initial negative displacement")


## run a number of randomly-chosen cases
for (trial_number in seq(length=NUMBER_OF_TRIALS)) {

    ## randomly select parameter values and initial state:
    initial_position <- runif(1, -100, 100)[1]
    initial_velocity <- runif(1, -100, 100)[1]
    mass <- runif(1, 0, 100)[1]
    spring_constant <- runif(1, 0, 100)[1]

    run_trial(initial_position, initial_velocity, mass, spring_constant, default_solver, "random parameters and initial state")
}

## test each solver method using a really weak spring (so the Euler methods still work)
all_solver_types <- get_all_system_solvers()
for (solver_type in all_solver_types) {
	solver <- default_solver
	solver$type <- solver_type
	description <- paste("using the ", solver_type, " method", sep="")
	run_trial(initial_position = 1, initial_velocity = 0, mass = 1, spring_constant = 0.0001, solver, description)
}