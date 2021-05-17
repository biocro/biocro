module_info <- function(module_name, verbose = TRUE)
{
    # Example: getting information about the thermal_time_linear module
    #
    #  info <- module_info("thermal_time_linear")
    #  inputs <- info[['inputs']]
    #
    # The function returns a list with several named elements containing information
    # about the module:
    #
    # - module_name: the module's name
    #
    # - inputs: a list with named numeric elements representing the module's
    #           inputs
    #
    # - outputs: a list with named numeric elements representing the module's
    #            outputs
    #
    # - type: the module's type (either `derivative` or `steady state`)
    # - euler_requirement: indicates whether the module requires a fixed-step
    #                      Euler integrator when used in a BioCro simulation
    #
    # - creation_error_message: describes any errors that occurred while
    #                           creating an instance of the module
    #
    # If the `verbose` input argument is TRUE, this information will also be
    # printed to the R console

    # Make sure the module name is a string
    if (!is.character(module_name) & length(module_name) != 1) {
        stop('"module_name" must be a string')
    }

    # Make sure verbose is a logical variable
    verbose = lapply(verbose, as.logical)

    result = .Call(R_module_info, module_name, verbose)
    return(result)
}

evaluate_module <- function(module_name, input_parameters)
{
    # Example: testing the output of the thermal_time_linear module
    #
    #  info <- module_info("thermal_time_linear")
    #  inputs <- info[['inputs']]
    #  <<modify individual input parameters to desired values>>
    #  outputs <- evaluate_module("big_leaf_multilayer_canopy", inputs, TRUE)
    #  <<check the values of the output parameters to confirm they are correct>>

    if (!is.character(module_name) & length(module_name) != 1) {
        stop('"module_name" must be a string')
    }

    if(!is.list(input_parameters)) {
        stop('"input_parameters" must be a list')
    }

    if(length(input_parameters) != length(unlist(input_parameters))) {
        item_lengths = unlist(lapply(input_parameters, length))
        error_message = sprintf("The following parameters have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
        stop(error_message)
    }

    input_parameters = lapply(input_parameters, as.numeric)

    result = .Call(R_evaluate_module, module_name, input_parameters)
    result = result[order(names(result))]
    return(result)
}
