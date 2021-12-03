module_info <- function(module_name, verbose = TRUE)
{
    # Check that the following type conditions are met:
    # - `module_name` should be a character vector of length 1
    # - `verbose` should be a boolean vector of length 1
    error_messages <- check_vector(list(
        module_name = module_name,
        verbose = verbose
    ))

    error_messages <- append(
        error_messages,
        check_strings(list(module_name = module_name))
    )

    error_messages <- append(
        error_messages,
        check_boolean(list(verbose = verbose))
    )

    error_messages <- append(
        error_messages,
        check_length(list(
            module_name = module_name,
            verbose = verbose
        ))
    )

    send_error_messages(error_messages)

    # Make sure verbose is a logical variable
    verbose <- lapply(verbose, as.logical)

    # Get the info list
    result <- .Call(R_module_info, module_name, verbose)

    return(invisible(result))
}

check_module_input_quantities <- function(module_name, input_quantities) {
    # Check that the following type conditions are met:
    # - `module_name` should be a character vector of length 1
    # - `input_quantities` should be a list of named numeric elements, each of
    #    which has length 1
    error_messages <- check_vector(list(module_name = module_name))

    error_messages <- append(
        error_messages,
        check_strings(list(module_name = module_name))
    )

    error_messages <- append(
        error_messages,
        check_length(list(module_name = module_name))
    )

    error_messages <- append(
        error_messages,
        check_list(list(input_quantities = input_quantities))
    )

    error_messages <- append(
        error_messages,
        check_names(list(input_quantities = input_quantities))
    )

    error_messages <- append(
        error_messages,
        check_numeric(list(input_quantities = input_quantities))
    )

    error_messages <- append(
        error_messages,
        check_element_length(list(input_quantities = input_quantities))
    )

    # Check for any missing module input quantities
    missing_input_quantities <- setdiff(
        module_info(module_name, verbose = FALSE)[['inputs']],
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

    return(error_messages)
}

evaluate_module <- function(module_name, input_quantities)
{
    # Check that the following type conditions are met:
    # - `module_name` should be a character vector of length 1
    # - `input_quantities` should be a list of named numeric elements, each of
    #    which has length 1
    error_messages <- check_vector(list(module_name = module_name))

    error_messages <- append(
        error_messages,
        check_strings(list(module_name = module_name))
    )

    error_messages <- append(
        error_messages,
        check_length(list(module_name = module_name))
    )

    error_messages <- append(
        error_messages,
        check_list(list(input_quantities = input_quantities))
    )

    error_messages <- append(
        error_messages,
        check_names(list(input_quantities = input_quantities))
    )

    error_messages <- append(
        error_messages,
        check_numeric(list(input_quantities = input_quantities))
    )

    error_messages <- append(
        error_messages,
        check_element_length(list(input_quantities = input_quantities))
    )

    # Check to make sure the required input quantities were supplied
    error_messages <- append(
        error_messages,
        check_module_input_quantities(module_name, input_quantities)
    )

    send_error_messages(error_messages)

    # C++ requires that all the variables have type `double`
    input_quantities <- lapply(input_quantities, as.numeric)

    result <- .Call(R_evaluate_module, module_name, input_quantities)
    result <- result[order(names(result))]
    return(result)
}

partial_evaluate_module <- function(module_name, input_quantities, arg_names) {
    # Check that the following type conditions are met:
    # - `module_name` should be a character vector of length 1
    # - `input_quantities` should be a list of named numeric elements, each of
    #    which has length 1
    # - `arg_names` should be a character vector
    error_messages <- check_vector(list(
        module_name = module_name,
        arg_names = arg_names
    ))

    error_messages <- append(
        error_messages,
        check_strings(list(
            module_name = module_name,
            arg_names = arg_names
        ))
    )

    error_messages <- append(
        error_messages,
        check_length(list(module_name = module_name))
    )

    error_messages <- append(
        error_messages,
        check_list(list(input_quantities = input_quantities))
    )

    error_messages <- append(
        error_messages,
        check_names(list(input_quantities = input_quantities))
    )

    error_messages <- append(
        error_messages,
        check_numeric(list(input_quantities = input_quantities))
    )

    error_messages <- append(
        error_messages,
        check_element_length(list(input_quantities = input_quantities))
    )

    # Check to make sure the quantities specified in `arg_names` are actually
    # required by the module
    info <- module_info(module_name, verbose = FALSE)
    extraneous_args <- arg_names[!arg_names %in% info$inputs]

    if (length(extraneous_args) > 0) {
        error_messages <- append(
            error_messages,
            paste0(
                "`", extraneous_args, "` was provided in `arg_names`, but ",
                "the `", module_name, "` module does not require this ",
                "quantity\n"
            )
        )
    }

    # Check to make sure the required input quantities were supplied (or will
    # be supplied later, in the case of quantities from `arg_names`)
    for (name in arg_names) {
        input_quantities[[name]] <- NA
    }

    error_messages <- append(
        error_messages,
        check_module_input_quantities(module_name, input_quantities)
    )

    send_error_messages(error_messages)

    # Truncate the `input_quantities` list to only include quantities that are
    # actually required by the module
    input_quantities <- input_quantities[info$inputs]

    # Make a function that calls `evaluate_module` with new values for the
    # quantities specified in `arg_names`, returning the inputs and outputs as
    # named elements of a list
    function(x) {
        x = unlist(x)
        if (length(x) != length(arg_names)) {
            stop("The `x` argument does not have the correct number of elements")
        }
        temp_input_quantities = input_quantities
        for (i in seq_along(x)) {
            temp_input_quantities[[arg_names[i]]] <- x[i]
        }
        output_quantities <- evaluate_module(module_name, temp_input_quantities)
        list(inputs = temp_input_quantities, outputs = output_quantities)
    }
}

module_response_curve <- function(
    module_name,
    input_quantities,
    continuous_variable,
    discrete_variable = list()
)
{
    # Make sure the module is suitable for `module_response_curve`, which can
    # only be used when the module's inputs and outputs are distinct
    error_messages <- character()
    info <- module_info(module_name, verbose = FALSE)
    overlapping_io <- intersect(info$inputs, info$outputs)
    if (length(overlapping_io) > 0) {
        error_messages <- append(
            error_messages,
            paste0(
                "The `", module_name, "` module has at least one quantity ",
                "that is both an input and an output, so ",
                "`module_response_curve` cannot be used"
            )
        )
    }

    # Check to see if a discrete variable was supplied
    discrete_supplied <- !identical(discrete_variable, list())

    # Check that the variables are lists of one named numeric element (but
    # discrete variable can be empty). Other error checks will be performed by
    # `partial_evaluate_module`.
    check_names(continuous_variable)

    error_messages <- append(
        error_messages,
            check_list(list(
            continuous_variable = continuous_variable,
            discrete_variable = discrete_variable
        ))
    )

    error_messages <- append(
        error_messages,
        check_numeric(list(
            continuous_variable = continuous_variable,
            discrete_variable = discrete_variable
        ))
    )

    error_messages <- append(
        error_messages,
        check_length(list(continuous_variable = continuous_variable))
    )

    if (discrete_supplied) {
        check_names(discrete_variable)
        error_messages <- append(
            error_messages,
            check_length(list(discrete_variable = discrete_variable))
        )
    }

    send_error_messages(error_messages)

    # Create a vector of `arg_names` and a corresponding data frame of values
    arg_names <- character(0)
    arg_values <- data.frame()

    if (discrete_supplied) {
        arg_names <- c(names(continuous_variable), names(discrete_variable))
        arg_values <- setNames(
            data.frame(
                rep.int(unlist(continuous_variable), length(discrete_variable[[1]])),
                rep(unlist(discrete_variable), each = length(continuous_variable[[1]])),
                row.names = NULL
            ),
            NULL
        )
    } else {
        arg_names <- names(continuous_variable)
        arg_values <- setNames(
            data.frame(unlist(continuous_variable), row.names = NULL),
            NULL
        )
    }

    # Use `partial_evaluate_module` to create a function that calls the module
    # with the appropriate inputs
    evaluation_function <-
        partial_evaluate_module(module_name, input_quantities, arg_names)

    # Apply the function to each row in the data frame of inputs, producing a
    # list of data frames that each have 1 row and whose columns represent the
    # module's input and output quantities
    df_list <- apply(arg_values, 1, function(x) {
        result <- evaluation_function(x)
        as.data.frame(c(result$inputs, result$outputs))
    })

    # Combine the data frames into one data frame, add the module name as the
    # first column, and return it
    cbind(module_name, do.call(rbind, df_list))
}

quantity_list_from_names <- function(quantity_names)
{
    error_messages <- check_strings(list(quantity_names = quantity_names))

    send_error_messages(error_messages)

    setNames(as.list(rep_len(1, length(quantity_names))), quantity_names)
}
