## This tests that the order in which steady-state modules are
## specified in the list passed to Gro_solver does not affect the
## simulation result.

context(paste("Check simulation result",
              "is unaffected by steady-state module re-ordering"))

SAMPLE_SIZE <- 5     # Number of time points to test in each simulation result.

# Number of permutations of the steady-state module list to compare
# against the unpermuted list:
NUMBER_OF_PERMUTATIONS <- 4


## Modules to use in the simulations:

sorghum_ss_modules <- c(
    "soil_type_selector",
    "stomata_water_stress_linear",
    "leaf_water_stress_exponential",
    "parameter_calculator",
    "soil_evaporation",
    "c4_canopy",
    "partitioning_coefficient_selector",
    "partitioning_growth_calculator"
)

sorghum_deriv_modules <- c(
    "thermal_time_senescence",
    "partitioning_growth",
    "thermal_time_accumulator",
    "one_layer_soil_profile"
)


## Run the simulation:
baseline_result  <- Gro_solver(sorghum_initial_state,
                               sorghum_parameters,
                               get_growing_season_climate(weather05),
                               sorghum_ss_modules,
                               sorghum_deriv_modules)


## Variables to define the scope of the survey:
column_names <- names(baseline_result)
row_range <- 1:length(baseline_result$hour)


## Convenience function: compares the given "result" against
## "baseline_result" at a single row index:
compare_simulation_trial <- function(result, index) {

	description <- paste0("After reordering the steady-state modules, ",
                          "the simulation result agrees with ",
                          "the baseline simulation result at row ",
                          index)
                         
	test_that(description, {
		for (variable in column_names) {
			expect_equal(baseline_result[[variable]][index], result[[variable]][index])
		}
	})
}

for (count in 1:NUMBER_OF_PERMUTATIONS) {

    permuted_ss_module_list <- sample(sorghum_ss_modules)

    result <- Gro_solver(sorghum_initial_state,
                         sorghum_parameters,
                         get_growing_season_climate(weather05),
                         permuted_ss_module_list,
                         sorghum_deriv_modules)

    # Randomly choose a number of indices and compare result against
    # baseline at each index:
    sample <- sample(row_range, SAMPLE_SIZE)
    for (index in sample) {
        compare_simulation_trial(result, index)
    }
}