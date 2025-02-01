# The purpose of this file is to make sure certain types of model errors are
# caught by the model testing functions

source('crop_model_testing_helper_functions.R')

example_test_case <- PLANT_TESTING_INFO[[1]]

test_that('definitions must be valid', {
    expect_error(
        run_model_test_cases(
            list(
                within(example_test_case, {parameters = list()})
            )
        ),
        'The `miscanthus_x_giganteus` simulation does not have a valid definition.'
    )
})

test_that('missing files are detected', {
    expect_error(
        run_model_test_cases(
            list(
                within(example_test_case, {stored_result_file = 'fake_file.csv'})
            )
        ),
        'Stored result file `fake_file.csv` does not exist.'
    )
})

test_that('simulation must run to completion', {
    expect_error(
        run_model_test_cases(
            list(
                within(example_test_case, {drivers = drivers[seq_len(3), ]})
            )
        ),
        'The `miscanthus_x_giganteus` simulation result has 3 rows, but the saved result has 4296 rows.'
    )
})

test_that('separate errors are reported for each model', {
    bad_cases <- PLANT_TESTING_INFO
    bad_cases[[1]]$parameters <- list()
    bad_cases[[2]]$stored_result_file <- 'fake_file.csv'
    bad_cases[[3]]$drivers <- bad_cases[[3]]$drivers[seq_len(3), ]

    expect_error(
        run_model_test_cases(bad_cases),
        'The `miscanthus_x_giganteus` simulation does not have a valid definition.\n  Stored result file `fake_file.csv` does not exist.\n  The `soybean` simulation result has 3 rows, but the saved result has 3288 rows.'
    )
})

test_that('warning occurs for extra columns in new results', {
    expect_warning(
        run_model_test_cases(
            list(
                within(example_test_case, {direct_modules = append(direct_modules, 'BioCro:total_biomass')})
            )
        ),
        'The `miscanthus_x_giganteus` simulation result contains columns that are not in the saved result. Is this intentional? Extra columns: total_biomass.',
        fixed = TRUE
    )
})

test_that('new result must include all stored columns', {
    expect_error(
        run_model_test_cases(
            list(
                within(example_test_case, {
                    parameters$time_zone_offset = drivers$time_zone_offset[1]
                    drivers$time_zone_offset = NULL
                })
            )
        ),
        'The `miscanthus_x_giganteus` simulation result is missing required columns from the saved result: time_zone_offset.'
    )
})

test_that('new values must agree with old values', {
    expect_error(
        run_model_test_cases(
            list(
                within(example_test_case, {drivers$time_zone_offset = -5})
            )
        ),
        'The new `miscanthus_x_giganteus` simulation result does not agree with the stored result for the following columns: canopy_conductance, canopy_height, canopy_transpiration_rate, cosine_zenith_angle, gbw_canopy, gmst, lai, Leaf, LeafLitter, lha, lmst, Rhizome, Root, solar_azimuth_angle, solar_zenith_angle, Stem, StemLitter, time_zone_offset.'
    )

    expect_error(
        run_model_test_cases(
            list(
                within(example_test_case, {parameters$Rd = parameters$Rd * 1.1})
            )
        ),
        'The new `miscanthus_x_giganteus` simulation result does not agree with the stored result for the following columns: canopy_conductance, canopy_height, lai, Leaf, LeafLitter, Rhizome, Root, Stem, StemLitter.'
    )
})
