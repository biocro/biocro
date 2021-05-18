# module_wrapper_pointer returns an "R external pointer" object that points to a
# module_wrapper_base C++ object
module_wrapper_pointer <- function(module_name)
{
    # Make sure the module name is a string
	if (!is.character(module_name) & length(module_name) != 1) {
		stop('"module_name" must be a string')
	}

    return(.Call(R_module_wrapper_pointer, module_name))
}
