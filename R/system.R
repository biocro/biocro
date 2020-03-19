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