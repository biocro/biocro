# Helping function for checking carbon accounting for a particular growth
# calculator module using the soybean model as a base. Here we use a nonzero
# `growth_respiration_fraction` to test that it is properly accounted for.
test_soybean_carbon_accounting <- function(partitioning_calculator) {
    description <- paste0(
        'the soybean model accounts for all carbon when using `',
        partitioning_calculator,
        '` as its partitioning growth calculator module'
    )

    test_that(description, {
        soybean_res <- expect_silent(
            with(soybean, {run_biocro(
                c(initial_values, list(
                    canopy_assimilation = 0,
                    canopy_gross_assimilation = 0,
                    canopy_non_photorespiratory_CO2_release = 0,
                    canopy_photorespiration = 0,
                    canopy_transpiration = 0,
                    Grain_gr = 0,
                    Grain_mr = 0,
                    Leaf_gr = 0,
                    Leaf_mr = 0,
                    Leaf_WS_loss = 0,
                    Rhizome_gr = 0,
                    Rhizome_mr = 0,
                    Root_gr = 0,
                    Root_mr = 0,
                    Shell_gr = 0,
                    Shell_mr = 0,
                    soil_evaporation = 0,
                    Stem_gr = 0,
                    Stem_mr = 0,
                    total_precip = 0,
                    whole_plant_growth_respiration = 0
                )),
                within(parameters, {
                    growth_respiration_fraction = 0.01
                }),
                soybean_weather[['2002']],
                c(
                    list(
                        'BioCro:total_biomass',
                        'BioCro:total_growth_and_maintenance_respiration'
                    ),
                    within(direct_modules, {
                        partitioning_growth_calculator = partitioning_calculator
                    })
                ),
                c(differential_modules, list(
                    'BioCro:cumulative_carbon_dynamics',
                    'BioCro:cumulative_water_dynamics'
                ))
            )})
        )

        # Check that all the assimilated carbon (gross assimilation) is balanced
        # by the sum costs of photorespiration, non-photorespiratory CO2 release
        # by the leaf, growth respiration, maintenance respiration, tissue
        # growth, and litter formation.
        soybean_res$total_carbon_use <- with(soybean_res, {
            (total_intact_biomass - total_intact_biomass[1]) +
            (total_litter_biomass - total_litter_biomass[1]) +
            canopy_non_photorespiratory_CO2_release +
            canopy_photorespiration +
            total_growth_respiration +
            total_maintenance_respiration
        })

        expect_equal(
            soybean_res$canopy_gross_assimilation,
            soybean_res$total_carbon_use
        )

        ## Uncomment this when debugging test failures to visually check whether
        ## the difference is real
        #dev.new()
        #print(lattice::xyplot(
        #    total_carbon_use + canopy_gross_assimilation ~ fractional_doy,
        #    data = soybean_res,
        #    type = 'l',
        #    auto = TRUE,
        #    main = partitioning_calculator
        #))

        # Check that all CO2 loss rates are non-negative
        with(soybean_res, {
            expect_true(all(canopy_photorespiration_rate >= 0))
            expect_true(all(canopy_non_photorespiratory_CO2_release_rate >= 0))
            expect_true(all(Grain_gr_rate >= 0))
            expect_true(all(Grain_mr_rate >= 0))
            expect_true(all(Leaf_gr_rate >= 0))
            expect_true(all(Leaf_mr_rate >= 0))
            expect_true(all(Leaf_WS_loss_rate >= 0))
            expect_true(all(Rhizome_gr_rate >= 0))
            expect_true(all(Rhizome_mr_rate >= 0))
            expect_true(all(Root_gr_rate >= 0))
            expect_true(all(Root_mr_rate >= 0))
            expect_true(all(Shell_gr_rate >= 0))
            expect_true(all(Shell_mr_rate >= 0))
            expect_true(all(Stem_gr_rate >= 0))
            expect_true(all(Stem_mr_rate >= 0))
            expect_true(all(whole_plant_growth_respiration_rate >= 0))
        })

        # Check that gross assimilation is non-negative
        expect_true(all(soybean_res$canopy_gross_assimilation >= 0))
    })
}

# Test each possible partitioning growth calculator module
partitioning_calculator_modules <- c(
    'BioCro:partitioning_growth_calculator_leaf_costs',
    'BioCro:partitioning_growth_calculator'
)

for (pm in partitioning_calculator_modules) {
    test_soybean_carbon_accounting(pm)
}

# Make sure all partitioning growth calculators have the same inputs and outputs
test_that('all partitioning calculators have the same inputs and outputs', {
    expect_equal(
        module_info('BioCro:partitioning_growth_calculator', verbose = FALSE)$inputs,
        module_info('BioCro:partitioning_growth_calculator_leaf_costs', verbose = FALSE)$inputs
    )

    expect_equal(
        module_info('BioCro:partitioning_growth_calculator', verbose = FALSE)$outputs,
        module_info('BioCro:partitioning_growth_calculator_leaf_costs', verbose = FALSE)$outputs
    )
})
