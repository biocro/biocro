get_module_info <- function(module_name, verbose = TRUE)
{	
	# Example: getting information about the big_leaf_multilayer_canopy module
	#
	#  big_leaf_multilayer_canopy_inputs <- get_module_info("big_leaf_multilayer_canopy")
	#
	# The function returns a dataframe with all the inputs required by the module
	# It also prints the module's type (derivative or steady state), its inputs, and its outputs to the R console
	
	# Make sure the module name is a string
	if (!is.character(module_name) & length(module_name) != 1) {
		stop('"module_name" must be a string')
	}
	
	# Make sure verbose is a logical variable
	verbose = lapply(verbose, as.logical)
	
	result = .Call(R_get_module_info, module_name, verbose)
	result = result[order(names(result))]
	return(result)
}

test_module <- function(module_name, input_parameters)
{
	# Example: testing the output of the big_leaf_multilayer_canopy module
	#
	#  big_leaf_multilayer_canopy_inputs <- get_module_info("big_leaf_multilayer_canopy")
	#  <<modify individual input parameters to desired values>>
	#  big_leaf_multilayer_canopy_param <- test_module("big_leaf_multilayer_canopy", big_leaf_multilayer_canopy_inputs, TRUE)
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
	result = result[order(names(result))]
	return(result)
}