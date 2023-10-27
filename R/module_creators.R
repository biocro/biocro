module_creators <- function(module_names)
{
    # Check that the following type conditions are met:
    # - `module_names` should be a vector or list of strings with elements of
    #    length 1
    error_messages <- check_strings(list(module_names = module_names))

    error_messages <- append(
        error_messages,
        check_element_length(list(module_names = module_names))
    )

    send_error_messages(error_messages)

    # Make sure the module names are a vector
    module_names <- unlist(module_names)

    # Get the module creators, retaining any element names
    return(stats::setNames(
        .Call(R_module_creators, module_names),
        names(module_names)
    ))
}
