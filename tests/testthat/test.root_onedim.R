# We use the module testing framework to test the one-dimensional root solvers
# via the `BioCro:root_onedim_test` module. Here, we make sure that the test
# cases are properly defined. In order to fully test the solvers:
#
# - There should be one test case where `single_guess` does not equal `answer`
#
# - There should be one test case where `single_guess` equals `answer`
#
# - There should be one test case where `lower_bracket` equals `answer`

# - There should be one test case where `upper_bracket` equals `answer`
#
# - All of the `*_last_x_diff` outputs must be zero

module <- 'BioCro:root_onedim_test'

cases <- cases_from_csv(
    module,
    file.path('..', 'module_test_cases')
)

last_x_diff_col <- grepl('_last_x_diff', names(cases[[1]]$expected_outputs))

incorrect_initial_guess <- sapply(cases, function(x) {
    x$inputs$single_guess != x$inputs$answer
})

correct_initial_guess <- sapply(cases, function(x) {
    x$inputs$single_guess == x$inputs$answer
})

correct_lower_bracket <- sapply(cases, function(x) {
    x$inputs$lower_bracket == x$inputs$answer
})

correct_upper_bracket <- sapply(cases, function(x) {
    x$inputs$upper_bracket == x$inputs$answer
})

last_x_diff_zero <- sapply(cases, function(x) {
    all(x$expected_outputs[last_x_diff_col] == 0)
})

test_that('At least one 1D root solver test case has an incorrect starting guess', {
    expect_true(any(incorrect_initial_guess))
})

test_that('At least one 1D root solver test case has a correct starting guess', {
    expect_true(any(correct_initial_guess))
})

test_that('At least one 1D root solver test case has a lower bracket equal to the root', {
    expect_true(any(correct_lower_bracket))
})

test_that('At least one 1D root solver test case has a lower bracket equal to the root', {
    expect_true(any(correct_upper_bracket))
})

test_that('All last_x_diff outputs are expected to be zero', {
    expect_true(all(last_x_diff_zero))
})
