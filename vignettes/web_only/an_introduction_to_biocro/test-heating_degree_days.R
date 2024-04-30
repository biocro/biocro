context('Test basic functioning of the differential module "heating_degree_days".')

## Given the (average) ambient temperature over an hour-long interval
## and given a threshold temperature, return the number of heating
## degree days that accumulate over the interval.
calculate_heating_degree_days <- function(ambient_temperature, threshold_temperature) {
    input_quantities <- list(temp = ambient_temperature, base_temperature = threshold_temperature)
    result <- evaluate_module("heating_degree_days", input_quantities)
    result$heating_degree_days
}

threshold_temperature = 18.3 # ~65 deg F

test_that("If the ambient temperature is one degree below the threshold temperature, 1/24 of a heating degree day accumulates in each hour interval", {
    ambient_temperature = threshold_temperature - 1
    hdd <- calculate_heating_degree_days(ambient_temperature, threshold_temperature)
    expect_equal(hdd, 1/24)
})

test_that("When the ambient temperature is equal to the threshold temperature, no heating degree days accumulate", {
    ambient_temperature = threshold_temperature
    hdd <- calculate_heating_degree_days(ambient_temperature, threshold_temperature)
    expect_equal(hdd, 0)
})


test_that("When the ambient temperature is greater than the threshold temperature, no heating degree days accumulate", {
    number_of_samples <- 10
    temperature_surpluses <- sample(1:200, number_of_samples)/10 # test surpluses in the range of 0.1 to 20.0
    for (surplus in temperature_surpluses) {
        ambient_temperature = threshold_temperature + surplus
        hdd <- calculate_heating_degree_days(ambient_temperature, threshold_temperature)
        expect_equal(hdd, 0)
    }
})

test_that("When the ambient temperature is less than the threshold temperature, the heating degree day accumulation for the hour interval is 1/24 the difference", {
    number_of_samples <- 10
    temperature_deficits <- sample(1:500, number_of_samples)/10 # test deficits in the range of 0.1 to 50.0
    for (deficit in temperature_deficits) {
        ambient_temperature = threshold_temperature - deficit
        hdd <- calculate_heating_degree_days(ambient_temperature, threshold_temperature)
        expect_equal(hdd, deficit/24)
    }
})
