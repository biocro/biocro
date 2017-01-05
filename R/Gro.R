
Gro <- function(initial_values, parameters, varying_parameters, modules)
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
    # The column names are not consistent the in the BioCro weather data sets. They should be unified, but for now find them and rename them.
    column_inds = match(c('solarR', 'DailyTemp.C'), names(varying_parameters))  # Get the indexes of the columns.
    not_na_inds = !is.na(column_inds)  # There is no guaruntee that the column is in the data set, so keep track of which ones were found.
    names(varying_parameters)[column_inds[not_na_inds]] = c('solar', 'temp')[not_na_inds]
    names(varying_parameters) = tolower(names(varying_parameters))

    ans = as.data.frame(.Call(RGro, initial_values, parameters, varying_parameters, modules$canopy_module_name, modules$soil_module_name))
    ans = cbind(ans, varying_parameters)
    return(ans)
}

