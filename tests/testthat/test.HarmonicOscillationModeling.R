context("Test basic system building and solving using the harmonic oscillator modules.")

DEBUG_TEST <- FALSE

NUMBER_OF_TRIALS <- 10 # number of different sets of parameters and initial conditions to test
MAX_INDEX <- 100       # how long to run the simulation
SAMPLE_SIZE <- 5       # number of time points to test in each simulation result


## Equations for the position x and velocity v of an undamped oscillating mass are
##
##   x =  A sin(ωt + φ)
##   v = ωA cos(ωt + φ)
##
## where A is the amplitude and ω is the angular frequency.
##
## A, φ, and ω may in turn be computed from the mass m, the spring constant k, the
## initial position x0, and the initial velocity v0 from the equations
##
##   A = sqrt(x0^2 + m/k * v0^s)
##   φ = arctan(x0 * ω / v0)      (if v0 >= 0)
##     = arctan(x0 * ω / v0) + π  (if v0 <= 0)
##   ω = sqrt(k/m)
##
## The total energy of the system is constant over time and is given by
##
##   E = 1/2 * kA^2


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

for (trial_number in 1:NUMBER_OF_TRIALS) {

    ## randomly select parameter values and initial state:
    initial_position <- runif(1, -100, 100)[1]
    initial_velocity <- runif(1, -100, 100)[1]
    mass <- runif(1, 0, 100)[1]
    spring_constant <- runif(1, 0, 100)[1]

    initial_state <- list(position=initial_position, velocity=initial_velocity)
    invariant_parameters <- list(mass=mass, spring_constant=spring_constant, timestep=1)

    
    debug_print(initial_state)
    debug_print(invariant_parameters)

    
    varying_parameters <- list(doy=rep(0, MAX_INDEX), hour=seq(from=0, by=1, length=MAX_INDEX))

    ## compute equation of motion parameters:
    amplitude <- sqrt(initial_position^2 + initial_velocity^2 * mass / spring_constant)
    angular_frequency <- sqrt(spring_constant / mass)
    phase <- atan(initial_position * angular_frequency / initial_velocity) + (if (initial_velocity < 0) pi else 0)

    ## compute the total energy, which doesn't depend on time:
    total_energy <- 0.5 * spring_constant * amplitude^2

    
    debug_print(list(amplitude = amplitude, phase = phase, angular_frequency = angular_frequency))

    
    ## try out the solver
    result <- Gro_solver(initial_state, invariant_parameters, varying_parameters, steady_state_modules, derivative_modules)

    ## add useful columns to the resulting data frame:    
    result$time <- result$doy_dbl * 24
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

    
    test_that("Harmonic oscillator position and velocity values match the expected values.", {
        expect_true(class(result) == "data.frame") # sanity check

        sample <- sample(1:MAX_INDEX, SAMPLE_SIZE)

        for (index in sample) {
            time <- result$time[index] # for convenience

            ## in these tests, we set the tolerance to 0.2% of the maximum posible value for each quantity:
            expect_equal(result$position[index], result$expected_position[index], tolerance = amplitude / 500)
            expect_equal(result$velocity[index], result$expected_velocity[index], tolerance = angular_frequency * amplitude / 500)
            expect_equal(result$total_energy[index], total_energy, tolerance = total_energy / 500)
        }
    })
}
