# A helping function that checks the inputs to `model_test_case`. If any issues
# are found, this function will return a string describing them; otherwise, it
# will return an empty string.
check_model_test_case_inputs <- function(
    test_case_name,
    model_definition,
    drivers,
    check_outputs,
    directory,
    row_interval,
    quantities_to_ignore,
    relative_tolerance
)
{
    error_message <- character()

    # The model_definition should be a list
    error_message <- append(
        error_message,
        check_list(
            list(
                model_definition = model_definition
            )
        )
    )

    # The drivers should be a data frame
    error_message <- append(
        error_message,
        check_data_frame(list(drivers = drivers))
    )

    # The drivers should not be empty
    if (length(drivers) == 0) {
        error_message <- append(error_message, 'The drivers cannot be empty')
    }

    # The test_case_name, directory, and quantities_to_ignore should be vectors or
    # lists of strings
    error_message <- append(
        error_message,
        check_strings(
            list(
                test_case_name = test_case_name,
                directory = directory,
                quantities_to_ignore = quantities_to_ignore
            )
        )
    )

    # check_outputs should be a boolean
    error_message <- append(
        error_message,
        check_boolean(list(check_outputs = check_outputs))
    )

    # The row_interval and relative_tolerance should be
    # numeric
    error_message <- append(
        error_message,
        check_numeric(
            list(
                row_interval = row_interval,
                relative_tolerance = relative_tolerance
            )
        )
    )

    # test_case_name, directory, check_outputs, row_interval, and
    # relative_tolerance should have length one
    error_message <- append(
        error_message,
        check_length(
            list(
                test_case_name = test_case_name,
                directory = directory,
                check_outputs = check_outputs,
                row_interval = row_interval,
                relative_tolerance = relative_tolerance
            )
        )
    )

    # Make sure the model definition has the required elements
    error_message <- append(
        error_message,
        check_required_elements(
            list(model_definition = model_definition),
            c(
                'initial_values',
                'parameters',
                'direct_modules',
                'differential_modules',
                'ode_solver'
            )
        )
    )

    return(error_message)
}

model_test_case <- function(
    test_case_name,
    model_definition,
    drivers,
    check_outputs,
    directory = '.',
    row_interval = 1,
    quantities_to_ignore = character(),
    relative_tolerance = 5e-3
)
{
    # Check over the inputs arguments for possible issues
    error_messages <- check_model_test_case_inputs(
        test_case_name,
        model_definition,
        drivers,
        check_outputs,
        directory,
        row_interval,
        quantities_to_ignore,
        relative_tolerance
    )

    stop_and_send_error_messages(error_messages)

    # Define the model test case
    list(
        test_case_name = test_case_name,
        initial_values = model_definition[['initial_values']],
        parameters = model_definition[['parameters']],
        drivers = drivers,
        direct_modules = model_definition[['direct_modules']],
        differential_modules = model_definition[['differential_modules']],
        ode_solver = model_definition[['ode_solver']],
        check_outputs = check_outputs,
        stored_result_file = file.path(
            directory,
            paste0(test_case_name, '_simulation.csv')
        ),
        row_interval = row_interval,
        quantities_to_ignore = quantities_to_ignore,
        relative_tolerance = relative_tolerance
    )
}

# A helping function that runs a model to get a new result. The values will be
# rounded, and only a subset of the rows will be retained. This function will
# return the result as a data frame.
run_model_simulation <- function(mtc) {
    # Run the model
    simulation_result <- run_biocro(
        mtc[['initial_values']],
        mtc[['parameters']],
        mtc[['drivers']],
        mtc[['direct_modules']],
        mtc[['differential_modules']],
        mtc[['ode_solver']]
    )

    # Store the number of rows in the result
    simulation_result$nrow <- nrow(simulation_result)

    # Only keep a subset of the rows
    row_to_keep <-
        seq(1, nrow(simulation_result), by = mtc[['row_interval']])

    simulation_result <- simulation_result[row_to_keep, ]

    # Round the numeric values in the result
    for (cn in colnames(simulation_result)) {
        if (is.numeric(simulation_result[[cn]])) {
            simulation_result[[cn]] <-
                signif(simulation_result[[cn]], digits = 5)
        }
    }

    return(simulation_result)
}

# A helping function that loads a stored model result. If an issue occurs, this
# function will return a descriptive message. Otherwise, it will return a data
# frame of the stored test results.
get_stored_result <- function(mtc) {
    stored_result_file <- mtc[['stored_result_file']]

    if (!file.exists(stored_result_file)) {
        msg <- paste0(
            'Stored result file `', stored_result_file,
            '` does not exist.'
        )
        return(msg)
    } else {
        return(utils::read.csv(stored_result_file, stringsAsFactors = FALSE))
    }
}

compare_model_output <- function(mtc, columns_to_keep = NULL) {
    # Get the stored result, if it exists
    stored_result <- get_stored_result(mtc)

    if (is.character(stored_result)) {
        stop(stored_result, call. = FALSE)
    }

    # Get the new result
    new_result <- run_model_simulation(mtc)

    # Determine which columns to keep
    if (is.null(columns_to_keep)) {
        columns_to_keep <- colnames(new_result)

        columns_to_keep <-
            columns_to_keep[columns_to_keep %in% colnames(stored_result)]
    }

    # Restrict columns to avoid problems with `rbind`
    new_result  <- new_result[, columns_to_keep]
    stored_result <- stored_result[, columns_to_keep]

    # Add version info
    new_result[['version']]    <- 'new'
    stored_result[['version']] <- 'stored'

    return(rbind(new_result, stored_result))
}

update_stored_model_results <- function(mtc) {
    # Get the new result
    plant_result <- run_model_simulation(mtc)

    # Save it as a csv file
    utils::write.csv(
        plant_result,
        file = mtc[['stored_result_file']],
        quote = FALSE,
        eol = '\n',
        na = '',
        row.names = FALSE
    )
}

# A helping function that runs a single model test. If the checks all pass, this
# function will return an empty string. Otherwise it will return a message
# describing the first issue detected.
test_model <- function(mtc) {
    # Check the model for validity
    model_valid <- validate_dynamical_system_inputs(
        mtc[['initial_values']],
        mtc[['parameters']],
        mtc[['drivers']],
        mtc[['direct_modules']],
        mtc[['differential_modules']],
        verbose = FALSE
    )

    if (!model_valid) {
        msg <- paste0(
            'The `', mtc[['test_case_name']],
            '` simulation does not have a valid definition.'
        )
        return(msg)
    }

    # Run the model to get the new result, if possible
    new_result <- tryCatch(
        run_model_simulation(mtc),
        error = function(cond) {
            paste0(
                'The `', mtc[['test_case_name']],
                '` simulation produced an error message: ', cond
            )
        },
        warning = function(cond) {
            paste0(
                'The `', mtc[['test_case_name']],
                '` simulation produced a warning message: ', cond
            )
        }
    )

    if (is.character(new_result)) {
        return(new_result)
    }

    # Optionally compare the new results to stored results
    if (mtc[['check_outputs']]) {
        # Get the stored result, if it exists
        stored_result <- get_stored_result(mtc)

        if (is.character(stored_result)) {
            return(stored_result)
        }

        # Make sure the number of rows has not changed
        new_nrow    <- new_result[1, 'nrow']
        stored_nrow <- stored_result[1, 'nrow']

        if (stored_nrow != new_nrow) {
            msg <- paste0(
                'The `', mtc[['test_case_name']],
                '` simulation result has ', new_nrow, ' rows, but the saved ',
                'result has ', stored_nrow, ' rows.'
            )

            return(msg)
        }

        # If there are columns in the new result that are not in the stored
        # result, we should notify the user, but we don't need to treat
        # this as an error condition
        unexpected_columns <- !colnames(new_result) %in% colnames(stored_result)
        if (any(unexpected_columns)) {
            msg <- paste0(
                'The `', mtc[['test_case_name']],
                '` simulation result contains columns that are not in ',
                'the saved result. Is this intentional? Extra columns: ',
                paste(colnames(new_result)[unexpected_columns], collapse = ', '),
                '.'
            )
            warning(msg, call. = FALSE)
        }

        # Remove any ignored columns from the stored result
        col_to_keep <- !colnames(stored_result) %in% mtc[['quantities_to_ignore']]
        stored_result <- stored_result[, col_to_keep]

        # Make sure all the columns in the stored result are in the new
        # result
        missing_columns <- !colnames(stored_result) %in% colnames(new_result)
        if (any(missing_columns)) {
            msg <- paste0(
                'The `', mtc[['test_case_name']],
                '` simulation result is missing required columns from ',
                'the saved result: ',
                paste(colnames(stored_result)[missing_columns], collapse = ', '),
                '.'
            )
            return(msg)
        }

        # Check whether the values of columns have changed
        values_equal <- sapply(colnames(stored_result), function(cn) {
            isTRUE(
                all.equal(
                    as.numeric(stored_result[[cn]]),
                    as.numeric(new_result[[cn]]),
                    tolerance = mtc[['relative_tolerance']]
                )
            )
        })

        if (any(!values_equal)) {
            msg <- paste0(
                'The new `', mtc[['test_case_name']],
                '` simulation result does not agree with the stored result ',
                'for the following columns: ',
                paste(colnames(stored_result)[!values_equal], collapse = ', '),
                '.'
            )
            return(msg)
        }
    }

    # If no issues were found, return an empty string
    return(character())
}

run_model_test_cases <- function(model_test_cases) {
    # Run all the model test cases
    test_result <- lapply(model_test_cases, test_model)

    # Thow an error if any problems occurred
    test_result <- unlist(test_result)

    if (length(test_result) > 0) {
        test_result <- append(
            paste(
                'Problems occurred while testing models:'
            ),
            test_result
        )
        stop(paste(test_result, collapse = '\n  '), call. = FALSE)
    }

    return(TRUE)
}
