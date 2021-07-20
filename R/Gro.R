Gro_deriv <- function(
    initial_values = list(),
    parameters = list(),
    drivers,
    steady_state_module_names = list(),
    derivative_module_names = list()
)
{
    # Gro_deriv is used to create a function that can be called by a integrator
    # such as LSODES.
    #
    # Important note: this strategy is not recommended since run_biocro
    # now implements the ODEINT stiff system integrator in a more efficient way
    # than LSODES would.
    #
    # initial_values: a list of named quantities representing the initial values
    # of quantities that follow differential evolution rules. Note: the values
    # of these parameters are not important and won't be used in this function,
    # but their names are critical.
    #
    # parameters: a list of named quantities that don't change with time. This
    # list should include a `timestep` element that specifies the time spacing
    # used in the drivers (in units of hours).
    #
    # drivers: a dataframe of quantities defined at equally spaced time
    # intervals. The drivers should include a "time" column with units of days;
    # if "time" is not included, then "doy" and "hour" must be included.
    #
    # steady_state_module_names: a character vector or list of steady state
    # module names.
    #
    # derivative_module_names: a character vector or list of derivative module
    # names.
    #
    # The return value of Gro_deriv is a function with three inputs (`t`,
    # `state`, and `param`) that returns derivatives for each of the parameters
    # in the state. These parameters must have the same names as the state
    # variables defined in initial_values. Here, `state` must be a numeric
    # vector with names, rather than a list.
    #
    # --------------------------------------------------------------------------
    #
    # Example 1: a simple oscillator with derivatives only. Note that we need to
    # define `timestep`, `doy`, and `hour` parameters as required by the C++
    # `System` class, even though `doy` and `hour` won't be used for this
    # example.
    #
    #     oscillator_initial_values <- list(
    #         position = 0,
    #         velocity = 1
    #     )
    #
    #     oscillator_parameters <- list(
    #         timestep = 1,
    #         mass = 1,
    #         spring_constant = 1
    #     )
    #
    #     oscillator_ss_modules <- c()
    #
    #     oscillator_deriv_modules <- c("harmonic_oscillator")
    #
    #     oscillator_system <- Gro_deriv(
    #         oscillator_initial_values,
    #         oscillator_parameters,
    #         get_growing_season_climate(weather05),
    #         oscillator_ss_modules,
    #         oscillator_deriv_modules
    #     )
    #
    #     iv <- as.numeric(oscillator_initial_values)
    #     names(iv) <- names(oscillator_initial_values)
    #     times = seq(0, 5, length=100)
    #
    #     library(deSolve)
    #
    #     result = as.data.frame(lsodes(iv, times, oscillator_system))
    #
    #     lattice::xyplot(position + velocity ~ time, type='l', auto=TRUE, data=result)
    #
    # --------------------------------------------------------------------------
    #
    # Example 2: solving 100 hours of a soybean simulation. This will run very
    # slow compared to a regular call to run_biocro.
    #
    #     soybean_system <- Gro_deriv(
    #         soybean_initial_values,
    #         soybean_parameters,
    #         soybean_weather2002,
    #         soybean_steady_state_modules,
    #         soybean_derivative_modules
    #     )
    #
    #     iv <- as.numeric(soybean_initial_values)
    #     names(iv) <- names(soybean_initial_values)
    #     times = seq(from=0, to=10, by=1)
    #
    #     library(deSolve)
    #
    #     result = as.data.frame(lsodes(iv, times, soybean_system))
    #
    #     lattice::xyplot(Leaf + Stem ~ time, type='l', auto=TRUE, data=result)
    #
    # --------------------------------------------------------------------------
    # Example 3: calculating a single derivative (here we use the same
    # `soybean_system` created in example 2):
    #
    #     derivs <- soybean_system(0, iv, NULL)
    #     View(derivs)

    error_messages = character()

    # Check that quantity definitions are lists.
    args_to_check=list(initial_values=initial_values, parameters=parameters, drivers=drivers)
    for (i in seq_along(args_to_check)) {
        arg = args_to_check[[i]]
        if (!is.list(arg)) {
            error_messages = append(error_message, sprintf('"%s" must be a list.', names(args_to_check)[i]))
        }
    }

    # Check to make sure the initial values and parameters are properly defined
    args_to_check=list(initial_values=initial_values, parameters=parameters)
    for (i in seq_along(args_to_check)) {
        arg = args_to_check[[i]]
        if (length(arg) != length(unlist(arg))) {
            item_lengths = unlist(lapply(arg, length))
            message = sprintf("The following %s members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", names(args_to_check)[i], paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
            error_messages = append(error_messages, message)
        }
    }

    if (length(error_messages) > 0) {
        stop(paste(error_messages, collapse=' '))
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

    # C++ requires that all the variables have type `double`
    initial_values = lapply(initial_values, as.numeric)
    parameters = lapply(parameters, as.numeric)
    drivers = lapply(drivers, as.numeric)

    # Create a function that returns a derivative
    function(t, state, parms)
    {
        # Note: parms is required by LSODES but we aren't using it here. We
        # don't need to do any format checking here because LSODES will have
        # already done it.

        # Convert the state into the proper format
        temp_state <- initial_values;
        for(i in seq_along(state)) {
            param_name = names(state[i])
            param_value = as.numeric(state[i])
            temp_state[param_name] = param_value
        }

        # Call the C++ code that calculates a derivative
        derivs <- .Call(
            R_Gro_deriv,
            temp_state,
            t,
            parameters,
            drivers,
            steady_state_module_names,
            derivative_module_names
        )

        # Return the result
        result <- list(derivs)
        return(result)
    }
}

Gro_ode <- function(
    state = list(),
    steady_state_module_names = list(),
    derivative_module_names = list()
)
{
    # Important note: this function is clunky and not recommended for solving a
    # system, and should only be used to check the output values of a single
    # derivative calculation. Even for this application, Gro_deriv might be more
    # straightforward.
    #
    # This function calculates derivatives using the quantities defined in the
    # state as inputs to the supplied steady state and derivative modules.
    #
    # The state should contain quantities that follow differential evolution
    # rules along with any other required quantities.
    #
    # The derivative modules will only provide derivatives for quantities that
    # follow differential evolution rules.
    #
    # Example: calculating derivatives for a harmonic oscillator
    #
    #     oscillator_deriv_modules <- c("harmonic_oscillator")
    #
    #     oscillator_ss_modules <- c("harmonic_energy")
    #
    #     oscillator_state <- data.frame(
    #         "mass" = 0.5,
    #         "spring_constant" = 0.3,
    #         "position" = 2,
    #         "velocity" = 1
    #     )
    #
    #     oscillator_deriv <- Gro_ode(
    #         oscillator_state,
    #         oscillator_ss_modules,
    #         oscillator_deriv_modules
    #     )
    #
    #     View(oscillator_deriv)
    #
    # There are several things to notice:
    #
    #  (1) Even though time and timestep were not supplied, they show up in the
    #      lists of parameters and drivers. The C++ `System` class requires
    #      these quantities, so Gro_ode supplies default values if none are
    #      specified, as in the case of this example.
    #
    #  (2) No derivatives were supplied for `spring_constant` or `mass`, yet
    #      they are included in the `oscillator_deriv output`. Note that their
    #      "derivative" values in the output are just zero, the default value
    #      for a quantity that does not change with time.

    # Check to make sure the state is properly defined
    if (!is.list(state)) {
        stop('"state" must be a list')
    }

    if (length(state) != length(unlist(state))) {
        item_lengths = unlist(lapply(state, length))
        error_message = sprintf("The following parameters have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
        stop(error_message)
    }

    # Check to make sure the module names are vectors or lists of strings
    steady_state_module_names <- unlist(steady_state_module_names)
    if (length(steady_state_module_names) > 0 & !is.character(steady_state_module_names)) {
        stop('"steady_state_module_names" must be a vector or list of strings')
    }

    derivative_module_names <- unlist(derivative_module_names)
    if (length(derivative_module_names) > 0 & !is.character(derivative_module_names)) {
        stop('"derivative_module_names" must be a vector or list of strings')
    }

    # C++ requires that all the variables have type `double`
    state = lapply(state, as.numeric)

    # Run the C++ code
    result = as.data.frame(.Call(
        R_Gro_ode,
        state,
        steady_state_module_names,
        derivative_module_names
    ))

    return(result)
}
