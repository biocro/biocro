## This tests that each crop model is well defined, and, if necessary, that
## re-running a previously-run simulation yields the same result as before.
##
## If any of the crop-specific defaults for the initial values, parameters,
## modules or weather data change, or if the behavior of any of these modules
## changes, the stored data for one or more crops will likely need to be
## updated. Instructions for doing so are included in
## `regression_testing_helper_functions.R`.

source('crop_model_testing_helper_functions.R')

# Run all the tests
expect_true(
    run_model_test_cases(PLANT_TESTING_INFO)
)
