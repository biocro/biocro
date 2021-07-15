# This file defines several helper functions related to creating and running
# module tests:
# - `test_module`
# - `case`
# - `csv_from_cases`
# - `cases_from_csv`
# - `initialize_csv`
# - `add_csv_row`
# - `update_csv_cases`
#
# Most of these functions use relative file paths for reading and writing test
# case files and are intended to be called from the `tests/testhat` directory.
#
# When specifying test cases for a new module, the `initialize_csv` function can
# initialize a csv file in the correct format.
#
# When adding new test cases for a module with a pre-existing csv file, the
# `add_csv_row` function can automatically add a row based on a set of supplied
# inputs and the corresponding description.
#
# If a module's behavior has changed, the `update_csv_cases` function can be
# used to recalculate the module outputs that are stored in its csv file.
#
# The `update_csv_cases` function can also be used to batch-initialize a set of
# test cases. In this situation, the user could supply inputs and descriptions
# for one or more test cases by saving them in the module's csv file. Then a
# call to `update_csv_cases` would calculate and save all the corresponding
# output values. See the documentation for `update_csv_cases` for an example.
#
# For an example of a case file that includes multiple test cases with
# descriptions, see `tests/testthat/module_tests/thermal_time_trilinear.csv`.
#
# To check whether a module named MODULE will be able to pass its tests, a user
# can call `test_module(MODULE, cases_from_csv(MODULE)).
#
# A user will not typically need to use the `case` or `csv_from_cases` function
# directly.
#
# Although it is possible, directly editing the case files is not recommended
# since `initialize_csv`, `add_csv_row`, and `update_csv_cases` are easier to
# use. There are two exceptions to this suggestion: one is when a case must be
# deleted, since that would be easier to do by directly editing a case file. The
# other is during the initialization of a test file, where a user may wish to
# directly specify the inputs for multiple cases and then use the
# `update_csv_cases` to automatically fill in the output values, as described
# above.
#
# Case files can easily be viewed using Excel or other spreadsheet viewers, and
# are also nicely formatted when viewed on the GitHub website for the repo.
#
# ------------------------------------------------------------------------------
#

# test_module: A function to help simplify module testing by running one or more
#              test cases. For each case, failure occurs if the module produces
#              an error or warning message when it is evaluated or if its output
#              does not match the expected value.
#
# Inputs:
#
# - module_name: a string specifying the name of the module to be tested
#
# - case_list: a list where each element is a test case, which is itself a list
#   with three named elements:
#
#   - case[['inputs']]: a list of module inputs, i.e., a list of named numeric
#     elements corresponding to the module's input quantities
#
#   - case[['expected_outputs']]: a list of expected module outputs, i.e., a
#     list of named numeric elements corresponding to the expected values of the
#     module's output quantities
#
#   - case[['description']]: a string describing the test case, e.g. "temp
#     below tbase". If the description contains a comma, it must be wrapped in
#     quotes. This is not advised.
#
# Output: none
#
#
test_module <- function(
    module_name,
    case_list
)
{
    # If this module has 1 or more input quantities, we should make sure it
    # produces the correct error message when in its inputs are not defined.
    info <- module_info(module_name, FALSE)
    if (length(info[['inputs']]) > 0) {
        # Write a description for the quantity access error test
        quantity_access_error_description <- paste(
            module_name,
            "produces a 'quantity_access_error' when its inputs are missing"
        )

        # Try to run the module without specifying any input quantities, storing
        # any error messages that occur
        error_msg <- ""
        tryCatch(
            {
                # Code to be executed initially
                outputs <- evaluate_module(module_name, list())
            },
            error=function(cond) {
                # Code for handling errors
                error_msg <<- cond
            },
            warning=function(cond) {
                # Code for handling warnings
                error_msg <<- cond
            },
            finally={
                # Code to be executed after the initial code and handling
            }
        )

        # An error message containing "Caught quantity access error" should be
        # produced by the above call to 'evaluate_module'
        test_that(quantity_access_error_description, {
            expect_true(
                grepl("Caught quantity access error", error_msg, fixed=TRUE)
            )
        })
    }

    # Define a helping function that tests one case
    test_one_case <- function(one_case) {
        # Write descriptions for the two types of tests
        error_test_description <- paste0(
            module_name,
            " case '",
            one_case[['description']],
            "' produces no errors"
        )

        case_test_description <- paste0(
            module_name,
            " case '",
            one_case[['description']],
            "' produces the expected output"
        )

        # Get the expected outputs
        expected_outputs <- one_case[['expected_outputs']]

        # Try to get the actual outputs, storing any error messages that may
        # occur
        error_msg <- ""
        actual_outputs <- tryCatch(
            {
                evaluate_module(module_name, one_case[['inputs']])
            },
            error=function(cond)
            {
                error_msg <<- cond
            },
            warning=function(cond)
            {
                error_msg <<- cond
            }
        )

        # If the value of error_msg has changed from its default, an error has
        # occurred, and there is no reason to compare the actual and expected
        # output values
        test_that(error_test_description, {
            expect_identical(!!error_msg, "")
        })

        if (length(error_msg) > 1) {
            return()
        }

        # Make sure the outputs are ordered the same way (otherwise
        # `expect_equal` may indicate an error when there isn't one)
        actual_outputs <- actual_outputs[order(names(actual_outputs))]
        expected_outputs <-  expected_outputs[order(names(expected_outputs))]

        # Compare the expected and actual outputs
        test_that(case_test_description, {
            expect_equal(!!actual_outputs, !!expected_outputs)
        })
    }

    # Test each case
    lapply(case_list, test_one_case)
}

# case: A function to help define test cases for module testing by combining the
#       required elements into a list with the correct element names as required
#       by the `test_module` function.
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
# - case_description: a string describing the test case, e.g. "temp below tbase"
#   If the description contains a comma, it must be wrapped in quotes. This is
#   not advised.
#
# Output: a list with three named elements (`inputs`, `expected_outputs`, and
# `description`) formed from the input arguments
#
case <- function(module_inputs, expected_module_outputs, case_description) {
    list(
        inputs = module_inputs,
        expected_outputs = expected_module_outputs,
        description = case_description
    )
}

# csv_from_cases: A function to help define test cases for module testing by
#                 writing a list of test cases to a csv file, whose name will be
#                 determined from the module's name.
#
# Inputs:
#
# - module_name: the name of a module
#
# - case_list: a list of cases for that module, as described in the
#   documentation for the `test_module` function
#
# Outputs: none
#
# This function will store the cases in a properly formatted csv file in the
# `tests/testthat/module_tests` directory whose name is determined from the
# `module_name` input as `module_name.csv`. This csv file can be read by the
# `cases_from_csv` function. This function is intended to be called from the
# `tests/testhat` directory.
#
csv_from_cases <- function(module_name, case_list)
{
    # Generate the filename
    filename <- paste0("module_tests/", module_name, ".csv")

    # Get info about the module
    info <- module_info(module_name, FALSE)
    inputs <- info[['inputs']]
    outputs <- info[['outputs']]

    # Make a data frame describing the module's inputs, outputs, and all test
    # cases
    n_inputs <- length(inputs)
    n_outputs <- length(outputs)
    n_cases <- length(case_list)
    csv_case <- data.frame(
        matrix(
            nrow=(2 + n_cases),
            ncol=(n_inputs + n_outputs + 1)
        )
    )

    for (i in seq_along(inputs)) {
        input_name <- names(inputs)[i]

        csv_case[1,i] <- input_name
        csv_case[2,i] <- "input"

        for (j in seq_len(n_cases)) {
            case <- case_list[[j]]
            case_inputs <- case[['inputs']]
            csv_case[(2+j),i] <- case_inputs[[input_name]]
        }
    }

    for (i in seq_along(outputs)) {
        output_name <- names(outputs)[i]
        k <- i + n_inputs

        csv_case[1,k] <- output_name
        csv_case[2,k] <- "output"

        for (j in seq_len(n_cases)) {
            case <- case_list[[j]]
            case_outputs <- case[['expected_outputs']]
            csv_case[(2+j),k] <- case_outputs[[output_name]]
        }
    }

    description_index <- n_inputs + n_outputs + 1
    csv_case[1,description_index] <- "description"
    csv_case[2,description_index] <- "NA"

    for (j in seq_len(n_cases)) {
        case <- case_list[[j]]
        description <- case[['description']]
        csv_case[(2+j),description_index] <- description
    }

    # Write the case to a new .csv file
    write.table(
        csv_case,
        filename,
        sep = ",",
        dec = ".",
        eol = "\n",
        quote = FALSE,
        row.names = FALSE,
        col.names = FALSE,
        append = FALSE
    )
}

# cases_from_csv: A function to help define test cases for module testing by
#                 creating a list of test cases from a csv file
#
# Inputs:
#
# - module_name: the name of a module
#
# Output: a list of test cases created by the `case` function above that is
# suitable for passing to the `test_module` function
#
# There must be a corresponding file called `module_name.csv` in the
# `module_tests` directory. The first row of this file should be the quantity
# names, the second row should be the quantity types (`input` or `output`), and
# the remaining rows should each specify input quantity values along with the
# expected output values they should produce. The last column should be about
# `description` column containing short descriptions of the test cases.
#
# This function is intended to be called from the `tests/testhat` directory.
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

    # Get the column names and types, making sure to trim any whitespace
    column_names <- file_contents[1,]
    column_names <- trimws(column_names)

    column_types <- file_contents[2,]
    column_types <- trimws(column_types)

    # Get the quantity values
    test_data <- file_contents[3:length(file_contents[,1]),]

    # If there is no test data, return an empty list. This may happen if a
    # module has no inputs or outputs, as is the case with the
    # `empty_senescence` modules
    if (is.null(nrow(test_data))) {
        return(list())
    }

    # Get the indices of the input, output, and description columns
    input_columns <- c()
    output_columns <- c()
    description_column <- c()
    for (i in seq_along(column_types)) {
        if (is.na(column_types[[i]])) {
            description_column <- c(i)
        } else if (column_types[[i]] == "input") {
            input_columns <- append(input_columns, i)
        } else if (column_types[[i]] == "output") {
            output_columns <- append(output_columns, i)
        }
    }

    # Get the names of the input and output columns
    input_names <- column_names[input_columns]
    output_names <- column_names[output_columns]

    # Make a helping function that defines a case from a row in the data, making
    # sure the quantity values are treated as numeric
    case_helper <- function(row_indx) {
        row_inputs <- as.list(as.numeric(test_data[row_indx, input_columns]))
        names(row_inputs) <- input_names

        row_outputs <- as.list(as.numeric(test_data[row_indx, output_columns]))
        names(row_outputs) <- output_names

        row_description <- trimws(test_data[row_indx, description_column])

        return(case(row_inputs, row_outputs, row_description))
    }

    # Convert each row into a test case
    test_cases <- lapply(seq_len(nrow(test_data)), case_helper)

    return(test_cases)
}

# initialize_csv: A function to help define test cases for module testing by
#                 initializing the csv file for one module based on either a set
#                 of default input values or user-supplied ones.
#
# Inputs:
#
# - module_name: the name of a module
#
# - nonstandard_inputs: an optional list of input quantities to be passed to the
#   module in order to define the test case
#
# - nonstandard_case_description: an optional string describing the case, which
#   should be succinct and not contain any newline characters, e.g. "temp below
#   tbase". If the description contains a comma, it must be wrapped in quotes.
#   This is not advised.
#
# Outputs: none
#
# This function will evaluate the module for a set of input quantities and store
# the results as a csv file that specifies a test case and can be read using the
# `cases_from_csv` function.
#
# If the user doesn't supply input values, defaults will be determined using the
# `module_info` function.
#
# If the user doesn't supply a case description, a default description will be
# used instead.
#
# Typically, both of these optional arguments can be omitted. However, some
# modules produce errors when all inputs are set to 1. In this case, it would be
# necessary to supply nonstandard inputs and (possibly) an alternate case
# description.
#
# This function is intended to be called from the `tests/testhat` directory.
#
initialize_csv <- function(
    module_name,
    nonstandard_inputs = list(),
    nonstandard_case_description = ""
)
{
    # Get info about the module
    info <- module_info(module_name, FALSE)

    # Decide what to use as the input quantities
    inputs <- info[['inputs']]
    if (length(nonstandard_inputs) > 0) {
        inputs <- nonstandard_inputs
    }

    # Run the module using the inputs
    outputs <- evaluate_module(module_name, inputs)

    # Decide what to use for the description
    description <- "automatically-generated test case"
    if (nchar(nonstandard_case_description) > 0) {
        description <- nonstandard_case_description
    }

    # Make a case list with one element
    case_list <- list(
        case(inputs, outputs, description)
    )

    # Write the case to a new .csv file
    csv_from_cases(module_name, case_list)
}

# add_csv_row: A function to help define test cases for module testing by adding
#              one test case to a module's csv file based on the user-supplied
#              input and description.
#
# Inputs:
#
# - module_name: the name of a module
#
# - inputs: a list of input quantities to pass to the module
#
# - case_description: a string describing the case, which should should be
#   succinct and not contain any newline characters, e.g. "temp below tbase". If
#   the description contains a comma, it must be wrapped in quotes. This is not
#   advised.
#
# Outputs: none
#
# This function will evaluate the module for a set of input quantities, define a
# test case from the resulting outputs and the description, and add it to the
# module's corresponding csv file.
#
# This function is intended to be called from the `tests/testhat` directory.
#
add_csv_row <- function(module_name, inputs, case_description)
{
    # Get any test cases already defined in the module's csv file
    case_list <- cases_from_csv(module_name)

    # Run the module using the inputs
    outputs <- evaluate_module(module_name, inputs)

    # Add a new case to the list
    case_list <- append(
        case_list,
        list(
            case(
                inputs,
                outputs,
                case_description
            )
        )
    )

    # Save the full list to the file
    csv_from_cases(module_name, case_list)
}

# update_csv_cases: A function to help define cases for module testing by
#                   updating the expected output values for each case stored in
#                   a module's csv file.
#
# Inputs:
#
# - module_name: the name of a module
#
# Outputs: none
#
# This function will evaluate the module for all input values specified in its
# csv case file and update the stored values of the corresponding outputs.
#
# This function is intended to be called from the `tests/testhat` directory.
#
# Example showing how `update_csv_cases` can be used to initialize a testing
# file for the `thermal_time_linear` module: to begin, we need to know which
# input quantities are required for this module. This information can be
# determined by calling `info <- module_info("thermal_time_linear")`. In this
# case, there are two inputs: `temp` (representing the air temperature) and
# `tbase` (the base temperature for phenological development). As test cases,
# we would like to check the output for three situations where the air
# temperature is below, equal to, or above the base temperature. We can specify
# these cases by creating a `tests/testthat/thermal_time_linear.csv` file with
# the following contents:
#
# tbase,temp,description
# input,input,NA
# 20,10,temp below tbase
# 20,20,temp equal to tbase
# 20,44,temp above tbase
#
# Note that here we only specify the inputs and description for each case. Now
# the `update_csv_cases` function can be used to fill in the output values by
# calling `update_csv_cases("thermal_time_linear")` from the `tests/testthat`
# directory. Afterwards, the case file's contents are updated to the following:
#
# tbase,temp,TTc,description
# input,input,output,NA
# 20,10,0,temp below tbase
# 20,20,0,temp equal to tbase
# 20,44,1,temp above tbase
#
# This is now a fully-functional test case file for the module.
#
update_csv_cases <- function(module_name)
{
    # Get any test cases already defined in the module's csv file
    case_list <- cases_from_csv(module_name)

    # Define a helping function that updates a test case by running the module
    # using the inputs and storing the result as the expected outputs
    update_case <- function(one_case) {
        one_case[['expected_outputs']] <-
            evaluate_module(module_name, one_case[['inputs']])
        return(one_case)
    }

    # Update all the test cases
    updated_case_list <- lapply(case_list, update_case)

    # Save the new list to the file
    csv_from_cases(module_name, updated_case_list)

}
