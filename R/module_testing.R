test_module <- function(module_name, case_to_test)
{
    # Get the expected and actual outputs
    expected_outputs <- case_to_test[['expected_outputs']]
    actual_outputs <- evaluate_module(module_name, case_to_test[['inputs']])

    # Make sure the outputs are ordered the same way (otherwise `all.equal` may
    # indicate a difference when there isn't one)
    expected_outputs <-  expected_outputs[order(names(expected_outputs))]
    actual_outputs <- actual_outputs[order(names(actual_outputs))]

    # Check to see if the expected and actual outputs match
    if (!isTRUE(all.equal(expected_outputs, actual_outputs))) {
        msg <- paste0(
            "The module did not produce expected output values for test case '",
            case_to_test[['description']],
            "'"
        )
        stop(msg)
    }

    return(invisible(NULL))
}

case <- function(inputs, expected_outputs, description) {
    list(
        inputs = inputs,
        expected_outputs = expected_outputs,
        description = description
    )
}

# csv_from_cases: A function to help define test cases for module testing by
# writing a list of test cases to a csv file, whose name will be determined from
# the module's name.
#
# Inputs:
#
# - module_name: the name of a module
#
# - case_list: a list of cases for that module, as described in the
#   documentation for the `test_module` function
#
# - directory: the directory to store the test case file, e.g.
#   file.path('tests', 'testthat')
#
# Outputs: none
#
# This function will store the cases in a properly formatted csv file in the
# specified directory whose name is determined from the `module_name` input as
# `module_name.csv`. This csv file can be read by the `cases_from_csv` function.
# BioCro users typically do not need to access this function directly.
#
csv_from_cases <- function(module_name, case_list, directory)
{
    # Generate the filename
    filename <- file.path(directory, paste0(module_name, ".csv"))

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
        input_name <- inputs[i]

        csv_case[1,i] <- input_name
        csv_case[2,i] <- "input"

        for (j in seq_len(n_cases)) {
            case <- case_list[[j]]
            case_inputs <- case[['inputs']]
            csv_case[(2+j),i] <- case_inputs[[input_name]]
        }
    }

    for (i in seq_along(outputs)) {
        output_name <- outputs[i]
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

cases_from_csv <- function(module_name, directory)
{
    # Generate the filename
    filename <- file.path(directory, paste0(module_name, ".csv"))

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

initialize_csv <- function(
    module_name,
    directory,
    nonstandard_inputs = list(),
    nonstandard_description = ""
)
{
    # Get info about the module
    info <- module_info(module_name, FALSE)

    # Decide what to use as the input quantities
    inputs <- quantity_list_from_names(info[['inputs']])
    if (length(nonstandard_inputs) > 0) {
        inputs <- nonstandard_inputs
    }

    # Run the module using the inputs
    outputs <- evaluate_module(module_name, inputs)

    # Decide what to use for the description
    description <- "automatically-generated test case"
    if (nchar(nonstandard_description) > 0) {
        description <- nonstandard_description
    }

    # Make a case list with one element
    case_list <- list(
        case(inputs, outputs, description)
    )

    # Write the case to a new .csv file
    csv_from_cases(module_name, case_list, directory)
}

add_csv_row <- function(module_name, directory, inputs, description)
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
                description
            )
        )
    )

    # Save the full list to the file
    csv_from_cases(module_name, directory, case_list)
}

update_csv_cases <- function(module_name, directory)
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
    csv_from_cases(module_name, directory, updated_case_list)

}
