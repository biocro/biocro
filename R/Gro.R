
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

    varying_parameters = as.data.frame(lapply(as.list(varying_parameters), as.numeric))  # C++ requires that all the variables have type `double  # C++ requires that all the variables have type `double`.
    names(varying_parameters) = tolower(names(varying_parameters))  # Convert all names to lower case for easy of use.

    ans = as.data.frame(.Call(RGro, initial_values, parameters, varying_parameters, modules$canopy_module_name, modules$soil_module_name, modules$growth_module_name, modules$senescence_module_name))
    ans = cbind(ans, varying_parameters)
    return(ans)
}

partial_gro = function(initial_values, parameters, varying_parameters, modules, arg_name) {
# Accepts the same parameters as Gro() with an addiation 'arg_name' parameter.
# Returns a function with that runs Gro() with all of the parameters, except 'arg_name'
# set as default. The only parameter in the new function is the value of 'arg_name'.
# This technique is called partial application, hence the name partial_gro.

# initial_values: same as Gro()
# parameters: same as Gro()
# varying_parameters: same as Gro()
# modules: same as Gro()
# arg_name: character. The name of the argument that will be the only argument of the new function.
#          It must be the name of a parameter in 'intial_values', 'parameters', or 'varying_parameters'.

# returns f(arg).
    if (!is.na(ind <- match(arg_name, names(initial_values)))) {
        control = 'initial_values'
        parm_ind = ind
    }
    else if (!is.na(ind <- match(arg_name, names(parameters)))) {
        control = 'parameters'
        parm_ind = ind
    }
    else if (!is.na(ind <- match(arg_name, names(varying_parameters)))) {
        control = 'varying_parameters'
        parm_ind = ind
    }
    else stop(paste(arg_name, " is not in any of the lists of parameters"))
    arg_list = list(initial_values=initial_values, parameters=parameters, varying_parameters=varying_parameters, modules=modules)
    function(x) {
        arg_list[[control]][[arg_name]] = x
        do.call(Gro, arg_list)
    }
}

