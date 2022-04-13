context("Run test cases for all modules")

# Test all the modules in the library, with a few exceptions
modules_to_skip <- c(
    'ed_c4_leaf_photosynthesis',
    'ed_c4_leaf_photosynthesis2',
    'ed_c4_leaf_photosynthesis3',
    'ed_c4_leaf_photosynthesis4',
    'ed_nikolov_conductance_free_solve',
    'ed_p_m_temperature_solve',
    'ed_ten_layer_c4_canopy'
)

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
             "The `BioCro:thermal_time_linear` module requires `sowing_time` as an input quantity\n",
             "  The `BioCro:thermal_time_linear` module requires `tbase` as an input quantity\n",
             "  The `BioCro:thermal_time_linear` module requires `temp` as an input quantity\n",
             "  The `BioCro:thermal_time_linear` module requires `time` as an input quantity\n"
        )
    )
})
