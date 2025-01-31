check_model_test_case_inputs <- function(
    model_name,
    model_definition,
    drivers,
    should_run,
    quantities_to_ignore
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

    # The model_name and quantities_to_ignore should be vectors or lists of
    # strings
    error_message <- append(
        error_message,
        check_strings(
            list(
                model_name = model_name,
                quantities_to_ignore = quantities_to_ignore
            )
        )
    )

    # should_run should be a boolean with one element
    error_message <- append(
        error_message,
        check_boolean(list(should_run = should_run))
    )

    error_message <- append(
        error_message,
        check_length(list(should_run = should_run))
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
    quantities_to_ignore = character(0)
)
{
    # Check over the inputs arguments for possible issues
    error_messages <- check_model_test_case_inputs(
        model_name,
        model_definition,
        drivers,
        should_run,
        quantities_to_ignore
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
        quantities_to_ignore = quantities_to_ignore
    )
}
