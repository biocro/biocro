solve_simultaneous_equations <- function(
    direct_modules,
    known_quantities,
    unknown_quantities,
    lower_bounds,
    upper_bounds,
    abs_error_tols,
    rel_error_tols,
    max_it = 50,
    solver_type = "newton_raphson_backtrack_boost",
    silent = FALSE)
{
    # C++ requires that all the variables have type `double`
    known_quantities <- lapply(known_quantities, as.numeric)
    unknown_quantities <- lapply(unknown_quantities, as.numeric)
    lower_bounds <- lapply(lower_bounds, as.numeric)
    upper_bounds <- lapply(upper_bounds, as.numeric)
    abs_error_tols <- lapply(abs_error_tols, as.numeric)
    rel_error_tols <- lapply(rel_error_tols, as.numeric)
    max_it <- as.numeric(max_it)

    # Make sure silent is a logical variable
    silent <- lapply(silent, as.logical)

    # Make module creators from the specified names and libraries
    direct_module_creators <- sapply(
        direct_modules,
        check_out_module
    )

    # Run the C++ code
    result <- .Call(
        R_solve_simultaneous_equations,
        known_quantities,
        unknown_quantities,
        lower_bounds,
        upper_bounds,
        abs_error_tols,
        rel_error_tols,
        direct_module_creators,
        solver_type,
        max_it,
        silent)

    return(as.data.frame(result))
}


validate_simultaneous_equations <- function(direct_modules, known_quantities, unknown_quantities, silent = FALSE)
{
    # C++ requires that all the variables have type `double`
    known_quantities <- lapply(known_quantities, as.numeric)

    # Make sure silent is a logical variable
    silent <- lapply(silent, as.logical)

    # Make module creators from the specified names and libraries
    direct_module_creators <- sapply(
        direct_modules,
        check_out_module
    )

    # Run the C++ code
    result <- .Call(R_validate_simultaneous_equations, known_quantities, unknown_quantities, direct_module_creators, silent)
    return(result)
}

test_simultaneous_equations <- function(direct_modules, known_quantities, unknown_quantities)
{
    # C++ requires that all the variables have type `double`
    known_quantities <- lapply(known_quantities, as.numeric)
    unknown_quantities <- lapply(unknown_quantities, as.numeric)

    # Make module creators from the specified names and libraries
    direct_module_creators <- sapply(
        direct_modules,
        check_out_module
    )

    # Run the C++ code
    result <- .Call(R_test_simultaneous_equations, known_quantities, unknown_quantities, direct_module_creators)
    return(result)
}
