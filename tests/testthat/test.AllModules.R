# This tests each module to make sure it can be instantiated
# Problems can arise e.g. if a module's get_outputs() or get_inputs() functions don't actually represent its real inputs and outputs

context("Test to make sure each module can be instantiated")

# A function that executes an R error action when a module produces a quantity access error
check_module_error <- function(module_error) {
	if (grepl("Caught quantity access error", module_error, fixed=TRUE) == TRUE) {
		stop(module_error)
	}
}

# A function that tries to instantiate and run a module, checking for any
# quantity access errors that may occur during instantiation
run_module_trial <- function(module_name)
{
	input_parameters <- get_module_info(module_name, verbose=FALSE)
	
	error_msg <- ""
	
	tryCatch(
		{
			# Code to be executed initially
			output_parameters <- test_module(module_name, input_parameters)
		},
		error=function(cond) {
			# Code for handling errors
			error_msg <<- cond
		},
		warning=function(cond) {
			# Code for handling warnings
			error_msg <<- cond
		},
		finally={
			# Code to be executed after the initial code and handling
		}
	)
	
	description <- paste("The ", module_name, " module can be instantiated", sep="")
	
	test_that(description, {
		expect_error(check_module_error(error_msg), regexp = NA)	# regexp = NA indicates that no error should be encountered
	})
}

# Test each module
all_modules <- get_all_modules()

for (module_name in all_modules) {
	run_module_trial(module_name)
}