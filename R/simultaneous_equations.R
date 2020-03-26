solve_simultaneous_equations <- function(
    steady_state_module_names,
	known_quantities,
    unknown_quantities,
    lower_bounds,
    upper_bounds,
    solver = list(type="fixed_point", rel_error_tol=1e-4, abs_error_tol=1e-4, max_it=50),
    silent = FALSE)
{
	# Unpack the solver's details
	solver_type = solver$type
	rel_error_tol = solver$rel_error_tol
	abs_error_tol = solver$abs_error_tol
	max_it = solver$max_it
	
	# C++ requires that all the variables have type `double`
	known_quantities = lapply(known_quantities, as.numeric)
	unknown_quantities = lapply(unknown_quantities, as.numeric)
	lower_bounds = lapply(lower_bounds, as.numeric)
	upper_bounds = lapply(upper_bounds, as.numeric)
	rel_error_tol = as.numeric(rel_error_tol)
	abs_error_tol = as.numeric(abs_error_tol)
	max_it = as.numeric(max_it)
	
	# Make sure silent is a logical variable
	silent = lapply(silent, as.logical)
	
	# Run the C++ code
	result = .Call(
		R_solve_simultaneous_equations,
		known_quantities,
		unknown_quantities,
		lower_bounds,
		upper_bounds,
		steady_state_module_names,
		solver_type,
		rel_error_tol,
		abs_error_tol,
		max_it,
		silent)
	return(result)
}


validate_simultaneous_equations <- function(steady_state_module_names, known_quantities, unknown_quantities, silent = FALSE)
{	
	# C++ requires that all the variables have type `double`
	known_quantities = lapply(known_quantities, as.numeric)
	
	# Make sure silent is a logical variable
	silent = lapply(silent, as.logical)
	
	# Run the C++ code
	result = .Call(R_validate_simultaneous_equations, known_quantities, unknown_quantities, steady_state_module_names, silent)
	return(result)
}

test_simultaneous_equations <- function(steady_state_module_names, known_quantities, unknown_quantities)
{	
	# C++ requires that all the variables have type `double`
	known_quantities = lapply(known_quantities, as.numeric)
	unknown_quantities = lapply(unknown_quantities, as.numeric)
	
	# Run the C++ code
	result = .Call(R_test_simultaneous_equations, known_quantities, unknown_quantities, steady_state_module_names)
	return(result)
}