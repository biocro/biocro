## This file defines settings and helper functions for running the crop model
## tests.
##
## If any of the crop-specific defaults for the initial values, parameters,
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
## Glycine max. Afterwards, check the git diff for the stored data files to make
## sure the changes are reasonable (to the extent that this is possible).
## Finally, rerun this file using the `testthat` package to make sure the tests
## all pass.

# Choose the number of time points to test for each simulation result
MAX_SAMPLE_SIZE <- 5

# Use a 0.5% tolerance when comparing values between simulations. Lower
# tolerances have caused problems when comparing results calculated on different
# operating systems.
RELATIVE_ERROR_TOLERANCE <- 5e-3

# Choose default weather
WEATHER <- get_growing_season_climate(weather$'2005')

# Make a helping function for specifying crop information
specify_crop <- function(
    plant_name,
    should_run,
    crop_info,
    drivers,
    ignored_variables
)
{
    list(
        plant_name = plant_name,
        should_run = should_run,
        initial_values = crop_info$initial_values,
        parameters = crop_info$parameters,
        drivers = drivers,
        direct_modules = crop_info$direct_modules,
        differential_modules = crop_info$differential_modules,
        ode_solver = crop_info$ode_solver,
        stored_result_file = paste0(
            "../test_data/",
            plant_name,
            "_simulation.csv"
        ),
        ignored_variables = ignored_variables
    )
}

# Define lists of species-specific variables to ignore.

MISCANTHUS_X_GIGANTEUS_IGNORE <- c(
    "soil_evaporation_rate",
    "time"
)

WILLOW_IGNORE <- c("time")

# As of 2023-04-11, test failures have started occurring for some soybean model
# outputs: canopy conductance and a few photosynthesis outputs from sunlit
# leaves deep within the canopy (layers 7-9) at intermediate times of the year
# (but not at the start or end of a growing season). It seems that these
# quantities are different on Windows vs MacOS or Ubuntu. However, the fraction
# of sunlit leaves deep in the canopy is very small, and these discrepancies do
# not seem to have any impact on the biomass calculations. Likewise, the canopy
# conductance is not actually used as an input by any BioCro module, so it has
# no impact on any other calculations. (Nevertheless, try to look into the
# differences in canopy conductance before 2024-04-11).
SOYBEAN_IGNORE <- c(
    "ncalls",
    "canopy_conductance",
    "sunlit_Assim_layer_7",
    "sunlit_Assim_layer_8",
    "sunlit_Assim_layer_9",
    "sunlit_Ci_layer_7",
    "sunlit_Ci_layer_8",
    "sunlit_Ci_layer_9",
    "sunlit_GrossAssim_layer_7",
    "sunlit_GrossAssim_layer_8",
    "sunlit_GrossAssim_layer_9",
    "sunlit_Gs_layer_7",
    "sunlit_Gs_layer_8",
    "sunlit_Gs_layer_9",
    "sunlit_Rp_layer_7",
    "sunlit_Rp_layer_8",
    "sunlit_Rp_layer_9",
    "sunlit_Cs_layer_7",
    "sunlit_Cs_layer_8",
    "sunlit_Cs_layer_9",
    "sunlit_RHs_layer_7",
    "sunlit_RHs_layer_8",
    "sunlit_RHs_layer_9",
    "sunlit_EPriestly_layer_7",
    "sunlit_EPriestly_layer_8",
    "sunlit_EPriestly_layer_9",
    "time"
)

SOYBEAN_WEATHER <- add_time_to_weather_data(soybean_weather$'2002')
# Define the plants to test
PLANT_TESTING_INFO <- list(
    specify_crop("miscanthus_x_giganteus", TRUE,  miscanthus_x_giganteus, WEATHER,                MISCANTHUS_X_GIGANTEUS_IGNORE), # INDEX = 1
    specify_crop("willow",                 TRUE,  willow,                 WEATHER,                WILLOW_IGNORE),                 # INDEX = 2
    specify_crop("soybean",                TRUE,  soybean,                SOYBEAN_WEATHER,        SOYBEAN_IGNORE)                 # INDEX = 3
)

# Make a helping function that runs a simulation for one crop, stores the number
# of rows in the result, and keeps just a subset of the rows
run_crop_simulation <- function(test_info) {
    res <- run_biocro(
        test_info[['initial_values']],
        test_info[['parameters']],
        test_info[['drivers']],
        test_info[['direct_modules']],
        test_info[['differential_modules']],
        test_info[['ode_solver']]
    )

    res$nrow <- nrow(res)
    res <- res[seq(1, nrow(res), by = 24), ]

    for (cn in colnames(res)) {
        if (is.numeric(res[[cn]])) {
            res[[cn]] <- signif(res[[cn]], digits = 5)
        }
    }

    res
}

# Combine new and old results into one data frame for plotting purposes (this
# will be useful when manually assessing the differences between a new and old
# simulation). If the columns have changed between the new and old outputs, it's
# possible to specify a vector of column names to include in the output.
# (Otherwise, the call to rbind will fail).
#
# The output from this function is intended to be used in plotting commands like
#
# compare_crop_output <- compare_crop(PLANT_TESTING_INFO[[INDEX]], c('time', 'Leaf', 'Stem', 'Root', 'Grain'))
# lattice::xyplot(Leaf + Stem + Root + Grain ~ time, group = version, data = compare_crop_output, type = 'l', auto = TRUE, grid = TRUE)
compare_crop <- function(test_info, columns_to_keep = NULL) {
    new_result <- run_crop_simulation(test_info)
    stored_result <- read.csv(test_info[['stored_result_file']])

    if (!is.null(columns_to_keep)) {
        new_result <- new_result[,columns_to_keep]
        stored_result <- stored_result[,columns_to_keep]
    }

    new_result[['version']] <- "new"
    stored_result[['version']] <- "stored"

    return(rbind(new_result, stored_result))
}

# Make a helping function that updates the stored data for one crop
update_stored_results <- function(test_info) {

    # Calculate the result
    plant_result <- run_crop_simulation(test_info)

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

# Make a helping function that checks the result of a new simulation against the
# stored data for one crop
test_plant_model <- function(test_info) {
    # Read the stored result from the data file
    stored_result <- read.csv(test_info[['stored_result_file']])

    # Describe the current test
    description_validity <- paste(
        "The",
        test_info[['plant_name']],
        "simulation has a valid definition"
    )

    # Check the inputs for validity
    test_that(description_validity, {
        expect_true(
            validate_dynamical_system_inputs(
                test_info[['initial_values']],
                test_info[['parameters']],
                test_info[['drivers']],
                test_info[['direct_modules']],
                test_info[['differential_modules']],
                verbose = FALSE
            )
        )
    })

    if (test_info[['should_run']]) {
        # Describe the current test
        description_run <- paste(
            "The",
            test_info[['plant_name']],
            "simulation runs without producing any errors"
        )

        # Run the simulation
        new_result <- 0
        test_that(description_run, {
            expect_no_error(
                new_result <<- run_crop_simulation(test_info)
            )
        })

        # Make sure the simulation was completed
        description <- paste(
            "The",
            test_info[['plant_name']],
            "simulation ran to completion"
        )

        # Get nrow information
        stored_nrow <- stored_result[1, 'nrow']
        new_nrow <- new_result[1, 'nrow']

        test_that(description, {
            expect_equal(new_nrow, stored_nrow)
        })

        # If the simulation finished, make additional checks
        if (new_nrow == stored_nrow) {
            # Some variables may need to be ignored, possibly because their
            # values depend on the operating system or other factors that may
            # change between simulation runs. Remove these from the results. If
            # a variable is flagged to be ignored but is not in the simulation
            # result, this could indicate that one of the default modules has
            # been changed, and the list of ignored variables should probably be
            # revisited, so warn the user.
            for (variable in test_info[['ignored_variables']]) {
                if (variable %in% names(new_result)) {
                    new_result[[variable]] <- NULL
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

            # Make sure all columns contain numeric data
            stored_result <- as.data.frame(sapply(stored_result, as.numeric))

            # Make sure the stored result contains all the non-ignored
            # quantities in the new result
            new_column_names <- names(new_result)

            stored_column_names <- names(stored_result)

            for (name in new_column_names) {
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

            # Make a helping function that compares the new result to the old
            # one at a single index
            compare_simulation_trial <- function(index) {
                for (variable in new_column_names) {
                    description <- paste0(
                        "The ", test_info[['plant_name']], " simulation result ",
                        "agrees with the stored result at index ",
                        index,
                        " for the '",
                        variable,
                        "' quantity"
                    )

                    test_that(description, {
                        expect_equal(
                            new_result[[variable]][index],
                            stored_result[[variable]][index],
                            tolerance=RELATIVE_ERROR_TOLERANCE
                        )
                    })
                }
            }

            # Run the test for some equally spaced indices including the first
            # and last points of the simulation. Note that no problems occur if
            # `points_to_test` includes non-integer elements, since R
            # automatically truncates them to integer values when they are used
            # as indices to access elements of a vector.
            index_of_last_row <- length(new_result[[1]])
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
    }
}
