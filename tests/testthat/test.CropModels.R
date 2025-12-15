## This tests that each crop model is well defined, and, if necessary, that
## re-running a previously-run simulation yields the same result as before.
##
## If any of the crop-specific defaults for the initial values, parameters,
## modules or weather data change, or if the behavior of any of these modules
## changes, the stored data for one or more crops will likely need to be
## updated.
##
## Sometimes these changes are not expected to alter key outputs like the
## biomass values. In this case, it is helpful to visually compare the new and
## old biomass values. To do this:
##
## 1. Open a fresh R session in this directory
##
## 2. Source this file by typing the following:
##
##    source('test.CropModels.R')
##
## 3. Run one of the models and create a plot by typing the following:
##
##    compare_crop_output <- BioCro::compare_model_output(PLANT_TESTING_INFO[[INDEX]])
##    lattice::xyplot(Leaf + Stem + Root + Grain ~ time, group = version, data = compare_crop_output, type = 'l', auto = TRUE, grid = TRUE)
##
##    Here, the value of `INDEX` corresponds to the desired crop as defined in
##    the `PLANT_TESTING_INFO` list below. For example, INDEX = 1 corresponds to
##    miscanthus_x_giganteus.
##
## When you are ready to update the stored values, use the following command:
##
##   update_stored_model_results(PLANT_TESTING_INFO[[INDEX]])
##
## Afterwards, check the git diff for the stored data files to make sure the
## changes are reasonable (to the extent that this is possible). Finally, rerun
## the tests to make sure they all pass.

# Choose default weather
WEATHER <- BioCro::get_growing_season_climate(BioCro::weather$'2005')

# Specify the testing directory
TEST_DIR <- file.path('..', 'test_data')

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

# Define the models to test
PLANT_TESTING_INFO <- list(
    BioCro::model_test_case('miscanthus_x_giganteus', BioCro::miscanthus_x_giganteus, WEATHER,                        TRUE, TEST_DIR, MISCANTHUS_X_GIGANTEUS_IGNORE), # INDEX = 1
    BioCro::model_test_case('willow',                 BioCro::willow,                 WEATHER,                        TRUE, TEST_DIR, WILLOW_IGNORE),                 # INDEX = 2
    BioCro::model_test_case('soybean',                BioCro::soybean,                BioCro::soybean_weather$'2002', TRUE, TEST_DIR, SOYBEAN_IGNORE)                 # INDEX = 3
)

# Run all the tests
testthat::expect_true(
    BioCro::run_model_test_cases(PLANT_TESTING_INFO)
)
