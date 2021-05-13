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
    test_case <- function(case) {
        expect_equal(
            case[['expected_outputs']],
            evaluate_module(module_name, case[['inputs']])
        )
    }

    # Test each case
    lapply(case_list, test_case)
}
