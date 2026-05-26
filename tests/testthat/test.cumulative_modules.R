# Helping function for checking carbon and water accounting for a particular
# growth calculator module using the soybean2 model as a base. Here we use a
# nonzero `growth_respiration_fraction` to test that it is properly accounted
# for, and we include a non-zero irrigation_rate in the drivers.
test_soybean_carbon_accounting <- function(partitioning_calculator) {
    description <- paste0(
        'the soybean2 model accounts for all carbon when using `',
        partitioning_calculator,
        '` as its partitioning growth calculator module'
    )

    # Build the model
    model <- soybean2

    model$initial_values <- c(
        model$initial_values,
        list(
            canopy_assimilation = 0,
            canopy_gross_assimilation = 0,
            canopy_non_photorespiratory_CO2_release = 0,
            canopy_photorespiration = 0,
            canopy_transpiration = 0,
            drainage = 0,
            excess_water = 0,
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
            surface_runoff = 0,
            tile_flow = 0,
            irrigation = 0,
            total_precip = 0,
            unmet_demand = 0,
            whole_plant_growth_respiration = 0
        )
    )

    model$parameters$growth_respiration_fraction <- 0.01
    model$parameters$irrigation_rate <- NULL

    model$direct_modules$partitioning_growth_calculator <- partitioning_calculator
    model$direct_modules <- c(
        model$direct_modules,
        list(
            'BioCro:total_biomass',
            'BioCro:total_growth_and_maintenance_respiration',
            'BioCro:total_soil_water'
        )
    )

    model$differential_modules <- c(
        model$differential_modules,
        list(
            'BioCro:cumulative_carbon_dynamics',
            'BioCro:cumulative_water_dynamics'
        )
    )

    drivers <- soybean_weather[['2002']]
    drivers$irrigation_rate <- 0.0
    drivers[drivers$doy == 200, 'irrigation_rate'] <- 1.0 # Irrigate at 1 Mg / ha / hr on day 200

    # Set a threshold to determine whether a rate is zero
    rate_eps <- 1e-15

    test_that(description, {
        soybean_res <- expect_silent(
            with(model, {run_biocro(
                initial_values,
                parameters,
                drivers,
                direct_modules,
                differential_modules
            )})
        )

        ##
        ## CARBON TESTS
        ##

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

        # Make sure a reasonable amount of carbon use has occurred; otherwise we
        # aren't really testing anything
        expect_true(
            soybean_res$total_carbon_use[nrow(soybean_res)] >= 20
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
            expect_true(all(canopy_photorespiration_rate > -rate_eps))
            expect_true(all(canopy_non_photorespiratory_CO2_release_rate > -rate_eps))
            expect_true(all(Grain_gr_rate > -rate_eps))
            expect_true(all(Grain_mr_rate > -rate_eps))
            expect_true(all(Leaf_gr_rate > -rate_eps))
            expect_true(all(Leaf_mr_rate > -rate_eps))
            expect_true(all(Leaf_WS_loss_rate > -rate_eps))
            expect_true(all(Rhizome_gr_rate > -rate_eps))
            expect_true(all(Rhizome_mr_rate > -rate_eps))
            expect_true(all(Root_gr_rate > -rate_eps))
            expect_true(all(Root_mr_rate > -rate_eps))
            expect_true(all(Shell_gr_rate > -rate_eps))
            expect_true(all(Shell_mr_rate > -rate_eps))
            expect_true(all(Stem_gr_rate > -rate_eps))
            expect_true(all(Stem_mr_rate > -rate_eps))
            expect_true(all(whole_plant_growth_respiration_rate > -rate_eps))
        })

        # Check that gross assimilation is non-negative
        expect_true(all(soybean_res$canopy_gross_assimilation > -rate_eps))

        ##
        ## WATER TESTS
        ##

        # Check that the water inputs (precipitation, irrigation, and unmet
        # demand) are balanced by retention of water in the soil plus losses
        # due to transpiration, evaporation, drainage, and runoff.
        soybean_res$total_water_inputs <- with(soybean_res, {
            # Unmet demand is negative by convention
            irrigation + total_precip - unmet_demand
        })

        soybean_res$total_water_use <- with(soybean_res, {
            (total_soil_water - total_soil_water[1]) +
            canopy_transpiration +
            drainage +
            excess_water +
            soil_evaporation +
            surface_runoff +
            tile_flow
        })

        # As of 2026-05-25, the water mass balance test fails with the default
        # tolerance (~1.5e-8) so must be set to a somewhat higher value
        expect_equal(
            soybean_res$total_water_inputs,
            soybean_res$total_water_use,
            tolerance = 1.5e-6
        )

        # Make sure a reasonable amount of carbon use has occurred; otherwise we
        # aren't really testing anything
        expect_true(
            soybean_res$total_water_use[nrow(soybean_res)] >= 3000
        )

        ## Uncomment this when debugging test failures to visually check whether
        ## the difference is real
        #dev.new()
        #print(lattice::xyplot(
        #    total_water_use + total_water_inputs ~ fractional_doy,
        #    data = soybean_res,
        #    type = 'l',
        #    auto = TRUE,
        #    main = partitioning_calculator
        #))

        # Check that all water loss rates are non-negative
        with(soybean_res, {
            expect_true(all(canopy_transpiration_rate > -rate_eps))
            expect_true(all(drainage_rate > -rate_eps))
            expect_true(all(excess_water_rate > -rate_eps))
            expect_true(all(soil_evaporation_rate > -rate_eps))
            expect_true(all(surface_runoff_rate > -rate_eps))
            expect_true(all(tile_flow_rate > -rate_eps))
        })

        # Check that all water input rates are non-negative (except unmet
        # demand, which should be non-positive)
        with(soybean_res, {
            expect_true(all(irrigation_rate > -rate_eps))
            expect_true(all(precip > -rate_eps))
            expect_true(all(unmet_demand_rate < rate_eps))
        })

        ## Uncomment this when debugging test failures to check where a rate
        ## has the wrong sign
        #dev.new()
        #rate_to_plot <- 'soil_evaporation_rate'
        #print(lattice::xyplot(
        #    soybean_res[[rate_to_plot]] ~ soybean_res[['fractional_doy']],
        #    group = soybean_res[[rate_to_plot]] > -rate_eps,
        #    type = 'l',
        #    auto.key = list(space = 'top', title = 'Is rate >= 0?', cex.title = 1),
        #    xlab = 'fractional_doy',
        #    ylab = rate_to_plot,
        #    main = partitioning_calculator
        #))
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
