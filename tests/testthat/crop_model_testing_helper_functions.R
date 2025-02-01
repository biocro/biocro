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

# Use a 0.5% tolerance when comparing values between simulations. Lower
# tolerances have caused problems when comparing results calculated on different
# operating systems.
RELATIVE_ERROR_TOLERANCE <- 5e-3

# Choose default weather
WEATHER <- get_growing_season_climate(weather$'2005')

# Specify the testing directory
TEST_DIR <- file.path('..', 'test_data')

# Specify the row interval to keep
ROW_INTERVAL <- 24

# Define lists of species-specific variables to ignore.

MISCANTHUS_X_GIGANTEUS_IGNORE <- c(
    "soil_evaporation_rate"
)

WILLOW_IGNORE <- character(0)

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
    "sunlit_EPriestly_layer_9"
)

# Define the plants to test
PLANT_TESTING_INFO <- list(
    model_test_case('miscanthus_x_giganteus', miscanthus_x_giganteus, WEATHER,                TRUE, TEST_DIR, ROW_INTERVAL, MISCANTHUS_X_GIGANTEUS_IGNORE, RELATIVE_ERROR_TOLERANCE), # INDEX = 1
    model_test_case('willow',                 willow,                 WEATHER,                TRUE, TEST_DIR, ROW_INTERVAL, WILLOW_IGNORE,                 RELATIVE_ERROR_TOLERANCE), # INDEX = 2
    model_test_case('soybean',                soybean,                soybean_weather$'2002', TRUE, TEST_DIR, ROW_INTERVAL, SOYBEAN_IGNORE,                RELATIVE_ERROR_TOLERANCE)  # INDEX = 3
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

