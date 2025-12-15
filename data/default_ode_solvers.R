default_ode_solvers <- list(
    auto = list(
        type = 'auto',
        output_step_size = 1.0,
        adaptive_rel_error_tol = 1e-4,
        adaptive_abs_error_tol = 1e-4,
        adaptive_max_steps = 200
    ),
    homemade_euler = list(
        type = 'homemade_euler',
        output_step_size = NA,
        adaptive_rel_error_tol = NA,
        adaptive_abs_error_tol = NA,
        adaptive_max_steps = NA
    ),
    boost_euler = list(
        type = 'boost_euler',
        output_step_size = 1,
        adaptive_rel_error_tol = NA,
        adaptive_abs_error_tol = NA,
        adaptive_max_steps = NA
    ),
    boost_rk4 = list(
        type = 'boost_rk4',
        output_step_size = 1,
        adaptive_rel_error_tol = NA,
        adaptive_abs_error_tol = NA,
        adaptive_max_steps = NA
    ),
    boost_rkck54 = list(
        type = 'boost_rkck54',
        output_step_size = 1.0,
        adaptive_rel_error_tol = 1e-4,
        adaptive_abs_error_tol = 1e-4,
        adaptive_max_steps = 200
    ),
    boost_rosenbrock = list(
        type = 'boost_rosenbrock',
        output_step_size = 1.0,
        adaptive_rel_error_tol = 1e-4,
        adaptive_abs_error_tol = 1e-4,
        adaptive_max_steps = 200
    )
)
