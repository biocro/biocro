# Test all the modules in the library
modules_to_skip <- c()

test_that("All modules are functioning correctly", {
    expect_no_error(
        test_module_library(
            "BioCro",
            file.path("..", "module_test_cases"),
            modules_to_skip
        )
    )
})

test_that("all module inputs must be defined when calling `evaluate_module`", {
    expect_error(
        evaluate_module("BioCro:thermal_time_linear", list()),
        regexp = paste0(
            "The `BioCro:thermal_time_linear` module requires `fractional_doy` as an input quantity\n",
            "  The `BioCro:thermal_time_linear` module requires `sowing_fractional_doy` as an input quantity\n",
            "  The `BioCro:thermal_time_linear` module requires `tbase` as an input quantity\n",
            "  The `BioCro:thermal_time_linear` module requires `temp` as an input quantity\n"
        )
    )
})

test_that("unexpected module outputs produce a warning", {
    case <- cases_from_csv(
        "BioCro:solar_position_michalsky",
        file.path("..", "module_test_cases")
    )[[1]]

    case$expected_outputs$cosine_zenith_angle <- NULL

    expect_warning(
        test_module("BioCro:solar_position_michalsky", case),
        "Module `BioCro:solar_position_michalsky` test case `night`: unexpected outputs were found: cosine_zenith_angle"
    )
})
