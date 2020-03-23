get_all_modules <- function()
{
	# Intended usage:
	#
	#  all_modules <- get_all_modules()
	#  View(all_modules)
	
	result = .Call(R_get_all_modules)
	return(result)
}

get_all_quantities <- function()
{
	# Example 1: getting a list of all unique quantities (which can be useful when writing a new module
	# that is intended to work along with pre-existing modules)
	#
	#  all_quantities <- get_all_quantities()
	#  all_quantities_names <- all_quantities$quantities_name
	#  all_unique_quantities <- unique(all_quantity_names)
	#  all_unique_quantities <- sort(all_unique_quantities)
	#  View(all_unique_quantities)
	#
	#
	# Example 2: getting a list of all modules that have "ci" or "Ci" as an input or output
	#
	#  all_quantities <- get_all_quantities()
	#  ci_modules <- subset(all_quantities, quantity_name=="ci" | quantity_name=="Ci")
	#  View(ci_modules)
	#
	#
	# Example 3: counting the number of modules that use each quantity as an input or output
	#
	#  install.packages("plyr")	# required for the count function
	#  library(plyr)
	#  all_quantities <- get_all_quantities()
	#  all_quantities_subset <- all_quantities[c("quantity_name", "module_name")]	# remove information about whether each quantity is an input or output
	#  all_quantities_subset <- unique(all_quantities_subset)						# remove any duplicated names where a quantity is both an input and an output for the same module
	#  quantities_usage <- count(all_quantities_subset, "quantity_name")
	#  View(quantities_usage)
	
	result = as.data.frame(.Call(R_get_all_quantities))
	return(result)
}

get_all_system_solvers <- function()
{
	# Intended usage:
	#
	#  View(get_all_system_solvers())
	
	result = .Call(R_get_all_system_solvers)
	return(result)
}

get_all_se_solvers <- function()
{
	# Intended usage:
	#
	#  View(get_all_se_solvers())
	
	result = .Call(R_get_all_se_solvers)
	return(result)
}