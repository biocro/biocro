validate_dynamical_system_inputs <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_module_specifications = list(),
    differential_module_specifications = list(),
    verbose = TRUE
)
{
    # The inputs to this function have the same requirements as the `run_biocro`
    # inputs with the same names
    error_messages <- check_run_biocro_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_specifications,
        differential_module_specifications,
        verbose = verbose
    )

    send_error_messages(error_messages)

    # If the drivers input doesn't have a time column, add one
    drivers <- add_time_to_weather_data(drivers)

    # Make module creators from the specified names and libraries
    direct_module_creators <- sapply(
        direct_module_specifications,
        check_out_module
    )

    differential_module_creators <- sapply(
        differential_module_specifications,
        check_out_module
    )

    # C++ requires that all the variables have type `double`
    initial_values <- lapply(initial_values, as.numeric)
    parameters <- lapply(parameters, as.numeric)
    drivers <- lapply(drivers, as.numeric)

    # Make sure verbose is a logical variable
    verbose <- lapply(verbose, as.logical)

    # Run the C++ code
    result <- .Call(
        R_validate_dynamical_system_inputs,
        initial_values,
        parameters,
        drivers,
        direct_module_creators,
        differential_module_creators,
        verbose
    )

    return(result)
}
