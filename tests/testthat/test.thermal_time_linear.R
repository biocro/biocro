context("Run test cases for the `thermal_time_linear` module")

source("module_testing_helper_functions.R") # for the test_module function

# Define the inputs and expected outputs for the test cases
test_cases <- list(
    # air temperature below base temperature
    list(
        inputs = list(
            temp = 10,  # degrees C
            tbase = 20  # degrees C
        ),
        expected_outputs = list(
            TTc = 0     # degrees C * day / hr
        )
    ),
    # air temperature equal to base temperature
    list(
        inputs = list(
            temp = 20,  # degrees C
            tbase = 20  # degrees C
        ),
        expected_outputs = list(
            TTc = 0     # degrees C * day / hr
        )
    ),
    # air temperature above base temperature
    list(
        inputs = list(
            temp = 44,  # degrees C
            tbase = 20  # degrees C
        ),
        expected_outputs = list(
            TTc =  1    # degrees C * day / hr
        )
    )
)

# Test each set of inputs
test_module("thermal_time_linear", test_cases)
