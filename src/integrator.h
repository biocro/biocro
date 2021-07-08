#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <vector>
#include <boost/numeric/odeint.hpp>  // For use with ODEINT
#include "state_map.h"
#include "dynamical_system.h"

// An abstract class for a generic numerical integrator. Its `integrate()`
// function provides a uniform interface for all derived integrators, and its
// constructor requires inputs that are common to all integrators.
class integrator
{
   public:
    integrator(
        std::string integrator_name,
        bool check_adaptive_compatible,
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps) : integrator_name(integrator_name),
                         check_adaptive_compatible(check_adaptive_compatible),
                         output_step_size(step_size),
                         adaptive_rel_error_tol(rel_error_tolerance),
                         adaptive_abs_error_tol(abs_error_tolerance),
                         adaptive_max_steps(max_steps) {}

    virtual ~integrator() {}

    state_vector_map integrate(std::shared_ptr<dynamical_system> sys);

    std::string generate_info_report() const
    {
        return std::string("Name: ") + integrator_name +
               get_param_info();
    }

    std::string generate_integrate_report() const
    {
        if (!integrate_method_has_been_called) {
            return std::string("The integrator has not been called yet");
        } else {
            return get_solution_info();
        }
    }

   protected:
    double get_output_step_size() const { return output_step_size; }
    double get_adaptive_rel_error_tol() const { return adaptive_rel_error_tol; }
    double get_adaptive_abs_error_tol() const { return adaptive_abs_error_tol; }
    int get_adaptive_max_steps() const { return adaptive_max_steps; }

   private:
    const std::string integrator_name;
    const bool check_adaptive_compatible;

    double output_step_size;
    double adaptive_rel_error_tol;
    double adaptive_abs_error_tol;
    int adaptive_max_steps;

    bool integrate_method_has_been_called = false;

    virtual state_vector_map do_integrate(std::shared_ptr<dynamical_system> sys) = 0;
    virtual state_vector_map handle_adaptive_incompatibility(std::shared_ptr<dynamical_system> sys);
    virtual std::string get_param_info() const = 0;
    virtual std::string get_solution_info() const = 0;
};

// Define the standard response to a problem with adaptive compatibility
inline state_vector_map
    integrator::handle_adaptive_incompatibility(std::shared_ptr<dynamical_system> /*sys*/)
{
    throw std::logic_error(
        std::string("integrator '") + integrator_name +
        std::string("' is not compatible with the input system.\n"));
}

#endif
