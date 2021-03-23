## This tests that re-running a previously-run simulation yields the same result
## as before.
##
## If any of the plant-specific defaults for the initial state, parameters,
## modules or weather data change, or if the behavior of any of these modules
## changes, the stored data for this test will likely need to be updated. To do
## this, create a new R session and set the working directory to
## "tests/test_data". Then load BioCro, run a new simulation with the same
## parameters used in the test below (storing the output as `Gro_result`), and
## save it as `PLANT_simulation.rda`, where PLANT is the name of the plant
## species. E.g., for soybean, the commands would be as follows:
##
## library(BioCro)
## solver <- list(type = 'Gro', output_step_size = 1.0, adaptive_rel_error_tol = 1e-5, adaptive_abs_error_tol = 1e-5, adaptive_max_steps = 200)
## Gro_result <- Gro(glycine_max_initial_state, glycine_max_parameters, get_growing_season_climate(weather05), glycine_max_modules, solver)
## save(Gro_result, file="glycine_max_simulation.rda")
##
## Finally, after updating the stored data for any of the plants whose output
## has changed, run this test to make sure it is passed.

context("Test Soybean-BioCro with known results to ensure that the output doesn't change.")

SAMPLE_SIZE <- 5       # number of time points to test in each simulation result

# Use a 0.5% tolerance when comparing values between simulations. Lower
# tolerances have caused problems when comparing results calculated on different
# operating systems.
RELATIVE_ERROR_TOLERANCE <- 5e-3

# When calculating a simulation result using an adaptive step size solver, we
# need to use a lower error tolerance than the default values (1e-4) to minimize
# differences due to operating system or other factors.
SOLVER <- list(
  type = 'Gro',
  output_step_size = 1.0,
  adaptive_rel_error_tol = 1e-5,
  adaptive_abs_error_tol = 1e-5,
  adaptive_max_steps = 200
)

# Define a function that checks the result of a new simulation against the
# stored data for one plant. For this test, we always use the the stored
# environmental data `get_growing_season_climate(weather05)`
test_plant_model <- function(test_info) {
  
  # Run the simulation
  result <- 0
  description <- paste("The", test_info[['plant_name']], "simulation runs without producing any errors")
  test_that(description, {
    expect_silent(
      result <<- Gro_solver(
        test_info[['initial_state']],
        test_info[['parameters']],
        get_growing_season_climate(weather05),
        test_info[['steadystate_modules']],
        test_info[['derivative_modules']],
        SOLVER
      )
    )
  })
  
  # Some variables may need to be ignored, possibly because their values
  # depend on the operating system or other factors that may change between
  # simulation runs. Remove these from the results. If a variable is flagged
  # to be ignored but is not in the simulation result, this could indicate
  # that one of the default modules has been changed, and the list of ignored
  # variables should probably be revisited, so warn the user.
  for (variable in test_info[['ignored_variables']]) {
    if (variable %in% names(result)) {
      result[[variable]] <- NULL
    } else {
      msg <- paste0("The regression test reports that '", variable, "' is no longer included in the ", test_info[['plant_name']], " simulation result. Did a default module change?")
      warning(msg)
    }
  }
  
  # Read the stored result ("Gro_result") from the data file:
  load(test_info[['stored_result_file']])
  
  # Make sure the stored result has the same number of time points
  index_of_last_row <- length(result[[1]])
  description <- paste("The", test_info[['plant_name']], "simulation result has the correct number of data points")
  test_that(description, {
    expect_equal(index_of_last_row, length(Gro_result[[1]]))
  })
  
  # Make sure the results have a sufficient number of time points
  description <- paste("The", test_info[['plant_name']], "simulation result has enough data points")
  test_that(description, {
    expect_gte(index_of_last_row, SAMPLE_SIZE)
  })
  
  # Make sure the stored result contains all the non-ignored quantities in the
  # new result
  column_names <- names(result)
  stored_column_names <- names(Gro_result)
  for (name in column_names) {
    description <- paste("The stored", test_info[['plant_name']], "simulation result includes the", name, "column")
    test_that(description, {
      expect_true(name %in% stored_column_names)
    })
  }
  
  # Define a function that compares the new result to the old one at a single
  # index
  compare_simulation_trial <- function(index) {
    for (variable in column_names) {
      description <- paste0("The ", test_info[['plant_name']], " simulation result agrees with the stored result at index ", index, " for the '", variable, "' quantity")
      test_that(description, {
        expect_equal(
          result[[variable]][index],
          Gro_result[[variable]][index],
          tolerance=RELATIVE_ERROR_TOLERANCE
        )
      })
    }
  }
  
  # Run the test for the final time point and some randomly chosen indices
  points_to_test <- sample(1:(index_of_last_row-1), SAMPLE_SIZE-1)
  points_to_test <- c(index_of_last_row, points_to_test)
  for (index in points_to_test) {
    compare_simulation_trial(index)
  }
  
}

# Make a helping function for specifying plant information
specify <- function(plant_name, initial_state, parameters, steadystate_modules, derivative_modules,
                    stored_result_file, ignored_variables) {
  list(
    plant_name = plant_name,
    initial_state = initial_state,
    parameters = parameters,
    steadystate_modules = steadystate_modules,
    derivative_modules = derivative_modules,
    stored_result_file = stored_result_file,
    ignored_variables = ignored_variables
  )
}

# Define lists of species-specific variables to ignore. Soybean and cassava
# both use the utilization growth modules and are solved with the Rosenbrock
# solver by default. Even with the decreased error tolerances specified above,
# small differences in the output have been found between operating systems, so
# we ignore the problematic quantities. The other species use the partitioning
# growth modules and are solved with the homemade Euler solver by default. No
# problematic quantities have been identified for these species, so there is no
# need to ignore any quantities.

soybean_ignore <- character(0)

# Define the plants to test
plant_testing_info <- list(
  specify("soybean",            soybean_initial_state,            soybean_parameters,            soybean_steadystate_modules,            soybean_derivative_modules,            "../test_data/soybean_simulation.rda",            soybean_ignore)
)

# Run all the tests
for(i in 1:length(plant_testing_info)) {
  test_plant_model(plant_testing_info[[i]])
}
