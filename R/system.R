# `validate_system_inputs` checks a set of parameters, drivers, modules, and
# initial values to see if they can properly define a dynamical system and can
# therefore be used as inputs to `run_biocro`.
#
# The return value from this function is a boolean indicating whether the inputs
# can form a valid system.
#
# The `run_biocro` function performs all the same checks. However,
# `validate_system_inputs` returns additional information, such as a list of all
# quantities that are defined in the parameters, initial values, and drivers
# that are not used as inputs to any modules.
#
# When silent is TRUE, no information will be printed to the R console.
validate_system_inputs <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    steady_state_module_names = list(),
    derivative_module_names = list(),
    silent = FALSE
)
{
    # The inputs have the same requirements as Gro_deriv
    error_messages = check_Gro_deriv_inputs(
        initial_values,
        parameters,
        drivers,
        steady_state_module_names,
        derivative_module_names
    )

    if (length(error_messages) > 0) {
        stop(paste(error_messages, collapse='  '))
    }

    # If the drivers input doesn't have a time column, add one
    drivers <- add_time_to_weather_data(drivers)

    # Make sure the module names are vectors of strings
    steady_state_module_names <- unlist(steady_state_module_names)
    derivative_module_names <- unlist(derivative_module_names)

    # C++ requires that all the variables have type `double`
    initial_values = lapply(initial_values, as.numeric)
    parameters = lapply(parameters, as.numeric)
    drivers = lapply(drivers, as.numeric)

    # Make sure silent is a logical variable
    silent = lapply(silent, as.logical)

    # Run the C++ code
    result = .Call(
        R_validate_system_inputs,
        initial_values,
        parameters,
        drivers,
        steady_state_module_names,
        derivative_module_names,
        silent
    )

    return(result)
}
