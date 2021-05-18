# test_module: a function to help simplify module testing
#
# Inputs:
#
# - module_name: a string specifying the name of the module to be tested
#
# - case_list: a list where each element is a test case, which is itself a list
#   with two named elements:
#
#   - case[['inputs']]: a list of module inputs, i.e., a list of named numeric
#     elements corresponding to the module's input quantities
#
#   - case[['expected_outputs']]: a list of expected module outputs, i.e., a
#     list of named numeric elements corresponding to the expected values of the
#     module's output quantities
#
# Output: none
#
# Examples demonstrating how to use this function:
#
# - test.thermal_time_linear.R (defines case_list using explicit calls to the
#   `list` function)
#
# - test.thermal_time_linear_extended.R (defines case_list using the `case`
#   helper function)
#
# - test.thermal_time_bilinear.R (defines case_list using the `case_function`
#   method)
#
test_module <- function(
    module_name,
    case_list
)
{
    # Define a helping function that tests one case
    test_one_case <- function(one_case) {
        expect_equal(
            one_case[['expected_outputs']],
            evaluate_module(module_name, one_case[['inputs']])
        )
    }

    # Test each case
    lapply(case_list, test_one_case)
}

# case: a function to help define test cases for module testing
#
# Inputs:
#
# - module_inputs: a list of module inputs, i.e., a list of named numeric
#   elements corresponding to the module's input quantities
#
# - expected_module_outputs: a list of module outputs that are expected to be
#   produced from the module_inputs, i.e., a list of named numeric elements
#   corresponding to the expected values of the module's output quantities
#
# Output: a list with two named elements (`inputs` and `expected_outputs`)
# formed from the input arguments
#
# Examples demonstrating how to use this function:
#
# - test.thermal_time_linear_extended.R
#
# - the `case_function` function below
#
case <- function(module_inputs, expected_module_outputs) {
    list(
        inputs = module_inputs,
        expected_outputs = expected_module_outputs
    )
}

# case_function: a function to help define test cases for module testing
#
# Inputs:
#
# - module_name: the name of a module
#
# Output: a function that accepts a numeric vector `x` as its input and creates
# a "test case" list from it, i.e., a list with two named elements (`inputs` and
# `expected_outputs`) that represent the `expected_outputs` that the module
# should produce from the quantities in the `inputs` list. The elements of `x`
# should be the module's inputs followed by its outputs, where the order should
# be the same as the input and output parameter lists determined by a call to
# the `module_info` function. E.g., if str(module_info(module_name)) produces
# the following:
#
#   $ inputs                :List of 2
#    ..$ tbase: num 1
#    ..$ temp : num 1
#   $ outputs               :List of 1
#    ..$ TTc: num 1
#
# then case_function(module_name) will return a function expecting an input
# vector `x` with three elements representing tbase, temp, and TTc (in that
# order).
#
# Example demonstrating how to use this function:
#
# - test.thermal_time_bilinear.R
#
case_function <- function(module_name) {
    info <- module_info(module_name, verbose = FALSE)

    inputs <- info[['inputs']]
    outputs <- info[['outputs']]

    input_names <- names(inputs)
    output_names <- names(outputs)

    function(x) {
        if (length(x) != (length(inputs) + length(outputs))) {
            stop("Wrong number of input arguments!")
        }

        for (i in seq_along(inputs)) {
            inputs[[input_names[i]]] <- x[i]
        }

        for (i in seq_along(outputs)) {
            j = i + length(inputs)
            outputs[[output_names[i]]] <- x[j]
        }

        return(case(inputs, outputs))
    }
}
