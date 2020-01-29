#ifndef BOOST_SOLVERS_H
#define BOOST_SOLVERS_H

#include "../system_solver.h"
#include <boost/numeric/ublas/vector.hpp>

// A class representing a generic boost system solver
template <class state_type>
class boost_system_solver : public system_solver
{
   public:
    boost_system_solver(std::string solver_name, bool check_adaptive_compatible) : system_solver(solver_name, check_adaptive_compatible) {}

   protected:
    template <class stepper_type>
    void run_integrate_const(stepper_type stepper, SystemCaller syscall, push_back_state_and_time<state_type> observer);

   private:
    state_type state;
    std::vector<state_type> state_vec;
    std::vector<double> time_vec;
    std::unordered_map<std::string, std::vector<double>> do_solve(std::shared_ptr<System> sys) override;
    virtual void do_boost_solve(SystemCaller syscall, push_back_state_and_time<state_type>& observer) = 0;
    std::string get_param_info() const override { return std::string("Output step size: ") + std::to_string(get_output_step_size()) + std::string("\n") + get_boost_param_info(); }  // All boost solvers use the output_step_size
    virtual std::string get_boost_param_info() const = 0;
};

// Store some information that will be useful to any type of boost solver, and then call the private do_boost_solve method
template <class state_type>
std::unordered_map<std::string, std::vector<double>> boost_system_solver<state_type>::do_solve(std::shared_ptr<System> sys)
{
    // Update and/or reset the stored objects
    sys->get_state(state);
    state_vec.clear();
    time_vec.clear();

    // Make an observer
    push_back_state_and_time<state_type> observer(state_vec, time_vec, sys->get_ntimes() - 1.0, false);

    // Make a system caller
    SystemCaller syscall(sys);

    // Solve the system (modifies state_vec and time_vec via the observer)
    do_boost_solve(syscall, observer);

    // Return the results
    return sys->get_results(state_vec, time_vec);
}

// Run integrate_const using stored information and the supplied stepper
template <class state_type>
template <class stepper_type>
void boost_system_solver<state_type>::run_integrate_const(stepper_type stepper, SystemCaller syscall, push_back_state_and_time<state_type> observer)
{
    try {
        boost::numeric::odeint::integrate_const(
            stepper,
            syscall,
            state,
            0.0,
            syscall.get_ntimes() - 1.0,
            get_output_step_size(),
            observer,
            boost::numeric::odeint::max_step_checker(get_adaptive_max_steps()));
    } catch (std::exception&) {
        // Don't do anything... just let the solver return the partial results
    }
}

// A class representing the boost euler solver
template <class state_type>
class boost_euler_system_solver : public boost_system_solver<state_type>
{
   public:
    boost_euler_system_solver() : boost_system_solver<state_type>("euler_odeint", false) {}

   private:
    void do_boost_solve(SystemCaller syscall, push_back_state_and_time<state_type>& observer) override
    {
        // Make an euler stepper
        typedef boost::numeric::odeint::euler<state_type, double, state_type, double> stepper_type;
        stepper_type stepper;

        // Run integrate_const
        this->run_integrate_const(stepper, syscall, observer);
    }
    std::string get_boost_param_info() const override { return std::string(""); }  // The boost Euler solver has no new parameters to report
};

// A class representing the boost RK4 solver
template <class state_type>
class boost_rk4_system_solver : public boost_system_solver<state_type>
{
   public:
    boost_rk4_system_solver() : boost_system_solver<state_type>("rk4", true) {}

   private:
    void do_boost_solve(SystemCaller syscall, push_back_state_and_time<state_type>& observer) override
    {
        // Make an rk4 stepper
        typedef boost::numeric::odeint::runge_kutta4<state_type, double, state_type, double> stepper_type;
        stepper_type stepper;

        // Run integrate_const
        this->run_integrate_const(stepper, syscall, observer);
    }
    std::string get_boost_param_info() const override { return std::string(""); }  // The boost RK4 solver has no new parameters to report
};

// A class representing the boost RKCK54 solver
template <class state_type>
class boost_rkck54_system_solver : public boost_system_solver<state_type>
{
   public:
    boost_rkck54_system_solver() : boost_system_solver<state_type>("rkck54", true) {}

   private:
    void do_boost_solve(SystemCaller syscall, push_back_state_and_time<state_type>& observer) override
    {
        // Set up an rkck54 stepper
        double const abs_err = this->get_adaptive_error_tol();
        double const rel_err = this->get_adaptive_error_tol();
        typedef boost::numeric::odeint::runge_kutta_cash_karp54<state_type, double, state_type, double> error_stepper_type;
        auto stepper = boost::numeric::odeint::make_controlled<error_stepper_type>(abs_err, rel_err);

        // Run integrate_const
        this->run_integrate_const(stepper, syscall, observer);
    }
    std::string get_boost_param_info() const override { return std::string("Error tolerance: ") + std::to_string(this->get_adaptive_error_tol()) + std::string("\nMaximum steps: ") + std::to_string(this->get_adaptive_max_steps()); }
};

// A class representing the boost rosenbrock solver
// Note that this solver is only compatible with boost::numeric::ublas::vector<double> state vectors
class boost_rsnbrk_system_solver : public boost_system_solver<boost::numeric::ublas::vector<double>>
{
   public:
    boost_rsnbrk_system_solver() : boost_system_solver<boost::numeric::ublas::vector<double>>("rsnbrk", true) {}

   private:
    void do_boost_solve(SystemCaller syscall, push_back_state_and_time<boost::numeric::ublas::vector<double>>& observer) override;
    std::string get_boost_param_info() const override;
};

#endif
