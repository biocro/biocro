check_model_test_case_inputs <- function(
    model_name,
    model_definition,
    drivers,
    should_run,
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

    # The model_name, directory, and quantities_to_ignore should be vectors or
    # lists of strings
    error_message <- append(
        error_message,
        check_strings(
            list(
                model_name = model_name,
                directory = directory,
                quantities_to_ignore = quantities_to_ignore
            )
        )
    )

    # should_run should be a boolean
    error_message <- append(
        error_message,
        check_boolean(list(should_run = should_run))
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

    # model_name, directory, should_run, row_interval, and relative_tolerance
    # should have length one
    error_message <- append(
        error_message,
        check_length(
            list(
                model_name = model_name,
                directory = directory,
                should_run = should_run,
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
    model_name,
    model_definition,
    drivers,
    should_run,
    directory = '.',
    row_interval = 1,
    quantities_to_ignore = character(0),
    relative_tolerance = 5e-3
)
{
    # Check over the inputs arguments for possible issues
    error_messages <- check_model_test_case_inputs(
        model_name,
        model_definition,
        drivers,
        should_run,
        directory,
        row_interval,
        quantities_to_ignore,
        relative_tolerance
    )

    stop_and_send_error_messages(error_messages)

    # Define the model test case
    list(
        model_name = model_name,
        initial_values = model_definition[['initial_values']],
        parameters = model_definition[['parameters']],
        drivers = drivers,
        direct_modules = model_definition[['direct_modules']],
        differential_modules = model_definition[['differential_modules']],
        ode_solver = model_definition[['ode_solver']],
        should_run = should_run,
        stored_result_file = file.path(
            directory,
            paste0(model_name, '_simulation.csv')
        ),
        row_interval = row_interval,
        quantities_to_ignore = quantities_to_ignore,
        relative_tolerance = relative_tolerance
    )
}

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

test_model <- function(mtc) {
    # Always check the model for validity
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
            'The `', mtc[['model_name']],
            '` simulation does not have a valid definition.'
        )
        return(msg)
    }

    # Optionally run the model and check the results
    if (mtc[['should_run']]) {
        # Get the stored result, if it exists
        stored_result_file <- mtc[['stored_result_file']]

        if (!file.exists(stored_result_file)) {
            msg <- paste0(
                'Stored result file `', stored_result_file,
                '` does not exist.'
            )
            return(msg)
        }

        stored_result <- read.csv(stored_result_file, stringsAsFactors = FALSE)

        # Run the model to get the new result, if possible
        new_result <- tryCatch(
            run_model_simulation(mtc),
            error = function(cond) {
                paste0(
                    'The `', mtc[['model_name']],
                    '` simulation produced an error message: ', cond
                )
            },
            warning = function(cond) {
                paste0(
                    'The `', mtc[['model_name']],
                    '` simulation produced a warning message: ', cond
                )
            }
        )

        if (is.character(new_result)) {
            return(new_result)
        }

        # Make sure the simulation was completed
        stored_nrow <- stored_result[1, 'nrow']
        new_nrow    <- new_result[1, 'nrow']

        if (stored_nrow != new_nrow) {
            msg <- paste0(
                'The `', mtc[['model_name']],
                '` simulation result has ', new_nrow, ' rows, but the saved ',
                'result has ', stored_nrow, ' rows.'
            )

            return(msg)
        } else {
            # If the simulation was completed, compare the new results to the
            # stored results

            # If there are columns in the new result that are not in the stored
            # result, we should notify the user, but we don't need to treat
            # this as an error condition
            unexpected_columns <- !colnames(new_result) %in% colnames(stored_result)
            if (any(unexpected_columns)) {
                msg <- paste0(
                    'The `', mtc[['model_name']],
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
                    'The `', mtc[['model_name']],
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
                    'The new `', mtc[['model_name']],
                    '` simulation result does not agree with the stored result ',
                    'for the following columns: ',
                    paste(colnames(stored_result)[!values_equal], collapse = ', '),
                    '.'
                )
                return(msg)
            }
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
