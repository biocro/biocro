#ifndef SYSTEM_SOLVER_H
#define SYSTEM_SOLVER_H

#include <vector>
#include <boost/numeric/ublas/vector.hpp>  // For use with ODEINT
#include <boost/numeric/odeint.hpp>        // For use with ODEINT
#include "state_map.h"
#include "system.h"

// An abstract class for a generic system solver
// Its operator() provides a uniform interface for all derived solvers,
//  and its constructor requires inputs that are common to all solvers
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

   protected:
    std::string const solver_name;
    double const output_step_size;

   private:
    bool check_adaptive_compatible;
    virtual std::unordered_map<std::string, std::vector<double>> do_solve(size_t ntimes, state_type state, std::shared_ptr<System> sys) const;
};

// Pure virtual destructor must be redefined outside the class
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
template <class state_type>
class boost_system_solver : public system_solver<state_type>
{
   public:
    boost_system_solver(
        std::string solver_name,
        double output_step_size,
        bool check_adaptive_compatible) : system_solver<state_type>(solver_name, output_step_size, check_adaptive_compatible)
    {
    }

   private:
    std::unordered_map<std::string, std::vector<double>> do_solve(size_t ntimes, state_type state, std::shared_ptr<System> sys) const;
    virtual void do_boost_solve(
        size_t ntimes,
        state_type state,
        std::vector<state_type>& state_vec,
        std::vector<double>& time_vec,
        SystemCaller syscall,
        push_back_state_and_time<state_type> observer) const;
};

// Create some variables that will be useful to any type of boost solver, and then call the private do_boost_solve method
template <class state_type>
std::unordered_map<std::string, std::vector<double>> boost_system_solver<state_type>::do_solve(size_t ntimes, state_type state, std::shared_ptr<System> sys) const
{
    // Make vectors to store the observer output
    std::vector<state_type> state_vec;
    std::vector<double> time_vec;

    // Make the observer
    push_back_state_and_time<state_type> observer(state_vec, time_vec, ntimes - 1.0, false);

    // Make a system caller
    SystemCaller syscall(sys);

    // Solve the system, storing the results in state_vec and time_vec
    do_boost_solve(ntimes, state, state_vec, time_vec, syscall, observer);

    // Return the results
    return sys->get_results(state_vec, time_vec);
}

// Throw an error if a derived class hasn't defined its own do_boost_solve method
template <class state_type>
inline void boost_system_solver<state_type>::do_boost_solve(
    size_t ntimes,
    state_type state,
    std::vector<state_type>& state_vec,
    std::vector<double>& time_vec,
    SystemCaller syscall,
    push_back_state_and_time<state_type> observer) const
{
    throw std::logic_error(std::string("boost_system_solver '") + this->solver_name + std::string("' does not have a 'do_boost_solve()' method defined.\n"));
}

// A class representing the boost euler solver
template <class state_type>
class boost_euler_system_solver : public boost_system_solver<state_type>
{
   public:
    boost_euler_system_solver(double output_step_size) : boost_system_solver<state_type>("euler_odeint", output_step_size, false)
    {
    }

   private:
    void do_boost_solve(
        size_t ntimes,
        state_type state,
        std::vector<state_type>& state_vec,
        std::vector<double>& time_vec,
        SystemCaller syscall,
        push_back_state_and_time<state_type> observer) const;
};

template <class state_type>
void boost_euler_system_solver<state_type>::do_boost_solve(
    size_t ntimes,
    state_type state,
    std::vector<state_type>& state_vec,
    std::vector<double>& time_vec,
    SystemCaller syscall,
    push_back_state_and_time<state_type> observer) const
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
        this->output_step_size,
        observer);
}

// A class representing the boost RK4 solver
template <class state_type>
class boost_rk4_system_solver : public boost_system_solver<state_type>
{
   public:
    boost_rk4_system_solver(double output_step_size) : boost_system_solver<state_type>("rk4", output_step_size, true)
    {
    }

   private:
    void do_boost_solve(
        size_t ntimes,
        state_type state,
        std::vector<state_type>& state_vec,
        std::vector<double>& time_vec,
        SystemCaller syscall,
        push_back_state_and_time<state_type> observer) const;
};

template <class state_type>
void boost_rk4_system_solver<state_type>::do_boost_solve(
    size_t ntimes,
    state_type state,
    std::vector<state_type>& state_vec,
    std::vector<double>& time_vec,
    SystemCaller syscall,
    push_back_state_and_time<state_type> observer) const
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
        this->output_step_size,
        observer);
}

// A class representing the boost RKCK54 solver
template <class state_type>
class boost_rkck54_system_solver : public boost_system_solver<state_type>
{
   public:
    boost_rkck54_system_solver(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps) : boost_system_solver<state_type>("rkck54", output_step_size, true),
                                  adaptive_error_tol(adaptive_error_tol),
                                  adaptive_max_steps(adaptive_max_steps)
    {
    }

   private:
    double adaptive_error_tol;
    int adaptive_max_steps;
    void do_boost_solve(
        size_t ntimes,
        state_type state,
        std::vector<state_type>& state_vec,
        std::vector<double>& time_vec,
        SystemCaller syscall,
        push_back_state_and_time<state_type> observer) const;
};

template <class state_type>
void boost_rkck54_system_solver<state_type>::do_boost_solve(
    size_t ntimes,
    state_type state,
    std::vector<state_type>& state_vec,
    std::vector<double>& time_vec,
    SystemCaller syscall,
    push_back_state_and_time<state_type> observer) const
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
        this->output_step_size,
        observer);
}

// A class representing the boost rosenbrock solver
// Note that this solver is only compatible with boost::numeric::ublas::vector<double> state vectors
class boost_rsnbrk_system_solver : public boost_system_solver<boost::numeric::ublas::vector<double>>
{
   public:
    boost_rsnbrk_system_solver(
        double output_step_size,
        double adaptive_error_tol,
        int adaptive_max_steps) : boost_system_solver<boost::numeric::ublas::vector<double>>("rsnbrk", output_step_size, true),
                                  adaptive_error_tol(adaptive_error_tol),
                                  adaptive_max_steps(adaptive_max_steps)
    {
    }

   private:
    double adaptive_error_tol;
    int adaptive_max_steps;
    void do_boost_solve(
        size_t ntimes,
        boost::numeric::ublas::vector<double> state,
        std::vector<boost::numeric::ublas::vector<double>>& state_vec,
        std::vector<double>& time_vec,
        SystemCaller syscall,
        push_back_state_and_time<boost::numeric::ublas::vector<double>> observer) const;
};

void boost_rsnbrk_system_solver::do_boost_solve(
    size_t ntimes,
    boost::numeric::ublas::vector<double> state,
    std::vector<boost::numeric::ublas::vector<double>>& state_vec,
    std::vector<double>& time_vec,
    SystemCaller syscall,
    push_back_state_and_time<boost::numeric::ublas::vector<double>> observer) const
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

#endif