#ifndef SIMULTANEOUS_EQUATIONS_H
#define SIMULTANEOUS_EQUATIONS_H

#include "state_map.h"
#include "validate_system.h"

bool validate_simultanous_equations_inputs(
    std::string& message,
    state_map const& known_quantities,
    string_vector const& unknown_quantities,
    string_vector const& ss_module_names);

std::string analyze_simultanous_equations_inputs(
    state_map const& known_quantities,
    string_vector const& ss_module_names);

/**
 * @class simultaneous_equations
 * 
 * @brief Defines a set of simultaneous equations
 */
class simultaneous_equations
{
   public:
    simultaneous_equations(
        state_map const& known_quantities,
        string_vector const& unknown_quantities,
        string_vector const& ss_module_names);

    // For solving via an equation_solver
    template <typename vector_type>
    void operator()(vector_type const& unknown_inputs, vector_type& unknown_outputs);

    // For generating reports to the user
    int get_ncalls() const { return ncalls; }
    void reset_ncalls() { ncalls = 0; }
    std::string generate_startup_report() const { return startup_message; }

    std::string generate_usage_report() const
    {
        return std::string("The simultaneous equations were evaluated ") +
               std::to_string(ncalls) +
               std::string(" times");
    }

   private:
    // Quantity maps defined during construction
    state_map quantities;
    state_map module_output_map;

    // Module lists defined during construction
    module_vector steady_state_modules;

    // Pointers to quantity values defined during construction
    std::vector<double*> unknown_ptrs;
    std::vector<double*> steady_state_ptrs;
    std::vector<std::pair<double*, const double*>> unknown_ptr_pairs;
    std::vector<std::pair<double*, const double*>> steady_state_ptr_pairs;

    // For generating reports to the user
    int ncalls = 0;
    std::string startup_message;
};

/**
 * @brief Calculates values of the independent quantities given input values for the unknown quantities.
 * 
 * @param[in] unknown_inputs input values of the unknown quantities
 * @param[out] unknown_outputs calculated values of the unknown quantities
 */
template <typename vector_type>
void simultaneous_equations::operator()(vector_type const& unknown_inputs, vector_type& unknown_outputs)
{
    // Increment the counter
    ++ncalls;
    
    // Clear all the steady state outputs in the module output map
    for (double* const& x : steady_state_ptrs) {
        *x = 0.0;
    }
    
    // Set the input values of the unknown quantities in the main quantity map
    for (size_t i = 0; i < unknown_ptrs.size(); i++) {
        *(unknown_ptrs[i]) = unknown_inputs[i];
    }

    // Run each module and store its output in the main quantity map
    for (std::unique_ptr<Module> const& m : steady_state_modules) {
        m->run();
        for (auto const& x : steady_state_ptr_pairs) {
            *x.first = *x.second;
        }
    }
    
    // Get the calculated values of the unknown quantities from the module output map
    for (size_t i = 0; i < unknown_ptrs.size(); i++) {
        unknown_outputs[i] = *(unknown_ptrs[i]);
    }
    
}

string_vector get_unknown_quantities(std::vector<string_vector> module_name_vector);

#endif