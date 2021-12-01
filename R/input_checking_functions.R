# Checks whether `args_to_check` has names. The other checking functions require
# names to give useful error messages.
check_names <- function(args_to_check, arg_name = "args_to_check") {
    if(is.null(names(args_to_check))) {
        stop(paste0("`", arg_name, "` must have names"))
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
# of strings. If all elements meet this criterion, this function returns an
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
