# This tests each module to make sure it can be run
# Problems can arise e.g. if a module's get_outputs() or get_inputs() functions don't actually represent its real inputs and outputs

context("Test to make sure each module can be run")

all_modules <- get_all_modules()

run_module_trial <- function(module_name)
{
	input_parameters <- get_module_info(module_name, verbose=FALSE)
	
	description <- paste("The ", module_name, " can run without throwing any exceptions", sep="")
	
	test_that(description, {
		expect_error(
			test_module(module_name, input_parameters),
			regexp = NA
		)
	})
}

for (module_name in all_modules) {
	run_module_trial(module_name)
}