# The purpose of this file is to make sure certain types of model errors are
# caught by the model testing functions

example_test_case <- model_test_case(
    'miscanthus_x_giganteus',
    miscanthus_x_giganteus,
    get_growing_season_climate(weather$'2005'),
    TRUE,
    file.path('..', 'test_data'),
    'soil_evaporation_rate'
)

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
    bad_cases <- list(
        within(example_test_case, {parameters = list()}),
        within(example_test_case, {stored_result_file = 'fake_file.csv'}),
        within(example_test_case, {drivers = drivers[seq_len(3), ]})
    )

    expect_error(
        run_model_test_cases(bad_cases),
        'The `miscanthus_x_giganteus` simulation does not have a valid definition.\n  Stored result file `fake_file.csv` does not exist.\n  The `miscanthus_x_giganteus` simulation result has 3 rows, but the saved result has 4296 rows.'
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
        'The new `miscanthus_x_giganteus` simulation result does not agree with the stored result for the following columns:'
    )

    expect_error(
        run_model_test_cases(
            list(
                within(example_test_case, {parameters$Rd = parameters$Rd * 1.1})
            )
        ),
        'The new `miscanthus_x_giganteus` simulation result does not agree with the stored result for the following columns:'
    )
})

test_that('results can be stored and loaded', {
    model_with_tempfile <-
        within(example_test_case, {stored_result_file = tempfile()})

    expect_silent(
        update_stored_model_results(model_with_tempfile)
    )

    col_to_keep <- c('time', 'Leaf', 'Stem', 'Root', 'Grain')

    compare_crop_output <- expect_silent(
        compare_model_output(model_with_tempfile, col_to_keep)
    )

    expect_true(
        all(col_to_keep %in% colnames(compare_crop_output))
    )

    expect_equal(
        sort(unique(compare_crop_output[['version']])),
        c('new', 'stored')
    )
})
