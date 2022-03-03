get_all_modules <- function(package_name) {
    error_messages <-
        check_vector(list(package_name = package_name))

    error_messages <- append(
        error_messages,
        check_length(list(package_name = package_name))
    )

    error_messages <- append(
        error_messages,
        check_element_length(list(package_name = package_name))
    )

    error_messages <- append(
        error_messages,
        check_strings(list(package_name = package_name))
    )

    send_error_messages(error_messages)

    # Try to find the internal get_all_modules function
    library_func <- tryCatch(
        {
            function_from_package(package_name, 'get_all_modules_internal')
        },
        error = function(cond) {
            stop(paste0(
                "Encountered an issue with module library `",
                package_name,
                "`: ",
                cond
            ))
        }
    )

    library_func()
}

get_all_quantities <- function(package_name) {
    error_messages <-
        check_vector(list(package_name = package_name))

    error_messages <- append(
        error_messages,
        check_length(list(package_name = package_name))
    )

    error_messages <- append(
        error_messages,
        check_element_length(list(package_name = package_name))
    )

    error_messages <- append(
        error_messages,
        check_strings(list(package_name = package_name))
    )

    send_error_messages(error_messages)

    # Try to find the internal get_all_quantities function
    library_func <- tryCatch(
        {
            function_from_package(package_name, 'get_all_quantities_internal')
        },
        error = function(cond) {
            stop(paste0(
                "Encountered an issue with module library `",
                package_name,
                "`: ",
                cond
            ))
        }
    )

    library_func()
}

get_all_ode_solvers <- function()
{
	.Call(R_get_all_ode_solvers)
}

get_all_se_solvers <- function()
{
	.Call(R_get_all_se_solvers)
}
