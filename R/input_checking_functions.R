# Checks whether `args_to_check` has names. The other checking functions require
# names to give useful error messages.
check_names <- function(args_to_check) {
    if(is.null(names(args_to_check))) {
        stop("`args_to_check` must have names")
    }
}

# Sends the error messages to the user in the proper format
send_error_messages <- function(error_messages) {
    if (length(error_messages) > 0) {
        stop(paste(error_messages, collapse='  '))
    }
}

# Checks whether the elements of the `args_to_check` list are lists. If all
# elements meet this criterion, this functions returns an empty string.
# Otherwise, it returns an informative error message.
check_list <- function(args_to_check) {
    check_names(args_to_check)
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- args_to_check[[i]]
        if (!is.list(arg)) {
            error_message <- append(
                error_message,
                sprintf('`%s` must be a list.\n', names(args_to_check)[i])
            )
        }
    }
    return(error_message)
}

# Checks whether the elements of the `args_to_check` list are data frames. If
# all elements meet this criterion, this functions returns an empty string.
# Otherwise, it returns an informative error message.
check_data_frame <- function(args_to_check) {
    check_names(args_to_check)
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- args_to_check[[i]]
        if (!is.data.frame(arg)) {
            error_message <- append(
                error_message,
                sprintf('`%s` must be a data frame.\n', names(args_to_check)[i])
            )
        }
    }
    return(error_message)
}

# Checks whether the elements of the `args_to_check` list are lists of elements
# that each have length 1. If all elements meet this criterion, this function
# returns an empty string. Otherwise, it returns an informative error message.
check_element_length <- function(args_to_check) {
    check_names(args_to_check)
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- args_to_check[[i]]
        item_lengths <- sapply(arg, length)
        if (any(item_lengths != 1)) {
            tmp_message <- sprintf(
                "The following `%s` members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n",
                names(args_to_check)[i],
                paste(names(item_lengths)[which(item_lengths != 1)], collapse=', ')
            )
            error_message <- append(error_message, tmp_message)
        }
    }
    return(error_message)
}

# Checks whether the elements of the `args_to_check` list are vectors, lists, or
# data frames of numeric elements. (NA values are also acceptable here.) If all
# elements meet this criterion, this function returns an empty string.
# Otherwise, it returns an informative error message.
check_numeric <- function(args_to_check) {
    check_names(args_to_check)
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- args_to_check[[i]]
        is_numeric <- sapply(arg, function(x) {is.numeric(x) || all(is.na(x))})
        if (!all(is_numeric)) {
            tmp_message <- sprintf(
                "The following `%s` members are not numeric or NA, but all members must be numeric or NA: %s.\n",
                names(args_to_check)[i],
                paste(names(is_numeric)[which(!is_numeric)], collapse=', ')
            )
            error_message <- append(error_message, tmp_message)
        }
    }
    return(error_message)
}

# Checks whether the elements of the `args_to_check` list are vectors or lists
# of strings. If all elements meet this criterion, this function returns an
# empty string. Otherwise, it returns an informative error message.
check_strings <- function(args_to_check) {
    check_names(args_to_check)
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- args_to_check[[i]]
        is_character <- sapply(arg, is.character)
        if (!all(is_character)) {
            tmp_message <- sprintf(
                "The following `%s` members are not strings, but all members must be strings: %s.\n",
                names(args_to_check)[i],
                paste(arg[which(!is_character)], collapse=', ')
            )
            error_message <- append(error_message, tmp_message)
        }
    }
    return(error_message)
}

# Checks whether an `ode_solver` list is properly defined. If it is, this
# function returns an empty string. Otherwise, it returns an informative error
# message.
check_ode_solver <- function(ode_solver) {
    error_message <- character()

    # Should be a list
    error_message <- append(
        error_message,
        check_list(list(ode_solver=ode_solver))
    )

    # All elements should have length of 1
    error_message <- append(
        error_message,
        check_element_length(list(ode_solver=ode_solver))
    )

    # The type must be a string
    error_message <- append(
        error_message,
        check_strings(list(ode_solver_type=ode_solver['type']))
    )

    # All other elements must be numeric
    error_message <- append(
        error_message,
        check_numeric(list(ode_solver_other_than_type=ode_solver[!(names(ode_solver) == 'type')]))
    )

    return(error_message)
}

# Checks whether a set of inputs to the `system_derivatives` function are
# properly defined. If the inputs are properly defined, this function returns an
# empty string. Otherwise, it returns an informative error message.
check_system_derivatives_inputs <- function(
    initial_values,
    parameters,
    drivers,
    direct_module_names,
    differential_module_names
)
{
    error_message <- character()

    # The initial_values and parameters should be lists
    error_message <- append(
        error_message,
        check_list(
            list(
                initial_values=initial_values,
                parameters=parameters
            )
        )
    )

    # The drivers should be a data frame
    error_message <- append(
        error_message,
        check_data_frame(list(drivers=drivers))
    )

    # The elements of initial_values, parameters, direct_module_names, and
    # differential_module_names should all have length 1
    error_message <- append(
        error_message,
        check_element_length(
            list(
                initial_values=initial_values,
                parameters=parameters,
                direct_module_names=direct_module_names,
                differential_module_names=differential_module_names
            )
        )
    )

    # The initial_values, parameters, and drivers should all have numeric values
    error_message <- append(
        error_message,
        check_numeric(
            list(
                initial_values=initial_values,
                parameters=parameters,
                drivers=drivers
            )
        )
    )

    # The direct_module_names and differential_module_names should all be
    # vectors or lists of strings
    error_message <- append(
        error_message,
        check_strings(
            list(
                direct_module_names=direct_module_names,
                differential_module_names=differential_module_names
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
    direct_module_names,
    differential_module_names,
    ode_solver
)
{
    error_message <- character()

    # Most of the inputs have the same requirements as `system_derivatives`
    error_message <- append(
        error_message,
        check_system_derivatives_inputs(
            initial_values,
            parameters,
            drivers,
            direct_module_names,
            differential_module_names
        )
    )

    # Check the ode_solver
    error_message <- append(
        error_message,
        check_ode_solver(ode_solver)
    )

    return(error_message)
}
