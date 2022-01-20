validate_dynamical_system_inputs <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_modules = list(),
    differential_modules = list(),
    verbose = TRUE
)
{
    # The inputs to this function have the same requirements as the `run_biocro`
    # inputs with the same names
    error_messages <- check_run_biocro_inputs(
        initial_values,
        parameters,
        drivers,
        direct_modules,
        differential_modules,
        verbose = verbose
    )

    send_error_messages(error_messages)

    # If the drivers input doesn't have a time column, add one
    drivers <- add_time_to_weather_data(drivers)

    # Make sure the module names are vectors of strings
    direct_modules <- unlist(direct_modules)
    differential_modules <- unlist(differential_modules)

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
        direct_modules,
        differential_modules,
        verbose
    )

    return(result)
}
