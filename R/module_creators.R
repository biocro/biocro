module_creators <- function(module_names)
{
    # Check that the following type conditions are met:
    # - `module_names` should be a string vector
    error_messages <- check_vector(list(module_names = module_names))

    error_messages <- append(
        error_messages,
        check_strings(list(module_names = module_names))
    )

    send_error_messages(error_messages)

    return(.Call(R_module_creators, module_names))
}
