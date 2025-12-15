test_module <- function(module_name, case_to_test, verbose = TRUE) {
    # Get the expected outputs
    expected_outputs <- case_to_test[["expected_outputs"]]

    # Try to get the actual outputs
    msg <- character()
    actual_outputs <- tryCatch(
        {
            evaluate_module(
                module_name,
                case_to_test[["inputs"]],
                stop_on_exception = TRUE
            )
        },
        error = function(cond) {
            msg <<- paste0(
                "Module `",
                module_name,
                "` test case `",
                case_to_test[["description"]],
                "`: could not calculate outputs: ",
                cond
            )
        }
    )

    # If an error was thrown, return a message about it
    if (length(msg) > 0) {
        return(gsub("\n", "", msg, fixed = TRUE))
    }

    # Check to see if there are new outputs; if there are, throw a warning and
    # proceed to check the outputs that are present in the stored test case
    actual_output_names <- names(actual_outputs)
    expected_output_names <- names(expected_outputs)

    new_output_names <-
        actual_output_names[!actual_output_names %in% expected_output_names]

    if (length(new_output_names) > 0) {
        warning(
            paste0(
                "Module `",
                module_name,
                "` test case `",
                case_to_test[['description']],
                "`: unexpected outputs were found: ",
                paste(new_output_names, collapse = ', ')
            )
        )
        actual_outputs <-
            actual_outputs[actual_output_names %in% expected_output_names]
    }

    # Make sure the outputs are ordered the same way (otherwise `all.equal` may
    # indicate a difference when there isn't one)
    expected_outputs <- expected_outputs[order(names(expected_outputs))]
    actual_outputs <- actual_outputs[order(names(actual_outputs))]

    # Check to see if the expected and actual outputs match
    if (!isTRUE(all.equal(expected_outputs, actual_outputs))) {
        # Collect discrepancies into a table and print it to the terminal, if
        # desired
        if (verbose) {
            idx <- sapply(seq_along(expected_outputs), function(i) {
                qname <- names(expected_outputs)[[i]]
                expected <- expected_outputs[[i]]

                actual <- if (qname %in% names(actual_outputs)) {
                    actual_outputs[[qname]]
                } else {
                    NA
                }

                if (is.na(actual)) {
                    TRUE
                } else {
                    abs(expected - actual) > sqrt(.Machine$double.eps)
                }
            })

            qnames <- names(expected_outputs)[idx]
            expected <- unlist(expected_outputs)[idx]

            actual <- sapply(qnames, function(qn) {
                if (qn %in% names(actual_outputs)) {
                    actual_outputs[[qn]]
                } else {
                    NA
                }
            })

            differences <- data.frame(
                Quantity = qnames,
                Expected = expected,
                Actual = actual,
                stringsAsFactors = FALSE
            )

            rownames(differences) <- NULL

            cat(paste0(
                '\nDifferences found for Module `',
                module_name,
                '` test case `',
                case_to_test[['description']],
                '`:\n\n'
            ))

            print(differences)

            cat('\n')
        }

        return(
            paste0(
                "Module `",
                module_name,
                "` test case `",
                case_to_test[["description"]],
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

# A function for generating a test case file path from a fully-qualified module
# name of the form `library_name:local_module_name`. To form the file name, any
# colons in the fully-qualified module name are replaced by underscores and
# `.csv` is appended to the end. The full path is formed by including the
# directory. This is only intended to be used internally by `csv_from_cases` and
# `cases_from_csv`.
module_case_file_path <- function(module_name, directory) {
    file.path(
        directory,
        paste0(gsub(":", "_", module_name, fixed = TRUE), ".csv")
    )
}

# A helping function for creating a data frame of columns of one type, as
# extracted from a case list.
#
# Inputs:
#
# - case_list: a list of cases for a module, as described in the documentation
#   for the `test_module` function
#
# - col_type: the type of column to extract; either `inputs`,
#   `expected_outputs`, or `description`
#
# - col_names: the names of columns to extract; this would be a vector of
#   quantity names if the type is `inputs` or `expected_outputs`; the
#  `description` column does not have additional identifiers, so col_names
#   should be set to 1 (to take the first column) when the column type is
#   `description`
#
# Outputs: a data frame of values extracted from the case list
#
df_from_case_list <- function(case_list, col_type, col_names) {
    as.data.frame(
        do.call(cbind, lapply(col_names, function(name) {
            sapply(case_list, function(case) {
                case[[col_type]][[name]]
            })
        })),
        row.names = NULL,
        stringsAsFactors = FALSE
    )
}

# csv_from_cases: A function to store test cases for module testing by writing a
# list of test cases to a csv file, whose name will be determined from the
# module's name using `module_case_file_path`.
#
# Inputs:
#
# - module_name: a string specifying one BioCro module, formatted like
#   `library_name:module_name`, where `library_name` is the name of a library
#   that contains a module with name `module_name`.
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
# `module_name` input as `library_name_module_name.csv`. This csv file
# can be read by the `cases_from_csv` function. BioCro users typically do not
# need to access this function directly.
#
csv_from_cases <- function(
    module_name,
    directory,
    case_list) {
    filename <- module_case_file_path(module_name, directory)

    info <- module_info(module_name, FALSE)

    # Extract the inputs, expected outputs, and descriptions from the case list
    case_value_df <- cbind(
        df_from_case_list(case_list, "inputs", info[["inputs"]]),
        df_from_case_list(case_list, "expected_outputs", info[["outputs"]]),
        df_from_case_list(case_list, "description", 1)
    )

    # Make a data frame with the correct column headers
    header_df <- data.frame(
        matrix(nrow = 2, ncol = ncol(case_value_df)),
        stringsAsFactors = FALSE
    )

    header_df[1, ] <- c(
        rep.int("input", length(info[["inputs"]])),
        rep.int("output", length(info[["outputs"]])),
        "description"
    )

    header_df[2, ] <- c(info[["inputs"]], info[["outputs"]], NA)

    # Make sure the data frames have the same column names, or rbind will fail
    colnames(case_value_df) <- colnames(header_df)

    # Add the headers to the case value data frame and write the case to a file
    utils::write.table(
        rbind(header_df, case_value_df),
        filename,
        sep = ",",
        row.names = FALSE,
        col.names = FALSE,
        quote = ncol(case_value_df), # only quote the last column ('description')
        qmethod = "double"
    )
}

cases_from_csv <- function(module_name, directory) {
    # Generate the filename
    filename <- module_case_file_path(module_name, directory)

    # Make sure the file exists
    if (!file.exists(filename)) {
        stop(paste0("Module test case file `", filename, "` does not exist"))
    }

    # Read the data, making sure to never use factors
    file_contents <- utils::read.csv(
        filename,
        header = FALSE,
        stringsAsFactors = FALSE
    )

    # Get the column names and types, making sure to trim any whitespace
    column_types <- file_contents[1, ]
    column_types <- trimws(column_types)

    column_names <- file_contents[2, ]
    column_names <- trimws(column_names)

    # Get the quantity values
    test_data <- file_contents[-(1:2), ]

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
            "Could not find the `description` column in module ",
            "test case file `",
            filename,
            "`\n  ",
            "Note: this column name must be defined in the second row ",
            "of the file."
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
    overwrite = FALSE) {
    # Generate the filename
    filename <- module_case_file_path(module_name, directory)

    # Check to see if we should overwrite an existing file
    if (!overwrite && file.exists(filename)) {
        return(paste0("Did not initialize case file because `", filename, "` already exists"))
    }

    # Get info about the module
    info <- module_info(module_name, FALSE)

    # Get the default input quantities
    inputs <- quantity_list_from_names(info[["inputs"]])

    # Modify any inputs that should take nonstandard values
    inputs[names(nonstandard_inputs)] <- nonstandard_inputs

    # Run the module using the inputs
    outputs <- evaluate_module(module_name, inputs, stop_on_exception = TRUE)

    # Make a case list with one element
    case_list <- list(case(inputs, outputs, description))

    # Write the case to a new .csv file and send a message to the user
    csv_from_cases(module_name, directory, case_list)
    paste0("Case file `", filename, "` was initialized; any pre-existing file was overwritten")
}

add_csv_row <- function(module_name, directory, inputs, description) {
    # Generate the filename
    filename <- module_case_file_path(module_name, directory)

    # If the file doesn't exist, initialize it with the specified inputs and
    # description, and then exit
    if (!file.exists(filename)) {
        return(initialize_csv(module_name, directory, inputs, description))
    }

    # Get any test cases already defined in the module's csv file
    case_list <- cases_from_csv(module_name, directory)

    # Run the module using the inputs
    outputs <- evaluate_module(module_name, inputs, stop_on_exception = TRUE)

    # Add a new case to the list
    case_list <- append(
        case_list,
        list(case(inputs, outputs, description))
    )

    # Save the full list to the file and send a message to the user
    csv_from_cases(module_name, directory, case_list)
    paste0("Added new case to file `", filename, "`")
}

update_csv_cases <- function(module_name, directory) {
    # Get any test cases already defined in the module's csv file
    case_list <- cases_from_csv(module_name, directory)

    # Define a helping function that updates a test case by running the module
    # using the inputs and storing the result as the expected outputs
    update_case <- function(one_case) {
        one_case[["expected_outputs"]] <- evaluate_module(
            module_name,
            one_case[["inputs"]],
            stop_on_exception = TRUE
        )

        return(one_case)
    }

    # Update all the test cases
    updated_case_list <- lapply(case_list, update_case)

    # Save the new list to the file and send a message to the user
    csv_from_cases(module_name, directory, updated_case_list)
    paste0("Updated case file `", module_case_file_path(module_name, directory), "`")
}

# A helping function that determines fully-qualified module names from the test
# case CSV files in a directory
module_names_from_case_directory <- function(library_name, directory) {
    csv_files <- list.files(directory, "[Cc][Ss][Vv]$")

    module_names <- sapply(csv_files, function(fn) {
        # Get the un-qualified module name
        mn <- gsub(paste0(library_name, "_"), "", tools::file_path_sans_ext(fn))

        # Add the library name with proper formatting
        paste0(library_name, ":", mn)
    })

    module_names
}

test_module_library <- function(
    library_name,
    directory,
    modules_to_skip = c(),
    verbose = TRUE) {
    # Get the module names associated with the test cases in the directory
    directory_module_names <-
        module_names_from_case_directory(library_name, directory)

    # Get the names of all the modules in the library
    module_names <- get_all_modules(library_name)

    # Make sure all the test cases are for modules in this library
    if (!all(directory_module_names %in% module_names)) {
        extra_modules <-
            directory_module_names[!directory_module_names %in% module_names]

        msg <- paste0(
            "Found test cases in the `", directory,
            "` directory for modules that are not in the `", library_name,
            "` module library: ",
            paste(extra_modules, collapse = ", ")
        )

        stop(msg)
    }

    # Append the library name to the modules we should skip
    modules_to_skip <- module_paste(library_name, modules_to_skip)

    # Remove any modules that should be skipped
    module_names <- module_names[!module_names %in% modules_to_skip]

    # Run all the module tests
    test_result <- lapply(module_names, function(module) {
        # Make sure the module can be instantiated
        info <- module_info(module, verbose = FALSE)
        msg <- if (info[["creation_error_message"]] != "none") {
            paste0(
                "Module `",
                module,
                "`: could not be instantiated: ",
                info[["creation_error_message"]]
            )
        } else {
            character()
        }

        # Try to load the test cases for this module
        cases <- tryCatch(
            cases_from_csv(module, directory),
            condition = function(cond) {
                msg <<- append(msg, paste0(
                    "Module `",
                    module,
                    "`: could not load test cases: ",
                    cond
                ))
            }
        )

        # If any problems occurred, return a message about them
        if (length(msg) > 0) {
            return(gsub("\n", "", msg, fixed = TRUE))
        }

        # Run each test case
        lapply(cases, function(case) {
            test_module(module, case, verbose)
        })
    })

    # Thow an error if any problems occurred
    test_result <- unlist(test_result)

    if (length(test_result) > 0) {
        test_result <- append(
            paste0(
                "Problems occurred while testing modules from the `",
                library_name,
                "` library:"
            ),
            test_result
        )
        stop(paste(test_result, collapse = "\n  "))
    }

    return(invisible(NULL))
}
