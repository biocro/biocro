module_info <- function(module_name, verbose = TRUE)
{
    error_messages <- check_element_length(list(module_name = module_name))

    error_messages <- append(
        error_messages,
        check_strings(list(module_name = module_name))
    )

    if (length(module_name) != 1) {
        error_messages <- append(error_messages, "`module_name` must have length 1")
    }

    send_error_messages(error_messages)

    # Make sure verbose is a logical variable
    verbose <- lapply(verbose, as.logical)

    # Get the info list
    result <- .Call(R_module_info, module_name, verbose)

    return(result)
}

evaluate_module <- function(module_name, input_quantities)
{
    error_messages <- check_list(list(input_quantities = input_quantities))

    error_messages <- append(
        error_messages,
        check_element_length(list(
            module_name = module_name,
            input_quantities = input_quantities
        ))
    )

    error_messages <- append(
        error_messages,
        check_strings(list(module_name = module_name))
    )

    error_messages <- append(
        error_messages,
        check_numeric(list(input_quantities = input_quantities))
    )

    if (length(module_name) != 1) {
        error_messages <- append(error_messages, "`module_name` must have length 1")
    }

    missing_input_quantities <- setdiff(
        names(module_info(module_name, verbose = FALSE)[['inputs']]),
        names(input_quantities)
    )

    if (length(missing_input_quantities) > 0) {
        error_messages <- append(
            error_messages,
            paste0(
                "The `", module_name, "` module requires `",
                missing_input_quantities, "` as an input quantity\n"
            )
        )
    }

    send_error_messages(error_messages)

    # C++ requires that all the variables have type `double`
    input_quantities <- lapply(input_quantities, as.numeric)

    result <- .Call(R_evaluate_module, module_name, input_quantities)
    result <- result[order(names(result))]
    return(result)
}
