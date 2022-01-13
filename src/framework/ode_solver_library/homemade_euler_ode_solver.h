#ifndef HOMEMADE_EULER_ODE_SOLVER_H
#define HOMEMADE_EULER_ODE_SOLVER_H

#include "../ode_solver.h"
#include "../state_map.h"

// A class representing our homemade Euler ode_solver
template <class state_type>
class homemade_euler_ode_solver : public ode_solver
{
   public:
    homemade_euler_ode_solver(
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps) : ode_solver("homemade_euler", false, step_size, rel_error_tolerance, abs_error_tolerance, max_steps) {}

   private:
    state_vector_map do_integrate(std::shared_ptr<dynamical_system> sys) override;

    std::string get_param_info() const override
    {
        // The homemade Euler ode_solver has no new parameters to report
        return std::string("");
    }

    std::string get_solution_info() const override
    {
        // The homemade Euler ode_solver doesn't have much to contribute
        return std::string("N/A");
    }
};

template <class state_type>
state_vector_map homemade_euler_ode_solver<state_type>::do_integrate(std::shared_ptr<dynamical_system> sys)
{
    // Get the names of the output parameters and pointers to them
    std::vector<std::string> output_param_vector = sys->get_output_quantity_names();
    std::vector<const double*> output_ptr_vector = sys->get_quantity_access_ptrs(output_param_vector);

    // Make the results map
    state_vector_map results;

    // Make the result vector
    std::vector<double> temp(sys->get_ntimes());
    std::vector<std::vector<double>> result_vec(output_param_vector.size(), temp);

    // Get the current state in the correct format
    state_type state;
    sys->get_differential_quantities(state);

    // Make a vector to store the derivative
    state_type dstatedt = state;

    // Run through all the times
    for (size_t t = 0; t < sys->get_ntimes(); t++) {
        // Update all the parameters and calculate the derivative based on the current time and state
        sys->calculate_derivative(state, dstatedt, t);

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

#endif
