# Test all the modules in the library
modules_to_skip <- c()

test_that("All modules are functioning correctly", {
    expect_error(
        test_module_library(
            'BioCro',
            file.path('..', 'module_test_cases'),
            modules_to_skip
        ),
        regexp = NA
    )
})

# Make sure the `evaluate_module` function is properly reporting missing inputs
test_that("all module inputs must be defined when calling `evaluate_module`", {
    expect_error(
        evaluate_module('BioCro:thermal_time_linear', list()),
        regexp = paste0(
             "The `BioCro:thermal_time_linear` module requires `doy` as an input quantity\n",
             "  The `BioCro:thermal_time_linear` module requires `sowing_time` as an input quantity\n",
             "  The `BioCro:thermal_time_linear` module requires `tbase` as an input quantity\n",
             "  The `BioCro:thermal_time_linear` module requires `temp` as an input quantity"
        )
    )
})
