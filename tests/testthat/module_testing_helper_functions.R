# test_module: a function to help simplify module testing
#
# inputs:
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
# inputs:
#
# - module_inputs: a list of module inputs, i.e., a list of named numeric
#   elements corresponding to the module's input quantities
#
# - expected_module_outputs: a list of module outputs that are expected to be
#   produced from the module_inputs, i.e., a list of named numeric elements
#   corresponding to the expected values of the module's output quantities
#
case <- function(module_inputs, expected_module_outputs) {
    list(
        inputs = module_inputs,
        expected_outputs = expected_module_outputs
    )
}
