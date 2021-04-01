## This tests that re-running a previously-run simulation yields the same result
## as before.
##
## If any of the crop-specific defaults for the initial state, parameters,
## modules or weather data change, or if the behavior of any of these modules
## changes, the stored data for one or more crops will likely need to be
## updated. To do this, open a fresh R session in this directory, load the
## `BioCro` and `testthat` libraries, source this file, and type the following
## command:
##
## update_stored_results(PLANT_TESTING_INFO[[INDEX]])
##
## where the value of `INDEX` corresponds to the desired crop as defined in the
## `PLANT_TESTING_INFO` list below. For example, INDEX = 1 corresponds to
## soybean. Afterwards, check the git diff for the stored data files to make
## sure the changes are reasonable (to the extent that this is possible).
## Finally, rerun this file using the `testthat` package to make sure the tests
## all pass.

context("Test Soybean-BioCro with known results to ensure that the output doesn't change.")

MAX_SAMPLE_SIZE <- 5       # number of time points to test in each simulation result

# Use a 0.5% tolerance when comparing values between simulations. Lower
# tolerances have caused problems when comparing results calculated on different
# operating systems.
RELATIVE_ERROR_TOLERANCE <- 5e-3

# When calculating a simulation result using an adaptive step size solver, we
# need to use a lower error tolerance than the default values (1e-4) to minimize
# differences due to operating system or other factors.
SOLVER <- list(
  type = 'Gro_rkck54',
  output_step_size = 1.0,
  adaptive_rel_error_tol = 1e-5,
  adaptive_abs_error_tol = 1e-5,
  adaptive_max_steps = 200
)

# Define a function that checks the result of a new simulation against the
# stored data for one plant. For this test, we always use the the stored
# environmental data 'data/soybean_weather2002.csv'
test_plant_model <- function(test_info) {
  
  description <- paste(
    "The", 
    test_info[['plant_name']], 
    "simulation runs without producing any errors"
    )
  
  # Run the simulation
  result <- 0
  
  test_that(description, {
    expect_silent(
      result <<- Gro_solver(
        test_info[['initial_state']],
        test_info[['parameters']],
        test_info[['weather']],
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
      msg <- paste0(
        "The regression test reports that '", 
        variable, 
        "' is no longer included in the ", 
        test_info[['plant_name']], 
        " simulation result. Did a default module change?"
        )
      warning(msg)
    }
  }
  
  # Read the stored result from the data file
  Gro_result <- read.csv(test_info[['stored_result_file']])
  
  # Make sure all columns contain numeric data
  Gro_result <- as.data.frame(sapply(Gro_result, as.numeric))
  
  # Make sure the stored result has the same number of time points
  index_of_last_row <- length(result[[1]])
  
  description <- paste(
    "The",
    test_info[['plant_name']],
    "simulation result has the correct number of data points"
  )
  
  test_that(description, {
    expect_equal(index_of_last_row, length(Gro_result[[1]]))
  })
  
  # Make sure the results have a sufficient number of time points
  description <- paste(
    "The",
    test_info[['plant_name']],
    "simulation result has enough data points"
  )
  
  test_that(description, {
    expect_gt(index_of_last_row, 1.0)
  })
  
  # Make sure the stored result contains all the non-ignored quantities in the
  # new result
  column_names <- names(result)
  
  stored_column_names <- names(Gro_result)
  
  for (name in column_names) {
    description <- paste(
      "The stored",
      test_info[['plant_name']],
      "simulation result includes the",
      name,
      "column"
    )
    
    test_that(description, {
      expect_true(name %in% stored_column_names)
    })
  }
  
  # Make a helping function that compares the new result to the old one at a
  # single index
  compare_simulation_trial <- function(index) {
    for (variable in column_names) {
      description <- paste0(
        "The ",
        test_info[['plant_name']],
        " simulation result agrees with the stored result at index ",
        index,
        " for the '",
        variable,
        "' quantity"
      )
      
      test_that(description, {
        expect_equal(
          result[[variable]][index],
          Gro_result[[variable]][index],
          tolerance=RELATIVE_ERROR_TOLERANCE
        )
      })
    }
  }
  
  # Run the test for some equally spaced indices including the first and last
  # points of the simulation. Note that no problems occur if `points_to_test`
  # includes non-integer elements, since R automatically truncates them to
  # integer values when they are used as indices to access elements of a
  # vector.
  points_to_test = seq(
    from = 1,
    to = index_of_last_row,
    length.out = max(
      min(
        index_of_last_row,
        MAX_SAMPLE_SIZE
      ),
      2.0
    )
  )
  
  for (index in points_to_test) {
    compare_simulation_trial(index)
  }
  
}

# Make a helping function for specifying plant information
specify_crop <- function(
  plant_name, 
  initial_state, 
  parameters, 
  weather, 
  steadystate_modules, 
  derivative_modules,
  ignored_variables) 
  {
  list(
    plant_name = plant_name,
    initial_state = initial_state,
    parameters = parameters,
    weather = weather,
    steadystate_modules = steadystate_modules,
    derivative_modules = derivative_modules,
    stored_result_file = paste0(
      "../test_data/",
      plant_name,
      "_simulation.csv"
    ),
    ignored_variables = ignored_variables
  )
}

# Make a helping function that updates the stored data for one crop
update_stored_results <- function(test_info) {
  
  # Calculate the result
  plant_result <- Gro_solver(
    test_info[['initial_state']],
    test_info[['parameters']],
    test_info[['weather']],
    test_info[['steadystate_modules']],
    test_info[['derivative_modules']],
    SOLVER
  )
  
  # Save it as a csv file
  write.csv(
    plant_result,
    file=test_info[['stored_result_file']],
    quote=FALSE,
    eol="\n",
    na="",
    row.names=FALSE
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

SOYBEAN_IGNORE <- c("ncalls")

# Define the plants to test
PLANT_TESTING_INFO <- list(
  specify_crop("soybean",  soybean_initial_state,  soybean_parameters,  soybean_weather2002,  soybean_steadystate_modules,  soybean_derivative_modules,  SOYBEAN_IGNORE) # INDEX = 1
)

# Run all the tests
for(i in 1:length(PLANT_TESTING_INFO)) {
  
  test_plant_model(PLANT_TESTING_INFO[[i]])
}
