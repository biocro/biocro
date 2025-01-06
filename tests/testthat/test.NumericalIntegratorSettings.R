MAX_INDEX <- 100    # changing this value is not recommended
DEBUG_PRINT <- FALSE
DEBUG_PRINT_EXTRA <- FALSE

# Define a function that runs a harmonic oscillator simulation with the
# specified ode_solver parameters and returns the harmonic
# oscillator's final position. This function will produce a warning if the
# calculated time series does not have the expected length (determined from
# MAX_INDEX and the supplied output_step_size), which may occur if the numerical
# ode_solver ignores output_step_size or if it encounters a problem that
# requires it to abort the integration.
final_position <- function(ode_solver)
{
    result <- run_biocro(
        initial_values = list(
            position = 0.0,
            velocity = 1.0
        ),
        parameters = list(
            mass = 1.0,
            spring_constant = 1.0,
            timestep = 1.0
        ),
        drivers = data.frame(
            time=seq(from=0, by=1, length=MAX_INDEX)
        ),
        direct_module_names = c(),
        differential_module_names = "BioCro:harmonic_oscillator",
        ode_solver = ode_solver,
        verbose = DEBUG_PRINT_EXTRA
    )

    expected_length <- floor((MAX_INDEX - 1) / ode_solver$output_step_size) + 1
    actual_length <- length(result$position)

    if (DEBUG_PRINT_EXTRA) {
        str(
            list(
                expected_length = expected_length,
                actual_length = actual_length
            )
        )
    }

    if (actual_length < expected_length) {
        warning("The ode_solver did not produce the expected number of output points!")
    }

    return(result$position[length(result$position)])
}

# Specify ode_solver settings to use during the tests
default_output_step_size <- 1.0
default_adaptive_rel_error_tol <- 1e-4
default_adaptive_abs_error_tol <- 1e-8
default_adaptive_max_steps <- 200

small_output_step_size <- 0.1
large_output_step_size <- 10.0

bad_adaptive_rel_error_tol <- 5e-1
bad_adaptive_abs_error_tol <- 5e-1

better_adaptive_rel_error_tol <- 1e-3
better_default_adaptive_abs_error_tol <- 1e-6

best_adaptive_rel_error_tol <- 1e-6
best_adaptive_abs_error_tol <- 1e-10

bad_adaptive_max_steps <- 1

# Specify settings to use with the homemade Euler numerical ode_solver
homemade_euler_ode_solver_default <- list(
    type = 'homemade_euler',
    output_step_size = default_output_step_size,
    adaptive_rel_error_tol = default_adaptive_rel_error_tol,
    adaptive_abs_error_tol = default_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

homemade_euler_ode_solver_small_step <- list(
    type = 'homemade_euler',
    output_step_size = small_output_step_size,
    adaptive_rel_error_tol = default_adaptive_rel_error_tol,
    adaptive_abs_error_tol = default_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

# Specify settings to use with the ODEINT Euler numerical ode_solver
odeint_euler_ode_solver_default <- list(
    type = 'boost_euler',
    output_step_size = default_output_step_size,
    adaptive_rel_error_tol = default_adaptive_rel_error_tol,
    adaptive_abs_error_tol = default_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

odeint_euler_ode_solver_small_step <- list(
    type = 'boost_euler',
    output_step_size = small_output_step_size,
    adaptive_rel_error_tol = default_adaptive_rel_error_tol,
    adaptive_abs_error_tol = default_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

# Specify settings to use with the RK4 numerical ode_solver
rk4_ode_solver_default <- list(
    type = 'boost_rk4',
    output_step_size = default_output_step_size,
    adaptive_rel_error_tol = default_adaptive_rel_error_tol,
    adaptive_abs_error_tol = default_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

rk4_ode_solver_small_step <- list(
    type = 'boost_rk4',
    output_step_size = small_output_step_size,
    adaptive_rel_error_tol = default_adaptive_rel_error_tol,
    adaptive_abs_error_tol = default_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

# Specify settings to use with the RKCK54 numerical ode_solver
rkck54_ode_solver_bad <- list(
    type = 'boost_rkck54',
    output_step_size = large_output_step_size,
    adaptive_rel_error_tol = bad_adaptive_rel_error_tol,
    adaptive_abs_error_tol = bad_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

rkck54_ode_solver_better_rel <- list(
    type = 'boost_rkck54',
    output_step_size = large_output_step_size,
    adaptive_rel_error_tol = better_adaptive_rel_error_tol,
    adaptive_abs_error_tol = bad_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

rkck54_ode_solver_better_abs <- list(
    type = 'boost_rkck54',
    output_step_size = large_output_step_size,
    adaptive_rel_error_tol = bad_adaptive_rel_error_tol,
    adaptive_abs_error_tol = better_default_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

rkck54_ode_solver_error <- list(
    type = 'boost_rkck54',
    output_step_size = large_output_step_size,
    adaptive_rel_error_tol = bad_adaptive_rel_error_tol,
    adaptive_abs_error_tol = bad_adaptive_abs_error_tol,
    adaptive_max_steps = bad_adaptive_max_steps
)

# Specify settings to use with the RSNBRK numerical ode_solver
rsnbrk_ode_solver_bad <- list(
    type = 'boost_rosenbrock',
    output_step_size = large_output_step_size,
    adaptive_rel_error_tol = bad_adaptive_rel_error_tol,
    adaptive_abs_error_tol = bad_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

rsnbrk_ode_solver_better_rel <- list(
    type = 'boost_rosenbrock',
    output_step_size = large_output_step_size,
    adaptive_rel_error_tol = better_adaptive_rel_error_tol,
    adaptive_abs_error_tol = bad_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

rsnbrk_ode_solver_better_abs <- list(
    type = 'boost_rosenbrock',
    output_step_size = large_output_step_size,
    adaptive_rel_error_tol = bad_adaptive_rel_error_tol,
    adaptive_abs_error_tol = better_default_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

rsnbrk_ode_solver_best <- list(
    type = 'boost_rosenbrock',
    output_step_size = default_output_step_size,
    adaptive_rel_error_tol = best_adaptive_rel_error_tol,
    adaptive_abs_error_tol = best_adaptive_abs_error_tol,
    adaptive_max_steps = default_adaptive_max_steps
)

rsnbrk_ode_solver_error <- list(
    type = 'boost_rosenbrock',
    output_step_size = large_output_step_size,
    adaptive_rel_error_tol = bad_adaptive_rel_error_tol,
    adaptive_abs_error_tol = bad_adaptive_abs_error_tol,
    adaptive_max_steps = bad_adaptive_max_steps
)

# Run the tests
test_that(
    "We can successfully get an accurate calculation",
    {
        if (DEBUG_PRINT_EXTRA) {
            expect_output(best_result <- final_position(rsnbrk_ode_solver_best))
        } else {
            expect_silent(best_result <- final_position(rsnbrk_ode_solver_best))
        }
    }
)

# recalculate this outside of a test so its value can be used in other tests
best_result <- final_position(rsnbrk_ode_solver_best)

test_that(
    "The homemade Euler numerical ode_solver output is independent of output_step_size",
    {
        # This should produce a warning since the homemade Euler numerical
        # ode_solver should ignore the step size setting, ultimately producing
        # the wrong number of output points
        expect_warning(homemade_euler_result_small_step <- final_position(homemade_euler_ode_solver_small_step))

        expect_equal(
            homemade_euler_result_small_step,
            homemade_euler_result_default <- final_position(homemade_euler_ode_solver_default)
        )

        if (DEBUG_PRINT) {
            str(
                list(
                    name = "homemade euler test results",
                    final_position_small_step = homemade_euler_result_small_step,
                    final_position_default = homemade_euler_result_default
                )
            )
        }
    }
)

test_that(
    "The ODEINT Euler numerical ode_solver output is more accurate for smaller output_step_size",
    {
        odeint_euler_result_small_step <- final_position(odeint_euler_ode_solver_small_step)
        odeint_euler_result_default <- final_position(odeint_euler_ode_solver_default)

        expect_lt(
            abs(odeint_euler_result_small_step - best_result),
            abs(odeint_euler_result_default - best_result)
        )

        if (DEBUG_PRINT) {
            str(
                list(
                    name = "odeint euler test results",
                    final_position_best = best_result,
                    final_position_small_step = odeint_euler_result_small_step,
                    final_position_default = odeint_euler_result_default
                )
            )
        }
    }
)

test_that(
    "The ODEINT RK4 numerical ode_solver output is more accurate for smaller output_step_size",
    {
        rk4_result_small_step <- final_position(rk4_ode_solver_small_step)
        rk4_result_default <- final_position(rk4_ode_solver_default)

        expect_lt(
            abs(rk4_result_small_step - best_result),
            abs(rk4_result_default - best_result)
        )

        if (DEBUG_PRINT) {
            str(
                list(
                    name = "rk4 test results",
                    final_position_best = best_result,
                    final_position_small_step = rk4_result_small_step,
                    final_position_default = rk4_result_default
                )
            )
        }
    }
)

test_that(
    "The ODEINT RKCK54 numerical ode_solver output is more accurate for smaller tolerances",
    {
        rkck54_result_bad_settings <- final_position(rkck54_ode_solver_bad)
        rkck54_result_better_rel <- final_position(rkck54_ode_solver_better_rel)
        rkck54_result_better_abs <- final_position(rkck54_ode_solver_better_abs)

        expect_lt(
            abs(rkck54_result_better_rel - best_result),
            abs(rkck54_result_bad_settings - best_result)
        )

        expect_lt(
            abs(rkck54_result_better_abs - best_result),
            abs(rkck54_result_bad_settings - best_result)
        )

        if (DEBUG_PRINT) {
            str(
                list(
                    name = "rkck54 test results",
                    final_position_best = best_result,
                    final_position_bad = rkck54_result_bad_settings,
                    final_position_better_rel = rkck54_result_better_rel,
                    final_position_better_abs = rkck54_result_better_abs
                )
            )
        }
    }
)

test_that(
    "The ODEINT Rosenbrock numerical ode_solver output is more accurate for smaller tolerances",
    {
        rsnbrk_result_bad_settings <- final_position(rsnbrk_ode_solver_bad)
        rsnbrk_result_better_rel <- final_position(rsnbrk_ode_solver_better_rel)
        rsnbrk_result_better_abs <- final_position(rsnbrk_ode_solver_better_abs)

        expect_lt(
            abs(rsnbrk_result_better_rel - best_result),
            abs(rsnbrk_result_bad_settings - best_result)
        )

        expect_lt(
            abs(rsnbrk_result_better_abs - best_result),
            abs(rsnbrk_result_bad_settings - best_result)
        )

        if (DEBUG_PRINT) {
            str(
                list(
                    name = "rsnbrk test results",
                    final_position_best = best_result,
                    final_position_bad = rsnbrk_result_bad_settings,
                    final_position_better_rel = rsnbrk_result_better_rel,
                    final_position_better_abs = rsnbrk_result_better_abs
                )
            )
        }
    }
)

test_that(
    "Adaptive numerical ode_solvers fail for very low adaptive_max_steps",
    {
        expect_warning(final_position(rkck54_ode_solver_error))
        expect_warning(final_position(rsnbrk_ode_solver_error))
    }
)
