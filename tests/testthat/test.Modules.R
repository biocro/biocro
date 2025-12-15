# Test all the modules in the library
modules_to_skip <- c()

test_that("All modules are functioning correctly", {
    expect_no_error(
        test_module_library(
            'BioCro',
            file.path('..', 'module_test_cases'),
            modules_to_skip
        )
    )
})

test_that("all module inputs must be defined when calling `evaluate_module`", {
    expect_error(
        evaluate_module('BioCro:thermal_time_linear', list()),
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
        'BioCro:solar_position_michalsky',
        file.path('..', 'module_test_cases')
    )[[1]]

    case$expected_outputs$cosine_zenith_angle <- NULL

    expect_warning(
        test_module('BioCro:solar_position_michalsky', case),
        'Module `BioCro:solar_position_michalsky` test case `night`: unexpected outputs were found: cosine_zenith_angle'
    )
})

test_that('Errors can be ignored when evaluating modules', {
    fvcb_module <- 'BioCro:FvCB'

    basic_fvcb_inputs <- list(
        Gstar = 38.6,
        J = 170,
        Kc = 259,
        Ko = 179,
        Oi = 210,
        RL = 1,
        TPU = 11.8,
        Vcmax = 100,
        alpha_TPU = 0,
        electrons_per_carboxylation = 4,
        electrons_per_oxygenation = 4
    )

    neg_ci_error_msg <- 'Caught exception in R_evaluate_module: Thrown in FvCB_assim: Ci is negative.'

    expect_error(
        evaluate_module(
            fvcb_module,
            within(basic_fvcb_inputs, {Ci = -1}),
            stop_on_exception = TRUE
        ),
        neg_ci_error_msg
    )

    error_msg <- expect_silent(
        evaluate_module(
            fvcb_module,
            within(basic_fvcb_inputs, {Ci = -1}),
            stop_on_exception = FALSE
        )
    )

    expect_equal(
        error_msg$error_msg,
        neg_ci_error_msg
    )

    expect_error(
        module_response_curve(
            fvcb_module,
            basic_fvcb_inputs,
            data.frame(Ci = c(380, -1)),
            stop_on_exception = TRUE
        ),
        neg_ci_error_msg
    )

    rc <- expect_silent(
        module_response_curve(
            fvcb_module,
            basic_fvcb_inputs,
            data.frame(Ci = c(380, -1)),
            stop_on_exception = FALSE
        )
    )

    expect_equal(
        rc$error_msg,
        c(NA, neg_ci_error_msg)
    )
})
