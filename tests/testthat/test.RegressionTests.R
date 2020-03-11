## This tests that re-running a previously-run simulation yields the same result
## as before.

context("Test a biological system with a known result to ensure that the result doesn't change.")

SAMPLE_SIZE <- 5       # number of time points to test in each simulation result

# Run the simulation
result <- Gro(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_modules)

# Make sure some important variables are included in the output
column_names <- names(result)
for (name in c("hour", "TTc", "Leaf", "Stem", "Root")) {
	description <- paste("The simulation result includes the ", name, " column", sep="")
	test_that(description, {
		expect_true(name %in% column_names)
	})
}

# Read the stored result ("Gro_result") from the data file:
load("../test_data/sorghum_simulation.rda")

# Make sure the stored result has the same number of time points
index_of_last_row <- length(result$hour)
test_that("The simulation has the correct number of data points", {
	expect_equal(index_of_last_row, length(Gro_result$hour))
})

# Define a function that compares the new result to the old one at a single index
compare_simulation_trial <- function(index) {
	description <- paste("The simulation agrees with the stored result at index ", index, sep="")
	test_that(description, {
		for (variable in column_names) {
			expect_equal(result[[variable]][index], Gro_result[[variable]][index])
		}
	})
}

# Randomly choose a number of indices and test them
sample <- sample(1:index_of_last_row, SAMPLE_SIZE)
for (index in sample) {
	compare_simulation_trial(index)
}