tensor_string_vector = function(x, y, sep = "_", order_by_left_first = FALSE){
    out = outer(x, y, FUN=paste, sep=sep)

    if (order_by_left_first){
        out = t(out)
    }

    as.vector(out)
}
