#include "boost_ode_solvers.h"

void boost_rsnbrk_ode_solver::do_boost_integrate(
    dynamical_system_caller syscall,
    push_back_state_and_time<boost::numeric::ublas::vector<double>>& observer
)
{
    // Set up a rosenbrock stepper
    double const rel_err = get_adaptive_rel_error_tol();
    double const abs_err = get_adaptive_abs_error_tol();
    typedef boost::numeric::odeint::rosenbrock4<double> dense_stepper_type;
    auto stepper = boost::numeric::odeint::make_dense_output<dense_stepper_type>(abs_err, rel_err);

    // Run integrate_const
    run_integrate_const(stepper, syscall, observer);
}

std::string boost_rsnbrk_ode_solver::get_boost_param_info() const
{
    return std::string("\nRelative error tolerance: ") +
        std::to_string(get_adaptive_rel_error_tol()) +
        std::string("\nAbsolute error tolerance: ") +
        std::to_string(get_adaptive_abs_error_tol()) +
        std::string("\nMaximum attempts to find a new step size: ") +
        std::to_string(get_adaptive_max_steps());
}
