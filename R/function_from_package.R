# A function for retrieving another function from a particular package namespace
# using the `:::` operator. This function is used internally by other BioCro
# functions, and users typically do not need to access it. Its main purpose is
# to locate unexported functions like `get_all_modules_internal` from R packages
# that represent BioCro module libraries.
function_from_package <- function(library_name, function_name) {
    tryCatch(
        {
            do.call(`:::`, list(library_name, function_name))
        },
        error = function(cond) {
            stop(paste0(
                "A `",
                function_name,
                "` function could not be found in the `",
                library_name,
                "` package: ",
                cond
            ))
        }
    )
}
