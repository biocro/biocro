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
    # This function runs a full crop growth simulation with a user-specified
    # numerical ode_solver
    #
    # initial_values: a list of named quantities representing the initial values
    #     of the differential quantities, i.e., the quantities whose derivatives
    #     are calculated by differential modules
    #
    # parameters: a list of named quantities that don't change with time; must
    #     include a `timestep` parameter (see `drivers` for more info)
    #
    # drivers: a data frame of quantities defined at equally spaced time
    #     intervals. The time interval should be specified in the `parameters`
    #     as a quantity called `timestep` having units of hours. The drivers
    #     must include columns for either (1) `time` (in units of days) or
    #     (2) `doy` and `hour`.
    #
    # direct_module_names: a character vector or list specifying the names
    #     of direct modules to use in the system
    #
    # differential_module_names: a character vector or list of differential
    #     module names
    #
    # ode_solver: a list specifying details about the numerical ODE solver.
    #     The required elements are:
    #
    #         type: string specifying the numerical ODE solver to use. Can be
    #             one of the following:
    #
    #                 "auto": use `boost_rosenbrock` if possible; use
    #                     `homemade_euler` otherwise
    #
    #                 "homemade_euler": our own implementation of the fixed step
    #                     Euler method
    #
    #                 "boost_euler": the boost library version of the fixed step
    #                     Euler method
    #
    #                 "boost_rosenbrock": adaptive step-size Rosenbrock method
    #                     (implicit method useful for stiff systems)
    #
    #                 "boost_rk4": fixed-step 4th order Runge-Kutta method
    #
    #                 "boost_rkck54": adaptive step-size Runge-Kutta-Cash-Karp
    #                     method (5th order Runge-Kutta with 4th order Cash-Karp
    #                     error estimation)
    #
    #         output_step_size: the output step size. If smaller than 1, it
    #             should equal 1.0 / N for some integer N. If larger than 1, it
    #             should be an integer.
    #
    #         adaptive_error_tol: used to set the error tolerance for adaptive
    #             step size methods like Rosenbrock or RKCK54
    #
    #         adaptive_max_steps: determines how many times an adaptive step
    #             size method will attempt to find a new step size before
    #             indicating failure
    #
    # verbose: a logical variable indicating whether or not to print
    #     dynamical_system validation information. (More detailed startup
    #     information can be obtained with the
    #     `validate_dynamical_system_inputs` function.)
    #
    # --------------------------------------------------------------------------
    #
    # Example: running a sorghum simulation using weather data from 2005
    #
    #     result <- run_biocro(
    #         sorghum_initial_values,
    #         sorghum_parameters,
    #         get_growing_season_climate(weather2005),
    #         sorghum_direct_modules,
    #         sorghum_differential_modules,
    #         sorghum_ode_solver,
    #         TRUE
    #     )
    #
    #     lattice::xyplot(Leaf + Stem + Root + Grain ~ TTc, data=result, type='l', auto=TRUE)
    #
    # The result is a data frame showing all time-dependent quantities as they
    # change throughout the growing season. When run_biocro is run in
    # verbose mode (as in this example, where verbose = TRUE), information about
    # the validity of the input arguments will be printed to the R console. This
    # can be helpful when attempting to combine a set of modules for the first
    # time. More detailed information can be obtained using the
    # `validate_dynamical_system_inputs` function.
    #
    # In the sorghum example, the simulation is performed using the fixed step
    # size Euler method for numerical integration. One of its modules
    # (`thermal_time_senescence`) requires a history of some of its input
    # quantities, making it incompatible with any other integration method.
    #
    # --------------------------------------------------------------------------
    #
    # Example 2: running a Glycine max simulation using weather data from 2005
    #
    #     result <- run_biocro(
    #         glycine_max_initial_values,
    #         glycine_max_parameters,
    #         get_growing_season_climate(weather2005),
    #         glycine_max_direct_modules,
    #         glycine_max_differential_modules,
    #         glycine_max_ode_solver,
    #         TRUE
    #     )
    #
    #     lattice::xyplot(Leaf + Stem + Root + Grain ~ TTc, data=result, type='l', auto=TRUE)
    #
    # In this simulation, the `auto` ODE solver (specified by
    # `glycine_max_ode_solver`) automatically detects that all modules
    # are compatible with adapative step size integration methods. In this case,
    # it uses the `boost_rosenbrock` ODE solver to run the simulation.

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
    # Accepts the same parameters as run_biocro() with an additional
    # 'arg_names' parameter, which is a vector of character variables.
    #
    # Returns a function that runs run_biocro() with all of the
    # parameters (except those in 'arg_names') set as their default values. The
    # only parameter in the new function is a numerical vector specifying the
    # values of the quantities in 'arg_names'. This technique is called "partial
    # application," hence the name partial_run_biocro.
    #
    # initial_values: same as run_biocro()
    # parameters: same as run_biocro()
    # drivers: same as run_biocro()
    # direct_module_names: same as run_biocro()
    # differential_module_names: same as run_biocro()
    # ode_solver: same as run_biocro()
    # arg_names: vector of character variables. The names of the arguments that
    #            the new function accepts. Note: 'arg_names' can only contain
    #            the names of parameters in 'initial_values', 'parameters', or
    #            'drivers'.
    # verbose: same as run_biocro()
    #
    # returns f(arg).
    #
    # Example: varying the TTc values at which senescence starts for a sorghum
    # simulation; here we set them all to 2000 degrees C * day instead of the
    # default sorghum values.
    #
    #     senescence_simulation <- partial_run_biocro(
    #         sorghum_initial_values,
    #         sorghum_parameters,
    #         get_growing_season_climate(weather2005),
    #         sorghum_direct_modules,
    #         sorghum_differential_modules,
    #         sorghum_ode_solver,
    #         c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'),
    #         TRUE
    #     )
    #
    #     result = senescence_simulation(c(2000, 2000, 2000, 2000))

    error_messages <- check_run_biocro_inputs(
        initial_values,
        parameters,
        drivers,
        direct_module_names,
        differential_module_names,
        ode_solver
    )

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
        index=numeric(),
        stringsAsFactors=FALSE
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

    # Find the locations of the parameters specified in arg_names and check for
    # errors
    controls = df[df$arg_name %in% arg_names, ]
    missing_arg = arg_names[which(!arg_names %in% df$arg_name)]
    if (length(missing_arg) > 0) {
        error_messages <- append(
            error_messages, sprintf(
                '`%s` from `arg_names` is not in the `initial_values`, `parameters`, or `drivers`',
                missing_arg
            )
        )
    }

    send_error_messages(error_messages)

    # Make a function that calls run_biocro with new values for the
    # parameters specified in arg_names
    function(x)
    {
        x = unlist(x)
        if (length(x) != nrow(controls)) {
            stop("The `x` argument does not have the correct number of elements")
        }
        temp_arg_list = arg_list
        for (i in seq_along(x)) {
            c_row = controls[i, ]
            temp_arg_list[[c_row$control]][[c_row$arg_name]][c_row$index] = x[i]
        }
        do.call(run_biocro, temp_arg_list)
    }
}
