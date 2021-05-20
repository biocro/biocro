validate_system_inputs <- function(initial_state, parameters, drivers, steady_state_module_names, derivative_module_names, silent = FALSE)
{
    # If the drivers input doesn't have a time column, add one
    drivers <- add_time_to_weather_data(drivers)

    # C++ requires that all the variables have type `double`
    initial_state = lapply(initial_state, as.numeric)
    parameters = lapply(parameters, as.numeric)
    drivers = lapply(drivers, as.numeric)

    # Make sure silent is a logical variable
    silent = lapply(silent, as.logical)

    # Run the C++ code
    result = .Call(R_validate_system_inputs, initial_state, parameters, drivers, steady_state_module_names, derivative_module_names, silent)
    return(result)
}
