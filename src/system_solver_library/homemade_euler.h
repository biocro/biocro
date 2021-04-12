#ifndef HOMEMADE_EULER_H
#define HOMEMADE_EULER_H

#include "../system_solver.h"

// A class representing our homemade euler solver
template <class state_type>
class homemade_euler_solver : public system_solver
{
   public:
    homemade_euler_solver(
        double step_size,
        double rel_error_tolerance,
        double abs_error_tolerance,
        int max_steps) : system_solver("homemade_euler", false, step_size, rel_error_tolerance, abs_error_tolerance, max_steps) {}

   private:
    std::unordered_map<std::string, std::vector<double>> do_solve(std::shared_ptr<System> sys) override;

    std::string get_param_info() const override
    {
        // The homemade Euler solver has no new parameters to report
        return std::string("");
    }

    std::string get_solution_info() const override
    {
        // The homemade Euler solver doesn't have much to contribute
        return std::string("N/A");
    }
};

template <class state_type>
std::unordered_map<std::string, std::vector<double>> homemade_euler_solver<state_type>::do_solve(std::shared_ptr<System> sys)
{
    // Get the names of the output parameters and pointers to them
    std::vector<std::string> output_param_vector = sys->get_output_param_names();
    std::vector<const double*> output_ptr_vector = sys->get_quantity_access_ptrs(output_param_vector);

    // Make the results map
    std::unordered_map<std::string, std::vector<double>> results;

    // Make the result vector
    std::vector<double> temp(sys->get_ntimes());
    std::vector<std::vector<double>> result_vec(output_param_vector.size(), temp);

    // Get the current state in the correct format
    state_type state;
    sys->get_state(state);

    // Make a vector to store the derivative
    state_type dstatedt = state;

    // Run through all the times
    for (size_t t = 0; t < sys->get_ntimes(); t++) {
        // Update all the parameters and calculate the derivative based on the current time and state
        (*sys)(state, dstatedt, t);

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
