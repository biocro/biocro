
Gro <- function(initial_values, parameters, varying_parameters, canopy_module_name, soil_module_name)
{
    # If 'initial_values' or 'parameters' are supplied as data.frames, convert them to lists.
    for (var in c('initial_values', 'parameters')) {
        if (class(get(var)) == 'data.frame') {
            temp_var = as.list(get(var)$value)
            names(temp_var) = get(var)$symbol
            assign(var, temp_var)
        }
    }

    if (class(initial_values) == 'data.frame')
    for ( ilist in list(initial_values, parameters) ) {
        item_lengths = unlist(lapply(ilist, length))
        if (!(all(item_lengths == 1))) {
            error_message = sprintf("The following parameters have lengths other than 1, but all parameters must have a length of exactly 1: %s.\n", paste(names(item_lengths)[which(item_lengths > 1)], collapse=', '))
            stop(error_message)
        }
    }

    varying_parameters = as.data.frame(lapply(as.list(varying_parameters), as.numeric))
    names(varying_parameters)[match(c('solarR', 'DailyTemp.C'), names(varying_parameters))] = c('solar', 'temp')
    names(varying_parameters) = tolower(names(varying_parameters))

    ans = as.data.frame(.Call(RGro, initial_values, parameters, varying_parameters, canopy_module_name, soil_module_name))
    ans = cbind(ans, varying_parameters)
    return(ans)
}

