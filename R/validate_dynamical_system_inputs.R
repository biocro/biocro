validate_dynamical_system_inputs <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_module_names = list(),
    differential_module_names = list(),
    verbose = TRUE
)
{
    # Make sure weather data is properly handled
    adapted <- adapt_weather_data(drivers, direct_module_names)
    drivers <- adapted$drivers
    direct_module_names <- adapted$direct_module_names

    # The inputs to this function have the same requirements as the `run_biocro`
    # inputs with the same names
    error_messages <- check_run_biocro_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names,
        verbose = verbose
    )

    stop_and_send_error_messages(error_messages)

    # Make module creators from the specified names and libraries
    direct_module_creators <- sapply(
        direct_module_names,
        check_out_module
    )

    differential_module_creators <- sapply(
        differential_module_names,
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
