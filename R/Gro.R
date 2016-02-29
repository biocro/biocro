
Gro <- function(initial_values, parameters, varying_parameters, canopy_module_name)
{
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

    ans = as.data.frame(.Call(RGro, initial_values, parameters, varying_parameters, canopy_module_name))
    ans = cbind(ans, varying_parameters)
    return(ans)
}

