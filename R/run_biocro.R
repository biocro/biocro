default_ode_solver <- list(
    type = 'homemade_euler',
    output_step_size = NA,
    adaptive_rel_error_tol = NA,
    adaptive_abs_error_tol = NA,
    adaptive_max_steps = NA
)

run_biocro <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    direct_module_names = list(),
    differential_module_names = list(),
    ode_solver = default_ode_solver,
    verbose = FALSE
)
{
    # Check over the inputs arguments for possible issues
    error_messages <- check_run_biocro_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names,
        ode_solver
    )

    send_error_messages(error_messages)

    # If the drivers input doesn't have a time column, add one
    drivers <- add_time_to_weather_data(drivers)

    # Make sure the module names are vectors of strings
    direct_module_names <- unlist(direct_module_names)
    differential_module_names <- unlist(differential_module_names)

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
        direct_module_names,
        differential_module_names,
        ode_solver_type,
        ode_solver_output_step_size,
        ode_solver_adaptive_rel_error_tol,
        ode_solver_adaptive_abs_error_tol,
        ode_solver_adaptive_max_steps,
        verbose
    ))

    # Make sure doy and hour are properly defined
    result$doy = floor(result$time)
    result$hour = 24.0*(result$time - result$doy)

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
    ode_solver = default_ode_solver,
    arg_names,
    verbose = FALSE
)
{
    # Check over the inputs arguments for possible issues
    error_messages <- check_run_biocro_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names,
        ode_solver
    )

    send_error_messages(error_messages)

    arg_list = list(
        initial_values=initial_values,
        parameters=parameters,
        drivers=drivers,
        direct_module_names=direct_module_names,
        differential_module_names=differential_module_names,
        ode_solver=ode_solver,
        verbose=verbose
    )

    df = data.frame(
        control=character(),
        arg_name=character(),
        stringsAsFactors=FALSE
    )

    for (i in seq_along(arg_list)) {
        if (length(names(arg_list[[i]])) > 0) {
            df = rbind(
                df,
                data.frame(
                    control = names(arg_list)[i],
                    arg_name=names(arg_list[[i]]),
                    stringsAsFactors=FALSE
                )
            )
        }
    }

    # Find the locations of the parameters specified in arg_names and check for
    # errors
    controls = df[match(arg_names, df$arg_name), ]
    if (any(is.na(controls))) {
        missing = arg_names[which(is.na(controls$control))]
        stop(paste('The following arguments in "arg_names" are not in any of the paramter lists:', paste(missing, collapse=', ')))
    }

    # Make a function that calls run_biocro with new values for the
    # parameters specified in arg_names
    function(x)
    {
        if (length(x) != length(arg_names)) {
            stop("The length of x does not match the length of arguments when this function was defined.")
        }
        x = unlist(x)
        temp_arg_list = arg_list
        for (i in seq_along(arg_names)) {
            c_row = controls[i, ]
            temp_arg_list[[c_row$control]][[c_row$arg_name]] = x[i]
        }
        do.call(run_biocro, temp_arg_list)
    }
}
