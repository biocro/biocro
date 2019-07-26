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

    module_names = paste(c('canopy', 'soil', 'growth', 'senescence', 'stomata_water_stress', 'leaf_water_stress'), '_module_name', sep='')
    if (any(unused_modules_ind <- !names(modules) %in% module_names)) {
        message = 'The following modules were supplied, but are not used by this function: '
        unused_modules = paste(names(modules)[unused_modules_ind], collaspe=', ')
        stop(paste(message, unused_modules, ',', sep=''))
    }
    
    if (any(null_ind <- unlist(lapply(modules[module_names], is.null)))) {
        message = 'The following modules names are NULL, but they must be defined: '
        missing_modules = paste(module_names[null_ind], collapse=', ')
        stop(paste(message, missing_modules, '.', sep=''))
    }

    varying_parameters = as.data.frame(lapply(as.list(varying_parameters), as.numeric))  # C++ requires that all the variables have type `double`.
    names(varying_parameters) = tolower(names(varying_parameters))  # Convert all names to lower case for ease of use.

    result = as.data.frame(.Call(R_Gro, initial_values, parameters, varying_parameters, modules$canopy_module_name, modules$soil_module_name, modules$growth_module_name, modules$senescence_module_name, modules$stomata_water_stress_module_name, modules$leaf_water_stress_module_name))
    result = cbind(varying_parameters[c('year', 'doy', 'hour')], result)
    return(result)
}

Gro_ode <- function(state, steady_state_modules, derivative_modules, check_parameters=TRUE)
{
    if (check_parameters) {
        if (class(state) != 'list') {
            stop('"state" must be a list.')
        }
        
        if (length(state) != length(unlist(state))) {
            item_lengths = unlist(lapply(state, length))
            error_message = sprintf("The following parameters have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
            stop(error_message)
        }
        
        state = lapply(state, as.numeric)  # C++ requires that all the variables have type `double`.
    }

    result = .Call(R_Gro_ode, state, steady_state_modules, derivative_modules)
    return(result)
}

run_modules <- function(state, steady_state_modules, check_parameters=TRUE)
{
    if (check_parameters) {
        if (class(state) != 'list') {
            stop('"state" must be a list.')
        }
        
        if (length(state) != length(unlist(state))) {
            item_lengths = unlist(lapply(state, length))
            error_message = sprintf("The following parameters have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
            stop(error_message)
        }
        
        state = lapply(state, as.numeric)  # C++ requires that all the variables have type `double`.
    }

    result = .Call(R_run_modules, state, steady_state_modules)
    return(result)
}

get_module_requirements <- function(modules)
{
    if (class(modules) != 'character') {
        stop('"modules" must be a character vector')
    }

    result = .Call(R_get_module_requirements, modules)
    return(result)
}

partial_gro = function(initial_values, parameters, varying_parameters, modules, arg_names) {
# Accepts the same parameters as Gro() with an additional 'arg_names' parameter, which is a vector of character variables.
# Returns a function that runs Gro() with all of the parameters, except 'arg_names
# set as default. The only parameter in the new function is the value of 'arg_names'.
# This technique is called partial application, hence the name partial_gro.

# initial_values: same as Gro()
# parameters: same as Gro()
# varying_parameters: same as Gro()
# modules: same as Gro()
# arg_name: vector of character variables. The names of the arguments that the new function accepts.
#          It must contain the names of parameters in 'intial_values', 'parameters', or 'varying_parameters'.

# returns f(arg).
#
# Example:
# senescence_gro = partial_gro(sorghum_initial_state, sorghum_parameters, weather05, sorghum_modules, c('seneLeaf', 'seneStem', 'seneRoot', 'seneRhizome'))
# result = senescence_gro(c(3000, 3000, 3000, 3000))

    arg_list = list(initial_values=initial_values, parameters=parameters, varying_parameters=varying_parameters, modules=modules)

    df = data.frame(control=character(), arg_name=character(), stringsAsFactors=FALSE)
    for (i in seq_along(arg_list)) {
        df = rbind(df, data.frame(control = names(arg_list)[i], arg_name=names(arg_list[[i]]), stringsAsFactors=FALSE))
    }

    controls = df[match(arg_names, df$arg_name), ]
    if (any(is.na(controls))) {
        missing = arg_names[which(is.na(controls$control))]
        stop(paste('The following arguments in "arg_names" are not in any of the paramter lists:', paste(missing, collapse=', ')))
    }
    function(x) {
        if (length(x) != length(arg_names)) stop("The length of x does not match the length of arguments when this function was defined.")
        x = unlist(x)
        temp_arg_list = arg_list
        for (i in seq_along(arg_names)) {
            c_row = controls[i, ]
            temp_arg_list[[c_row$control]][[c_row$arg_name]] = x[i]
        }
        do.call(Gro, temp_arg_list)
    }
}

# Gro_deriv is used to create a function that can be called by a solver such as `lsodes`.
# parameters: a list of named parameters that don't change with time.
# varying_parameters: a function of the form `f(t)` that returns a named list of parameters, where `t` is time.
# steady_state_modules: a character vector module names.
# derivative: a character vector of module names.

# A simple example with only derivatives.
# oscillator_system = Gro_deriv(list(), function(t) return(list()), c(), c('position_oscillator', 'velocity_oscillator'))
# state = c(position=0, velocity=1)
# times = seq(0, 5, length=100)
# result = as.data.frame(lsodes(state, times, oscillator_system))

Gro_deriv = function (parameters, varying_parameters, steady_state_modules, derivative_modules) {
    state_names = character(0)
    result_names = character(0)
    result_name_length = 0
    state_diff = character(0)
    result_diff = character(0)

    function(t, state, parms) {
        vp = varying_parameters(t)
        all_state = c(state, parameters, vp)
        result = .Call(R_Gro_ode, all_state, steady_state_modules, derivative_modules)
        temp = names(result)
        if (length(temp) != result_name_length) {
            state_names <<- names(state)
            result_names <<- temp
            result_name_length <<- length(result_names)
            state_diff <<- setdiff(state_names, result_names) 
            result_diff <<- setdiff(result_names, state_names)
        }
        result[state_diff] = 0
        derivatives = result[state_names]
        state_of_interest = c(result[result_diff], vp)
        return(list(derivatives, c(state_of_interest, derivatives)))
    }
}

