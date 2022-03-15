# A helping function for "checking out" a module from a module library. Here,
# `module_specification` should be a string formatted like
# "module_library_name:module_name". Example: "BioCro:c3_canopy".
check_out_module <- function(module_specification) {
    error_messages <-
        check_vector(list(module_specification = module_specification))

    error_messages <- append(
        error_messages,
        check_length(list(module_specification = module_specification))
    )

    error_messages <- append(
        error_messages,
        check_element_length(list(module_specification = module_specification))
    )

    error_messages <- append(
        error_messages,
        check_strings(list(module_specification = module_specification))
    )

    send_error_messages(error_messages)

    # Try to extract the module library name and the module name from the
    # specification string
    parsed_string <- strsplit(module_specification, ':', fixed = TRUE)[[1]]

    if (length(parsed_string) != 2) {
        stop(paste0(
            "The module specification string `",
            module_specification,
            "` is not formatted as `module_library_name:module_name`"
        ))
    }

    library_name <- parsed_string[1]
    module_name <- parsed_string[2]

    # Try to find the module library function
    library_func <- tryCatch(
        {
            function_from_package(library_name, 'module_creators')
        },
        error = function(cond) {
            stop(paste0(
                "Encountered an issue with module specification `",
                module_specification,
                "`: ",
                cond
            ))
        }
    )

    return(library_func(module_name)[[1]])
}

# TO-DO: should the reported module name be a full module specification instead?
module_info <- function(module_specification, verbose = TRUE)
{
    # Check that the following type conditions are met:
    # - `module_specification` should be a string vector of length 1
    # - `verbose` should be a boolean vector of length 1
    error_messages <- check_vector(list(
        verbose = verbose,
        module_specification = module_specification
    ))

    error_messages <- append(
        error_messages,
        check_strings(list(module_specification = module_specification))
    )

    error_messages <- append(
        error_messages,
        check_boolean(list(verbose = verbose))
    )

    error_messages <- append(
        error_messages,
        check_length(list(
            module_specification = module_specification,
            verbose = verbose
        ))
    )

    send_error_messages(error_messages)

    # Check out the module
    module_creator <- lapply(module_specification, check_out_module)

    # Make sure verbose is a logical variable
    verbose <- lapply(verbose, as.logical)

    # Get the info list
    result <- .Call(R_module_info, module_creator, verbose)

    return(invisible(result))
}

check_module_input_quantities <- function(
    module_specification,
    input_quantities
)
{
    # Check that the following type conditions are met:
    # - `input_quantities` should be a list of named numeric elements, each of
    #    which has length 1
    # Type checks for `module_specification` will be made by the `module_info`
    # function
    error_messages <- check_list(list(input_quantities = input_quantities))

    error_messages <- append(
        error_messages,
        check_element_names(list(input_quantities = input_quantities))
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
    info <- module_info(module_specification, verbose = FALSE)
    missing_input_quantities <- setdiff(
        info[['inputs']],
        names(input_quantities)
    )

    if (length(missing_input_quantities) > 0) {
        error_messages <- append(
            error_messages,
            paste0(
                "The `", module_specification, "` module requires `",
                missing_input_quantities, "` as an input quantity\n"
            )
        )
    }

    return(error_messages)
}

evaluate_module <- function(module_specification, input_quantities)
{
    # Type checks for `module_specification` and `input_quantities` will be
    # performed by the `check_module_input_quantities` function

    # Check to make sure the required input quantities were supplied
    error_messages <-
        check_module_input_quantities(module_specification, input_quantities)

    send_error_messages(error_messages)

    # Check out the module
    module_creator <- lapply(module_specification, check_out_module)

    # C++ requires that all the variables have type `double`
    input_quantities <- lapply(input_quantities, as.numeric)

    result <- .Call(R_evaluate_module, module_creator, input_quantities)
    result <- result[order(names(result))]
    return(result)
}

partial_evaluate_module <- function(
    module_specification,
    input_quantities,
    arg_names
)
{
    # Check that the following type conditions are met:
    # - `arg_names` should be a character vector
    # Type checks for `module_specification` and `input_quantities` will be
    # performed by the `module_info` and `check_module_input_quantities`
    # functions
    error_messages <- check_vector(list(arg_names = arg_names))

    error_messages <- append(
        error_messages,
        check_strings(list(arg_names = arg_names))
    )

    # Check to make sure the quantities specified in `arg_names` are actually
    # required by the module
    info <- module_info(module_specification, verbose = FALSE)
    extraneous_args <- arg_names[!arg_names %in% info[['inputs']]]

    if (length(extraneous_args) > 0) {
        error_messages <- append(
            error_messages,
            paste0(
                "`", extraneous_args, "` was provided in `arg_names`, but ",
                "the `", module_specification,
                "` module does not require this quantity\n"
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
        check_module_input_quantities(module_specification, input_quantities)
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
        output_quantities <-
            evaluate_module(module_specification, temp_input_quantities)

        list(inputs = temp_input_quantities, outputs = output_quantities)
    }
}

module_response_curve <- function(
    module_specification,
    fixed_quantities,
    varying_quantities
)
{
    # Check that the following type conditions are met:
    # - `varying_quantities` should be a data frame of numeric elements with
    #    named columns
    # Type checks for `module_specification` and `fixed_quantities` will be
    # performed by the `module_info` and `partial_evaluate_module` functions
    error_messages <-
        check_data_frame(list(varying_quantities = varying_quantities))

    error_messages <- append(
        error_messages,
        check_numeric(list(varying_quantities = varying_quantities))
    )

    error_messages <- append(
        error_messages,
        check_element_names(list(varying_quantities = varying_quantities))
    )

    send_error_messages(error_messages)

    # Use `partial_evaluate_module` to create a function that calls the module
    # with the appropriate inputs
    evaluation_function <- partial_evaluate_module(
        module_specification,
        fixed_quantities,
        names(varying_quantities)
    )

    # Apply the function to each row in the data frame of inputs, producing a
    # list of data frames that each have 1 row and whose columns represent the
    # module's input and output quantities
    df_list <- apply(varying_quantities, 1, function(x) {
        result <- evaluation_function(x)
        as.data.frame(c(result$inputs, result$outputs), row.names = NULL)
    })

    # Combine the data frames into one data frame, add the module name as the
    # first column, and return it
    cbind(module_specification = module_specification, do.call(rbind, df_list))
}

quantity_list_from_names <- function(quantity_names)
{
    # Check that the following type conditions are met:
    # - `quantity_names` should be a character vector
    error_messages <- check_vector(list(quantity_names = quantity_names))

    error_messages <- append(
        error_messages,
        check_strings(list(quantity_names = quantity_names))
    )

    send_error_messages(error_messages)

    stats::setNames(as.list(rep_len(1, length(quantity_names))), quantity_names)
}
