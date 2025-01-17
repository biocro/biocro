# Checks whether a set of inputs to the `run_biocro` function are properly
# defined. If the inputs are properly defined, this function returns an empty
# string. Otherwise, it returns an informative error message.
check_run_biocro_inputs <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_module_names = list(),
    differential_module_names = list(),
    ode_solver = BioCro::default_ode_solvers$homemade_euler,
    verbose = FALSE
)
{
    error_message <- character()

    # The initial_values, parameters, and ode_solver should be lists
    error_message <- append(
        error_message,
        check_list(
            list(
                initial_values=initial_values,
                parameters=parameters,
                ode_solver=ode_solver
            )
        )
    )

    # The drivers should be a data frame
    error_message <- append(
        error_message,
        check_data_frame(list(drivers=drivers))
    )

    # The drivers should not be empty
    if (length(drivers) == 0) {
        error_message <- append(error_message, "The drivers cannot be empty")
    }

    # The components of initial_values, parameters, drivers, and ode_solver
    # should all have names
    error_message <- append(
        error_message,
        check_element_names(
            list(
                initial_values=initial_values,
                parameters=parameters,
                drivers=drivers,
                ode_solver=ode_solver
            )
        )
    )

    # The elements of initial_values, parameters, direct_module_names,
    # differential_module_names, and ode_solver should each have a length of 1
    error_message <- append(
        error_message,
        check_element_length(
            list(
                initial_values=initial_values,
                parameters=parameters,
                direct_module_names=direct_module_names,
                differential_module_names=differential_module_names,
                ode_solver=ode_solver
            )
        )
    )

    # The elements of initial_values, parameters, drivers, direct_module_names,
    # differential_module_names, and ode_solver should not have any duplicated
    # names
    error_message <- append(
        error_message,
        check_distinct_names(
            list(
                initial_values=initial_values,
                parameters=parameters,
                drivers=drivers,
                direct_module_names=direct_module_names,
                differential_module_names=differential_module_names,
                ode_solver=ode_solver
            )
        )
    )

    # The initial_values, parameters, drivers, and all elements of ode_solver
    # except `type` should have numeric values
    error_message <- append(
        error_message,
        check_numeric(
            list(
                initial_values=initial_values,
                parameters=parameters,
                drivers=drivers,
                ode_solver_other_than_type=ode_solver[!(names(ode_solver) == 'type')]
            )
        )
    )

    # The direct_module_names and differential_module_names should be vectors or
    # lists of strings. The ode_solver's `type` element should also be a string.
    error_message <- append(
        error_message,
        check_strings(
            list(
                direct_module_names=direct_module_names,
                differential_module_names=differential_module_names,
                ode_solver_type=ode_solver['type']
            )
        )
    )

    # Verbose should be a boolean with one element
    error_message <- append(
        error_message,
        check_boolean(list(verbose=verbose))
    )

    error_message <- append(
        error_message,
        check_length(list(verbose=verbose))
    )

    error_message <- append(
        error_message,
        check_time_is_sequential(
            drivers,
            differential_module_names,
            parameters
        )
    )

    return(error_message)
}

# A helping function for making convenient alterations when weather data is
# supplied
adapt_weather_data <- function(drivers, direct_module_names) {
    if ('doy' %in% names(drivers) && 'hour' %in% names(drivers)) {
        drivers <- add_time_to_weather_data(drivers)

        if (!'BioCro:format_time' %in% unlist(direct_module_names)) {
            direct_module_names <-
                append(direct_module_names, 'BioCro:format_time')
        }
    }

    list(
        direct_module_names = direct_module_names,
        drivers = drivers
    )
}

run_biocro <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_module_names = list(),
    differential_module_names = list(),
    ode_solver = BioCro::default_ode_solvers$homemade_euler,
    verbose = FALSE
)
{
    # Make sure weather data is properly handled
    adapted <- adapt_weather_data(drivers, direct_module_names)
    drivers <- adapted$drivers
    direct_module_names <- adapted$direct_module_names

    # Check over the inputs arguments for possible issues
    error_messages <- check_run_biocro_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names,
        ode_solver,
        verbose
    )

    stop_and_send_error_messages(error_messages)

    # Make module creators from the specified names and libraries
    direct_module_creators <- sapply(
        direct_module_names,
        check_out_module
    )

    differential_module_creators <- sapply(
        differential_module_names,
        check_out_module
    )

    # Collect the ode_solver info
    ode_solver_type <- ode_solver$type
    ode_solver_output_step_size <- ode_solver$output_step_size
    ode_solver_adaptive_rel_error_tol <- ode_solver$adaptive_rel_error_tol
    ode_solver_adaptive_abs_error_tol <- ode_solver$adaptive_abs_error_tol
    ode_solver_adaptive_max_steps <- ode_solver$adaptive_max_steps

    # C++ requires that all the variables have type `double`
    initial_values <- lapply(initial_values, as.numeric)
    parameters <- lapply(parameters, as.numeric)
    drivers <- lapply(drivers, as.numeric)
    ode_solver_output_step_size <- as.numeric(ode_solver_output_step_size)
    ode_solver_adaptive_rel_error_tol <- as.numeric(ode_solver_adaptive_rel_error_tol)
    ode_solver_adaptive_abs_error_tol <- as.numeric(ode_solver_adaptive_abs_error_tol)
    ode_solver_adaptive_max_steps <- as.numeric(ode_solver_adaptive_max_steps)

    # Make sure verbose is a logical variable
    verbose <- lapply(verbose, as.logical)

    # Run the C++ code
    result <- as.data.frame(.Call(
        R_run_biocro,
        initial_values,
        parameters,
        drivers,
        direct_module_creators,
        differential_module_creators,
        ode_solver_type,
        ode_solver_output_step_size,
        ode_solver_adaptive_rel_error_tol,
        ode_solver_adaptive_abs_error_tol,
        ode_solver_adaptive_max_steps,
        verbose
    ))

    # Sort the columns by name
    result <- result[,sort(names(result))]

    # Return the result
    return(result)
}

partial_run_biocro <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_module_names = list(),
    differential_module_names = list(),
    ode_solver = BioCro::default_ode_solvers$homemade_euler,
    arg_names,
    verbose = FALSE
)
{
    # If the drivers input doesn't have a time column, add one
    drivers <- add_time_to_weather_data(drivers)

    # The inputs to this function have the same requirements as the `run_biocro`
    # inputs with the same names
    error_messages <- check_run_biocro_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names,
        ode_solver
    )

    stop_and_send_error_messages(error_messages)

    arg_list = list(
        initial_values = initial_values,
        parameters = parameters,
        drivers = drivers,
        direct_module_names = direct_module_names,
        differential_module_names = differential_module_names,
        ode_solver = ode_solver,
        verbose = verbose
    )

    df = data.frame(
        control = character(),
        arg_name = character(),
        index = numeric(),
        stringsAsFactors = FALSE
    )

    for (i in seq_len(3)) {
        if (length(names(arg_list[[i]])) > 0) {
            for (j in seq_along(arg_list[[i]])) {
                df = rbind(
                    df,
                    data.frame(
                        control = names(arg_list)[i],
                        arg_name = names(arg_list[[i]])[j],
                        index = seq_along(arg_list[[i]][[j]]),
                        stringsAsFactors=FALSE
                    )
                )
            }
        }
    }

    # Find the locations of the quantities specified in arg_names and check for
    # errors. We can't use just %in% because that doesn't preserve the order.
    # However, match() only returns the first match, which doesn't work for
    # things like drivers. So use %in% for each arg_name so we can get all
    # elements in the correct order.
    controls <-
        do.call(rbind, lapply(arg_names, function(an) df[df$arg_name %in% an, ]))

    missing_arg = arg_names[which(!arg_names %in% df$arg_name)]
    if (length(missing_arg) > 0) {
        error_messages <- append(
            error_messages, sprintf(
                '`%s` from `arg_names` is not in the `initial_values`, `parameters`, or `drivers`',
                missing_arg
            )
        )
    }

    stop_and_send_error_messages(error_messages)

    # Make a function that calls run_biocro with new values for the quantities
    # specified in arg_names
    function(x)
    {
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

        x <- unlist(x)

        if (length(x) != nrow(controls)) {
            msg <- paste0(
                "The unlisted `x` argument (`unlist(x)`) does not have the ",
                "correct number of elements: required = ",
                nrow(controls),
                ", actual = ",
                length(x)
            )
            stop(msg)
        }

        temp_arg_list = arg_list

        for (i in seq_along(x)) {
            c_row = controls[i, ]
            temp_arg_list[[c_row$control]][[c_row$arg_name]][c_row$index] = x[i]
        }

        do.call(run_biocro, temp_arg_list)
    }
}
