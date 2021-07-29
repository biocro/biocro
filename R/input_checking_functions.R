# Checks whether the elements of the `args_to_check` list are lists. If all
# elements meet this criterion, this functions returns an empty string.
# Otherwise, it returns an informative error message. For a truly helpful
# message, the elements of `args_to_check` should be named.
check_list <- function(args_to_check) {
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- args_to_check[[i]]
        if (!is.list(arg)) {
            error_message <- append(
                error_message,
                sprintf('"%s" must be a list.', names(args_to_check)[i])
            )
        }
    }
    return(error_message)
}

# Checks whether the elements of the `args_to_check` list are lists of elements
# that each have length 1. If all elements meet this criterion, this function
# returns an empty string. Otherwise, it returns an informative error message.
# For a truly helpful message, the elements of `args_to_check` should be named.
check_element_length <- function(args_to_check) {
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- args_to_check[[i]]
        if (length(arg) != length(unlist(arg))) {
            item_lengths <- unlist(lapply(arg, length))
            tmp_message <- sprintf(
                "The following %s members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n",
                names(args_to_check)[i],
                paste(names(item_lengths)[which(item_lengths > 1)], collapse=', ')
            )
            error_message <- append(error_message, tmp_message)
        }
    }
    return(error_message)
}

# Checks whether the elements of the `args_to_check` list are vectors or lists
# of strings. If all elements meet this criterion, this function returns an
# empty string. Otherwise, it returns an informative error message. For a truly
# helpful message, the elements of `args_to_check` should be named.
check_strings <- function(args_to_check) {
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- unlist(args_to_check[[i]])
        if (length(arg) > 0 & !is.character(arg)) {
            error_message <- append(
                error_message,
                sprintf('"%s" must be a vector or list of strings', names(args_to_check)[i])
            )
        }
    }
    return(error_message)
}

# Checks whether an `integrator` list is properly defined. If it is, this
# function returns an empty string. Otherwise, it returns an informative error
# message. For a truly helpful message, the elements of `args_to_check` should
# be named.
check_integrator <- function(integrator) {
    error_message <- character()
    if (!is.list(integrator)) {
        error_message <- append(
            error_message,
            "'integrator' must be a list.\n"
        )
    } else {
        integrator_type <- integrator[['type']]
        if (!is.character(integrator_type) & length(integrator_type) != 1) {
            error_messages <- append(
                error_messages,
                'the "type" element of the integrator specification list must be a string.\n'
            )
        }
    }
    return(error_message)
}

# Checks whether a set of inputs to the `Gro_deriv` function are properly
# defined. If the inputs are properly defined, this function returns an empty
# string. Otherwise, it returns an informative error message.
check_Gro_deriv_inputs <- function(
    initial_values,
    parameters,
    drivers,
    steady_state_module_names,
    derivative_module_names
)
{
    error_message <- character()

    # The initial_values, parameters, and drivers should all be lists
    error_message <- append(
        error_message,
        check_list(
            list(
                initial_values=initial_values,
                parameters=parameters,
                drivers=drivers
            )
        )
    )

    # The elements of initial_values and parameters should all have length 1
    error_message <- append(
        error_message,
        check_element_length(
            list(
                initial_values=initial_values,
                parameters=parameters
            )
        )
    )

    # The steady_state_module_names and derivative_module_names should all be
    # vectors or lists of strings
    error_message <- append(
        error_message,
        check_element_length(
            list(
                initial_values=initial_values,
                parameters=parameters
            )
        )
    )

    return(error_message)
}

# Checks whether a set of inputs to the `run_biocro` function are properly
# defined. If the inputs are properly defined, this function returns an empty
# string. Otherwise, it returns an informative error message.
check_run_biocro_inputs <- function(
    initial_values,
    parameters,
    drivers,
    steady_state_module_names,
    derivative_module_names,
    integrator
)
{
    error_message <- character()

    # Most of the inputs have the same requirements as Gro_deriv
    error_message <- append(
        error_message,
        check_Gro_deriv_inputs(
            initial_values,
            parameters,
            drivers,
            steady_state_module_names,
            derivative_module_names
        )
    )

    # Check the integrator
    error_message <- append(
        error_message,
        check_integrator(integrator)
    )

    return(error_message)
}
