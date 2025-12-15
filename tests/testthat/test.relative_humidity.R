# Run the soybean model for one year so we can test its humidity outputs
soybean_res <- with(soybean, {
    run_biocro(
        initial_values,
        parameters,
        soybean_weather[['2004']],
        direct_modules,
        differential_modules,
        default_ode_solvers[['homemade_euler']]
    )
})

# Get the names of all the humidity columns
all_rh_name_components <- expand.grid(
    leaf_class = c('sunlit_', 'shaded_'),
    base_name = c('RH_canopy', 'RHs'),
    layer_prefix = '_layer_',
    layer_num = seq(0,9)
)

all_rh_column_names <- with(all_rh_name_components, {
    paste0(leaf_class, base_name, layer_prefix, layer_num)
})

# Test each column for negative values
for (cn in all_rh_column_names) {
    test_name <- paste('Check that', cn, 'is never negative')
    test_that(test_name, {
        expect_true(
            all(soybean_res[[cn]] >= 0)
        )
    })
}
