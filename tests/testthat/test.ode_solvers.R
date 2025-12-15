# These tests are designed to make sure each solver has default settings and can
# be run with these settings without causing errors. Other tests in
# `test.HarmonicOscillationModeling.R` check to make sure each solver accurately
# solves a simple set of ODEs.

# Specify the number of time points to use for testing
NTIMES <- 100

test_that('Each solver type has default settings', {
    ode_solver_types <- get_all_ode_solvers()

    ode_solver_settings <- names(default_ode_solvers)

    expect_true(
        all(ode_solver_types %in% ode_solver_settings)
    )
})

for (i in seq_along(default_ode_solvers)) {
    ode_solver_name <- names(default_ode_solvers)[i]
    ode_solver <- default_ode_solvers[[i]]

    test_name <- paste0(
        'The default settings for the `', ode_solver_name, '` ODE solver work'
    )

    test_that(test_name, {
        res <- expect_silent(
            run_biocro(
                list(position = 0, velocity = 1),
                list(mass = 1, spring_constant = 3, timestep = 1),
                drivers = data.frame(time = seq(0, by = 1, length.out = NTIMES)),
                'BioCro:harmonic_energy',
                'BioCro:harmonic_oscillator',
                ode_solver
            )
        )

        expect_equal(nrow(res), NTIMES)

        expect_true(all(!is.na(res[NTIMES, ])))
    })
}
