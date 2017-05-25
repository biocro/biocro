read_parameters = function(file_name) {
    parms_df = read.delim(file=file_name)
    parms = as.list(parms_df$value)
    names(parms) = parms_df$symbol
    return(parms)
}

