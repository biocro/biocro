biocro_simulation <- function(initial_values, parameters, drivers, steady_state_module_names, derivative_module_names, solver=list(type='Gro', output_step_size=1.0, adaptive_rel_error_tol=1e-4, adaptive_abs_error_tol=1e-4, adaptive_max_steps=200), verbose=FALSE)
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
    #  result <- biocro_simulation(sorghum_initial_values, sorghum_parameters, get_growing_season_climate(weather05), sorghum_ss_modules, sorghum_deriv_modules, "Gro", TRUE)
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
    #  result <- biocro_simulation(glycine_max_initial_values, glycine_max_parameters, get_growing_season_climate(weather05), glycine_max_ss_modules, glycine_max_deriv_modules, "Gro", TRUE)
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
    result = as.data.frame(.Call(R_biocro_simulation, initial_values, parameters, drivers, steady_state_module_names, derivative_module_names, solver_type, solver_output_step_size, solver_adaptive_rel_error_tol, solver_adaptive_abs_error_tol, solver_adaptive_max_steps, verbose))

    # Make sure doy and hour are properly defined
    result$doy = floor(result$time)
    result$hour = 24.0*(result$time - result$doy)

    # Sort the columns by name
    result <- result[,sort(names(result))]

    # Return the result
    return(result)
}

partial_biocro_simulation <- function(initial_values, parameters, drivers, steady_state_module_names, derivative_module_names, arg_names, solver=list(type='Gro', output_step_size=1.0, adaptive_rel_error_tol=1e-4, adaptive_abs_error_tol=1e-4, adaptive_max_steps=200), verbose=FALSE)
{
    # Accepts the same parameters as biocro_simulation() with an additional 'arg_names' parameter, which is a vector of character variables.
    # Returns a function that runs biocro_simulation() with all of the parameters, except 'arg_names'
    # set as default. The only parameter in the new function is the value of 'arg_names'.
    # This technique is called partial application, hence the name partial_gro.
    #
    # initial_values: same as biocro_simulation()
    # parameters: same as biocro_simulation()
    # drivers: same as biocro_simulation()
    # steady_state_module_names: same as biocro_simulation()
    # derivative_module_names: same as biocro_simulation()
    # verbose: same as biocro_simulation()
    # arg_names: vector of character variables. The names of the arguments that the new function accepts.
    #  Note: 'arg_names' must contain the names of parameters in 'initial_values', 'parameters', or 'drivers'.
    #
    # returns f(arg).
    #
    # Example: varying the TTc values at which senescence starts for a sorghum simulation
    #
    #  senescence_biocro_simulation <- partial_biocro_simulation(sorghum_initial_values, sorghum_parameters, get_growing_season_climate(weather05), sorghum_steady_state_modules, sorghum_derivative_modules, sorghum_solver, c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'), TRUE)
    #  result = senescence_gro(c(2000, 2000, 2000, 2000))
    #
    # Note that in this example, typing 'sorghum_parameters$seneStem' returns the original default value for seneStem: 3500.
    # However, after running senescence_biocro_simulation, the system startup messages (in verbose mode) indicate a value of 2000 for seneStem, as desired.
    # To remove system startup messages, change verbose to FALSE in the partial_gro command or omit it altogether to use the default value of FALSE, i.e.
    #
    #  senescence_biocro_simulation <- partial_biocro_simulation(sorghum_initial_values, sorghum_parameters, get_growing_season_climate(weather05), sorghum_steady_state_modules, sorghum_derivative_modules, sorghum_solver, c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'), FALSE)
    #  OR
    #  senescence_biocro_simulation <- partial_biocro_simulation(sorghum_initial_values, sorghum_parameters, get_growing_season_climate(weather05), sorghum_steady_state_modules, sorghum_derivative_modules, sorghum_solver, c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'))

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
        do.call(biocro_simulation, temp_arg_list)
    }
}
