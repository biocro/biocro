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
    system_solver(
        std::string solver_name,
        bool check_adaptive_compatible) : solver_name(solver_name),
                                          check_adaptive_compatible(check_adaptive_compatible) {}
    virtual ~system_solver() = 0;  // Make the destructor a pure virtual function so that no objects can be made directly from this class
    std::unordered_map<std::string, std::vector<double>> solve(
        std::shared_ptr<System> sys,
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps) const;

   protected:
    std::string const solver_name;

   private:
    bool check_adaptive_compatible;
    virtual std::unordered_map<std::string, std::vector<double>> handle_adaptive_incompatibility(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps,
        size_t ntimes,
        std::shared_ptr<System> sys) const;
    virtual std::unordered_map<std::string, std::vector<double>> do_solve(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps,
        size_t ntimes,
        std::shared_ptr<System> sys) const;
};

// Pure virtual destructor must be redefined outside the class
inline system_solver::~system_solver()
{
}

// Throw an error if a derived class hasn't defined its own do_solve method
inline std::unordered_map<std::string, std::vector<double>> system_solver::do_solve(
    double /*output_step_size*/,
    double /*adaptive_error_tol*/,
    int /*adaptive_max_steps*/,
    size_t /*ntimes*/,
    std::shared_ptr<System> /*sys*/) const
{
    throw std::logic_error(std::string("system_solver '") + solver_name + std::string("' does not have a 'do_solve()' method defined.\n"));
}

// Define the standard response to a problem with adaptive compatibility
inline std::unordered_map<std::string, std::vector<double>> system_solver::handle_adaptive_incompatibility(
    double /*output_step_size*/,
    double /*adaptive_error_tol*/,
    int /*adaptive_max_steps*/,
    size_t /*ntimes*/,
    std::shared_ptr<System> /*sys*/) const
{
    throw std::logic_error("Thrown by system_solver: the system is not compatible with the chosen solution method.\n");
}

// A class representing our homemade euler solver
template <class state_type>
class homemade_euler_solver : public system_solver
{
   public:
    homemade_euler_solver() : system_solver("homemade_euler", false) {}

   private:
    std::unordered_map<std::string, std::vector<double>> do_solve(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps,
        size_t ntimes,
        std::shared_ptr<System> sys) const;
};

template <class state_type>
std::unordered_map<std::string, std::vector<double>> homemade_euler_solver<state_type>::do_solve(
    double /*output_step_size*/,
    double /*adaptive_error_tol*/,
    int /*adaptive_max_steps*/,
    size_t ntimes,
    std::shared_ptr<System> sys) const
{
    // Get the names of the output parameters and pointers to them
    std::vector<std::string> output_param_vector = sys->get_output_param_names();
    std::vector<const double*> output_ptr_vector = sys->get_output_ptrs();

    // Make the results map
    std::unordered_map<std::string, std::vector<double>> results;

    // Make the result vector
    std::vector<double> temp(ntimes);
    std::vector<std::vector<double>> result_vec(output_param_vector.size(), temp);

    // Get the current state in the correct format
    state_type state;
    sys->get_state(state);

    // Make a vector to store the derivative
    std::vector<double> dstatedt = state;

    // Run through all the times
    for (int t = 0; t < ntimes; t++) {
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
    boost_system_solver(
        std::string solver_name,
        bool check_adaptive_compatible) : system_solver(solver_name, check_adaptive_compatible) {}

   private:
    std::unordered_map<std::string, std::vector<double>> do_solve(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps,
        size_t ntimes,
        std::shared_ptr<System> sys) const;
    virtual void do_boost_solve(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps,
        size_t ntimes,
        state_type state,
        SystemCaller syscall,
        push_back_state_and_time<state_type>& observer) const;
};

// Create some variables that will be useful to any type of boost solver, and then call the private do_boost_solve method
template <class state_type>
std::unordered_map<std::string, std::vector<double>> boost_system_solver<state_type>::do_solve(
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
    size_t ntimes,
    std::shared_ptr<System> sys) const
{
    // Get the current state in the correct format
    state_type state;
    sys->get_state(state);

    // Make vectors to store the observer output
    std::vector<state_type> state_vec;
    std::vector<double> time_vec;

    // Make the observer
    push_back_state_and_time<state_type> observer(state_vec, time_vec, ntimes - 1.0, false);

    // Make a system caller
    SystemCaller syscall(sys);

    // Solve the system, storing the results in state_vec and time_vec
    do_boost_solve(
        output_step_size,
        adaptive_error_tol,
        adaptive_max_steps, ntimes,
        state,
        syscall,
        observer);

    // Return the results
    return sys->get_results(state_vec, time_vec);
}

// Throw an error if a derived class hasn't defined its own do_boost_solve method
template <class state_type>
inline void boost_system_solver<state_type>::do_boost_solve(
    double /*output_step_size*/,
    double /*adaptive_error_tol*/,
    int /*adaptive_max_steps*/,
    size_t /*ntimes*/,
    state_type /*state*/,
    SystemCaller /*syscall*/,
    push_back_state_and_time<state_type>& /*observer*/) const
{
    throw std::logic_error(std::string("boost_system_solver '") + this->solver_name + std::string("' does not have a 'do_boost_solve()' method defined.\n"));
}

// A class representing the boost euler solver
template <class state_type>
class boost_euler_system_solver : public boost_system_solver<state_type>
{
   public:
    boost_euler_system_solver() : boost_system_solver<state_type>("euler_odeint", false) {}

   private:
    void do_boost_solve(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps,
        size_t ntimes,
        state_type state,
        SystemCaller syscall,
        push_back_state_and_time<state_type>& observer) const;
};

template <class state_type>
void boost_euler_system_solver<state_type>::do_boost_solve(
    double output_step_size,
    double /*adaptive_error_tol*/,
    int /*adaptive_max_steps*/,
    size_t ntimes,
    state_type state,
    SystemCaller syscall,
    push_back_state_and_time<state_type>& observer) const
{
    // Make an euler stepper
    typedef boost::numeric::odeint::euler<state_type, double, state_type, double> stepper_type;
    stepper_type stepper;

    // Use integrate_const to populate state_vec and time_vec
    boost::numeric::odeint::integrate_const(
        stepper,
        syscall,
        state,
        0.0,
        ntimes - 1.0,
        output_step_size,
        observer);
}

// A class representing the boost RK4 solver
template <class state_type>
class boost_rk4_system_solver : public boost_system_solver<state_type>
{
   public:
    boost_rk4_system_solver() : boost_system_solver<state_type>("rk4", true) {}

   private:
    void do_boost_solve(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps,
        size_t ntimes,
        state_type state,
        SystemCaller syscall,
        push_back_state_and_time<state_type>& observer) const;
};

template <class state_type>
void boost_rk4_system_solver<state_type>::do_boost_solve(
    double output_step_size,
    double /*adaptive_error_tol*/,
    int /*adaptive_max_steps*/,
    size_t ntimes,
    state_type state,
    SystemCaller syscall,
    push_back_state_and_time<state_type>& observer) const
{
    // Make an rk4 stepper
    typedef boost::numeric::odeint::runge_kutta4<state_type, double, state_type, double> stepper_type;
    stepper_type stepper;

    // Use integrate_const to populate state_vec and time_vec
    boost::numeric::odeint::integrate_const(
        stepper,
        syscall,
        state,
        0.0,
        ntimes - 1.0,
        output_step_size,
        observer);
}

// A class representing the boost RKCK54 solver
template <class state_type>
class boost_rkck54_system_solver : public boost_system_solver<state_type>
{
   public:
    boost_rkck54_system_solver() : boost_system_solver<state_type>("rkck54", true) {}

   private:
    void do_boost_solve(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps,
        size_t ntimes,
        state_type state,
        SystemCaller syscall,
        push_back_state_and_time<state_type>& observer) const;
};

template <class state_type>
void boost_rkck54_system_solver<state_type>::do_boost_solve(
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
    size_t ntimes,
    state_type state,
    SystemCaller syscall,
    push_back_state_and_time<state_type>& observer) const
{
    // Set up an rkck54 stepper
    double const abs_err = adaptive_error_tol;
    double const rel_err = adaptive_error_tol;
    typedef boost::numeric::odeint::runge_kutta_cash_karp54<state_type, double, state_type, double> error_stepper_type;

    // Use integrate_const to populate state_vec and time_vec
    boost::numeric::odeint::integrate_const(
        boost::numeric::odeint::make_controlled<error_stepper_type>(abs_err, rel_err),
        syscall,
        state,
        0.0,
        ntimes - 1.0,
        output_step_size,
        observer,
        boost::numeric::odeint::max_step_checker(adaptive_max_steps));
}

// A class representing the boost rosenbrock solver
// Note that this solver is only compatible with boost::numeric::ublas::vector<double> state vectors
class boost_rsnbrk_system_solver : public boost_system_solver<boost::numeric::ublas::vector<double>>
{
   public:
    boost_rsnbrk_system_solver() : boost_system_solver<boost::numeric::ublas::vector<double>>("rsnbrk", true) {}

   private:
    void do_boost_solve(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps,
        size_t ntimes,
        boost::numeric::ublas::vector<double> state,
        SystemCaller syscall,
        push_back_state_and_time<boost::numeric::ublas::vector<double>>& observer) const;
};

// A class representing the auto solver which chooses default methods
template <class state_type>
class auto_solver : public system_solver
{
   public:
    auto_solver() : system_solver("auto", true) {}

   private:
    std::unordered_map<std::string, std::vector<double>> handle_adaptive_incompatibility(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps,
        size_t ntimes,
        std::shared_ptr<System> sys) const;
    std::unordered_map<std::string, std::vector<double>> do_solve(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps,
        size_t ntimes,
        std::shared_ptr<System> sys) const;
};

template <class state_type>
std::unordered_map<std::string, std::vector<double>> auto_solver<state_type>::do_solve(
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
    size_t /*ntimes*/,
    std::shared_ptr<System> sys) const
{
    // The system is compatible with adaptive step size methods, so make a rosenbrock solver to solve the system
    boost_rsnbrk_system_solver solver();
    return solver.solve(sys, output_step_size, adaptive_error_tol, adaptive_max_steps);
}

template <class state_type>
std::unordered_map<std::string, std::vector<double>> auto_solver<state_type>::handle_adaptive_incompatibility(
    double output_step_size,
    double adaptive_error_tol,
    int adaptive_max_steps,
    size_t /*ntimes*/,
    std::shared_ptr<System> sys) const
{
    // The system is not compatible with adaptive step size methods, so use an euler solver to solve the system
    homemade_euler_solver<state_type> solver();
    return solver.solve(sys, output_step_size, adaptive_error_tol, adaptive_max_steps);
}

#endif