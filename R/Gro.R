Gro_deriv <- function(initial_values, parameters, drivers, steady_state_module_names, derivative_module_names)
{
    # Gro_deriv is used to create a function that can be called by a numerical_integrator such as LSODES
    #
    # Important note: this strategy is not recommended, biocro_simulation now implements
    #  the ODEINT stiff system numerical_integrator in a more efficient way then LSODES would
    #
    # initial_values: a list of named parameters representing state variables
    #  Note: the values of these parameters are not important and won't be used in this function, but their names are critical
    # parameters: a list of named parameters that don't change with time
    # drivers: a dataframe of parameters defined at equally spaced time intervals
    #  Note: the time interval should be specified as a parameter called "timestep" in the list of constant parameters
    #  Note: the drivers must include "doy" and "hour"
    # steady_state_module_names: a character vector of steady state module names
    # derivative_module_names: a character vector of derivative module names
    # verbose: a logical variable indicating whether or not to print system startup information
    #  Note: verbose should usually be FALSE for Gro_deriv, since this function would get called many times during a simulation
    #
    # The return value of Gro_deriv is a function with three inputs (t, state, and param) that returns derivatives for each of the
    # parameters in the state. These parameters must have the same names as the state variables defined in the initial_values.
    # Here, state must be a numeric vector with names, rather than a list
    #
    # Example 1: a simple oscillator with derivatives only
    # Note that we need to define timestep, doy, and hour parameters as required by the C++ "system" class, even though doy and hour
    # won't be used for this example
    #
    #  oscillator_ss_modules <- c()
    #  oscillator_deriv_modules <- c("position_oscillator", "velocity_oscillator")
    #  oscillator_initial_values <- list(position=0, velocity=1)
    #  oscillator_system <- Gro_deriv(oscillator_initial_values, list("timestep"=1), get_growing_season_climate(weather05), oscillator_ss_modules, oscillator_deriv_modules, FALSE)
    #  is <- as.numeric(oscillator_initial_values)       # We need to convert the initial values to a different format
    #  names(is) <- names(oscillator_initial_values)     # We need to convert the initial values to a different format
    #  times = seq(0, 5, length=100)
    #  library(deSolve)                                 # Required to use LSODES
    #  result = as.data.frame(lsodes(oscillator_initial_values, times, oscillator_system))
    #  xyplot(position + velocity ~ time, type='l', auto=TRUE, data=result)
    #
    # Example 2: solving 1000 hours of a soybean simulation
    #
    #  soybean_ss_modules <- c("soil_type_selector", "stomata_water_stress_linear", "leaf_water_stress_exponential", "parameter_calculator", "soil_evaporation", "c3_canopy", "utilization_growth_calculator", "utilization_senescence_calculator")
    #  soybean_deriv_modules <- c("utilization_growth", "utilization_senescence", "thermal_time_linear", "one_layer_soil_profile")
    #  soybean_system <- Gro_deriv(glycine_max_initial_values, glycine_max_parameters, get_growing_season_climate(weather05), soybean_ss_modules, soybean_deriv_modules, FALSE)
    #  is <- as.numeric(glycine_max_initial_values)      # We need to convert the initial values to a different format
    #  names(is) <- names(glycine_max_initial_values)    # We need to convert the initial values to a different format
    #  times = seq(from=0, to=1000, by=1)
    #  library(deSolve)                                 # Required to use LSODES
    #  result = as.data.frame(lsodes(is, times, soybean_system))
    #
    # Note that for this example, we needed to convert the initial values to a different format
    # It's also possible to just calculate a single derivative:
    #
    #  derivs <- soybean_system(0, is, NULL)
    #  View(derivs)

    # Check to make sure the initial_values is properly defined
    if(!is.list(initial_values)) {
        stop('"initial_values" must be a list')
    }

    if(length(initial_values) != length(unlist(initial_values))) {
        item_lengths = unlist(lapply(initial_values, length))
        error_message = sprintf("The following initial_values members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
        stop(error_message)
    }

    # Check to make sure the parameters are properly defined
    if(!is.list(parameters)) {
        stop('"parameters" must be a list')
    }

    if(length(parameters) != length(unlist(parameters))) {
        item_lengths = unlist(lapply(parameters, length))
        error_message = sprintf("The following parameters members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
        stop(error_message)
    }

    # Check to make sure the drivers are properly defined
    if(!is.list(drivers)) {
        stop('"drivers" must be a list')
    }

    # C++ requires that all the variables have type `double`
    initial_values = lapply(initial_values, as.numeric)
    parameters = lapply(parameters, as.numeric)
    drivers = lapply(drivers, as.numeric)

    # Define some items for the function
    state_names = character(0)
    result_names = character(0)
    result_name_length = 0
    state_diff = character(0)
    result_diff = character(0)

    # Create a function that returns a derivative
    function(t, state, parms)
    {
        # Note: parms is required by LSODES but we aren't using it here

        # We don't need to do any format checking here because LSODES will have already done it

        # Convert the state into the proper format
        temp_state <- initial_values;
        for(i in seq_along(state)) {
            param_name = names(state[i])
            param_value = as.numeric(state[i])
            temp_state[param_name] = param_value
        }

        # Call the C++ code that calculates a derivative
        derivs <- .Call(R_Gro_deriv, temp_state, t, parameters, drivers, steady_state_module_names, derivative_module_names)

        # Return the result
        result <- list(derivs)
        return(result)
    }
}

Gro_ode <- function(state, steady_state_module_names, derivative_module_names)
{
    # Important note: this function is clunky and not recommended for solving a system, and should only be
    #  used to check the output values of a single derivative calculation. Even for this application, Gro_deriv
    #  might be more straightforward.
    #
    # This function calculates derivatives using the parameters defined in the state as inputs to the
    # supplied steady state and derivative modules
    # Note: the state should contain state variables along with any other required parameters
    # Note: the derivative modules will only provide derivatives for state variables. Run the function
    # with verbose = TRUE to see which parameters do not have defined derivatives.
    #
    # Example: calculating derivatives for a harmonic oscillator
    #
    #  oscillator_deriv_modules <- c("harmonic_oscillator")
    #  oscillator_ss_modules <- c("harmonic_energy")
    #  oscillator_state <- data.frame("mass"=0.5, "spring_constant"=0.3, "position"=2, "velocity"=1)
    #  oscillator_deriv <- Gro_ode(oscillator_state, oscillator_ss_modules, oscillator_deriv_modules)
    #  View(oscillator_deriv)
    #
    # There are several things to notice:
    #  (1) Even though time and timestep were not supplied, they show up in the lists
    #      of parameters and drivers. The system requires these quantities, so Gro_ode supplies
    #      default values if none are specified, as in the case of this example
    #  (2) No derivatives were supplied for spring_constant or mass, yet they are included in the oscillator_deriv
    #      output. Note that their "derivative" values in the output are just zero, the default value for a quantity
    #      that does not change with time.

    # Check to make sure the state is properly defined
    if(!is.list(state)) {
        stop('"state" must be a list')
    }

    if(length(state) != length(unlist(state))) {
        item_lengths = unlist(lapply(state, length))
        error_message = sprintf("The following parameters have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
        stop(error_message)
    }

    # Check to make sure the module names are properly defined
    if (!is.character(steady_state_module_names)) {
        stop('"steady_state_module_names" must be a list of strings')
    }
    if (!is.character(derivative_module_names)) {
        stop('"derivative_module_names" must be a list of strings')
    }

    # C++ requires that all the variables have type `double`
    state = lapply(state, as.numeric)

    # Run the C++ code
    result = as.data.frame(.Call(R_Gro_ode, state, steady_state_module_names, derivative_module_names))
    return(result)
}
