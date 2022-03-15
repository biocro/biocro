test_module <- function(module_name, case_to_test)
{
    # Get the expected outputs
    expected_outputs <- case_to_test[['expected_outputs']]

    # Try to get the actual outputs
    msg <- character()
    actual_outputs <- tryCatch(
        evaluate_module(module_name, case_to_test[['inputs']]),
        condition = function(cond) {
            msg <<- paste0(
                "Module `",
                module_name,
                "` test case `",
                case_to_test[['description']],
                "`: could not calculate outputs: ",
                cond
            )
        }
    )

    # If an error was thrown, return a message about it
    if (length(msg) > 0) {
        return(gsub("\n", "", msg, fixed = TRUE))
    }

    # Make sure the outputs are ordered the same way (otherwise `all.equal` may
    # indicate a difference when there isn't one)
    expected_outputs <-  expected_outputs[order(names(expected_outputs))]
    actual_outputs <- actual_outputs[order(names(actual_outputs))]

    # Check to see if the expected and actual outputs match
    if (!isTRUE(all.equal(expected_outputs, actual_outputs))) {
        return(
            paste0(
                "Module `",
                module_name,
                "` test case `",
                case_to_test[['description']],
                "`: calculated outputs do not match expected outputs"
            )
        )
    } else {
        return(character())
    }
}

case <- function(inputs, expected_outputs, description) {
    list(
        inputs = inputs,
        expected_outputs = expected_outputs,
        description = description
    )
}

# csv_from_cases: A function to store test cases for module testing by writing a
# list of test cases to a csv file, whose name will be determined from the
# module's name.
#
# Inputs:
#
# - module_name: the name of a module
#
# - directory: the directory to store the test case file, e.g.
#   file.path('tests', 'testthat')
#
# - case_list: a list of cases for that module, as described in the
#   documentation for the `test_module` function
#
# Outputs: none
#
# This function will store the cases in a properly formatted csv file in the
# specified directory; the name of the file will be determined from the
# `module_name` input as `module_name.csv`. This csv file can be read by the
# `cases_from_csv` function. BioCro users typically do not need to access this
# function directly.
#
csv_from_cases <- function(module_name, directory, case_list, overwrite)
{
    # Generate the filename
    filename <- file.path(directory, paste0(module_name, ".csv"))

    # Check to see if the file exists already
    file_exists <- file.exists(filename)

    # If we aren't supposed to overwrite files, check to see if the file exists
    # already
    if (!overwrite && file_exists) {
        return("Module test case file already exists; it was not overwritten")
    }

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
    csv_case[2,description_index] <- "description"

    for (j in seq_len(n_cases)) {
        case <- case_list[[j]]
        description <- case[['description']]
        csv_case[(2+j),description_index] <- trimws(description)
    }

    # Write the case to a new .csv file
    write.table(
        csv_case,
        filename,
        sep = ",",
        row.names = FALSE,
        col.names = FALSE,
        quote = description_index,
        qmethod = "double"
    )

    if (file_exists) {
        return("Existing module test case file was overwritten")
    } else {
        return("New module test case file was created")
    }
}

cases_from_csv <- function(module_name, directory)
{
    # Generate the filename
    filename <- file.path(directory, paste0(module_name, ".csv"))

    # Make sure the file exists
    if (!file.exists(filename)) {
        stop(paste0("Module test case file `", filename, "` does not exist"))
    }

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
    test_data <- file_contents[-(1:2),]

    # Get the indices of the input, output, and description columns
    input_columns <- c()
    output_columns <- c()
    description_column <- c()
    for (i in seq_along(column_types)) {
        if (column_types[[i]] == "description") {
            description_column <- i
        } else if (column_types[[i]] == "input") {
            input_columns <- append(input_columns, i)
        } else if (column_types[[i]] == "output") {
            output_columns <- append(output_columns, i)
        }
    }

    if (length(description_column) < 1) {
        stop(paste0(
            "Could not find the `description` column in module test case file `",
            filename,
            "`\n  ",
            "Note: this column name must be defined in the second row of the file."
        ))
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
    description = "automatically-generated test case",
    overwrite = FALSE
)
{
    # Get info about the module
    info <- module_info(module_name, FALSE)

    # Get the default input quantities
    inputs <- quantity_list_from_names(info[['inputs']])

    # Modify any inputs that should take nonstandard values
    for (i in seq_along(nonstandard_inputs)) {
        ns_input_name <- names(nonstandard_inputs)[i]
        if (ns_input_name %in% names(inputs)) {
            inputs[[ns_input_name]] <- nonstandard_inputs[[i]]
        }
    }

    # Run the module using the inputs
    outputs <- evaluate_module(module_name, inputs)

    # Make a case list with one element
    case_list <- list(
        case(inputs, outputs, description)
    )

    # Write the case to a new .csv file
    csv_from_cases(module_name, directory, case_list, overwrite)
}

add_csv_row <- function(module_name, directory, inputs, description)
{
    # Initialize the csv file if it doesn't already exist
    msg <- paste(
        "Initializing csv file if required:",
        initialize_csv(module_name, directory, overwrite = FALSE)
    )

    # Get any test cases already defined in the module's csv file
    case_list <- cases_from_csv(module_name, directory)

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
    append(
        msg,
        paste(
            "Updating csv file with new case:",
            csv_from_cases(module_name, directory, case_list, TRUE)
        )
    )
}

update_csv_cases <- function(module_name, directory)
{
    # Get any test cases already defined in the module's csv file
    case_list <- cases_from_csv(module_name, directory)

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
    paste(
        "Updating csv file with new output values:",
        csv_from_cases(module_name, directory, updated_case_list, TRUE)
    )
}

test_module_library <- function(library_name, directory, modules_to_skip = c()) {
    # Get the names of all the modules in the library
    module_names <- get_all_modules(library_name)

    # Remove any modules that should be skipped
    module_names <- module_names[!module_names %in% modules_to_skip]

    # Run all the module tests
    test_result <- lapply(module_names, function(module) {
        # Try to load the test cases for this module
        msg <- character()
        cases <- tryCatch(
            cases_from_csv(module, directory),
            condition = function(cond) {
                msg <<- paste0(
                    "Module `",
                    module_paste(library_name, module),
                    "`: could not load test cases: ",
                    cond
                )
            }
        )

        # If an error was thrown, return a message about it
        if (length(msg) > 0) {
            return(gsub("\n", "", msg, fixed = TRUE))
        }

        # Run each test case
        lapply(cases, function(case) {
            test_module(module_paste(library_name, module), case)
        })
    })

    # Thow an error if any problems occurred
    test_result <- unlist(test_result)

    if (length(test_result) > 0) {
        test_result <- append(
            paste0(
                "Problems occurred while testing modules: from the `",
                library_name,
                "` library:"
            ),
            test_result
        )
        stop(paste(test_result, collapse = '\n  '))
    }
}
