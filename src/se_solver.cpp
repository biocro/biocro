#include <map>
#include <Rinternals.h>
#include "se_solver.h"
#include "se_solver_helper_functions.h"

/**
 * @brief Returns a string describing the se_solver's basic properties.
 */
std::string se_solver::generate_info_report() const
{
    return std::string("Name: ") + solver_name +
           std::string("\nMaximum number of iterations: ") + std::to_string(max_iterations);
}

/**
 * @brief Returns a string describing what happened during the most recent
 * attempt to find a root.
 */
std::string se_solver::generate_solve_report() const
{
    if (!solve_method_has_been_called) {
        return std::string("The solver has not been called yet");
    } else {
        auto bool_to_string = [](bool b) -> std::string {
            return b ? std::string("Yes") : std::string("No");
        };

        return std::string("Details about the most recent calculation:") +
               std::string("\n Converged according to absolute threshold: ") + bool_to_string(converged_abs) +
               std::string("\n Converged according to relative threshold: ") + bool_to_string(converged_rel) +
               std::string("\n Number of iterations: ") + std::to_string(num_iterations);
    }
}

/**
 * @brief Converts the output from an se_observer_push_back into a format that can easily be passed to R
 */
state_vector_map format_se_solver_results(
    std::vector<std::string> quantity_names,
    std::vector<std::vector<double>> uq_vector)
{
    // Set up the output
    state_vector_map result;
    std::vector<double> temp(uq_vector.size());
    for (std::string const& n : quantity_names) {
        result[n] = temp;
    }
    result["iteration"] = temp;

    // Fill in the results
    std::vector<double> uq;
    for (size_t i = 0; i < uq_vector.size(); ++i) {
        uq = uq_vector[i];
        for (size_t j = 0; j < quantity_names.size(); ++j) {
            result[quantity_names[j]][i] = uq[j];
        }
        result["iteration"][i] = i;
    }

    return result;
}
