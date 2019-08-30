#######################################
#                                     #
#  Functions for running simulations  #
#                                     #
#######################################

Gro <- function(initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, verbose = FALSE)
{
	# This function runs a full simulation using the fixed-step Euler method
	#
	# initial_state: a list of named parameters representing state variables
	# parameters: a list of named parameters that don't change with time
	# varying_parameters: a dataframe of parameters defined at equally spaced time intervals
	#  Note: the time interval should be specified as a parameter called "timestep" in the list of constant parameters
	#  Note: the varying parameters must include "doy" and "hour"
	# steady_state_module_names: a character vector of steady state module names
	# steady_state_module_names: a character vector of derivative module names
	# verbose: a logical variable indicating whether or not to print system startup information
	#
	# Example: running a sorghum simulation using weather data from 2005
	#
	#  sorghum_ss_modules <- c("soil_type_selector", "stomata_water_stress_linear", "leaf_water_stress_exponential", "parameter_calculator", "partitioning_coefficient_selector", "soil_evaporation", "c4_canopy", "partitioning_growth_calculator")
	#  sorghum_deriv_modules <- c("thermal_time_senescence", "partitioning_growth", "thermal_time_accumulator", "one_layer_soil_profile")
	#  result <- Gro(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_ss_modules, sorghum_deriv_modules, TRUE)
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
	#  glycine_max_deriv_modules <- c("utilization_growth", "utilization_senescence", "thermal_time_accumulator", "one_layer_soil_profile")
	#  result <- Gro(glycine_max_initial_state, glycine_max_parameters, get_growing_season_climate(weather05), glycine_max_ss_modules, glycine_max_deriv_modules, TRUE)
	#  xyplot(Leaf + Stem + Root + Grain ~ TTc, data=result, type='l', auto=TRUE)
	#
	# In the soybean simulation, Gro automatically detects that all modules are compatible with adapative step size integration methods. In this case, it uses
	# ODEINT's implementation of an implicit Rosenbrock solver to run the simulation.
	
	# Check to make sure the initial_state is properly defined
	if(!is.list(initial_state)) {
		stop('"initial_state" must be a list')
	}
	
	if(length(initial_state) != length(unlist(initial_state))) {
		item_lengths = unlist(lapply(initial_state, length))
		error_message = sprintf("The following initial_state members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
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
	
	# Check to make sure the varying_parameters are properly defined
	if(!is.list(varying_parameters)) {
		stop('"varying_parameters" must be a list')
	}
	
	# C++ requires that all the variables have type `double`
	initial_state = lapply(initial_state, as.numeric)
	parameters = lapply(parameters, as.numeric)
	varying_parameters = lapply(varying_parameters, as.numeric)
	
	# Make sure verbose is a logical variable
	verbose = lapply(verbose, as.logical)
	
	# Run the C++ code
	result = as.data.frame(.Call(R_Gro, initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, verbose))
	return(result)
}

partial_gro <- function(initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, arg_names, verbose = FALSE)
{
	# Accepts the same parameters as Gro() with an additional 'arg_names' parameter, which is a vector of character variables.
	# Returns a function that runs Gro() with all of the parameters, except 'arg_names
	# set as default. The only parameter in the new function is the value of 'arg_names'.
	# This technique is called partial application, hence the name partial_gro.
	# 
	# initial_state: same as Gro()
	# parameters: same as Gro()
	# varying_parameters: same as Gro()
	# steady_state_module_names: same as Gro()
	# derivative_module_names: same as Gro()
	# verbose: same as Gro()
	# arg_names: vector of character variables. The names of the arguments that the new function accepts.
	#  Note: 'arg_names' must contain the names of parameters in 'initial_state', 'parameters', or 'varying_parameters'.
	# 
	# returns f(arg).
	# 
	# Example: varying the TTc values at which senescence starts for a sorghum simulation
	# 
	#  sorghum_ss_modules <- c("soil_type_selector", "stomata_water_stress_linear", "leaf_water_stress_exponential", "parameter_calculator", "partitioning_coefficient_selector", "soil_evaporation", "c4_canopy", "partitioning_growth_calculator")
	#  sorghum_deriv_modules <- c("thermal_time_senescence", "partitioning_growth", "thermal_time_accumulator", "one_layer_soil_profile")
	#  senescence_gro <- partial_gro(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_ss_modules, sorghum_deriv_modules, c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'), TRUE)
	#  result = senescence_gro(c(2000, 2000, 2000, 2000))
	# 
	# Note that in this example, typing 'sorghum_parameters$seneStem' returns the original default value for seneStem: 3500.
	# However, after running senescence_gro, the system startup messages (in verbose mode) indicate a value of 2000 for seneStem, as desired.
	# To remove system startup messages, change verbose to FALSE in the partial_gro command or omit it altogether to use the default value of FALSE, i.e.
	# 
	#  senescence_gro <- partial_gro(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_ss_modules, sorghum_deriv_modules, c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'), FALSE)
	#  OR
	#  senescence_gro <- partial_gro(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_ss_modules, sorghum_deriv_modules, c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'))
	
	# Form the argument list to pass to Gro
	arg_list = list(initial_state=initial_state, parameters=parameters, varying_parameters=varying_parameters, steady_state_module_names=steady_state_module_names, derivative_module_names=derivative_module_names, verbose=verbose)
	
	# Make a data frame containing the names of all parameters in the initial_state, parameters, and varying_parameters inputs
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
		do.call(Gro, temp_arg_list)
	}
}

Gro_deriv <- function(initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, verbose=FALSE)
{
	# Gro_deriv is used to create a function that can be called by a solver such as LSODES
	#
	# initial_state: a list of named parameters representing state variables
	#  Note: the values of these parameters are not important and won't be used in this function, but their names are critical
	# parameters: a list of named parameters that don't change with time
	# varying_parameters: a dataframe of parameters defined at equally spaced time intervals
	#  Note: the time interval should be specified as a parameter called "timestep" in the list of constant parameters
	#  Note: the varying parameters must include "doy" and "hour"
	# steady_state_module_names: a character vector of steady state module names
	# steady_state_module_names: a character vector of derivative module names
	# verbose: a logical variable indicating whether or not to print system startup information
	#  Note: verbose should usually be FALSE for Gro_deriv, since this function would get called many times during a simulation
	#
	# The return value of Gro_deriv is a function with three inputs (t, state, and param) that returns derivatives for each of the
	# parameters in the state. These parameters must have the same names as the state variables defined in the initial_state.
	# Here, state must be a numeric vector with names, rather than a list
	#
	# Example 1: a simple oscillator with derivatives only
	# Note that we need to define timestep, doy, and hour parameters as required by the C++ "system" class, even though doy and hour
	# won't be used for this example
	#
	#  oscillator_ss_modules <- c()
	#  oscillator_deriv_modules <- c("position_oscillator", "velocity_oscillator")
	#  oscillator_initial_state <- list(position=0, velocity=1)
	#  oscillator_system <- Gro_deriv(oscillator_initial_state, list("timestep"=1), get_growing_season_climate(weather05), oscillator_ss_modules, oscillator_deriv_modules, FALSE)
	#  is <- as.numeric(oscillator_initial_state)		# We need to convert the initial state to a different format
	#  names(is) <- names(oscillator_initial_state)		# We need to convert the initial state to a different format
	#  times = seq(0, 5, length=100)
	#  library(deSolve)									# Required to use LSODES
	#  result = as.data.frame(lsodes(oscillator_initial_state, times, oscillator_system))
	#  xyplot(position + velocity ~ time, type='l', auto=TRUE, data=result)
	#
	# Example 2: solving 1000 hours of a soybean simulation
	#
	#  soybean_ss_modules <- c("soil_type_selector", "stomata_water_stress_linear", "leaf_water_stress_exponential", "parameter_calculator", "soil_evaporation", "c3_canopy", "utilization_growth_calculator", "utilization_senescence_calculator")
	#  soybean_deriv_modules <- c("utilization_growth", "utilization_senescence", "thermal_time_accumulator", "one_layer_soil_profile")
	#  soybean_system <- Gro_deriv(glycine_max_initial_state, glycine_max_parameters, get_growing_season_climate(weather05), soybean_ss_modules, soybean_deriv_modules, FALSE)
	#  is <- as.numeric(glycine_max_initial_state)		# We need to convert the initial state to a different format
	#  names(is) <- names(glycine_max_initial_state)	# We need to convert the initial state to a different format
	#  times = seq(from=0, to=1000, by=1)
	#  library(deSolve)									# Required to use LSODES
	#  result = as.data.frame(lsodes(is, times, soybean_system))
	#
	# Note that for this example, we needed to convert the initial state to a different format
	# It's also possible to just calculate a single derivative:
	#
	#  derivs <- soybean_system(0, is, NULL)
	#  View(derivs)
	
	# Check to make sure the initial_state is properly defined
	if(!is.list(initial_state)) {
		stop('"initial_state" must be a list')
	}
	
	if(length(initial_state) != length(unlist(initial_state))) {
		item_lengths = unlist(lapply(initial_state, length))
		error_message = sprintf("The following initial_state members have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
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
	
	# Check to make sure the varying_parameters are properly defined
	if(!is.list(varying_parameters)) {
		stop('"varying_parameters" must be a list')
	}
	
	# C++ requires that all the variables have type `double`
	initial_state = lapply(initial_state, as.numeric)
	parameters = lapply(parameters, as.numeric)
	varying_parameters = lapply(varying_parameters, as.numeric)
	
	# Make sure verbose is a logical variable
	verbose = lapply(verbose, as.logical)
	
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
		temp_state <- initial_state;
		for(i in seq_along(state)) {
			param_name = names(state[i])
			param_value = as.numeric(state[i])
			temp_state[param_name] = param_value
		}
		
		# Call the C++ code that calculates a derivative
		derivs <- .Call(R_Gro_deriv, temp_state, t, parameters, varying_parameters, steady_state_module_names, derivative_module_names, verbose)
		
		# Return the result
		result <- list(derivs)
		return(result)
	}
}

Gro_ode <- function(state, steady_state_module_names, derivative_module_names, verbose = FALSE)
{
	# Important note: this function is clunky and not recommended. Gro_deriv is better suited for
	#  calculating derivatives
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
	# To understand the output better, it may be helpful to run the Gro_ode command as:
	#
	#  oscillator_deriv <- Gro_ode(oscillator_state, oscillator_ss_modules, oscillator_deriv_modules, TRUE)
	#
	# There are several things to notice:
	#  (1) Even though doy_dbl and timestep were not supplied as parameters, they show up in the lists
	#      of invariant and varying parameters. The system requires these parameters, so Gro_ode supplies
	#      default values if none are specified, as in the case of this example
	#  (2) No derivatives were supplied for spring_constant or mass, yet they are included in the oscillator_deriv
	#      output. Note that their "derivative" values in the output are just zero, the default value for a parameter
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
	
	# Make sure verbose is a logical variable
	verbose = lapply(verbose, as.logical)
	
	# Run the C++ code
	result = as.data.frame(.Call(R_Gro_ode, state, steady_state_module_names, derivative_module_names, verbose))
	return(result)
}

###################################################
#                                                 #
#  Functions for exploring the available modules  #
#                                                 #
###################################################

get_all_modules <- function()
{
	# Intended usage:
	#
	#  all_modules <- get_all_modules()
	#  View(all_modules)
	
	result = .Call(R_get_all_modules)
	return(result)
}

get_module_info <- function(module_name)
{	
	# Example: getting information about the big_leaf_multilayer_canopy module
	#
	#  big_leaf_multilayer_canopy_inputs <- get_module_info("big_leaf_multilayer_canopy")
	#
	# The function returns a dataframe with all the inputs required by the module
	# It also prints the module's type (derivative or steady state), its inputs, and its outputs to the R console
	
	if (!is.character(module_name) & length(module_name) != 1) {
		stop('"module_name" must be a string')
	}
	
	result = .Call(R_get_module_info, module_name)
	return(result)
}

get_standalone_ss_info <- function(module_names)
{
	# Example: finding the required input parameters for a combination
	# of the water_vapor_properties_from_air_temperature, collatz_leaf, and
	# penman_monteith_transpiration modules
	#
	#  get_standalone_ss_info(c("water_vapor_properties_from_air_temperature", "collatz_leaf", "penman_monteith_transpiration"))
	#
	# Note that this function can also be helpful for verifying that the modules
	# have been combined in the correct order. For example, the command
	#
	#  get_standalone_ss_info(c("collatz_leaf", "water_vapor_properties_from_air_temperature", "penman_monteith_transpiration"))
	#
	# produces an error because some of the outputs from "water_vapor_properties_from_air_temperature" were already required as
	# inputs to "collatz_leaf"
	
	if (!is.character(module_names)) {
		stop('"module_name" must be a list of strings')
	}
	
	result = .Call(R_get_standalone_ss_info, module_names)
	return(result)
}

###############################################
#                                             #
#  Functions for running and testing modules  #
#                                             #
###############################################

test_module <- function(module_name, input_parameters)
{
	# Example: testing the output of the big_leaf_multilayer_canopy module
	#
	#  big_leaf_multilayer_canopy_inputs <- get_module_info("big_leaf_multilayer_canopy")
	#  <<modify individual input parameters to desired values>>
	#  big_leaf_multilayer_canopy_param <- test_module("big_leaf_multilayer_canopy", big_leaf_multilayer_canopy_inputs)
	#  <<check the values of the output parameters to confirm they are correct>>
	#
	# Important note: during the testing process, the output parameters are initialized to -1000000.0.
	# If the module fails to update one of its output parameters, it will retain this unusual value and should be obvious.
	
	if (!is.character(module_name) & length(module_name) != 1) {
		stop('"module_name" must be a string')
	}
	
	if(!is.list(input_parameters)) {
		stop('"input_parameters" must be a list')
	}
	
	if(length(input_parameters) != length(unlist(input_parameters))) {
		item_lengths = unlist(lapply(input_parameters, length))
		error_message = sprintf("The following parameters have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
		stop(error_message)
	}
	
	input_parameters = lapply(input_parameters, as.numeric)
	
	result = .Call(R_test_module, module_name, input_parameters)
	return(result)
}

test_system <- function(initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names)
{
	# Example: confirming that the parameters and modules for a sorghum simulation are properly defined
	#
	#  sorghum_ss_modules <- c("soil_type_selector", "stomata_water_stress_linear", "leaf_water_stress_exponential", "parameter_calculator", "partitioning_coefficient_selector", "soil_evaporation", "c4_canopy", "partitioning_growth_calculator")
	#  sorghum_deriv_modules <- c("thermal_time_senescence", "partitioning_growth", "thermal_time_accumulator", "one_layer_soil_profile")
	#  test_system(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_ss_modules, sorghum_deriv_modules)
	#
	# Note: this example doesn't generate any serious issues. However, if we omit an important module such as the soil_type_selector, we will have a problem. Try this example:
	#
	#  sorghum_ss_modules <- c("stomata_water_stress_linear", "leaf_water_stress_exponential", "parameter_calculator", "partitioning_coefficient_selector", "soil_evaporation", "c4_canopy", "partitioning_growth_calculator")
	#  sorghum_deriv_modules <- c("thermal_time_senescence", "partitioning_growth", "thermal_time_accumulator", "one_layer_soil_profile")
	#  test_system(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_ss_modules, sorghum_deriv_modules)
	#
	# In this case, some input parameters for the stomata_water_stress_linear, leaf_water_stress_exponential, soil_evaporation, and one_layer_soil_profile modules are not defined and an exception is thrown
	# Note that module order is important. Putting the soil_type_selector in the wrong spot also causes errors:
	#
	#  sorghum_ss_modules <- c("stomata_water_stress_linear", "leaf_water_stress_exponential", "parameter_calculator", "partitioning_coefficient_selector", "soil_evaporation", "c4_canopy", "partitioning_growth_calculator", "soil_type_selector")
	#  sorghum_deriv_modules <- c("thermal_time_senescence", "partitioning_growth", "thermal_time_accumulator", "one_layer_soil_profile")
	#  test_system(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_ss_modules, sorghum_deriv_modules)
	#
	# In this case, some input parameters for the stomata_water_stress_linear, leaf_water_stress_exponential, and soil_evaporation modules are not defined and an exception is thrown.
	# In contrast to the previous example, the one_layer_soil_profile module is fine because all steady state modules are run before any derivative modules
	
	# C++ requires that all the variables have type `double`
	initial_state = lapply(initial_state, as.numeric)
	parameters = lapply(parameters, as.numeric)
	varying_parameters = lapply(varying_parameters, as.numeric)
	
	# Run the C++ code
	result = .Call(R_test_system, initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names)
	return(result)
}

#######################################################################
#                                                                     #
#  Additional functions that may be helpful when writing new modules  #
#                                                                     #
#######################################################################

get_all_param <- function()
{
	# Example 1: getting a list of all unique parameters (which can be useful when writing a new module
	# that is intended to work along with pre-existing modules)
	#
	#  all_param <- get_all_param()
	#  all_param_names <- all_param$parameter_name
	#  all_unique_param <- unique(all_param_names)
	#  all_unique_param <- sort(all_unique_param)
	#  View(all_unique_param)
	#
	#
	# Example 2: getting a list of all modules that have "ci" or "Ci" as an input or output parameter
	#
	#  all_param <- get_all_param()
	#  ci_modules <- subset(all_param, parameter_name=="ci" | parameter_name=="Ci")
	#  View(ci_modules)
	#
	#
	# Example 3: counting the number of modules that use each parameter as an input or output
	#
	#  install.packages("plyr")	# required for the count function
	#  library(plyr)
	#  all_param <- get_all_param()
	#  all_param_subset <- all_param[c("parameter_name", "module_name")]	# remove information about whether each parameter is an input or output
	#  all_param_subset <- unique(all_param_subset)							# remove any duplicated names where a parameter is both an input and an output for the same module
	#  param_usage <- count(all_param_subset, "parameter_name")
	#  View(param_usage)
	
	result = as.data.frame(.Call(R_get_all_param))
	return(result)
}

## 'Comment out' older versions of Gro with an if(FALSE) block
if(FALSE) {
	Gro <- function(initial_values, parameters, varying_parameters, modules)
	{
		for ( ilist in list(initial_values, parameters) ) {
			if (class(ilist) != 'list') {
				stop('"initial_values" and "parameters" must each be a list.')
			}
			
			item_lengths = unlist(lapply(ilist, length))
			if (!(all(item_lengths == 1))) {
				error_message = sprintf("The following parameters have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
				stop(error_message)
			}
		}

		module_names = paste(c('canopy', 'soil', 'growth', 'senescence', 'stomata_water_stress', 'leaf_water_stress'), '_module_name', sep='')
		if (any(unused_modules_ind <- !names(modules) %in% module_names)) {
			message = 'The following modules were supplied, but are not used by this function: '
			unused_modules = paste(names(modules)[unused_modules_ind], collaspe=', ')
			stop(paste(message, unused_modules, ',', sep=''))
		}
		
		if (any(null_ind <- unlist(lapply(modules[module_names], is.null)))) {
			message = 'The following modules names are NULL, but they must be defined: '
			missing_modules = paste(module_names[null_ind], collapse=', ')
			stop(paste(message, missing_modules, '.', sep=''))
		}

		varying_parameters = as.data.frame(lapply(as.list(varying_parameters), as.numeric))  # C++ requires that all the variables have type `double`.
		names(varying_parameters) = tolower(names(varying_parameters))  # Convert all names to lower case for ease of use.

		result = as.data.frame(.Call(R_Gro, initial_values, parameters, varying_parameters, modules$canopy_module_name, modules$soil_module_name, modules$growth_module_name, modules$senescence_module_name, modules$stomata_water_stress_module_name, modules$leaf_water_stress_module_name))
		result = cbind(varying_parameters[c('year', 'doy', 'hour')], result)
		return(result)
	}

	Gro_ode <- function(state, steady_state_modules, derivative_modules, check_parameters=TRUE)
	{
		if (check_parameters) {
			if (class(state) != 'list') {
				stop('"state" must be a list.')
			}
			
			if (length(state) != length(unlist(state))) {
				item_lengths = unlist(lapply(state, length))
				error_message = sprintf("The following parameters have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
				stop(error_message)
			}
			
			state = lapply(state, as.numeric)  # C++ requires that all the variables have type `double`.
		}

		result = .Call(R_Gro_ode, state, steady_state_modules, derivative_modules)
		return(result)
	}

	run_modules <- function(state, steady_state_modules, check_parameters=TRUE)
	{
		if (check_parameters) {
			if (class(state) != 'list') {
				stop('"state" must be a list.')
			}
			
			if (length(state) != length(unlist(state))) {
				item_lengths = unlist(lapply(state, length))
				error_message = sprintf("The following parameters have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
				stop(error_message)
			}
			
			state = lapply(state, as.numeric)  # C++ requires that all the variables have type `double`.
		}

		result = .Call(R_run_modules, state, steady_state_modules)
		return(result)
	}

	get_module_requirements <- function(modules)
	{
		if (class(modules) != 'character') {
			stop('"modules" must be a character vector')
		}

		result = .Call(R_get_module_requirements, modules)
		return(result)
	}

	partial_gro = function(initial_values, parameters, varying_parameters, modules, arg_names) {
	# Accepts the same parameters as Gro() with an additional 'arg_names' parameter, which is a vector of character variables.
	# Returns a function that runs Gro() with all of the parameters, except 'arg_names
	# set as default. The only parameter in the new function is the value of 'arg_names'.
	# This technique is called partial application, hence the name partial_gro.

	# initial_values: same as Gro()
	# parameters: same as Gro()
	# varying_parameters: same as Gro()
	# modules: same as Gro()
	# arg_name: vector of character variables. The names of the arguments that the new function accepts.
	#          It must contain the names of parameters in 'intial_values', 'parameters', or 'varying_parameters'.

	# returns f(arg).
	#
	# Example:
	# senescence_gro = partial_gro(sorghum_initial_state, sorghum_parameters, weather05, sorghum_modules, c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'))
	# result = senescence_gro(c(3000, 3000, 3000, 3000))

		arg_list = list(initial_values=initial_values, parameters=parameters, varying_parameters=varying_parameters, modules=modules)

		df = data.frame(control=character(), arg_name=character(), stringsAsFactors=FALSE)
		for (i in seq_along(arg_list)) {
			df = rbind(df, data.frame(control = names(arg_list)[i], arg_name=names(arg_list[[i]]), stringsAsFactors=FALSE))
		}

		controls = df[match(arg_names, df$arg_name), ]
		if (any(is.na(controls))) {
			missing = arg_names[which(is.na(controls$control))]
			stop(paste('The following arguments in "arg_names" are not in any of the paramter lists:', paste(missing, collapse=', ')))
		}
		function(x) {
			if (length(x) != length(arg_names)) stop("The length of x does not match the length of arguments when this function was defined.")
			x = unlist(x)
			temp_arg_list = arg_list
			for (i in seq_along(arg_names)) {
				c_row = controls[i, ]
				temp_arg_list[[c_row$control]][[c_row$arg_name]] = x[i]
			}
			do.call(Gro, temp_arg_list)
		}
	}

	# Gro_deriv is used to create a function that can be called by a solver such as `lsodes`.
	# parameters: a list of named parameters that don't change with time.
	# varying_parameters: a function of the form `f(t)` that returns a named list of parameters, where `t` is time.
	# steady_state_modules: a character vector module names.
	# derivative: a character vector of module names.

	# A simple example with only derivatives.
	# oscillator_system = Gro_deriv(list(), function(t) return(list()), c(), c('position_oscillator', 'velocity_oscillator'))
	# state = c(position=0, velocity=1)
	# times = seq(0, 5, length=100)
	# result = as.data.frame(lsodes(state, times, oscillator_system))

	Gro_deriv = function (parameters, varying_parameters, steady_state_modules, derivative_modules) {
		state_names = character(0)
		result_names = character(0)
		result_name_length = 0
		state_diff = character(0)
		result_diff = character(0)

		function(t, state, parms) {
			vp = varying_parameters(t)
			all_state = c(state, parameters, vp)
			result = .Call(R_Gro_ode, all_state, steady_state_modules, derivative_modules)
			temp = names(result)
			if (length(temp) != result_name_length) {
				state_names <<- names(state)
				result_names <<- temp
				result_name_length <<- length(result_names)
				state_diff <<- setdiff(state_names, result_names) 
				result_diff <<- setdiff(result_names, state_names)
			}
			result[state_diff] = 0
			derivatives = result[state_names]
			state_of_interest = c(result[result_diff], vp)
			return(list(derivatives, c(state_of_interest, derivatives)))
		}
	}
}