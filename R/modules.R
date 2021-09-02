module_info <- function(module_name, verbose = TRUE)
{
    # Example: getting information about the thermal_time_linear module
    #
    #  info <- module_info("thermal_time_linear")
    #  inputs <- info[['inputs']]
    #
    # The function returns a list of several named elements containing info
    # about the module:
    #
    # - module_name: the module's name
    #
    # - inputs: a list of named numeric elements representing the module's
    #           inputs
    #
    # - outputs: a list of named numeric elements representing the module's
    #            outputs
    #
    # - type: the module's type (either `differential` or `direct`)
    # - euler_requirement: indicates whether the module requires a fixed-step
    #                      Euler ODE solver when used in a BioCro simulation
    #
    # - creation_error_message: describes any errors that occurred while
    #                           creating an instance of the module
    #
    # If the `verbose` input argument is TRUE, this information will also be
    # printed to the R console

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
    # Example: testing the output of the thermal_time_linear module
    #
    #  info <- module_info("thermal_time_linear")
    #  inputs <- info[['inputs']]
    #  <<modify individual input parameters to desired values>>
    #  outputs <- evaluate_module("big_leaf_multilayer_canopy", inputs, TRUE)
    #  <<check the values of the output parameters to confirm they are correct>>

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

    send_error_messages(error_messages)

    # C++ requires that all the variables have type `double`
    input_quantities <- lapply(input_quantities, as.numeric)

    result <- .Call(R_evaluate_module, module_name, input_quantities)
    result <- result[order(names(result))]
    return(result)
}
