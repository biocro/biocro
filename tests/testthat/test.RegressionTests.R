## This tests that re-running a previously-run simulation yields the same result
## as before.  We look at only the final row of each simulation result.

context("Test a biological system with a known result to ensure that the result doesn't change.")

result <- Gro(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_modules)

## read the stored result ("Gro_result") from the data file:
load("../test_data/sorghum_simulation.rda")

column_names <-names(result)
index_of_last_row <- length(result$hour)

for (variable in column_names) {
    expect_equal(result[[variable]][index_of_last_row], Gro_result[[variable]][index_of_last_row])
}

