context('Test basic functioning of the differential module "heating_degree_days".')

direct_modules <- c()
differential_modules <- c("heating_degree_days")

## Given an initial value for the number of heating degree days, a
## vector of equally-spaced time values, a corresponding vector of
## temperatures, a thresholdtemperature value, a timestep value, and the
## specification of a solver, run the system and return the final heatin
## g-degree-days value.
calculate_heating_degree_days <- function(initial_value, time_vector, temperature_vector, threshold_temperature, timestep, solver) {
    ## Time_vector must be hours.
    time <- 24 * (time_vector - 1)
    initial_values <- list(heating_degree_days = initial_value)
    parameters <- list(base_temperature = threshold_temperature, timestep = timestep)
    drivers <- list(time = time_vector, temp = temperature_vector)

    result <- run_biocro(initial_values, parameters, drivers,
                         direct_modules, differential_modules, solver)

    result$heating_degree_days[length(time_vector)]
}

euler_solvers <- list(
    list(type='homemade_euler', output_step_size=1, adaptive_rel_error_tol=1e-4, adaptive_abs_error_tol=1e-4, adaptive_max_steps=200),
    list(type='boost_euler', output_step_size=1, adaptive_rel_error_tol=1e-4, adaptive_abs_error_tol=1e-4, adaptive_max_steps=200)
)

other_solvers <- list(
    list(type='boost_rk4', output_step_size=1, adaptive_rel_error_tol=1e-4, adaptive_abs_error_tol=1e-4, adaptive_max_steps=200),
    list(type='boost_rkck54', output_step_size=1, adaptive_rel_error_tol=1e-4, adaptive_abs_error_tol=1e-4, adaptive_max_steps=200)
)

non_euler_solvers <- c(other_solvers, list(list(type='boost_rosenbrock', output_step_size=1, adaptive_rel_error_tol=1e-4, adaptive_abs_error_tol=1e-4, adaptive_max_steps=200)))

well_behaved_solvers <- c(euler_solvers, other_solvers) # Rosenbrook solver doesn't work with time sequences of length less than 3.

all_solvers <- c(euler_solvers, non_euler_solvers)

test_that("A day at one degree below the threshold temperature adds 1 heating degree day", {
    ## test using 24-hour timesteps:
    for (i in 1:length(well_behaved_solvers)) {
        hdd <- calculate_heating_degree_days(
            0,               # start at zero heating-degree days
            c(1, 2),         # days-of-year 1 and 2
            c(19, 19),       # assume a constant ambient temperature of 19 degrees ...
            20,              # ... which is one degree below the threshold temperature
            24,              # the time interval is 24 hours--the time between successive days
            well_behaved_solvers[[i]]
        )
        expect_equal(hdd, 1.0)
    }
    ## test using 1-hour timesteps:
    for (i in 1:length(all_solvers)) {
        hdd <- calculate_heating_degree_days(
            0,                   # start at zero heating-degree days
            seq(1, 2, 1/24.0),   # a sequence of times one-hour apart covering one day
            rep(19, 25),         # assume a constant ambient temperature of 19 degrees ...
            20,                  # ... which is one degree below the threshold temperature
            1,                   # the time interval is 1 hour
            all_solvers[[i]])
        expect_equal(hdd, 1.0)
    }
})

test_that("A random assortment of temperatures below the threshold
           temperature yields a number of heating degree days equal to the
           cumulative amount by which the temperatures are under the
           threshold temperature.", {
              days <- 30
              temperature_deficits <- sample(0:50, days)
              cumulative_temperature_deficit <- sum(temperature_deficits[1:(days - 1)]) # Don't count the last day's temperature.

              temperatures <- 20 - temperature_deficits

              for (i in 1:length(euler_solvers)) {
                  hdd <- calculate_heating_degree_days(
                      0,                  # start at zero heating-degree days
                      1:days,
                      temperatures,       # a random sequence of temperatures below the threshold temperature
                      20,                 # the threshold temperature
                      24,                 # the time interval is 24 hours--the time between successive days
                      euler_solvers[[i]])
                  expect_equal(hdd, cumulative_temperature_deficit)
              }

              ## For the non-Euler solvers, we have set a greater tolerance value than the default:

              for (i in 1:length(non_euler_solvers)) {
                  hdd <- calculate_heating_degree_days(
                      0,                      # start at zero heating-degree days
                      1:days,
                      temperatures,           # a random sequence of temperatures below the threshold temperature
                      20,                     # the threshold temperature
                      24,                     # the time interval is 24 hours--the time between successive days
                      non_euler_solvers[[i]])
                  expect_equal(hdd, cumulative_temperature_deficit, tolerance = 0.03)
              }

          }
)

test_that("A random assortment of temperatures above the threshold
           temperature results in zero heating degree days.", {
              days <- 30
              temperature_surpluses <- sample(0:50, days)

              temperatures <- 20 + temperature_surpluses

              for (i in 1:length(all_solvers)) {
                  hdd <- calculate_heating_degree_days(
                      0,                 # start at zero heating-degree days
                      1:days,
                      temperatures,      # a random sequence of temperatures above the threshold temperature
                      20,                # the threshold temperature
                      24,                # the time interval is 24 hours--the time between successive days
                      all_solvers[[i]])
                  expect_equal(hdd, 0)
              }
          }
)
