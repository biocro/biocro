validate_simultaneous_equations <- function(known_quantities, unknown_quantities, steady_state_module_names, silent = FALSE)
{	
	# C++ requires that all the variables have type `double`
	known_quantities = lapply(known_quantities, as.numeric)
	
	# Make sure silent is a logical variable
	silent = lapply(silent, as.logical)
	
	# Run the C++ code
	result = .Call(R_validate_simultaneous_equations, known_quantities, unknown_quantities, steady_state_module_names, silent)
	return(result)
}

test_simultaneous_equations <- function(known_quantities, unknown_quantities, steady_state_module_names)
{	
	# C++ requires that all the variables have type `double`
	known_quantities = lapply(known_quantities, as.numeric)
	unknown_quantities = lapply(unknown_quantities, as.numeric)
	
	# Run the C++ code
	result = .Call(R_test_simultaneous_equations, known_quantities, unknown_quantities, steady_state_module_names)
	return(result)
}