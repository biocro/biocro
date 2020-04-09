get_standalone_ss_info <- function(module_names)
{
	# Example: finding the required input parameters for a combination
	# of the water_vapor_properties_from_air_temperature, collatz_leaf, and
	# penman_monteith_transpiration modules
	#
	#  inputs <- get_standalone_ss_info(c("water_vapor_properties_from_air_temperature", "collatz_leaf", "penman_monteith_transpiration"))
	#
	# Note that this function can also be helpful for verifying that the modules
	# have been combined in the correct order. For example, the command
	#
	#  inputs <- get_standalone_ss_info(c("collatz_leaf", "water_vapor_properties_from_air_temperature", "penman_monteith_transpiration"))
	#
	# produces an error because some of the outputs from "water_vapor_properties_from_air_temperature" were already required as
	# inputs to "collatz_leaf"
	#
	# The function returns a dataframe with all the inputs required by the collection of modules
	# It also prints the module's inputs and outputs to the R console
	
	if (!is.character(module_names)) {
		stop('"module_names" must be a list of strings')
	}
	
	result = .Call(R_get_standalone_ss_info, module_names)
	return(result)
}

test_standalone_ss <- function(module_names, input_parameters, verbose = FALSE)
{
	# Example: testing the output of a combination
	# of the water_vapor_properties_from_air_temperature, collatz_leaf, and
	# penman_monteith_transpiration modules
	#
	#  inputs <- get_standalone_ss_info(c("water_vapor_properties_from_air_temperature", "collatz_leaf", "penman_monteith_transpiration"))	# get info about required parameters
	#  inputs$Catm = 400
	#  inputs$Rd = 0.8
	#  inputs$StomataWS = 0.99
	#  inputs$alpha = 2.04
	#  inputs$b0 = 0.08
	#  inputs$b1 = 3.0
	#  inputs$beta = 0.93
	#  inputs$incident_irradiance = 1000.00
	#  inputs$incident_par = 2127.66
	#  inputs$k_Q10 = 2.0
	#  inputs$kparm = 0.7
	#  inputs$layer_wind_speed = 1.07
	#  inputs$leaf_reflectance = 0.2
	#  inputs$leaf_transmittance = 0.2
	#  inputs$leafwidth = 0.04
	#  inputs$lowerT = 3.0
	#  inputs$rh = 0.68
	#  inputs$stefan_boltzman = 5.67e-8
	#  inputs$temp = 21.9
	#  inputs$theta = 0.83
	#  inputs$upperT = 37.5
	#  inputs$vmax = 41
	#  inputs$water_stress_approach = 1
	#  outputs <- test_standalone_ss(c("water_vapor_properties_from_air_temperature", "collatz_leaf", "penman_monteith_transpiration"), inputs, TRUE)
	
	if (!is.character(module_names)) {
		stop('"module_names" must be a list of strings')
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
	
	verbose = lapply(verbose, as.logical)
	
	result = .Call(R_test_standalone_ss, module_names, input_parameters, verbose)
	return(result)
}