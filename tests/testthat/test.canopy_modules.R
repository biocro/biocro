# Specify the tolerance to use and the default number of layers
TOLERANCE      <- 1e-6
DEFAULT_NLAYER <- 10

# Get the first 100 hours of the 2002 soybean weather data
WEATHER <- soybean_weather[['2002']][seq_len(100), ]

# Run the default soybean model
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

# The BioCro:c3_canopy module replaces several other modules
direct_modules_to_remove <- c(
    'BioCro:shortwave_atmospheric_scattering',
    'BioCro:incident_shortwave_from_ground_par',
    'BioCro:ten_layer_canopy_properties',
    'BioCro:ten_layer_c3_canopy',
    'BioCro:ten_layer_canopy_integrator'
)

alternate_direct_modules <- soybean$direct_modules[!soybean$direct_modules %in% direct_modules_to_remove]
alternate_direct_modules <- append(alternate_direct_modules, 'BioCro:c3_canopy')

# The BioCro:c3_canopy module needs several other input parameters
alternate_soybean <- within(soybean, {
    parameters = within(parameters, {
        nlayers = 0
        lnb0 = 0
        lnb1 = 0
    })

    direct_modules = alternate_direct_modules
})

# Use partial application to create a function that runs the alternate soybean
# model with a specified number of layers
pfunc <- with(alternate_soybean, {partial_run_biocro(
    initial_values,
    parameters,
    WEATHER,
    direct_modules,
    differential_modules,
    ode_solver,
    arg_name = 'nlayers'
)})

test_that('c3_canopy module produces the same results as the default soybean modules', {
    # Simulation must run without errors
    alternate_soybean_result <- expect_silent(
        pfunc(list(nlayers = DEFAULT_NLAYER))
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
            tolerance = TOLERANCE
        )
    }
})

test_that('c3_canopy module with fewer layers produces different results', {
    # Check the canopy assimilation rate when fewer layers are used
    assim_col <- 'canopy_assimilation_rate'

    alternate_soybean_result_fewer <- pfunc(list(nlayers = DEFAULT_NLAYER - 1))

    expect_false(
        isTRUE(all.equal(
            alternate_soybean_result_fewer[[assim_col]],
            default_soybean_result[[assim_col]],
            tolerance = TOLERANCE
        ))
    )

    # Check the canopy assimilation rate when the minimum number of layers is
    # used
    alternate_soybean_result_min <- pfunc(list(nlayers = 1))

    expect_false(
        isTRUE(all.equal(
            alternate_soybean_result_min[[assim_col]],
            default_soybean_result[[assim_col]],
            tolerance = TOLERANCE
        ))
    )
})
