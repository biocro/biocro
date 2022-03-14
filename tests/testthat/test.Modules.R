context("Run test cases for all modules")

# TO-DO: make a "test_all_modules" function that runs all module tests for a
# library, returning a list of any problematic test cases. This would be part of
# the BioCro framework and wouldn't use `testthat`. Then, this code can become
# even simpler, with just a single instance of the package name, making it
# easier to implement tests for external module libraries.

# Define a helping function that tests a module unless it should be skipped
run_test <- function(module_name, library_name) {
    modules_to_skip <- c(
        'ed_c4_leaf_photosynthesis',
        'ed_c4_leaf_photosynthesis2',
        'ed_c4_leaf_photosynthesis3',
        'ed_c4_leaf_photosynthesis4',
        'ed_nikolov_conductance_free_solve',
        'ed_p_m_temperature_solve',
        'ed_ten_layer_c4_canopy'
    )

    if (module_name %in% modules_to_skip) {
        print(paste('Test of', module_name, 'needs fixing.'))
    } else {
        cases <- cases_from_csv(module_name, file.path('module_tests'))
        test_that(
            paste0("The '", module_name, "' module is functioning"), {
                lapply(cases, function(x) {
                    expect_error(
                        test_module(module_paste(library_name, module_name), x),
                        regexp = NA
                    )
                })
        })
    }
}

# Check test cases for all modules
lib <- 'BioCro'
lapply(
    get_all_modules(lib),
    function(m) {run_test(m, lib)}
)

# Make sure the `evaluate_module` function is properly reporting missing inputs
test_that("all module inputs must be defined when calling `evaluate_module`", {
    expect_error(
        evaluate_module('BioCro:thermal_time_linear', list()),
        regexp = paste0(
             "The `thermal_time_linear` module requires `sowing_time` as an input quantity\n",
             "  The `thermal_time_linear` module requires `tbase` as an input quantity\n",
             "  The `thermal_time_linear` module requires `temp` as an input quantity\n",
             "  The `thermal_time_linear` module requires `time` as an input quantity\n"
        )
    )
})
