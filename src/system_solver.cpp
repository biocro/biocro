#include "system_solver.h"

// Store some information that will be useful to any type of solver, and then call the private do_solve method
std::unordered_map<std::string, std::vector<double>> system_solver::solve(
    std::shared_ptr<System> sys,
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps)
{
    // Update the stored objects
    this->output_step_size = output_step_size;
    this->adaptive_error_tol = adaptive_error_tol;
    this->adaptive_max_steps = adaptive_max_steps;
    this->ntimes = sys->get_ntimes();

    // Solve the system and return the results
    if (check_adaptive_compatible && !sys->is_adaptive_compatible()) {
        return this->handle_adaptive_incompatibility(sys);
    }

    else {
        return this->do_solve(sys);
    }
}

void boost_rsnbrk_system_solver::do_boost_solve(SystemCaller syscall, push_back_state_and_time<boost::numeric::ublas::vector<double>>& observer)
{
    // Set up a rosenbrock stepper
    double const abs_err = this->adaptive_error_tol;
    double const rel_err = this->adaptive_error_tol;
    typedef boost::numeric::odeint::rosenbrock4<double> dense_stepper_type;
    auto stepper = boost::numeric::odeint::make_dense_output<dense_stepper_type>(abs_err, rel_err);

    // Run integrate_const
    this->run_integrate_const(stepper, syscall, observer);
}