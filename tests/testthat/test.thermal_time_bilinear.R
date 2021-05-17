context("Run test cases for the `thermal_time_bilinear` module")

source("module_testing_helper_functions.R") # for test_module and case_function

# Make a function for specifying a test case from a set of input and output
# values. The order of the inputs to `tc` is set by the output of a call to
# `module_info` and can be determined by calling
# str(module_info('thermal_time_bilinear')) from an R terminal.
tc <- case_function('thermal_time_bilinear')

# Define the inputs and expected outputs for the test cases. The quantities have
# units as follows:
# - inputs:
#  - tmax: degrees C
#  - topt: degrees C
#  - temp: degrees C
#  - tbase: degrees C
# - outputs:
#  - TTc: degrees C * day / hr
test_cases <- list(
    #     tmax topt temp tbase | TTc
    tc(c(44,  32,  10,  20,     0.00)),  # air temp below base temp
    tc(c(44,  32,  20,  20,     0.00)),  # air temp equal to base temp
    tc(c(44,  32,  26,  20,     0.25)),  # air temp above base temp
    tc(c(44,  32,  32,  20,     0.50)),  # air temp equal to optimum temp
    tc(c(44,  32,  38,  20,     0.25)),  # air temp above optimum temp
    tc(c(44,  32,  50,  20,     0.00))   # air temp above maximum temp
)

# Test each set of inputs
test_module("thermal_time_bilinear", test_cases)
