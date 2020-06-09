#ifndef SIMULTANEOUS_EQUATIONS_H
#define SIMULTANEOUS_EQUATIONS_H

#include "state_map.h"        // For state_map, state_vector_map, etc
#include "validate_system.h"  // For string_vector, string_set, module_vector, etc

bool validate_simultaneous_equations_inputs(
    std::string& message,
    state_map const& known_quantities,
    string_vector const& unknown_quantities,
    string_vector const& ss_module_names);

std::string analyze_simultaneous_equations_inputs(string_vector const& ss_module_names);

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

    // For using as part of a module
    void update_known_quantities(std::vector<const double*> const& ptrs_to_values);

    template <typename vector_type>
    void get_all_outputs(std::vector<double>& vector_to_update, vector_type const& unknown_quantity_vector);

    // For solving via an equation_solver
    template <typename in_vector_type, typename out_vector_type>
    void operator()(in_vector_type const& unknown_quantity_vector, out_vector_type& difference_vector);

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
    std::vector<double*> known_ptrs;
    std::vector<double*> unknown_ptrs;
    std::vector<double*> steady_state_ptrs;
    std::vector<const double*> output_ptrs;
    std::vector<std::pair<double*, const double*>> unknown_ptr_pairs;
    std::vector<std::pair<double*, const double*>> steady_state_ptr_pairs;
    
    // For doing calculations
    template <typename vector_type>
    void run_modules(vector_type const& unknown_quantity_vector);

    // For generating reports to the user
    int ncalls = 0;
    std::string startup_message;
};

/**
 * @brief Updates the values of `vector_to_update` using the values pointed to by the elements
 * of the private member `output_ptrs.` The elements of `vector_to_update` must be sorted
 * according to quantity name. Values of the unknown quantities are used to calculate the outputs.
 */
template <typename vector_type>
void simultaneous_equations::get_all_outputs(std::vector<double>& vector_to_update, vector_type const& unknown_quantity_vector)
{
    if (vector_to_update.size() != output_ptrs.size()) {
        throw std::logic_error("Thrown by simultaneous_equations::update_known_quantities: vector_to_update has the wrong size.");
    }
    
    run_modules(unknown_quantity_vector);

    for (size_t i = 0; i < output_ptrs.size(); ++i) {
        vector_to_update[i] = *output_ptrs[i];
    }
}

/**
 * @brief Calculates the change in value of the unknown quantities given a vector of
 * initial values.
 * 
 * To solve the set of equations, we seek a set of initial values that produces a vector of
 * zeroes when used as an input to this function.
 * 
 * @param[in] unknown_quantity_vector a vector of initial values for the unknown quantities
 * 
 * @param[out] difference_vector a vector of (final - initial) values for the unknown quantities
 */
template <typename in_vector_type, typename out_vector_type>
void simultaneous_equations::operator()(in_vector_type const& unknown_quantity_vector, out_vector_type& difference_vector)
{
    run_modules(unknown_quantity_vector);

    // Get the calculated values of the unknown quantities from the main quantity map
    for (size_t i = 0; i < unknown_ptrs.size(); i++) {
        difference_vector[i] = *(unknown_ptrs[i]) - unknown_quantity_vector[i];
    }
}

/**
 * @brief Runs each module using the values from unknown_quantity_vector and updates
 * the central map of quantities.
 */
template <typename vector_type>
void simultaneous_equations::run_modules(vector_type const& unknown_quantity_vector)
{
    // Increment the counter
    ++ncalls;

    // Clear all the steady state outputs in the main quantity map
    for (auto const& x : steady_state_ptr_pairs) {
        *x.first = 0.0;
    }

    // Set the input values of the unknown quantities in the main quantity map
    for (size_t i = 0; i < unknown_ptrs.size(); i++) {
        *(unknown_ptrs[i]) = unknown_quantity_vector[i];
    }

    // Initialize the module output map so it has the same values as the main map
    module_output_map = quantities;

    // Run each module and store its output in the main quantity map
    for (std::unique_ptr<Module> const& m : steady_state_modules) {
        m->run();
        for (auto const& x : steady_state_ptr_pairs) {
            *x.first = *x.second;
        }
    }
}

string_vector get_unknown_quantities(std::vector<string_vector> module_name_vector);

#endif
