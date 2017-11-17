
##' Model crop growth.
##'
##' \code{Gro()} models crop growth over time given species-specific parameters and climate data. It can be used to predict yield, determine regions that are suitable for growing a crop, and identify traits that have the most influence on yield.
##' @param initial_values Parameter values that are known only at the start of the simulation period. These are recorded in the result. (a list of named numeric values)
##' @param parameters Parameter values that are constant for the entire simulation period. These are not recorded in the result. (a list of named numeric values)
##' @param varying_parameters Times at which a solution is desired and parameter values that are known \emph{a priori} for the entire simulation period. These are not recorded in the result. (a data frame with at least the columns \code{year}, \code{doy}, and \code{hour} and optionally columns for parameters that are known \emph{a priori}.)
##' @param modules The model modules that should be run. Required modules are given in the details. (a list of named character values)
##' @return A data frame with the columns \code{year}, \code{doy}, and \code{hour}; and a column for each parameter in \code{initial_values}; and a row for each row in \code{varying_parameters}:
##'
##' @details
##' \subsection{An overview of model definitions}{
##' A model can be fully defined using two types of arguments: (1) the set of equations and (2) the parameters required by those equations. The set of all parameters in the model is commonly called the state.
##' 
##' State values can be known, \emph{a priori}, for the entire simulation (\emph{a priori} state), or known only at the start of the simulation period (calculated state).
##' }
##'
##' \subsection{Model implementation using Gro()}{
##' In Gro(), the sets of equations are passed via the modules argument, and the state is passed via the \code{initial_values}, \code{parameters}, and \code{varying_parameters} arguments.
##' 
##' The set of modules determines the parameters that must be defined in the state arguments. Each module has parameters it requires, which must be defined in exactly one of the state arguments
##'
##' The state argument in which a parameter is defined determines whether a parameter is considered calculated state or \emph{a priori} state. Parameters defined in \code{initial_values} are considered calculated state, whereas parameters defined in either \code{parameters} or \code{varying_parameters} are considered \emph{a priori} state. The difference between \code{parameters} and \code{varying_parameters} is that values in \code{parameters} are considered constant for the simulation period, whereas values in \code{varying_parameters} may be different for each time point. Only calculated state variables are recorded in the output. A priori state variable values will overwrite any calculations of those values performed internally by the model.
##' }
##'
##'
##' \subsection{State argument details}{
##' \code{initial_values}: Parameter values that are known only at the start of the simulation period. Parameter values are recorded in the output. If the model calculates the parameter value, it will change with time. If the model does not calculate the value, it will be constant with time, but it will still be recorded.
##'
##' \code{parameters}: Parameter values that are known and constant for the entire simulation period. Values are not recorded in the output. If the model calculates the parameter value, the calculated value will be overwritten by the value in \code{parameters}.
##'
##' \code{varying_parameters}: Parameter values that are known \emph{a priori} at every time step in the entire simulation period. Values are not recorded in the output. If the model calculates the parameter value, the calculated value will be overwritten by the value in \code{varying_parameters}.
##'
##' The data frame \code{varying_parameters} must contain at least the columns \code{year}, \code{doy}, and \code{hour}, specifying the times at which a solution is required. Times must be evenly spaced, and the difference in times must be given in the \code{timestep} parameter (units are hours).
##' }
##'
##' \subsection{Required module names and available modules}{
##' \itemize{
##'     \item \code{canopy_module_name}
##'         \itemize{
##'             \item c3canopy, c4canopy
##'         }
##'     \item \code{soil_module_name}
##'         \itemize{
##'             \item one_layer_soil_profile, two_layer_soil_profile
##'         }
##'     \item \code{growth_module_name}
##'         \itemize{
##'             \item partitioning_growth
##'         }
##'     \item \code{senescence_module_name}
##'         \itemize{
##'             \item thermal_time_senescence, thermal_time_and_frost_senescence
##'         }
##' }
##' }
##'
##' \subsection{Precompiled module and parameter sets}{
##' Sets of parameters and modules are provided for sorghum, miscanthus, and willow, and are named [crop]_initial_state, [crop]_parameters, and [crop]_modules, e.g., sorghum_initial_state.
##' 
##' Weather data are provided. These are typically for one year (January 1 to December 31) and should be subsetted to include only the period of growth. The function \code{get_growing_season_climate()} is provided as one means of subsetting climate data.
##' }
##'
##' @examples
##' ## Simulate sorghum growth
##' result = Gro(sorghum_initial_state, sorghum_parameters, get_growing_season_climate(weather05), sorghum_modules)
##' lattice::xyplot(Stem + Leaf + Root ~ TTc, data=result, type='l', auto=list(points=FALSE, lines=TRUE), ylab=expression('Biomass'~(Mg/ha)), xlab=expression('Thermal time'~(degree*C~day)))
##' 
##' ### The results can be combined with the data that were used to drive the model.
##' result = cbind(result, get_growing_season_climate(weather05))
##' 
##' ## Change a subset of the parameters.
##' canopy_architecture = within(sorghum_parameters, {chil = 1.2; leafwidth = 0.05})
##' architechture_result = Gro(sorghum_initial_state, canopy_architecture, get_growing_season_climate(weather05), sorghum_modules)
##' lattice::xyplot(Stem + Leaf + Root ~ TTc, data=architechture_result, type='l', auto=list(points=FALSE, lines=TRUE), ylab=expression('Biomass'~(Mg/ha)), xlab=expression('Thermal time'~(degree*C~day)))
##'

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

Gro_deriv = function(parameters, varying_parameters, steady_state_modules, derivative_modules) {
    function(t, state, parms) {
        vp = varying_parameters(t)
        all_state = c(state, parameters, vp)
        #result = Gro_ode(all_state, steady_state_modules, derivative_modules, check_parameters=FALSE)
        result = .Call(BioCro:::R_Gro_ode, all_state, steady_state_modules, derivative_modules)
        result[setdiff(names(state), names(result))] = 0
        derivatives = result[names(state)]
        state_of_interest = c(result[setdiff(names(result), names(state))], vp)
        return(list(derivatives, c(state_of_interest, derivatives)))
    }
}

