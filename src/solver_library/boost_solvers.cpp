#include "boost_solvers.h"

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
