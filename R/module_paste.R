module_paste <- function(lib_name, local_module_names) {
    error_messages <-
        check_vector(list(lib_name = lib_name))

    error_messages <- append(
        error_messages,
        check_length(list(lib_name = lib_name))
    )

    error_messages <- append(
        error_messages,
        check_element_length(list(
            lib_name = lib_name,
            local_module_names = local_module_names
        ))
    )

    error_messages <- append(
        error_messages,
        check_strings(list(
            lib_name = lib_name,
            local_module_names = local_module_names
        ))
    )

    stop_and_send_error_messages(error_messages)

    full_module_names <- stats::setNames(
        paste0(lib_name, ':', local_module_names),
        names(local_module_names)
    )

    if (is.list(local_module_names)) {
        return(as.list(full_module_names))
    } else {
        return(full_module_names)
    }
}
