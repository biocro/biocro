
##' Simulate crop growth.
##'
##' Models crop growth given species-specific parameters and climate data.
##' @param initial_values Initial values for parameters that will be calculated by the model. These are recorded in the result. (a list of named numeric values)
##' @param parameters Values for parameters that are constant. (a list of named numeric values)
##' @param varying_parameters Times at which a solution is desired and values for parameters that vary. (a data frame with at least the columns \code{year}, \code{doy}, and \code{hour} and optionally columns for parameters that drive the model over time.)
##' @param modules The model modules that should be run. Required modules are given in the details. (a list of named character values)
##' @return A data frame with the columns \code{year}, \code{doy}, and \code{hour}, and a column for each parameter in \code{initial_values} and a row for each row in \code{varying_parameters}:
##'
##' @details
##' The models are defined by two types of arguments: the set of equations -- \code{modules} -- and the state -- \code{initial_values}, \code{parameters}, \code{varying_parameters.}.
##'
##' Each module has parameters it requires, which must be given in exactly one of the state arguments.
##'
##' The choice of state argument in which to include a parameter depends on whether you want to record the parameter in the output and whether they are used to drive the model.
##'
##' \code{initial_values}: Values are recorded in the output. They do not drive the model. If the model calculates the parameter, it will change with time. If the model does not calculate the value, it will be constant with time, but will still be recorded.
##'
##' \code{parameters}: Values are not recorded in the output. They do not drive the model. If the model calculates the parameter, it will change internally, but will not be recorded in the output.
##'
##' \code{varying_parameters}: Values are not recored in the output. They do drive the model. If the model calculates the parameter, that parameter will be overwritten by the value in \code{varying_parameters}. Values are not recorded in the output, because they are known beforehand.
##'
##' The list \code{varying_parameters} must contain at least the columns \code{year}, \code{doy}, and \code{hour}, specifying the times at which a solution is required. Times must be evenly spaced, and the difference in times must be given in the \code{timestep} parameter (units are hours).
##'
##' Required module names and available modules:
##'
##' \itemize{
##'     \item \code{canopy_module_name}
##'         \itemize{
##'             \item c3canopy
##'             \item c4canopy
##'         }
##'     \item \code{soil_module_name}
##'         \itemize{
##'             \item one_layer_soil_profile
##'             \item two_layer_soil_profile
##'         }
##'     \item \code{growth_module_name}
##'         \itemize{
##'             \item partitioning_growth
##'         }
##'     \item \code{senescence_module_name}
##'         \itemize{
##'             \item thermal_time_senescence
##'             \item thermal_time_and_frost_senescence
##'         }
##' }
##'
##' Sets of parameters and modules are provided for sorghum, miscanthus, and willow, and are named [crop]_initial_state, [crop]_parameters, and [crop]_modules, e.g., sorghum_initial_state.
##' 
##' Weather data are provided. These are typically for one year (January 1 to December 31) and should be subsetted to include only the period of growth. The function \code{get_growing_season_climate()} is provided as one means of subsetting climate data.
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

