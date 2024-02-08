default_ode_solver <- function(type = 'homemade_euler') {
    if (type == 'homemade_euler') {
        list(
            type = 'homemade_euler',
            output_step_size = NA,
            adaptive_rel_error_tol = NA,
            adaptive_abs_error_tol = NA,
            adaptive_max_steps = NA
        )
    } else if (type == 'boost_euler') {
        list(
            type = 'boost_euler',
            output_step_size = 1,
            adaptive_rel_error_tol = NA,
            adaptive_abs_error_tol = NA,
            adaptive_max_steps = NA
        )
    } else if (type == 'boost_rk4') {
        list(
            type = 'boost_rk4',
            output_step_size = 1,
            adaptive_rel_error_tol = NA,
            adaptive_abs_error_tol = NA,
            adaptive_max_steps = NA
        )
    } else if (type == 'boost_rkck54') {
        list(
            type = 'boost_rkck54',
            output_step_size = 1.0,
            adaptive_rel_error_tol = 1e-4,
            adaptive_abs_error_tol = 1e-4,
            adaptive_max_steps = 200
        )
    } else if (type == 'boost_rosenbrock') {
        list(
            type = 'boost_rosenbrock',
            output_step_size = 1.0,
            adaptive_rel_error_tol = 1e-4,
            adaptive_abs_error_tole = 1e-4,
            adaptive_max_steps = 200
        )
    } else {
        stop('Unsupported solver type:', type)
    }
}
