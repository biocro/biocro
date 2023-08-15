test_that('a real function can be found without any errors', {
    expect_no_error(BioCro:::function_from_package('BioCro', 'run_biocro'))
})

test_that('helpful error occurs when package does not exist', {
    expect_error(
        BioCro:::function_from_package('fake_package', 'fake_function'),
        'There is no package called `fake_package`',
        fixed = TRUE
    )
})

test_that('helpful error occurs when function does not exist', {
    expect_error(
        BioCro:::function_from_package('BioCro', 'fake_function'),
        'A `fake_function` function could not be found in the `BioCro` package',
        fixed = TRUE
    )
})
