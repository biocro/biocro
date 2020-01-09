#include "system_solver.h"

// Create some variables that will be useful to any type of solver, and then call the private do_solve method
std::unordered_map<std::string, std::vector<double>> system_solver::solve(
    std::shared_ptr<System> sys,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps) const
{
    // Get the number of time points
    size_t ntimes = sys->get_ntimes();

    // Solve the system and return the results
    if (check_adaptive_compatible && !sys->is_adaptive_compatible()) {
        return this->handle_adaptive_incompatibility(output_step_size, adaptive_error_tol, adaptive_max_steps, ntimes, sys);
    }

    else {
        return this->do_solve(output_step_size, adaptive_error_tol, adaptive_max_steps, ntimes, sys);
    }
}

void boost_rsnbrk_system_solver::do_boost_solve(
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
    size_t ntimes,
    boost::numeric::ublas::vector<double> state,
    SystemCaller syscall,
    push_back_state_and_time<boost::numeric::ublas::vector<double>>& observer) const
{
    // Set up a rosenbrock stepper
    double const abs_err = adaptive_error_tol;
    double const rel_err = adaptive_error_tol;
    typedef boost::numeric::odeint::rosenbrock4<double> dense_stepper_type;

    // Use integrate_const to populate state_vec and time_vec
    boost::numeric::odeint::integrate_const(
        boost::numeric::odeint::make_dense_output<dense_stepper_type>(abs_err, rel_err),
        std::make_pair(syscall, syscall),
        state,
        0.0,
        ntimes - 1.0,
        output_step_size,
        observer,
        boost::numeric::odeint::max_step_checker(adaptive_max_steps));
}