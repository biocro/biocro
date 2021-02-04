## This tests that re-running a previously-run simulation yields the same result
## as before.
##
## If any of the plant-specific defaults for the initial state, parameters,
## modules or weather data changes, or if the behavior of any of these modules
## changes, the stored data for this test will likely need to be updated. To do
## this, create a new R session and set the working directory to
## "biocro-dev\tests\test_data". Then load BioCro, run a default simulation
## (storing the output as `Gro_result`), and save it as `PLANT_simulation.rda`,
## where PLANT is the name of the plant species. E.g., for sorghum, the commands
## would be as follows:
##
## library(BioCro)
## Gro_result <- Gro(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_modules)
## save(Gro_result, file="sorghum_simulation.rda")
##
## Finally, after updating the stored data for any of the plants whose output
## has changed, run this test to make sure it is passed.

context("Test several biological systems with known results to ensure that the output doesn't change.")

SAMPLE_SIZE <- 5       # number of time points to test in each simulation result

# Define a function that checks the result of a new simulation against the
# stored data for one plant. For this test, we always use the the stored
# environmental data `get_growing_season_climate(weather05)`
test_plant_model <- function(test_info) {

    # Run the simulation
    result <- 0
    description <- paste("The ", test_info[['plant_name']], " simulation runs without producing any errors", sep="")
    test_that(description, {
        expect_silent(
            result <<- Gro(
                test_info[['initial_state']],
                test_info[['parameters']],
                get_growing_season_climate(weather05),
                test_info[['modules']]
            )
        )
    })

    # Make sure some important variables are included in the output
    column_names <- names(result)
    for (name in c("hour", "TTc", "Leaf", "Stem", "Root")) {
        description <- paste("The ", test_info[['plant_name']], " simulation result includes the ", name, " column", sep="")
        test_that(description, {
            expect_true(name %in% column_names)
        })
    }

    # Read the stored result ("Gro_result") from the data file:
    load(test_info[['stored_result_file']])

    # Make sure the stored result has the same number of time points
    index_of_last_row <- length(result$hour)
    description <- paste("The ", test_info[['plant_name']], " simulation result has the correct number of data points", sep="")
    test_that(description, {
        expect_equal(index_of_last_row, length(Gro_result$hour))
    })

    # Define a function that compares the new result to the old one at a single index
    compare_simulation_trial <- function(index) {
        description <- paste("The ", test_info[['plant_name']], " simulation result agrees with the stored result at index ", index, sep="")
        test_that(description, {
            for (variable in column_names) {
                expect_equal(result[[variable]][index], Gro_result[[variable]][index])
            }
        })
    }

    # Randomly choose a number of indices and test them
    sample <- sample(1:index_of_last_row, SAMPLE_SIZE)
    for (index in sample) {
        compare_simulation_trial(index)
    }

}

# Make a helping function for specifying plant information
specify <- function(plant_name, initial_state, parameters, modules, stored_result_file) {
    list(
        plant_name = plant_name,
        initial_state = initial_state,
        parameters = parameters,
        modules = modules,
        stored_result_file = stored_result_file
    )
}

# Define the plants to test
plant_testing_info <- list(
    specify("glycine_max",            glycine_max_initial_state,            glycine_max_parameters,            glycine_max_modules,            "../test_data/glycine_max_simulation.rda"),
    specify("manihot_esculenta",      manihot_esculenta_initial_state,      manihot_esculenta_parameters,      manihot_esculenta_modules,      "../test_data/manihot_esculenta_simulation.rda"),
    specify("miscanthus_x_giganteus", miscanthus_x_giganteus_initial_state, miscanthus_x_giganteus_parameters, miscanthus_x_giganteus_modules, "../test_data/miscanthus_x_giganteus_simulation.rda"),
    specify("sorghum",                sorghum_initial_state,                sorghum_parameters,                sorghum_modules,                "../test_data/sorghum_simulation.rda"),
    specify("willow",                 willow_initial_state,                 willow_parameters,                 willow_modules,                 "../test_data/willow_simulation.rda"),
    specify("zea_mays",               zea_mays_initial_state,               zea_mays_parameters,               zea_mays_modules,               "../test_data/zea_mays_simulation.rda")
)

# Run all the tests
for(i in 1:length(plant_testing_info)) {
    test_plant_model(plant_testing_info[[i]])
}
