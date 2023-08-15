test_that("a fully qualified module name can be parsed", {
    expect_no_error(BioCro:::parse_module_name('library_name:module_name'))
})

test_that("an improper module name produces an error", {
    expect_error(
        BioCro:::parse_module_name('bad_name'),
        'The module name string `bad_name` is not formatted as `module_library_name:module_name`'
    )
})
