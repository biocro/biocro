default_integrator <- list(
    type = 'homemade_euler',
    output_step_size = NA,
    adaptive_rel_error_tol = NA,
    adaptive_abs_error_tol = NA,
    adaptive_max_steps = NA
)

biocro_simulation <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    steady_state_module_names = list(),
    derivative_module_names = list(),
    integrator = default_integrator,
    verbose = FALSE
)
{
    # This function runs a full crop growth simulation with a user-specified
    # numerical integrator
    #
    # initial_values: a list of named quantities representing the initial values
    #                 of quantities that follow a differential evolution rule.
    #
    # parameters: a list of named quantities that don't change with time; must
    #             include a `timestep` parameter (see `drivers` for more info)
    #
    # drivers: a data frame of quantities defined at equally spaced time
    #          intervals. The time interval should be specified as a quantity
    #          called `timestep` having units of hours. The drivers must include
    #          columns for either (1) `time` (in units of days) or (2) `doy` and
    #          `hour`.
    #
    # steady_state_module_names: a character vector or list of steady state
    #                            module names
    #
    # derivative_module_names: a character vector or list of derivative module
    #                          names
    #
    # integrator: a list specifying details about the numerical integrator.
    #             Elements are:
    #
    #             type: string specifying the numerical integrator to
    #                   use. Options are:
    #
    #                   "auto": automatically uses `boost_rosenbrock` if
    #                           possible; uses `homemade_euler` otherwise
    #
    #                   "homemade_euler": our own implementation of the fixed
    #                                     step Euler method
    #
    #                   "boost_euler": the boost library version of the fixed
    #                                  step Euler method
    #
    #                   "boost_rosenbrock": adaptive step-size Rosenbrock method
    #                                       (implicit method useful for stiff
    #                                       systems)
    #
    #                   "boost_rk4": fixed-step 4th order Runge-Kutta method
    #
    #                   "boost_rkck54": adaptive step-size Runge-Kutta-Cash-Karp
    #                                   method (5th order Runge-Kutta with 4th
    #                                   order Cash-Karp error estimation)
    #
    #             output_step_size: the output step size. If smaller than 1, it
    #                               should equal 1.0 / N for some integer N. If
    #                               larger than 1, it should be integer.
    #
    #             adaptive_error_tol: used to set the error tolerance for
    #                                 adaptive step size methods like Rosenbrock
    #                                  or RKCK54
    #
    #             adaptive_max_steps: determines how many times an adaptive step
    #                                 size method will attempt to find a new
    #                                 step size before indicating failure
    #
    # verbose: a logical variable indicating whether or not to print System
    #          validation information. (More detailed startup information can
    #          be obtained with the `validate_system_inputs` function.)
    #
    # --------------------------------------------------------------------------
    #
    # Example: running a sorghum simulation using weather data from 2005
    #
    #     result <- biocro_simulation(
    #         sorghum_initial_values,
    #         sorghum_parameters,
    #         get_growing_season_climate(weather05),
    #         sorghum_steady_state_modules,
    #         sorghum_derivative_modules,
    #         sorghum_integrator,
    #         TRUE
    #     )
    #
    #     lattice::xyplot(Leaf + Stem + Root + Grain ~ TTc, data=result, type='l', auto=TRUE)
    #
    # The result is a data frame showing all time-dependent quantities as they
    # change throughout the growing season. When biocro_simulation is run in
    # verbose mode (as in this example, where verbose = TRUE), information about
    # the validity of the input arguments will be printed to the R console. This
    # can be helpful when attempting to combine a set of modules for the first
    # time. More detailed information can be obtained using the
    # `validate_system_inputs` function.
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
    #     result <- biocro_simulation(
    #         glycine_max_initial_values,
    #         glycine_max_parameters,
    #         get_growing_season_climate(weather05),
    #         glycine_max_steady_state_modules,
    #         glycine_max_derivative_modules,
    #         glycine_max_integrator,
    #         TRUE
    #     )
    #
    #     lattice::xyplot(Leaf + Stem + Root + Grain ~ TTc, data=result, type='l', auto=TRUE)
    #
    # In this simulation, the `auto` integrator (specified by
    # `glycine_max_integrator`) automatically detects that all modules
    # are compatible with adapative step size integration methods. In this case,
    # it uses the `boost_rosenbrock` integrator to run the simulation.

    # Check to make sure the initial values are properly defined
    if (!is.list(initial_values)) {
        stop('"initial_values" must be a list')
    }

    if (length(initial_values) != length(unlist(initial_values))) {
        item_lengths = unlist(lapply(initial_values, length))
        error_message = sprintf("The following initial_values members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
        stop(error_message)
    }

    # Check to make sure the parameters are properly defined
    if (!is.list(parameters)) {
        stop('"parameters" must be a list')
    }

    if (length(parameters) != length(unlist(parameters))) {
        item_lengths = unlist(lapply(parameters, length))
        error_message = sprintf("The following parameters members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
        stop(error_message)
    }

    # Check to make sure the drivers are properly defined
    if (!is.list(drivers)) {
        stop('"drivers" must be a list')
    }

    # If the drivers input doesn't have a time column, add one
    drivers <- add_time_to_weather_data(drivers)

    # Check to make sure the module names are vectors or lists of strings
    steady_state_module_names <- unlist(steady_state_module_names)
    if (length(steady_state_module_names) > 0 & !is.character(steady_state_module_names)) {
        stop('"steady_state_module_names" must be a vector or list of strings')
    }

    derivative_module_names <- unlist(derivative_module_names)
    if (length(derivative_module_names) > 0 & !is.character(derivative_module_names)) {
        stop('"derivative_module_names" must be a vector or list of strings')
    }

    # Check to make sure the numerical integrator properties are properly
    # defined
    if (!is.list(integrator)) {
        stop("'integrator' must be a list")
    }
    integrator_type <- integrator$type
    if (!is.character(integrator_type) & length(integrator_type) != 1) {
        stop('"integrator_type" must be a string')
    }
    integrator_output_step_size <- integrator$output_step_size
    integrator_adaptive_rel_error_tol <- integrator$adaptive_rel_error_tol
    integrator_adaptive_abs_error_tol <- integrator$adaptive_abs_error_tol
    integrator_adaptive_max_steps <- integrator$adaptive_max_steps

    # C++ requires that all the variables have type `double`
    initial_values = lapply(initial_values, as.numeric)
    parameters = lapply(parameters, as.numeric)
    drivers = lapply(drivers, as.numeric)
    integrator_output_step_size = as.numeric(integrator_output_step_size)
    integrator_adaptive_rel_error_tol = as.numeric(integrator_adaptive_rel_error_tol)
    integrator_adaptive_abs_error_tol = as.numeric(integrator_adaptive_abs_error_tol)
    integrator_adaptive_max_steps = as.numeric(integrator_adaptive_max_steps)

    # Make sure verbose is a logical variable
    verbose = lapply(verbose, as.logical)

    # Run the C++ code
    result = as.data.frame(.Call(
        R_biocro_simulation,
        initial_values,
        parameters,
        drivers,
        steady_state_module_names,
        derivative_module_names,
        integrator_type,
        integrator_output_step_size,
        integrator_adaptive_rel_error_tol,
        integrator_adaptive_abs_error_tol,
        integrator_adaptive_max_steps,
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

partial_biocro_simulation <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    steady_state_module_names = list(),
    derivative_module_names = list(),
    integrator = default_integrator,
    arg_names,
    verbose = FALSE
)
{
    # Accepts the same parameters as biocro_simulation() with an additional
    # 'arg_names' parameter, which is a vector of character variables.
    #
    # Returns a function that runs biocro_simulation() with all of the
    # parameters (except those in 'arg_names') set as their default values. The
    # only parameter in the new function is a numerical vector specifying the
    # values of the quantities in 'arg_names'. This technique is called "partial
    # application," hence the name partial_biocro_simulation.
    #
    # initial_values: same as biocro_simulation()
    # parameters: same as biocro_simulation()
    # drivers: same as biocro_simulation()
    # steady_state_module_names: same as biocro_simulation()
    # derivative_module_names: same as biocro_simulation()
    # integrator: same as biocro_simulation()
    # arg_names: vector of character variables. The names of the arguments that
    #            the new function accepts. Note: 'arg_names' can only contain
    #            the names of parameters in 'initial_values', 'parameters', or
    #            'drivers'.
    # verbose: same as biocro_simulation()
    #
    # returns f(arg).
    #
    # Example: varying the TTc values at which senescence starts for a sorghum
    # simulation; here we set them all to 2000 degrees C * day instead of the
    # default sorghum values.
    #
    #     senescence_simulation <- partial_biocro_simulation(
    #         sorghum_initial_values,
    #         sorghum_parameters,
    #         get_growing_season_climate(weather05),
    #         sorghum_steady_state_modules,
    #         sorghum_derivative_modules,
    #         sorghum_integrator,
    #         c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'),
    #         TRUE
    #     )
    #
    #     result = senescence_simulation(c(2000, 2000, 2000, 2000))

    arg_list = list(
        initial_values=initial_values,
        parameters=parameters,
        drivers=drivers,
        steady_state_module_names=steady_state_module_names,
        derivative_module_names=derivative_module_names,
        integrator=integrator,
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

    # Make a function that calls biocro_simulation with new values for the
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
        do.call(biocro_simulation, temp_arg_list)
    }
}
