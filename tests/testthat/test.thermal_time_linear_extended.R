context("Run test cases for the `thermal_time_linear_extended` module")

source("module_testing_helper_functions.R") # for test_module and case

# Define a set of "standard" inputs and outputs for this module
standard_inputs <- list(
    temp = 10,  # degrees C
    tbase = 20, # degrees C
    tupper = 44 # degrees C
)
standard_outputs <- list(
    TTc = 0     # degrees C * day / hr
)

# Define the inputs and expected outputs for the test cases
test_cases <- list(
    # air temperature below base temperature
    case(
        standard_inputs,
        standard_outputs
    ),
    # air temperature equal to base temperature
    case(
        within(standard_inputs, temp <- 20),
        standard_outputs
    ),
    # air temperature above base temperature
    case(
        within(standard_inputs, temp <- 32),
        within(standard_outputs, TTc <- 0.5)
    ),
    # air temperature equal to upper temperature
    case(
        within(standard_inputs, temp <- 44),
        within(standard_outputs, TTc <- 1)
    ),
    # air temperature above upper temperature
    case(
        within(standard_inputs, temp <- 50),
        within(standard_outputs, TTc <- 1)
    )
)

# Test each set of inputs
test_module("thermal_time_linear_extended", test_cases)
