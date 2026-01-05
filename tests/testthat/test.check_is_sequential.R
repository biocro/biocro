# Set up harmonic oscillator inputs
times_sequential <- seq(0, 10, 1)            # gives correct behavior
times_repeated   <- c(0, rep(1:5, each = 3)) # repeats times
times_disordered <- c(0, 2, 1, 7, 10, 8, 3)  # not sequential or evenly spaced
times_reversed   <- rev(times_sequential)    # evenly spaced but not increasing
times_has_na     <- c(0,1,2, NA, 4, 5, 6)  
times_different_timestep <- seq(0, 10, 2)
times_sorted_but_nonlinear <- (0:9)^2       

testing_model <- list(
    initial_values = list(position = 1, velocity = 0),
    parameters = list(spring_constant = 1, mass = 1, timestep=1),
    direct_module_names = list(),
    differential_module_names = list("BioCro:harmonic_oscillator"),
    ode_solver = soybean$ode_solver
)

test_that('Time errors are caught', {
    # Sequential times should work
    expect_silent(
        with(testing_model, {run_biocro(
            initial_values,
            parameters,
            data.frame(time = times_sequential),
            direct_module_names,
            differential_module_names,
            ode_solver
        )})
    )
    
    # missing time variable
    expect_error(
        with(testing_model, {run_biocro(
            initial_values,
            parameters,
            data.frame(times = times_sequential),
            direct_module_names,
            differential_module_names,
            ode_solver
        )}),
        "^No `time` variable found"
    )

    
    # Repeated times should cause an error
    expect_error(
        with(testing_model, {run_biocro(
            initial_values,
            parameters,
            data.frame(time = times_repeated),
            direct_module_names,
            differential_module_names,
            ode_solver
        )}),
        "^Duplicates found"
    )

    # Disordered times should cause an error
    expect_error(
        with(testing_model, {run_biocro(
            initial_values,
            parameters,
            data.frame(time = times_disordered),
            direct_module_names,
            differential_module_names,
            ode_solver
        )}),
        regexp = "`time` not sorted"
    )

    # Reversed times should cause an error
    expect_error(
        with(testing_model, {run_biocro(
            initial_values,
            parameters,
            data.frame(time = times_reversed),
            direct_module_names,
            differential_module_names,
            ode_solver
        )}),
        regexp = "^`time` not sorted"
    )
    
    # Reversed times should cause an error
    expect_error(
        with(testing_model, {run_biocro(
            initial_values,
            parameters,
            data.frame(time = times_sorted_but_nonlinear),
            direct_module_names,
            differential_module_names,
            ode_solver
        )}),
        regexp = "^`time` not linear"
    )    
    
    # Reversed times should cause an error
    expect_error(
        with(testing_model, {run_biocro(
            initial_values,
            parameters,
            data.frame(time = times_different_timestep),
            direct_module_names,
            differential_module_names,
            ode_solver
        )}),
        regexp = "^Timestep mismatch"
    )

    # NAs should cause an error
    expect_error(
        with(testing_model, {run_biocro(
            initial_values,
            parameters,
            data.frame(time = times_has_na),
            direct_module_names,
            differential_module_names,
            ode_solver
        )}),
        regexp = "^Missing values found"
    )

    # Check single row drivers should pass.
    expect_silent(
        with(testing_model, {run_biocro(
            initial_values,
            parameters,
            data.frame(time = times_sequential[1]),
            direct_module_names,
            differential_module_names,
            ode_solver
        )})
    )

    # # Check two row drivers should pass.
    # expect_silent(
    #     with(testing_model, {run_biocro(
    #         initial_values,
    #         parameters,
    #         data.frame(time = times_sequential[1:2]),
    #         direct_module_names,
    #         differential_module_names,
    #         ode_solver
    #     )})
    # )
})
