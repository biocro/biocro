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
    direct_module_names = list(),
    differential_module_names = list(),
    silent = FALSE
)
{
    # Check to make sure the initial values are properly defined
    if (!is.list(initial_values)) {
        stop('"initial_values" must be a list')
    }

    if (length(initial_values) != length(unlist(initial_values))) {
        item_lengths = unlist(lapply(initial_values, length))
        error_message = sprintf("The following initial_values members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
        stop(error_message)
    }

    # Check to make sure the parameters are properly defined
    if (!is.list(parameters)) {
        stop('"parameters" must be a list')
    }

    if (length(parameters) != length(unlist(parameters))) {
        item_lengths = unlist(lapply(parameters, length))
        error_message = sprintf("The following parameters members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
        stop(error_message)
    }

    # Check to make sure the drivers are properly defined
    if (!is.list(drivers)) {
        stop('"drivers" must be a list')
    }

    # If the drivers input doesn't have a time column, add one
    drivers <- add_time_to_weather_data(drivers)

    # Check to make sure the module names are vectors or lists of strings
    direct_module_names <- unlist(direct_module_names)
    if (length(direct_module_names) > 0 & !is.character(direct_module_names)) {
        stop('"direct_module_names" must be a vector or list of strings')
    }

    differential_module_names <- unlist(differential_module_names)
    if (length(differential_module_names) > 0 & !is.character(differential_module_names)) {
        stop('"differential_module_names" must be a vector or list of strings')
    }

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
        direct_module_names,
        differential_module_names,
        silent
    )

    return(result)
}
