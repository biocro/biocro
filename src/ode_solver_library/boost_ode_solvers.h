#ifndef BOOST_ODE_SOLVERS_H
#define BOOST_ODE_SOLVERS_H

#include <boost/numeric/ublas/vector.hpp>
#include "../ode_solver.h"
#include "../dynamical_system_caller.h"
#include "../state_map.h"  // for state_vector_map

/**
 *  @brief A class representing a generic boost system ode_solver.
 */
template <class state_type>
class boost_ode_solver : public ode_solver
{
   public:
    boost_ode_solver(
        std::string ode_solver_name,
        bool check_euler_requirement,
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps)
        : ode_solver{
              ode_solver_name,
              check_euler_requirement,
              step_size,
              rel_error_tolerance,
              abs_error_tolerance,
              max_steps}
    {
    }

   protected:
    template <class stepper_type>
    void run_integrate_const(stepper_type stepper, dynamical_system_caller syscall, push_back_state_and_time<state_type> observer);

   private:
    std::string boost_error_string;
    size_t nsteps;

    state_type state;
    std::vector<state_type> state_vec;
    std::vector<double> time_vec;
    std::string observer_message;
    state_vector_map do_integrate(std::shared_ptr<dynamical_system> sys) override;
    virtual void do_boost_integrate(dynamical_system_caller syscall, push_back_state_and_time<state_type>& observer) = 0;

    std::string get_param_info() const override
    {
        // All boost ode_solvers use the output_step_size
        return std::string("\nOutput step size: ") +
               std::to_string(get_output_step_size()) +
               get_boost_param_info();
    }

    virtual std::string get_boost_param_info() const = 0;

    std::string get_solution_info() const override
    {
        if (boost_error_string.empty()) {
            return std::string("boost::numeric::odeint::integrate_const required ") +
                   std::to_string(nsteps) +
                   std::string(" steps to integrate the system\n\nThe observer reports the following:\n") +
                   observer_message;
        } else {
            return std::string("boost::numeric::odeint::integrate_const ") +
                   std::string("encountered an error and has returned ") +
                   std::string("a partial result:\n") + boost_error_string;
        }
    }
};

// Store some information that will be useful to any type of boost ode_solver, and then call the private do_boost_integrate method
template <class state_type>
state_vector_map boost_ode_solver<state_type>::do_integrate(std::shared_ptr<dynamical_system> sys)
{
    // Update and/or reset the stored objects
    sys->get_differential_quantities(state);
    state_vec.clear();
    time_vec.clear();
    observer_message = std::string("");

    // Make an observer
    push_back_state_and_time<state_type> observer(state_vec, time_vec, sys->get_ntimes() - 1.0, observer_message);

    // Make a system caller
    dynamical_system_caller syscall{sys};

    // integrate the system (modifies state_vec and time_vec via the observer)
    do_boost_integrate(syscall, observer);

    // Return the results
    return get_results_from_system(sys, state_vec, time_vec);
}

// Run integrate_const using stored information and the supplied stepper
template <class state_type>
template <class stepper_type>
void boost_ode_solver<state_type>::run_integrate_const(stepper_type stepper, dynamical_system_caller syscall, push_back_state_and_time<state_type> observer)
{
    try {
        nsteps = boost::numeric::odeint::integrate_const(
            stepper,
            syscall,
            state,
            0.0,
            syscall.get_ntimes() - 1.0,
            get_output_step_size(),
            observer,
            boost::numeric::odeint::max_step_checker(get_adaptive_max_steps()));
        boost_error_string.clear();
    } catch (std::exception& e) {
        // Store the error message and let the ode_solver return the partial results
        nsteps = 0;
        boost_error_string = std::string(e.what());
    }
}

// A class representing the boost Euler ode_solver
template <class state_type>
class boost_euler_ode_solver : public boost_ode_solver<state_type>
{
   public:
    boost_euler_ode_solver(
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps) : boost_ode_solver<state_type>("euler_odeint", false, step_size, rel_error_tolerance, abs_error_tolerance, max_steps) {}

   private:
    void do_boost_integrate(dynamical_system_caller syscall, push_back_state_and_time<state_type>& observer) override
    {
        // Make an euler stepper
        typedef boost::numeric::odeint::euler<state_type, double, state_type, double> stepper_type;
        stepper_type stepper;

        // Run integrate_const
        this->run_integrate_const(stepper, syscall, observer);
    }
    std::string get_boost_param_info() const override
    {
        // The boost Euler ode_solver has no new parameters to report
        return std::string("");
    }
};

// A class representing the boost RK4 ode_solver
template <class state_type>
class boost_rk4_ode_solver : public boost_ode_solver<state_type>
{
   public:
    boost_rk4_ode_solver(
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps) : boost_ode_solver<state_type>("rk4", true, step_size, rel_error_tolerance, abs_error_tolerance, max_steps) {}

   private:
    void do_boost_integrate(dynamical_system_caller syscall, push_back_state_and_time<state_type>& observer) override
    {
        // Make an rk4 stepper
        typedef boost::numeric::odeint::runge_kutta4<state_type, double, state_type, double> stepper_type;
        stepper_type stepper;

        // Run integrate_const
        this->run_integrate_const(stepper, syscall, observer);
    }
    std::string get_boost_param_info() const override
    {
        // The boost RK4 ode_solver has no new parameters to report
        return std::string("");
    }
};

// A class representing the boost RKCK54 ode_solver
template <class state_type>
class boost_rkck54_ode_solver : public boost_ode_solver<state_type>
{
   public:
    boost_rkck54_ode_solver(
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps) : boost_ode_solver<state_type>("rkck54", true, step_size, rel_error_tolerance, abs_error_tolerance, max_steps) {}

   private:
    void do_boost_integrate(dynamical_system_caller syscall, push_back_state_and_time<state_type>& observer) override
    {
        // Set up an rkck54 stepper
        double const rel_err = this->get_adaptive_rel_error_tol();
        double const abs_err = this->get_adaptive_abs_error_tol();
        typedef boost::numeric::odeint::runge_kutta_cash_karp54<state_type, double, state_type, double> error_stepper_type;
        auto stepper = boost::numeric::odeint::make_controlled<error_stepper_type>(abs_err, rel_err);

        // Run integrate_const
        this->run_integrate_const(stepper, syscall, observer);
    }
    std::string get_boost_param_info() const override
    {
        return std::string("\nRelative error tolerance: ") +
               std::to_string(this->get_adaptive_rel_error_tol()) +
               std::string("\nAbsolute error tolerance: ") +
               std::to_string(this->get_adaptive_abs_error_tol()) +
               std::string("\nMaximum attempts to find a new step size: ") +
               std::to_string(this->get_adaptive_max_steps());
    }
};

// A class representing the boost rosenbrock ode_solver
// Note that this ode_solver is only compatible with boost::numeric::ublas::vector<double> state vectors
class boost_rsnbrk_ode_solver : public boost_ode_solver<boost::numeric::ublas::vector<double>>
{
   public:
    boost_rsnbrk_ode_solver(
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps) : boost_ode_solver<boost::numeric::ublas::vector<double>>("rsnbrk", true, step_size, rel_error_tolerance, abs_error_tolerance, max_steps) {}

   private:
    void do_boost_integrate(
        dynamical_system_caller syscall,
        push_back_state_and_time<boost::numeric::ublas::vector<double>>& observer) override;

    std::string get_boost_param_info() const override;
};

#endif
