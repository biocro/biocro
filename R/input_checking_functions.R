# Checks whether `args_to_check` has names. The other checking functions require
# names to give useful error messages.
check_names <- function(args_to_check) {
    if(is.null(names(args_to_check))) {
        stop(paste0("`", substitute(args_to_check), "` must have names"))
    }
}

# Sends the error messages to the user in the proper format
send_error_messages <- function(error_messages) {
    if (length(error_messages) > 0) {
        stop(paste(error_messages, collapse='  '))
    }
}

# Checks whether the non-empty elements of the `args_to_check` list have names.
# If all elements meet this criterion, this function returns an empty string.
# Otherwise, it returns an informative error message.
check_element_names <- function(args_to_check) {
    check_names(args_to_check)
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- args_to_check[[i]]
        if (length(arg) > 1 && is.null(names(arg))) {
            error_message <- append(
                error_message,
                sprintf('`%s` must have names.\n', names(args_to_check)[i])
            )
        }
    }
    return(error_message)
}

# Checks whether the elements of the `args_to_check` list have distinct names
# for each of their elements; any such elements without names will be ignored
# during this check. If all elements meet this criterion, this function returns
# an empty string. Otherwise, it returns an informative error message. Note: if
# an element of `args_to_check` has no names, no duplicates will be detected,
# and no error message will be produced.
check_distinct_names <- function(args_to_check) {
    check_names(args_to_check)
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- args_to_check[[i]]

        # Find any duplicated names and their associated values
        arg_names <- names(arg)
        dup <- duplicated(arg_names, incomparables = '')
        dup_names <- unique(arg_names[dup])
        dup_values <- lapply(dup_names, function(n) {arg[arg_names == n]})

        if (length(dup_values) > 0) {
            # Indicate that some duplicated names were detected
            error_message <- append(
                error_message,
                sprintf(
                    '`%s` contains multiple instances of some quantities:\n',
                    names(args_to_check)[i]
                )
            )

            for (j in seq_along(dup_values)) {
                # Report one of the duplicated names
                error_message <- append(
                    error_message,
                    sprintf(
                        '  `%s` takes the following values:\n',
                        dup_names[j]
                    )
                )

                vals <- dup_values[[j]]
                for (k in seq_along(vals)) {
                    # Report one of the values associated with this name
                    val <- vals[[k]]

                    max_n <- 5
                    msg <- if (length(val) > max_n) {
                        paste0(
                            "    ",
                            paste(val[seq_len(max_n)], collapse = ", "),
                            ", ...\n"
                        )
                    } else {
                        paste0(
                            "    ",
                            paste(val, collapse = ", "),
                            "\n"
                        )
                    }
                    error_message <- append(error_message, msg)
                }
            }
        }
    }
    return(error_message)
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

# Checks whether the elements of the `args_to_check` are vectors. If all
# elements meet this criterion, this function returns an empty string.
# Otherwise, it returns an informative error message.
check_vector <- function(args_to_check) {
    check_names(args_to_check)
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- args_to_check[[i]]
        if (length(arg) > 0 && (!is.vector(arg) || 'list' %in% class(arg))) {
            error_message <- append(
                error_message,
                sprintf('`%s` must be a vector.\n', names(args_to_check)[i])
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
                "The following `%s` members have lengths other than 1, but all members must have a length of exactly 1: %s.\n",
                names(args_to_check)[i],
                paste(names(item_lengths)[which(item_lengths != 1)], collapse=', ')
            )
            error_message <- append(error_message, tmp_message)
        }
    }
    return(error_message)
}

# Checks whether the elements of the `args_to_check` list each have length 1. If
# all elements meet this criterion, this function returns an empty string.
# Otherwise, it returns an informative error message.
check_length <- function(args_to_check) {
    check_names(args_to_check)
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        if (length(args_to_check[[i]]) != 1) {
            error_message <- append(
                error_message,
                sprintf('`%s` must have length 1.\n', names(args_to_check)[i])
            )
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

# Checks whether the elements of the `args_to_check` list are vectors or lists
# of `externalptr` objects. If all elements meet this criterion, this function
# returns an empty string. Otherwise, it returns an informative error message.
check_pointers <- function(args_to_check) {
    check_names(args_to_check)
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- args_to_check[[i]]
        is_pointer <- sapply(arg, class) == 'externalptr'
        if (!all(is_pointer)) {
            tmp_message <- sprintf(
                "The following `%s` members are not externalptrs, but all members must be externalptrs: %s.\n",
                names(args_to_check)[i],
                paste(arg[which(!is_pointer)], collapse=', ')
            )
            error_message <- append(error_message, tmp_message)
        }
    }
    return(error_message)
}

# Checks whether the elements of the `args_to_check` list are vectors or lists
# of booleans. If all elements meet this criterion, this function returns an
# empty string. Otherwise, it returns an informative error message.
check_boolean <- function(args_to_check) {
    check_names(args_to_check)
    error_message <- character()
    for (i in seq_along(args_to_check)) {
        arg <- args_to_check[[i]]
        is_boolean <- sapply(arg, is.logical)
        if (!all(is_boolean)) {
            tmp_message <- sprintf(
                "The following `%s` members are not booleans, but all members must be booleans: %s.\n",
                names(args_to_check)[i],
                paste(arg[which(!is_boolean)], collapse=', ')
            )
            error_message <- append(error_message, tmp_message)
        }
    }
    return(error_message)
}


# Checks whether the `time` driver has
check_time_is_correct_format <- function(drivers, parameters, rtol = sqrt(.Machine$double.eps)){
    drivers_contains_time = 'time' %in% names(drivers)
    if(!drivers_contains_time){
        error_message <- "No `time` variable found in the `drivers` dataframe. \n
        The `drivers` must contain a `time` column, and the `time` variable must\n
        be a sequence of integer multiples of the `timestep`.\n
            It must satisfy: time[n] = time[1] + timestep * (n-1)"
        return(error_message)
    }
    time <- drivers[['time']]
    timestep <- parameters[['timestep']]

    delta_time = diff(time)
    dt = diff(time) - timestep
    is_zero = abs(dt) < rtol
    not_all_zero = !all(is_zero)
    if (not_all_zero) {
        error_message <- "
        The `time` variable is not a sequence of integer multiples of the `timestep`.\n
        `time` must satisfy: time[n] = time[1] + timestep * (n-1)"

        return(error_message)
    }

    error_message <- character()
    return(error_message)

}
