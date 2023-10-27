get_all_modules <- function(library_name) {
    error_messages <-
        check_vector(list(library_name = library_name))

    error_messages <- append(
        error_messages,
        check_length(list(library_name = library_name))
    )

    error_messages <- append(
        error_messages,
        check_element_length(list(library_name = library_name))
    )

    error_messages <- append(
        error_messages,
        check_strings(list(library_name = library_name))
    )

    send_error_messages(error_messages)

    # Try to find the internal get_all_modules function
    library_func <- tryCatch(
        {
            function_from_package(library_name, 'get_all_modules_internal')
        },
        error = function(cond) {
            stop(paste0(
                "Encountered an issue with module library `",
                library_name,
                "`: ",
                cond
            ))
        }
    )

    module_paste(library_name, library_func())
}

get_all_quantities <- function(library_name) {
    error_messages <-
        check_vector(list(library_name = library_name))

    error_messages <- append(
        error_messages,
        check_length(list(library_name = library_name))
    )

    error_messages <- append(
        error_messages,
        check_element_length(list(library_name = library_name))
    )

    error_messages <- append(
        error_messages,
        check_strings(list(library_name = library_name))
    )

    send_error_messages(error_messages)

    # Try to find the internal get_all_quantities function
    library_func <- tryCatch(
        {
            function_from_package(library_name, 'get_all_quantities_internal')
        },
        error = function(cond) {
            stop(paste0(
                "Encountered an issue with module library `",
                library_name,
                "`: ",
                cond
            ))
        }
    )

    all_quantities <- library_func()

    # The C++ code returns local module names, but here we want the
    # fully-qualified module names
    all_quantities[['module_name']] <-
        module_paste(library_name, all_quantities[['module_name']])

    all_quantities
}

get_all_ode_solvers <- function()
{
	.Call(R_get_all_ode_solvers)
}
