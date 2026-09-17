# The temperature responses of Vcmax and Jmax are calculated using a peaked
# Arrhenius equation. The stored test cases for the `c3_parameters` module check
# individual values; here we check properties of the response that should hold
# for any parameter values:
#
# - It is normalized to 1 at the reference temperature (25 degrees C).
#
# - When the enthalpy of deactivation is infinite and the entropy is negative
#   infinity, it reduces to the (non-peaked) Arrhenius equation and does not
#   produce any non-finite values.
#
# - When finite values are supplied, the response has an optimum temperature,
#   and lies below the Arrhenius response at high temperature.

# Define constants
celsius_to_kelvin <- 273.15             # K or degrees C
ideal_gas_constant <- 8.31446261815324  # J / K / mol
reference_temperature <- 25             # degrees C

# A copy of the C++ function with the same name
arrhenius_exponential <- function(
    activation_energy,        # J / mol
    reference_temperature_k,  # K
    temperature_k             # K
)
{
    exp(activation_energy / ideal_gas_constant *
        (1 / reference_temperature_k - 1 / temperature_k))
}

# Default inputs for the `c3_parameters` module. The values of `Jmax_Hd`,
# `Jmax_S`, `Vcmax_Hd`, and `Vcmax_S` will be set in each test.
c3_parameters_inputs <- list(
    gm_Ha = 49600,
    gm_Hd = 437400,
    gm_S = 1400,
    Gstar_Ea = 37830,
    Jmax_Ha = 43540,
    Kc_Ea = 79430,
    Ko_Ea = 36380,
    phi_PSII_0 = 0.352,
    phi_PSII_1 = 0.022,
    phi_PSII_2 = -3.4e-4,
    RL_Ea = 46390,
    theta_0 = 0.76,
    theta_1 = 0.018,
    theta_2 = -3.7e-4,
    Tp_Ha = 62990,
    Tp_Hd = 182140,
    Tp_S = 588,
    Vcmax_Ha = 65330
)

# A helping function that calculates a temperature response curve
c3_parameters_curve <- function(
    Hd,    # J / mol
    S,     # J / K / mol
    Tleaf  # degrees C
)
{
    module_response_curve(
        'BioCro:c3_parameters',
        within(c3_parameters_inputs, {
            Jmax_Hd = Hd
            Jmax_S = S
            Vcmax_Hd = Hd
            Vcmax_S = S
        }),
        data.frame(Tleaf = Tleaf)
    )
}

# Leaf temperatures to use for the tests
leaf_temperatures <- seq(0, 50, by = 2.5)  # degrees C

# Values of Hd and S that produce an optimum within the temperature range. These
# are illustrative values of the order of those reported in Medlyn et al. (2002)
# [https://doi.org/10.1046/j.1365-3040.2002.00891.x].
finite_Hd <- 200000  # J / mol
finite_S <- 650      # J / K / mol

test_that('Vcmax and Jmax responses reduce to Arrhenius responses when deactivation is disabled', {
    res <- c3_parameters_curve(Inf, -Inf, leaf_temperatures)

    expected_vcmax_norm <- arrhenius_exponential(
        c3_parameters_inputs[['Vcmax_Ha']],
        reference_temperature + celsius_to_kelvin,
        leaf_temperatures + celsius_to_kelvin
    )

    expected_jmax_norm <- arrhenius_exponential(
        c3_parameters_inputs[['Jmax_Ha']],
        reference_temperature + celsius_to_kelvin,
        leaf_temperatures + celsius_to_kelvin
    )

    expect_equal(res[['Vcmax_norm']], expected_vcmax_norm)
    expect_equal(res[['Jmax_norm']], expected_jmax_norm)

    # Infinite parameter values should not produce NaN or infinite outputs
    expect_true(all(is.finite(res[['Vcmax_norm']])))
    expect_true(all(is.finite(res[['Jmax_norm']])))
})

test_that('Vcmax and Jmax responses are normalized at the reference temperature', {
    for (param in list(c(Inf, -Inf), c(finite_Hd, finite_S), c(150000, 490))) {
        res <- c3_parameters_curve(param[1], param[2], reference_temperature)

        expect_equal(res[['Vcmax_norm']], 1)
        expect_equal(res[['Jmax_norm']], 1)
    }
})

test_that('Vcmax and Jmax responses are peaked when deactivation is enabled', {
    arrhenius_res <- c3_parameters_curve(Inf, -Inf, leaf_temperatures)
    peaked_res <- c3_parameters_curve(finite_Hd, finite_S, leaf_temperatures)

    n <- length(leaf_temperatures)

    for (norm_name in c('Vcmax_norm', 'Jmax_norm')) {
        peaked_norm <- peaked_res[[norm_name]]
        arrhenius_norm <- arrhenius_res[[norm_name]]

        # The Arrhenius response always increases with temperature
        expect_true(all(diff(arrhenius_norm) > 0))

        # The peaked response has a maximum inside the temperature range
        optimum_index <- which.max(peaked_norm)
        expect_true(optimum_index > 1 && optimum_index < n)

        # The peaked response increases below its optimum and decreases above it
        expect_true(all(diff(peaked_norm[seq_len(optimum_index)]) > 0))
        expect_true(all(diff(peaked_norm[seq(optimum_index, n)]) < 0))

        # The peaked response is below the Arrhenius one at high temperature
        expect_true(peaked_norm[n] < arrhenius_norm[n])
    }
})

test_that('Vcmax and Jmax deactivation parameters do not affect other responses', {
    arrhenius_res <- c3_parameters_curve(Inf, -Inf, leaf_temperatures)
    peaked_res <- c3_parameters_curve(finite_Hd, finite_S, leaf_temperatures)

    other_outputs <- setdiff(
        module_info('BioCro:c3_parameters', verbose = FALSE)[['outputs']],
        c('Vcmax_norm', 'Jmax_norm')
    )

    for (output_name in other_outputs) {
        expect_identical(peaked_res[[output_name]], arrhenius_res[[output_name]])
    }
})
