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
            evaluate_module(!!module_name, !!one_case[['inputs']]),
            !!one_case[['expected_outputs']]
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

# cases_from_csv: a function to help define test cases for module testing
#
# Inputs:
#
# - module_name: the name of a module
#
# Output: a list of test cases created by the `case` function above that are
# suitable for passing to the `test_module` function
#
# There must be a corresponding file called `module_name.csv` in the
# `module_tests` directory. The first row of this file should be the quantity
# names, the second row should be the quantity types (`input` or `output`), and
# the remaining rows should each specify input quantity values along with the
# expected output values they should produce.
#
cases_from_csv <- function(module_name)
{
    # Generate the filename
    filename <- paste0("module_tests/", module_name, ".csv")

    # Read the data, making sure to never use factors
    file_contents <- read.csv(
        filename,
        header = FALSE,
        stringsAsFactors = FALSE
    )

    # Get the quantity names and types, making sure to trim any whitespace
    column_names <- file_contents[1,]
    column_names <- trimws(column_names)

    column_types <- file_contents[2,]
    column_types <- trimws(column_types)

    # Get the quantity values, making sure they are treated as numeric variables
    test_data <- file_contents[3:length(file_contents[,1]),]
    test_data <- as.data.frame(lapply(test_data, as.numeric))

    # Get the indices of the input and output columns
    input_columns <- c()
    output_columns <- c()
    for (i in seq_along(column_types)) {
        if (column_types[[i]] == "input") {
            input_columns <- append(input_columns, i)
        } else {
            output_columns <- append(output_columns, i)
        }
    }

    # Get the names of the input and output columns
    input_names <- column_names[input_columns]
    output_names <- column_names[output_columns]

    # Make a helping function that defines a case from a row in the data
    case_helper <- function(row_indx) {
        row_inputs <- as.list(test_data[row_indx, input_columns])
        names(row_inputs) <- input_names

        row_outputs <- as.list(test_data[row_indx, output_columns])
        names(row_outputs) <- output_names

        return(case(row_inputs, row_outputs))
    }

    # Convert each row into a test case
    test_cases <- lapply(seq_len(nrow(test_data)), case_helper)

    return(test_cases)
}
