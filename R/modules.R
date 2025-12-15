# A helper function for extracting the library name and local module name
parse_module_name <- function(module_name) {
  parsed_string <- strsplit(module_name, ':', fixed = TRUE)[[1]]

  if (length(parsed_string) != 2) {
    stop(
      "The module name string `",
      module_name,
      "` is not formatted as `module_library_name:module_name`"
    )
  }

  list(library_name = parsed_string[1], local_module_name = parsed_string[2])
}

# A helper function for comparing framework versions
compare_framework_versions <- function(library_name, module_fv) {
    # Get the BioCro framework version as a numeric_version object
    biocro_fv <- numeric_version(framework_version())

    # Convert the module library framework version to a numeric_version object
    module_fv <- numeric_version(module_fv)

    # Send a warning if there is a version mismatch
    if (module_fv > biocro_fv) {
        # Here, the module library is using a newer version of the BioCro C++
        # framework
        warning(
            "The `", library_name, "` module library R package uses a newer ",
            "version of the BioCro C++ framework than the `BioCro` R package (",
            module_fv, " vs. ", biocro_fv, ").\nTry ",
            "updating the `BioCro` R package to the latest ",
            "version; if that does not solve the problem, contact the ",
            "`BioCro` R package maintainer."
        )
    } else if (module_fv < biocro_fv) {
        # Here, the module library is using an older version of the BioCro C++
        # framework
        warning(
            "The `", library_name, "` module library R package uses an older ",
            "version of the BioCro C++ framework than the `BioCro` R package (",
            module_fv, " vs. ", biocro_fv, ").\nTry ",
            "updating the `", library_name, "` R package to the latest ",
            "version; if that does not solve the problem, contact the `",
            library_name, "` R package maintainer."
        )
    }
}

# A helping function for "checking out" a module from a module library. Here,
# `module_name` should be a fully-qualified module name string formatted like
# "module_library_name:local_module_name". Example: "BioCro:c3_canopy".
check_out_module <- function(module_name) {
    module_name_check_funcs <- list(
        check_vector,
        check_length,
        check_element_length,
        check_strings
    )

    error_messages <- character()

    for (func in module_name_check_funcs) {
        error_messages <- append(
            error_messages,
            func(list(module_name = module_name))
        )
    }

    stop_and_send_error_messages(error_messages)

    # Extract the module library name and the local module name from the
    # fully-qualified module name string
    module_props <- parse_module_name(module_name)
    library_name <- module_props$library_name
    local_module_name <- module_props$local_module_name

    # Try to find important functions from the module library R package
    module_library_function_names <- list(
        framework_version = 'framework_version',
        module_creators = 'module_creators'
    )

    module_library_funcs <- lapply(module_library_function_names, function(fn) {
        tryCatch(
            {function_from_package(library_name, fn)},
            error = function(cond) {
                stop(paste0(
                    "Encountered an issue with module `",
                    module_name,
                    "`: ",
                    cond
                ))
            }
        )
    })

    # Compare the framework versions used by the module library and BioCro
    compare_framework_versions(
        library_name,
        module_library_funcs$framework_version()
    )

    return(module_library_funcs$module_creators(local_module_name)[[1]])
}

# TO-DO: should the reported module name be a fully-qualified module name instead?
module_info <- function(module_name, verbose = TRUE)
{
    # Check that the following type conditions are met:
    # - `module_name` should be a string vector of length 1
    # - `verbose` should be a boolean vector of length 1
    error_messages <- check_vector(list(
        verbose = verbose,
        module_name = module_name
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

    stop_and_send_error_messages(error_messages)

    # Check out the module
    module_creator <- lapply(module_name, check_out_module)

    # Make sure verbose is a logical variable
    verbose <- lapply(verbose, as.logical)

    # Get the info list
    result <- .Call(R_module_info, module_creator, verbose)

    return(invisible(result))
}

check_module_input_quantities <- function(
    module_name,
    input_quantities
)
{
    # Check that the following type conditions are met:
    # - `input_quantities` should be a list of named numeric elements, each of
    #    which has length 1; duplicated element names are not allowed
    # Type checks for `module_name` will be made by the `module_info`
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

    error_messages <- append(
        error_messages,
        check_distinct_names(list(input_quantities = input_quantities))
    )

    # Check for any missing module input quantities
    info <- module_info(module_name, verbose = FALSE)
    missing_input_quantities <- setdiff(
        info[['inputs']],
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

evaluate_module <- function(
    module_name,
    input_quantities,
    stop_on_exception = FALSE
)
{
    # Type checks for `module_name` and `input_quantities` will be
    # performed by the `check_module_input_quantities` function

    # Check to make sure the required input quantities were supplied
    error_messages <-
        check_module_input_quantities(module_name, input_quantities)

    stop_and_send_error_messages(error_messages)

    # Check out the module
    module_creator <- lapply(module_name, check_out_module)

    # C++ requires that all the variables have type `double`
    input_quantities <- lapply(input_quantities, as.numeric)

    # Try to run the module
    result <- tryCatch(
        .Call(R_evaluate_module, module_creator, input_quantities),
        error = function(e) {
            if (stop_on_exception) {
                stop(e)
            } else {
                list(error_msg = conditionMessage(e))
            }
        }
    )

    # Reorder and return the results
    result <- result[order(names(result))]
    return(result)
}

partial_evaluate_module <- function(
    module_name,
    input_quantities,
    arg_names,
    stop_on_exception = FALSE
)
{
    # Check that the following type conditions are met:
    # - `arg_names` should be a character vector
    # Type checks for `module_name` and `input_quantities` will be
    # performed by the `module_info` and `check_module_input_quantities`
    # functions
    error_messages <- check_vector(list(arg_names = arg_names))

    error_messages <- append(
        error_messages,
        check_strings(list(arg_names = arg_names))
    )

    # Check to make sure the quantities specified in `arg_names` are actually
    # required by the module
    info <- module_info(module_name, verbose = FALSE)
    extraneous_args <- arg_names[!arg_names %in% info[['inputs']]]

    if (length(extraneous_args) > 0) {
        error_messages <- append(
            error_messages,
            paste0(
                "`", extraneous_args, "` was provided in `arg_names`, but ",
                "the `", module_name,
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
        check_module_input_quantities(module_name, input_quantities)
    )

    stop_and_send_error_messages(error_messages)

    # Truncate the `input_quantities` list to only include quantities that are
    # actually required by the module
    input_quantities <- input_quantities[info$inputs]

    # Make a function that calls `evaluate_module` with new values for the
    # quantities specified in `arg_names`, returning the inputs and outputs as
    # named elements of a list
    function(x) {
        if (!is.null(names(x))) {
            if (length(names(x)) != length(arg_names) || !all(names(x) %in% arg_names) || !all(arg_names %in% names(x))) {
                msg <- paste0(
                    "The names of the `x` argument do not match those ",
                    "specified by `arg_names`:\n  `arg_names`: ",
                    paste(arg_names, collapse = ", "),
                    "\n  `names(x)`: ",
                    paste(names(x), collapse = ", ")
                )
                stop(msg)
            }
            x <- x[arg_names]
        }

        x = unlist(x)

        if (length(x) != length(arg_names)) {
            msg <- paste0(
                "The unlisted `x` argument (`unlist(x)`) does not have the ",
                "correct number of elements: required = ",
                length(arg_names),
                ", actual = ",
                length(x)
            )
            stop(msg)
        }

        temp_input_quantities = input_quantities

        for (i in seq_along(x)) {
            temp_input_quantities[[arg_names[i]]] <- x[i]
        }

        output_quantities <- evaluate_module(
            module_name,
            temp_input_quantities,
            stop_on_exception
        )

        list(inputs = temp_input_quantities, outputs = output_quantities)
    }
}

module_response_curve <- function(
    module_name,
    fixed_quantities,
    varying_quantities,
    stop_on_exception = FALSE
)
{
    # Check that the following type conditions are met:
    # - `varying_quantities` should be a data frame of numeric elements with
    #    named columns; duplicated column names are not allowed
    # Type checks for `module_name` and `fixed_quantities` will be
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

    error_messages <- append(
        error_messages,
        check_distinct_names(list(varying_quantities = varying_quantities))
    )

    stop_and_send_error_messages(error_messages)

    # Use `partial_evaluate_module` to create a function that calls the module
    # with the appropriate inputs
    evaluation_function <- partial_evaluate_module(
        module_name,
        fixed_quantities,
        names(varying_quantities),
        stop_on_exception
    )

    # Apply the function to each row in the data frame of inputs, producing a
    # list of data frames that each have 1 row and whose columns represent the
    # module's input and output quantities
    df_list <- apply(varying_quantities, 1, function(x) {
        result <- evaluation_function(x)
        as.data.frame(c(result$inputs, result$outputs), row.names = NULL)
    })

    # Find all unique column names
    df_cnames <- unique(unlist(lapply(df_list, colnames)))

    # For each data frame, set any missing columns to NA
    df_list <- lapply(df_list, function(x) {
      x[setdiff(df_cnames, names(x))] <- NA
      x
    })

    # Combine the data frames into one data frame, add the module name as the
    # first column, and return it
    cbind(module_name = module_name, do.call(rbind, df_list))
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

    stop_and_send_error_messages(error_messages)

    stats::setNames(as.list(rep_len(1, length(quantity_names))), quantity_names)
}
