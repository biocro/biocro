#ifndef SYSTEM_SOLVER_H
#define SYSTEM_SOLVER_H

#include <vector>
#include <boost/numeric/ublas/vector.hpp>  // For use with ODEINT
#include <boost/numeric/odeint.hpp>        // For use with ODEINT
#include "state_map.h"
#include "system.h"

// An abstract class for a generic system solver
// Its operator() provides a uniform interface for all derived solvers
template <class state_type>
class system_solver
{
   public:
    system_solver(
        std::string solver_name,
        double output_step_size,
        bool check_adaptive_compatible) : solver_name(solver_name),
                                          output_step_size(output_step_size),
                                          check_adaptive_compatible(check_adaptive_compatible)
    {
    }
    virtual ~system_solver() = 0;  // Make the destructor a pure virtual function so that no objects can be made directly from this class
    std::unordered_map<std::string, std::vector<double>> operator()(std::shared_ptr<System> sys) const;

   private:
    virtual std::unordered_map<std::string, std::vector<double>> do_solve(size_t ntimes, state_type state, std::shared_ptr<System> sys) const;
    bool check_adaptive_compatible;

   protected:
    std::string const solver_name;
    double const output_step_size;
};

// Destructor must be defined outside the class
template <class state_type>
inline system_solver<state_type>::~system_solver()
{
}

// Create some variables that will be useful to any type of solver, and then call the private do_solve method
template <class state_type>
std::unordered_map<std::string, std::vector<double>> system_solver<state_type>::operator()(std::shared_ptr<System> sys) const
{
    // Check compatibility with adaptive step sizes
    if (check_adaptive_compatible && !sys->is_adaptive_compatible()) throw std::logic_error("Thrown by system_solver: the system is not compatible with the chosen solution method.\n");

    // Get the number of time points
    size_t ntimes = sys->get_ntimes();

    // Get the current state in the correct format
    state_type state;
    sys->get_state(state);

    // Solve the system and return the results
    return this->do_solve(ntimes, state, sys);
}

// Throw an error if a derived class hasn't defined its own do_solve method
template <class state_type>
inline std::unordered_map<std::string, std::vector<double>> system_solver<state_type>::do_solve(size_t ntimes, state_type state, std::shared_ptr<System> sys) const
{
    throw std::logic_error(std::string("system_solver '") + solver_name + std::string("' does not have a 'do_solve()' method defined.\n"));
}

// A class representing a generic boost system solver
template <class state_type, class stepper_type>
class boost_system_solver : public system_solver<state_type>
{
   public:
    boost_system_solver(
        std::string solver_name,
        double output_step_size,
        int adaptive_max_steps,
        bool check_adaptive_compatible) : system_solver<state_type>(solver_name, output_step_size, check_adaptive_compatible),
                                          adaptive_max_steps(adaptive_max_steps)
    {
    }

   private:
    std::unordered_map<std::string, std::vector<double>> do_solve(size_t ntimes, state_type state, std::shared_ptr<System> sys) const;
    virtual void do_boost_solve(size_t ntimes, state_type state, std::vector<state_type>& state_vec, std::vector<double>& time_vec, SystemCaller syscall) const;

   protected:
    int const adaptive_max_steps;
    stepper_type stepper;
};

// Create some variables that will be useful to any type of boost solver, and then call the private do_boost_solve method
template <class state_type, class stepper_type>
std::unordered_map<std::string, std::vector<double>> boost_system_solver<state_type, stepper_type>::do_solve(size_t ntimes, state_type state, std::shared_ptr<System> sys) const
{
    // Make vectors to store the observer output
    std::vector<state_type> state_vec;
    std::vector<double> time_vec;

    // Make a system caller
    SystemCaller syscall(sys);

    // Solve the system, storing the results in state_vec and time_vec
    do_boost_solve(ntimes, state, state_vec, time_vec, syscall);

    // Return the results
    return sys->get_results(state_vec, time_vec);
}

// Throw an error if a derived class hasn't defined its own do_boost_solve method
template <class state_type, class stepper_type>
inline void boost_system_solver<state_type, stepper_type>::do_boost_solve(
    size_t ntimes,
    state_type state,
    std::vector<state_type>& state_vec,
    std::vector<double>& time_vec,
    SystemCaller syscall) const
{
    throw std::logic_error(std::string("boost_system_solver '") + this->solver_name + std::string("' does not have a 'do_boost_solve()' method defined.\n"));
}

// A class representing a boost solver using an explicit stepper
template <class state_type, class stepper_type>
class boost_explicit_system_solver : public boost_system_solver<state_type, stepper_type>
{
   public:
    boost_explicit_system_solver(
        std::string solver_name,
        double output_step_size,
        int adaptive_max_steps,
        bool check_adaptive_compatible) : boost_system_solver<state_type, stepper_type>(solver_name, output_step_size, adaptive_max_steps, check_adaptive_compatible)
    {
    }

   private:
    void do_boost_solve(size_t ntimes, state_type state, std::vector<state_type>& state_vec, std::vector<double>& time_vec, SystemCaller syscall) const;
};

// Use integrate_const to populate state_vec and time_vec
template <class state_type, class stepper_type>
void boost_explicit_system_solver<state_type, stepper_type>::do_boost_solve(
    size_t ntimes,
    state_type state,
    std::vector<state_type>& state_vec,
    std::vector<double>& time_vec,
    SystemCaller syscall) const
{
    boost::numeric::odeint::integrate_const(
        this->stepper,
        syscall,
        state,
        0.0,
        ntimes - 1.0,
        this->output_step_size,
        push_back_state_and_time<state_type>(state_vec, time_vec, ntimes - 1.0, false),
        boost::numeric::odeint::max_step_checker(this->adaptive_max_steps));
}

// A class representing a boost solver using an implicit stepper
template <class state_type, class stepper_type>
class boost_implicit_system_solver : public boost_system_solver<state_type, stepper_type>
{
   public:
    boost_implicit_system_solver(
        std::string solver_name,
        double output_step_size,
        int adaptive_max_steps,
        bool check_adaptive_compatible) : boost_system_solver<state_type, stepper_type>(solver_name, output_step_size, adaptive_max_steps, check_adaptive_compatible)
    {
    }

   private:
    void do_boost_solve(size_t ntimes, state_type state, std::vector<state_type>& state_vec, std::vector<double>& time_vec, SystemCaller syscall) const;
};

// Use integrate_const to populate state_vec and time_vec
template <class state_type, class stepper_type>
void boost_implicit_system_solver<state_type, stepper_type>::do_boost_solve(
    size_t ntimes,
    state_type state,
    std::vector<state_type>& state_vec,
    std::vector<double>& time_vec,
    SystemCaller syscall) const
{
    boost::numeric::odeint::integrate_const(
        this->stepper,
        std::make_pair(syscall, syscall),
        state,
        0.0,
        ntimes - 1.0,
        this->output_step_size,
        push_back_state_and_time<state_type>(state_vec, time_vec, ntimes - 1.0, false),
        boost::numeric::odeint::max_step_checker(this->adaptive_max_steps));
}

#endif