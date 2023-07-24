test_that("a real module can be checked out", {
    expect_no_error(BioCro:::check_out_module('BioCro:thermal_time_linear'))
})

test_that("modules from fake packages cannot be checked out", {
    expect_error(
        BioCro:::check_out_module('fakePKG:module'),
        "Encountered an issue with module `fakePKG:module`: Error in function_from_package(library_name, fn): There is no package called `fakePKG`",
        fixed = TRUE
    )
})

test_that("fake modules cannot be checked out", {
    expect_error(
        BioCro:::check_out_module('BioCro:fake_module'),
        '"fake_module" was given as a module name, but no module with that name could be found.',
        fixed = TRUE
    )
})
