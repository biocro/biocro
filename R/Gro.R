
Gro <- function(initial_values, parameters, varying_parameters, modules)
{
    for ( ilist in list(initial_values, parameters) ) {
        if (class(ilist) != 'list') {
            stop('"initial_values" and "parameters" must each be a list.')
        }
        
        item_lengths = unlist(lapply(ilist, length))
        if (!(all(item_lengths == 1))) {
            error_message = sprintf("The following parameters have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
            stop(error_message)
        }
    }
    module_names = paste(c('canopy', 'soil', 'growth', 'senescence'), '_module_name', sep='')
    
    if (any(null_ind <- unlist(lapply(modules[module_names], is.null)))) {
        message = 'The following modules names are NULL, but they must be defined: '
        missing_modules = paste(module_names[null_ind], collapse=', ')
        stop(paste(message, missing_modules, '.', sep=''))
    }

    varying_parameters = as.data.frame(lapply(as.list(varying_parameters), as.numeric))  # C++ requires that all the variables have type `double  # C++ requires that all the variables have type `double`.
    names(varying_parameters) = tolower(names(varying_parameters))  # Convert all names to lower case for easy of use.

    result = as.data.frame(.Call(R_Gro, initial_values, parameters, varying_parameters, modules$canopy_module_name, modules$soil_module_name, modules$growth_module_name, modules$senescence_module_name))
    result = cbind(varying_parameters[c('year', 'doy', 'hour')], result)
    return(result)
}

partial_gro = function(initial_values, parameters, varying_parameters, modules, arg_name) {
# Accepts the same parameters as Gro() with an additional 'arg_name' parameter.
# Returns a function that runs Gro() with all of the parameters, except 'arg_name'
# set as default. The only parameter in the new function is the value of 'arg_name'.
# This technique is called partial application, hence the name partial_gro.

# initial_values: same as Gro()
# parameters: same as Gro()
# varying_parameters: same as Gro()
# modules: same as Gro()
# arg_name: character. The name of the argument that will be the only argument of the new function.
#          It must be the name of a parameter in 'intial_values', 'parameters', or 'varying_parameters'.

# returns f(arg).
    arg_list = list(initial_values=initial_values, parameters=parameters, varying_parameters=varying_parameters, modules=modules)
    for (i in seq_along(arg_list)) {
        if (!is.na(ind <- match(arg_name, names(arg_list[[i]])))) {
            control = names(arg_list)[i]
            parm_ind = ind
            break
        }
    }
    if (is.na(ind)) stop(paste(arg_name, " is not in any of the lists of parameters."))

    function(x) {
        arg_list[[control]][[arg_name]] = x
        do.call(Gro, arg_list)
    }
}

