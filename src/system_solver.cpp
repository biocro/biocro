#include "system_solver.h"

void boost_rsnbrk_system_solver::do_boost_solve(
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
        this->output_step_size,
        observer);
}