# Get the first 100 hours of the 2002 soybean weather data
WEATHER <- soybean_weather[['2002']][seq_len(100), ]

# Run the soybean model
default_soybean_result <- with(soybean, {run_biocro(
    initial_values,
    parameters,
    WEATHER,
    direct_modules,
    differential_modules,
    ode_solver
)})

# Define an alternate version of the soybean model using the `BioCro:c3_canopy`
# module

# The BioCro:c3_canopy module replaces the following other modules, provided
# the number of layers is set to 10
direct_modules_to_remove <- c(
    'BioCro:shortwave_atmospheric_scattering',
    'BioCro:incident_shortwave_from_ground_par',
    'BioCro:ten_layer_canopy_properties',
    'BioCro:ten_layer_c3_canopy',
    'BioCro:ten_layer_canopy_integrator'
)

alternate_direct_modules <- soybean$direct_modules[!soybean$direct_modules %in% direct_modules_to_remove]
alternate_direct_modules <- append(alternate_direct_modules, 'BioCro:c3_canopy')

alternate_soybean <- within(soybean, {
    parameters = within(parameters, {
        nlayers = 10 # needed for BioCro:c3_canopy
        lnb0 = 0     # needed for BioCro:c3_canopy
        lnb1 = 0     # needed for BioCro:c3_canopy
    })

    direct_modules = alternate_direct_modules
})

test_that('c3_canopy module produces the same results as the default soybean modules', {
    # Definition must be valid
    expect_true(
        with(alternate_soybean, {validate_dynamical_system_inputs(
            initial_values,
            parameters,
            WEATHER,
            direct_modules,
            differential_modules,
            verbose = FALSE
        )})
    )

    # Simulation must run without errors
    alternate_soybean_result <- expect_silent(
        with(alternate_soybean, {run_biocro(
            initial_values,
            parameters,
            WEATHER,
            direct_modules,
            differential_modules,
            ode_solver
        )})
    )

    # Some columns should be in the default result but not the alternate result
    removed_columns <- c('shaded_RHs_layer_6', 'sunlit_absorbed_ppfd_layer_0')

    expect_true(
        all(removed_columns %in% colnames(default_soybean_result))
    )

    expect_true(
        all(!removed_columns %in% colnames(alternate_soybean_result))
    )

    # Key columns must be in both results
    col_to_check <- c('canopy_assimilation_rate', 'Leaf', 'Stem', 'Grain')

    expect_true(
        all(col_to_check %in% colnames(default_soybean_result))
    )

    expect_true(
        all(col_to_check %in% colnames(alternate_soybean_result))
    )

    # Key columns must have the same values in both results
    for (cn in col_to_check) {
        expect_equal(
            alternate_soybean_result[[cn]],
            default_soybean_result[[cn]],
            tolerance = 1e-3
        )
    }
})
