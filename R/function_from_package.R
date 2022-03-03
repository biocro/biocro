function_from_package <- function(package_name, function_name) {
    tryCatch(
        {
            do.call(`:::`, list(package_name, function_name))
        },
        error = function(cond) {
            stop(paste0(
                "A `",
                function_name,
                "` function could not be found in the `",
                package_name,
                "` package: ",
                cond
            ))
        }
    )
}
