## This tests that re-running a previously-run simulation yields the same result
## as before.
##
## If any of the crop-specific defaults for the initial values, parameters,
## modules or weather data change, or if the behavior of any of these modules
## changes, the stored data for one or more crops will likely need to be
## updated. Instructions for doing so are included in
## `regression_testing_helper_functions.R`.

source('regression_testing_helper_functions.R')

context(paste(
    "Test several biological systems with known results to ensure that the",
    "output doesn't change."
))

# Run all the tests
for(i in 1:length(PLANT_TESTING_INFO)) {
    test_plant_model(PLANT_TESTING_INFO[[i]])
}
