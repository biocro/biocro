
check_if_dynamical_system <- function(x){
    if (attr(x, "class") != "biocro_dynamical_system"){
        error_messages <- "`dynamical_system` must be an R object produced by `create_dynamical_system`"
    } else {
        error_messages <- character(0)
    }
}

create_dynamical_system <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_module_names = list(),
    differential_module_names = list()
)
{
    # Make sure weather data is properly handled
    adapted <- adapt_weather_data(drivers, direct_module_names)
    drivers <- adapted$drivers
    direct_module_names <- adapted$direct_module_names

    # Check over the inputs arguments for possible issues
    error_messages <- check_run_biocro_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names
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
    # Run the C++ code
    result <- .Call(
        R_create_dynamical_system,
        initial_values,
        parameters,
        drivers,
        direct_module_creators,
        differential_module_creators
    )

    attr(result, "class") <- "biocro_dynamical_system"

    # Return the result
    return(result)
}

get_system_modules <- function(
    dynamical_system
)
{
    msg <- check_if_dynamical_system(dynamical_system)

    stop_and_send_error_messages(msg)

    # Run the C++ code
    result <- .Call(
        R_get_system_modules,
        dynamical_system
    )

    # Return the result
    return(result)
}


calculative_derivative <- function(
    dynamical_system, time, state
)
{
    msg <- check_if_dynamical_system(dynamical_system)

    stop_and_send_error_messages(msg)
    time <- as.double(time)
    state <- as.double(state)
    # Run the C++ code
    result <- .Call(
        R_calculate_derivative,
        dynamical_system,
        time,
        state
    )

    # Return the result
    return(result)
}


