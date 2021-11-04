validate_dynamical_system_inputs <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_module_names = list(),
    differential_module_names = list(),
    silent = FALSE
)
{
    # The inputs have the same requirements as `system_derivatives`
    error_messages <- check_system_derivatives_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names
    )

    send_error_messages(error_messages)

    # If the drivers input doesn't have a time column, add one
    drivers <- add_time_to_weather_data(drivers)

    # Make sure the module names are vectors of strings
    direct_module_names <- unlist(direct_module_names)
    differential_module_names <- unlist(differential_module_names)

    # C++ requires that all the variables have type `double`
    initial_values <- lapply(initial_values, as.numeric)
    parameters <- lapply(parameters, as.numeric)
    drivers <- lapply(drivers, as.numeric)

    # Make sure silent is a logical variable
    silent <- lapply(silent, as.logical)

    # Run the C++ code
    result <- .Call(
        R_validate_dynamical_system_inputs,
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names,
        silent
    )

    return(result)
}
