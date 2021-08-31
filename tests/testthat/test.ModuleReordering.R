## This tests that the order in which steady-state modules are
## specified in the list passed to run_biocro does not affect the
## simulation result.

context(paste("Check simulation result",
              "is unaffected by steady-state module re-ordering"))

SAMPLE_SIZE <- 5     # Number of time points to test in each simulation result.

# Number of permutations of the steady-state module list to compare
# against the unpermuted list:
NUMBER_OF_PERMUTATIONS <- 4


## Run the simulation:
baseline_result  <- run_biocro(sorghum_initial_values,
                               sorghum_parameters,
                               get_growing_season_climate(weather05),
                               sorghum_steady_state_modules,
                               sorghum_derivative_modules)


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

    permuted_steady_state_module_list <- sample(sorghum_steady_state_modules)

    result <- run_biocro(sorghum_initial_values,
                         sorghum_parameters,
                         get_growing_season_climate(weather05),
                         permuted_steady_state_module_list,
                         sorghum_derivative_modules)

    # Randomly choose a number of indices and compare result against
    # baseline at each index:
    sample <- sample(row_range, SAMPLE_SIZE)
    for (index in sample) {
        compare_simulation_trial(result, index)
    }
}
