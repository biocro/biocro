## This tests that the order in which direct modules are
## specified in the list passed to run_biocro does not affect the
## simulation result.

context(paste("Check simulation result",
              "is unaffected by direct module re-ordering"))

SAMPLE_SIZE <- 5     # Number of time points to test in each simulation result.

# Number of permutations of the direct module list to compare
# against the unpermuted list:
NUMBER_OF_PERMUTATIONS <- 4


## Run the simulation:
baseline_result  <- run_biocro(miscanthus_x_giganteus_initial_values,
                               miscanthus_x_giganteus_parameters,
                               get_growing_season_climate(weather2005),
                               module_creators(miscanthus_x_giganteus_direct_modules),
                               module_creators(miscanthus_x_giganteus_differential_modules))


## Variables to define the scope of the survey:
column_names <- names(baseline_result)
row_range <- 1:length(baseline_result$hour)


## Convenience function: compares the given "result" against
## "baseline_result" at a single row index:
compare_simulation_trial <- function(result, index) {

	description <- paste0("After reordering the direct modules, ",
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

    permuted_direct_module_list <- sample(miscanthus_x_giganteus_direct_modules)

    result <- run_biocro(miscanthus_x_giganteus_initial_values,
                         miscanthus_x_giganteus_parameters,
                         get_growing_season_climate(weather2005),
                         module_creators(permuted_direct_module_list),
                         module_creators(miscanthus_x_giganteus_differential_modules))

    # Randomly choose a number of indices and compare result against
    # baseline at each index:
    sample <- sample(row_range, SAMPLE_SIZE)
    for (index in sample) {
        compare_simulation_trial(result, index)
    }
}
