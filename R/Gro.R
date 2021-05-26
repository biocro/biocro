Gro_solver <- function(initial_values, parameters, drivers, steady_state_module_names, derivative_module_names, solver=list(type='Gro', output_step_size=1.0, adaptive_rel_error_tol=1e-4, adaptive_abs_error_tol=1e-4, adaptive_max_steps=200), verbose=FALSE)
{
    # This function runs a full crop growth simulation with a user-specified solver
    #
    # initial_values: a list of named parameters representing state variables
    # parameters: a list of named parameters that don't change with time
    # drivers: a dataframe of parameters defined at equally spaced time intervals
    #  Note: the time interval should be specified as a parameter called "timestep" in the list of constant parameters
    #  Note: the drivers must include "doy" and "hour"
    # steady_state_module_names: a character vector of steady state module names
    # steady_state_module_names: a character vector of derivative module names
    # solver: a list specifying details about the solver. Elements are:
    #  type: s string specifying the solver to use. Options are:
    #   "Gro": automatically uses Rosenbrock if possible, uses Euler otherwise
    #   "Gro_euler": fixed-step Euler method
    #   "Gro_rsnbrk": adaptive step-size Rosenbrock method (implicit method useful for stiff systems)
    #   "Gro_rk4": fixed-step 4th order Runge-Kutta method
    #   "Gro_rkck54": adaptive step-size Runge-Kutta-Cash-Karp method (5th order Runge-Kutta with 4th order error estimation)
    #  output_step_size: the output step size... can be smaller than 1.0, but should equal 1.0 / N for some integer NULL
    #  adaptive_error_tol: used to set the error tolerance for adaptive step size methods like Rosenbrock or RKCK54
    #  adaptive_max_steps: determines how many times an adaptive step size method will attempt to find a new step size before failing
    # verbose: a logical variable indicating whether or not to print system startup information
    #
    # Example: running a sorghum simulation using weather data from 2005
    #
    #  sorghum_ss_modules <- c("soil_type_selector", "stomata_water_stress_linear", "leaf_water_stress_exponential", "parameter_calculator", "partitioning_coefficient_selector", "soil_evaporation", "c4_canopy", "partitioning_growth_calculator")
    #  sorghum_deriv_modules <- c("thermal_time_senescence", "partitioning_growth", "thermal_time_linear", "one_layer_soil_profile")
    #  result <- Gro_solver(sorghum_initial_values, sorghum_parameters, get_growing_season_climate(weather05), sorghum_ss_modules, sorghum_deriv_modules, "Gro", TRUE)
    #  xyplot(Leaf + Stem + Root + Grain ~ TTc, data=result, type='l', auto=TRUE)
    #
    # The result is a data frame showing all time-dependent variables as they change throughout the growing season.
    # When Gro is run in verbose mode (as in this example, where verbose = TRUE), information about the input and output parameters
    # will be printed to the R console before the simulation runs. This can be very useful when attempting to combine a set of modules
    # for the first time.
    #
    # In the sorghum example, the simulation is performed using the fixed-step size Euler method for numerical integration. One of its modules (thermal_time_senescence)
    # requires a history of all parameters, making it incompatible with any other integration method.
    #
    # Example 2: running a soybean simulation using weather data from 2005
    #
    #  glycine_max_ss_modules <- c("soil_type_selector", "stomata_water_stress_linear", "leaf_water_stress_exponential", "parameter_calculator", "soil_evaporation", "c3_canopy", "utilization_growth_calculator", "utilization_senescence_calculator")
    #  glycine_max_deriv_modules <- c("utilization_growth", "utilization_senescence", "thermal_time_linear", "one_layer_soil_profile")
    #  result <- Gro_solver(glycine_max_initial_values, glycine_max_parameters, get_growing_season_climate(weather05), glycine_max_ss_modules, glycine_max_deriv_modules, "Gro", TRUE)
    #  xyplot(Leaf + Stem + Root + Grain ~ TTc, data=result, type='l', auto=TRUE)
    #
    # In the soybean simulation, Gro automatically detects that all modules are compatible with adapative step size integration methods. In this case, it uses
    # ODEINT's implementation of an implicit Rosenbrock solver to run the simulation.

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

    # If the drivers input doesn't have a time column, add one
    drivers <- add_time_to_weather_data(drivers)

    # Check to make sure the module names are vectors or lists of strings
    steady_state_module_names <- unlist(steady_state_module_names)
    if (length(steady_state_module_names) > 0 & !is.character(steady_state_module_names)) {
        stop('"steady_state_module_names" must be a vector or list of strings')
    }

    derivative_module_names <- unlist(derivative_module_names)
    if(length(derivative_module_names) > 0 & !is.character(derivative_module_names)) {
        stop('"derivative_module_names" must be a vector or list of strings')
    }

    # Check to make sure the solver properties are properly defined
    if(!is.list(solver)) {
        stop("'solver' must be a list")
    }
    solver_type <- solver$type
    if (!is.character(solver_type) & length(solver_type) != 1) {
        stop('"solver_type" must be a string')
    }
    solver_output_step_size <- solver$output_step_size
    solver_adaptive_rel_error_tol <- solver$adaptive_rel_error_tol
    solver_adaptive_abs_error_tol <- solver$adaptive_abs_error_tol
    solver_adaptive_max_steps <- solver$adaptive_max_steps

    # C++ requires that all the variables have type `double`
    initial_values = lapply(initial_values, as.numeric)
    parameters = lapply(parameters, as.numeric)
    drivers = lapply(drivers, as.numeric)
    solver_output_step_size = as.numeric(solver_output_step_size)
    solver_adaptive_rel_error_tol = as.numeric(solver_adaptive_rel_error_tol)
    solver_adaptive_abs_error_tol = as.numeric(solver_adaptive_abs_error_tol)
    solver_adaptive_max_steps = as.numeric(solver_adaptive_max_steps)

    # Make sure verbose is a logical variable
    verbose = lapply(verbose, as.logical)

    # Run the C++ code
    result = as.data.frame(.Call(R_Gro_solver, initial_values, parameters, drivers, steady_state_module_names, derivative_module_names, solver_type, solver_output_step_size, solver_adaptive_rel_error_tol, solver_adaptive_abs_error_tol, solver_adaptive_max_steps, verbose))

    # Make sure doy and hour are properly defined
    result$doy = floor(result$time)
    result$hour = 24.0*(result$time - result$doy)

    # Sort the columns by name
    result <- result[,sort(names(result))]

    # Return the result
    return(result)
}

partial_gro_solver <- function(initial_values, parameters, drivers, steady_state_module_names, derivative_module_names, arg_names, solver=list(type='Gro', output_step_size=1.0, adaptive_rel_error_tol=1e-4, adaptive_abs_error_tol=1e-4, adaptive_max_steps=200), verbose=FALSE)
{
    # Accepts the same parameters as Gro_solver() with an additional 'arg_names' parameter, which is a vector of character variables.
    # Returns a function that runs Gro_solver() with all of the parameters, except 'arg_names'
    # set as default. The only parameter in the new function is the value of 'arg_names'.
    # This technique is called partial application, hence the name partial_gro.
    #
    # initial_values: same as Gro_solver()
    # parameters: same as Gro_solver()
    # drivers: same as Gro_solver()
    # steady_state_module_names: same as Gro_solver()
    # derivative_module_names: same as Gro_solver()
    # verbose: same as Gro_solver()
    # arg_names: vector of character variables. The names of the arguments that the new function accepts.
    #  Note: 'arg_names' must contain the names of parameters in 'initial_values', 'parameters', or 'drivers'.
    #
    # returns f(arg).
    #
    # Example: varying the TTc values at which senescence starts for a sorghum simulation
    #
    #  senescence_gro_solver <- partial_gro_solver(sorghum_initial_values, sorghum_parameters, get_growing_season_climate(weather05), sorghum_steady_state_modules, sorghum_derivative_modules, sorghum_solver, c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'), TRUE)
    #  result = senescence_gro(c(2000, 2000, 2000, 2000))
    #
    # Note that in this example, typing 'sorghum_parameters$seneStem' returns the original default value for seneStem: 3500.
    # However, after running senescence_gro_solver, the system startup messages (in verbose mode) indicate a value of 2000 for seneStem, as desired.
    # To remove system startup messages, change verbose to FALSE in the partial_gro command or omit it altogether to use the default value of FALSE, i.e.
    #
    #  senescence_gro_solver <- partial_gro_solver(sorghum_initial_values, sorghum_parameters, get_growing_season_climate(weather05), sorghum_steady_state_modules, sorghum_derivative_modules, sorghum_solver, c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'), FALSE)
    #  OR
    #  senescence_gro_solver <- partial_gro_solver(sorghum_initial_values, sorghum_parameters, get_growing_season_climate(weather05), sorghum_steady_state_modules, sorghum_derivative_modules, sorghum_solver, c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'))

    arg_list = list(initial_values=initial_values, parameters=parameters, drivers=drivers, steady_state_module_names=steady_state_module_names, derivative_module_names=derivative_module_names, solver=solver, verbose=verbose)

    df = data.frame(control=character(), arg_name=character(), stringsAsFactors=FALSE)
    for (i in seq_along(arg_list)) {
        if (length(names(arg_list[[i]])) > 0) {
            df = rbind(df, data.frame(control = names(arg_list)[i], arg_name=names(arg_list[[i]]), stringsAsFactors=FALSE))
        }
    }

    # Find the locations of the parameters specified in arg_names and check for errors
    controls = df[match(arg_names, df$arg_name), ]
    if (any(is.na(controls))) {
        missing = arg_names[which(is.na(controls$control))]
        stop(paste('The following arguments in "arg_names" are not in any of the paramter lists:', paste(missing, collapse=', ')))
    }

    # Make a function that calls Gro with new values for the parameters specified in arg_names
    function(x)
    {
        if (length(x) != length(arg_names)) stop("The length of x does not match the length of arguments when this function was defined.")
        x = unlist(x)
        temp_arg_list = arg_list
        for (i in seq_along(arg_names)) {
            c_row = controls[i, ]
            temp_arg_list[[c_row$control]][[c_row$arg_name]] = x[i]
        }
        do.call(Gro_solver, temp_arg_list)
    }
}

Gro_deriv <- function(initial_values, parameters, drivers, steady_state_module_names, derivative_module_names)
{
    # Gro_deriv is used to create a function that can be called by a solver such as LSODES
    #
    # Important note: this strategy is not recommended, since Gro, Gro_auto, and Gro_rsnbrk now implement
    #  the ODEINT stiff system solver in a more efficient way then LSODES would
    #
    # initial_values: a list of named parameters representing state variables
    #  Note: the values of these parameters are not important and won't be used in this function, but their names are critical
    # parameters: a list of named parameters that don't change with time
    # drivers: a dataframe of parameters defined at equally spaced time intervals
    #  Note: the time interval should be specified as a parameter called "timestep" in the list of constant parameters
    #  Note: the drivers must include "doy" and "hour"
    # steady_state_module_names: a character vector of steady state module names
    # steady_state_module_names: a character vector of derivative module names
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
