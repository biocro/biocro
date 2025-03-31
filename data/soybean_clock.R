soybean_clock <- list(
    direct_modules = list(
        "BioCro:format_time",
        "BioCro:light_from_solar",
        "BioCro:oscillator_clock_calculator"
    ),
    differential_modules = list(
        "BioCro:night_and_day_trackers",
        "BioCro:poincare_clock"
    ),
    ode_solver = list(
        type = 'boost_rkck54',
        output_step_size = 1.0,
        adaptive_rel_error_tol = 1e-4,
        adaptive_abs_error_tol = 1e-4,
        adaptive_max_steps = 200
    ),
    parameters = list(
        clock_gamma = 0.1,
        clock_period = 24.0,
        clock_r0 = 1.5,
        kick_strength = 0.8,
        light_exp_at_zero = 10,
        light_threshold = 60,
        timestep = 1.0,
        tracker_rate = 4.6
    )
)

soybean_clock$initial_values = list(
    dawn_a = soybean_clock$parameters$clock_r0 * cos(200 * pi / 180),
    dawn_b = soybean_clock$parameters$clock_r0 * sin(200 * pi / 180),
    day_tracker = 0.0,
    dusk_a = soybean_clock$parameters$clock_r0 * cos(80 * pi / 180),
    dusk_b = soybean_clock$parameters$clock_r0 * sin(80 * pi / 180),
    night_tracker = 1.0,
    ref_a = 1.0,
    ref_b = 0.0
)
