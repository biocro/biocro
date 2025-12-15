## The purpose of this file is to make sure certain types of model errors are
## caught by the model testing functions. It uses a simple harmonic oscillator
## model. It is unlikely that the stored test results will need to be updated.
## But, if they do, follow these steps:
##
## 1. Open a fresh R session in this directory
##
## 2. Source this file by typing the following:
##
##    source('test.model_testing.R')
##
## 3. Update the stored values:
##
##   update_stored_model_results(example_test_case)
##
## Afterwards, check the git diff for the stored data files to make sure the
## changes are reasonable (to the extent that this is possible). Finally, rerun
## the tests to make sure they all pass.

# Define oscillator model and its test case
oscillator_model <- list(
    initial_values = list(
        position = 1,
        velocity = 0
    ),
    parameters = list(
        spring_constant = 0.5,
        timestep = 1
    ),
    direct_modules = 'BioCro:harmonic_energy',
    differential_modules = 'BioCro:harmonic_oscillator',
    ode_solver = default_ode_solvers$boost_rkck54
)

oscillator_drivers <- data.frame(
    time = seq_len(100),
    mass = rep_len(1, 100)
)

example_test_case <- model_test_case(
    'harmonic_oscillator',
    oscillator_model,
    oscillator_drivers,
    TRUE,
    file.path('..', 'test_data'),
    row_interval = 10
)

# Run tests
test_that('definitions must be valid', {
    expect_error(
        run_model_test_cases(
            list(
                within(example_test_case, {parameters = list()})
            )
        ),
        'The `harmonic_oscillator` simulation does not have a valid definition.'
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
        'The `harmonic_oscillator` simulation result has 3 rows, but the saved result has 100 rows.'
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
        'The `harmonic_oscillator` simulation does not have a valid definition.\n  Stored result file `fake_file.csv` does not exist.\n  The `harmonic_oscillator` simulation result has 3 rows, but the saved result has 100 rows.'
    )
})

test_that('warning occurs for extra columns in new results', {
    expect_warning(
        run_model_test_cases(
            list(
                within(example_test_case, {drivers$new_column <- 3})
            )
        ),
        'The `harmonic_oscillator` simulation result contains columns that are not in the saved result. Is this intentional? Extra columns: new_column.',
        fixed = TRUE
    )
})

test_that('new result must include all stored columns', {
    expect_error(
        run_model_test_cases(
            list(
                within(example_test_case, {
                    parameters$mass = drivers$mass[1]
                    drivers$mass = NULL
                })
            )
        ),
        'The `harmonic_oscillator` simulation result is missing required columns from the saved result: mass.'
    )
})

test_that('new values must agree with old values', {
    expect_error(
        run_model_test_cases(
            list(
                within(example_test_case, {drivers$mass = 2})
            )
        ),
        'The new `harmonic_oscillator` simulation result does not agree with the stored result for the following columns:'
    )

    expect_error(
        run_model_test_cases(
            list(
                within(example_test_case, {parameters$spring_constant = parameters$spring_constant * 1.1})
            )
        ),
        'The new `harmonic_oscillator` simulation result does not agree with the stored result for the following columns:'
    )
})

test_that('results can be stored and loaded', {
    model_with_tempfile <-
        within(example_test_case, {stored_result_file = tempfile()})

    expect_silent(
        update_stored_model_results(model_with_tempfile)
    )

    col_to_keep <- c('time', 'position', 'velocity', 'kinetic_energy', 'spring_energy')

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
