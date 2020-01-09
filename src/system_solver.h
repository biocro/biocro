#ifndef SYSTEM_SOLVER_H
#define SYSTEM_SOLVER_H

#include <vector>
#include <boost/numeric/ublas/vector.hpp>  // For use with ODEINT
#include <boost/numeric/odeint.hpp>        // For use with ODEINT
#include "state_map.h"
#include "system.h"

// An abstract class for a generic system solver
// Its solve function provides a uniform interface for all derived solvers,
//  and its constructor requires inputs that are common to all solvers
class system_solver
{
   public:
    system_solver(std::string solver_name, bool check_adaptive_compatible) : solver_name(solver_name), check_adaptive_compatible(check_adaptive_compatible) {}
    virtual ~system_solver() = 0;  // Make the destructor a pure virtual function so that no objects can be made directly from this class

    std::unordered_map<std::string, std::vector<double>> solve(
        std::shared_ptr<System> sys,
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps);

   protected:
    std::string const solver_name;
    double output_step_size;
    double adaptive_error_tol;
    int adaptive_max_steps;
    size_t ntimes;

   private:
    bool check_adaptive_compatible;
    virtual std::unordered_map<std::string, std::vector<double>> do_solve(std::shared_ptr<System> sys);
    virtual std::unordered_map<std::string, std::vector<double>> handle_adaptive_incompatibility(std::shared_ptr<System> sys);
};

// Pure virtual destructor must be redefined outside the class
inline system_solver::~system_solver() {}

// Throw an error if a derived class hasn't defined its own do_solve method
inline std::unordered_map<std::string, std::vector<double>> system_solver::do_solve(std::shared_ptr<System> /*sys*/)
{
    throw std::logic_error(std::string("system_solver '") + solver_name + std::string("' does not have a 'do_solve()' method defined.\n"));
}

// Define the standard response to a problem with adaptive compatibility
inline std::unordered_map<std::string, std::vector<double>> system_solver::handle_adaptive_incompatibility(std::shared_ptr<System> /*sys*/)
{
    throw std::logic_error(std::string("system_solver '") + solver_name + std::string("' is not compatible with the input system.\n"));
}

// A class representing our homemade euler solver
template <class state_type>
class homemade_euler_solver : public system_solver
{
   public:
    homemade_euler_solver() : system_solver("homemade_euler", false) {}

   private:
    std::unordered_map<std::string, std::vector<double>> do_solve(std::shared_ptr<System> sys);
};

template <class state_type>
std::unordered_map<std::string, std::vector<double>> homemade_euler_solver<state_type>::do_solve(std::shared_ptr<System> sys)
{
    // Get the names of the output parameters and pointers to them
    std::vector<std::string> output_param_vector = sys->get_output_param_names();
    std::vector<const double*> output_ptr_vector = sys->get_output_ptrs();

    // Make the results map
    std::unordered_map<std::string, std::vector<double>> results;

    // Make the result vector
    std::vector<double> temp(this->ntimes);
    std::vector<std::vector<double>> result_vec(output_param_vector.size(), temp);

    // Get the current state in the correct format
    state_type state;
    sys->get_state(state);

    // Make a vector to store the derivative
    state_type dstatedt = state;

    // Run through all the times
    for (size_t t = 0; t < this->ntimes; t++) {
        // Update all the parameters and calculate the derivative based on the current time and state
        sys->operator()(state, dstatedt, t);

        // Store the current parameter values
        for (size_t i = 0; i < result_vec.size(); i++) (result_vec[i])[t] = *output_ptr_vector[i];

        // Update the state for the next step
        for (size_t j = 0; j < state.size(); j++) state[j] += dstatedt[j];  // The derivative has already been multiplied by the timestep
    }

    // Fill in the result map
    for (size_t i = 0; i < output_param_vector.size(); i++) results[output_param_vector[i]] = result_vec[i];

    // Add the number of derivative calculations
    std::fill(temp.begin(), temp.end(), sys->get_ncalls());
    results["ncalls"] = temp;

    return results;
}

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
    std::unordered_map<std::string, std::vector<double>> do_solve(std::shared_ptr<System> sys);
    virtual void do_boost_solve(SystemCaller syscall, push_back_state_and_time<state_type>& observer);
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
    push_back_state_and_time<state_type> observer(state_vec, time_vec, this->ntimes - 1.0, false);

    // Make a system caller
    SystemCaller syscall(sys);

    // Solve the system (modifies state_vec and time_vec via the observer)
    do_boost_solve(syscall, observer);

    // Return the results
    return sys->get_results(state_vec, time_vec);
}

// Throw an error if a derived class hasn't defined its own do_boost_solve method
template <class state_type>
inline void boost_system_solver<state_type>::do_boost_solve(SystemCaller /*syscall*/, push_back_state_and_time<state_type>& /*observer*/)
{
    throw std::logic_error(std::string("boost_system_solver '") + this->solver_name + std::string("' does not have a 'do_boost_solve()' method defined.\n"));
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
            this->ntimes - 1.0,
            this->output_step_size,
            observer,
            boost::numeric::odeint::max_step_checker(this->adaptive_max_steps));
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
    void do_boost_solve(SystemCaller syscall, push_back_state_and_time<state_type>& observer)
    {
        // Make an euler stepper
        typedef boost::numeric::odeint::euler<state_type, double, state_type, double> stepper_type;
        stepper_type stepper;

        // Run integrate_const
        this->run_integrate_const(stepper, syscall, observer);
    }
};

// A class representing the boost RK4 solver
template <class state_type>
class boost_rk4_system_solver : public boost_system_solver<state_type>
{
   public:
    boost_rk4_system_solver() : boost_system_solver<state_type>("rk4", true) {}

   private:
    void do_boost_solve(SystemCaller syscall, push_back_state_and_time<state_type>& observer)
    {
        // Make an rk4 stepper
        typedef boost::numeric::odeint::runge_kutta4<state_type, double, state_type, double> stepper_type;
        stepper_type stepper;

        // Run integrate_const
        this->run_integrate_const(stepper, syscall, observer);
    }
};

// A class representing the boost RKCK54 solver
template <class state_type>
class boost_rkck54_system_solver : public boost_system_solver<state_type>
{
   public:
    boost_rkck54_system_solver() : boost_system_solver<state_type>("rkck54", true) {}

   private:
    void do_boost_solve(SystemCaller syscall, push_back_state_and_time<state_type>& observer)
    {
        // Set up an rkck54 stepper
        double const abs_err = this->adaptive_error_tol;
        double const rel_err = this->adaptive_error_tol;
        typedef boost::numeric::odeint::runge_kutta_cash_karp54<state_type, double, state_type, double> error_stepper_type;
        auto stepper = boost::numeric::odeint::make_controlled<error_stepper_type>(abs_err, rel_err);

        // Run integrate_const
        this->run_integrate_const(stepper, syscall, observer);
    }
};

// A class representing the boost rosenbrock solver
// Note that this solver is only compatible with boost::numeric::ublas::vector<double> state vectors
class boost_rsnbrk_system_solver : public boost_system_solver<boost::numeric::ublas::vector<double>>
{
   public:
    boost_rsnbrk_system_solver() : boost_system_solver<boost::numeric::ublas::vector<double>>("rsnbrk", true) {}

   private:
    void do_boost_solve(SystemCaller syscall, push_back_state_and_time<boost::numeric::ublas::vector<double>>& observer);
};

// A class representing the auto solver which chooses default methods
template <class state_type>
class auto_solver : public system_solver
{
   public:
    auto_solver() : system_solver("auto", true) {}

   private:
    std::unordered_map<std::string, std::vector<double>> do_solve(std::shared_ptr<System> sys)
    {
        // The system is compatible with adaptive step size methods, so use a rosenbrock solver to solve the system
        boost_rsnbrk_system_solver solver;
        return solver.solve(sys, this->output_step_size, this->adaptive_error_tol, this->adaptive_max_steps);
    }

    std::unordered_map<std::string, std::vector<double>> handle_adaptive_incompatibility(std::shared_ptr<System> sys)
    {
        // The system is not compatible with adaptive step size methods, so use an euler solver to solve the system
        homemade_euler_solver<state_type> solver;
        return solver.solve(sys, this->output_step_size, this->adaptive_error_tol, this->adaptive_max_steps);
    }
};

#endif