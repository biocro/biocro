validate_system_inputs <- function(initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, silent = FALSE)
{	
	# C++ requires that all the variables have type `double`
	initial_state = lapply(initial_state, as.numeric)
	parameters = lapply(parameters, as.numeric)
	varying_parameters = lapply(varying_parameters, as.numeric)
	
	# Make sure silent is a logical variable
	silent = lapply(silent, as.logical)
	
	# Run the C++ code
	result = .Call(R_validate_system_inputs, initial_state, parameters, varying_parameters, steady_state_module_names, derivative_module_names, silent)
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