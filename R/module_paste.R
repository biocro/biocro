module_paste <- function(lib_name, module_names) {
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
            module_names = module_names
        ))
    )

    error_messages <- append(
        error_messages,
        check_strings(list(
            lib_name = lib_name,
            module_names = module_names
        ))
    )

    send_error_messages(error_messages)

    full_module_names <-
        setNames(paste0(lib_name, ':', module_names), names(module_names))

    if (is.list(module_names)) {
        return(as.list(full_module_names))
    } else {
        return(full_module_names)
    }
}
