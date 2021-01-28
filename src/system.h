#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>
#include <string>
#include <memory>             // For unique_ptr and shared_ptr
#include "validate_system.h"  // For string_vector, string_set, module_vector, etc
#include "state_map.h"        // For state_map, state_vector_map, etc
#include "modules.h"
#include "system_helper_functions.h"

/**
 * @class System
 *
 * @brief Defines a system of differential equations. Intended to be passed to a system_solver object.
 */
class System
{
   public:
    System(
        state_map const& init_state,
        state_map const& invariant_params,
        state_vector_map const& varying_params,
        string_vector const& ss_module_names,
        string_vector const& deriv_module_names);

    // For integrating via a system_solver
    size_t get_ntimes() const
    {
        auto vp = varying_parameters.begin();
        return (vp->second).size();
    }

    /// Check that the system is adaptive compatible based on the
    /// modules that it uses.
    bool is_adaptive_compatible() const
    {
        return check_adaptive_compatible(&steady_state_modules) &&
               check_adaptive_compatible(&derivative_modules);
    }

    template <typename state_type>
    void get_state(state_type& x) const;

    // For calculating derivatives
    template <typename state_type, typename time_type>
    void update(const state_type& x, const time_type& t);

    template <typename state_type, typename time_type>
    void operator()(const state_type& x, state_type& dxdt, const time_type& t);

    // For returning the results of a calculation
    std::vector<const double*> get_quantity_access_ptrs(string_vector quantity_names) const;
    string_vector get_state_parameter_names() const { return keys(initial_state); }
    string_vector get_output_param_names() const;

    // For generating reports to the user
    int get_ncalls() const { return ncalls; }
    void reset_ncalls() { ncalls = 0; }
    std::string generate_startup_report() const { return startup_message; }

    std::string generate_usage_report() const
    {
        return std::to_string(ncalls) +
               std::string(" derivatives were calculated");
    }

    // For fitting via nlopt
    void reset();

   private:
    // For storing the constructor inputs
    const state_map initial_state;
    const state_map invariant_parameters;
    const state_vector_map varying_parameters;
    string_vector steady_state_module_names; // These may be re-ordered in the constructor.
    const string_vector derivative_module_names;

    // Quantity maps defined during construction
    state_map quantities;
    state_map derivative_module_outputs;

    // Module lists defined during construction
    module_vector steady_state_modules;
    module_vector derivative_modules;

    // Pointers to quantity values defined during construction
    double* timestep_ptr;
    std::vector<double*> state_ptrs;
    std::vector<std::pair<double*, const double*>> state_ptr_pairs;
    std::vector<std::pair<double*, const std::vector<double>*>> varying_ptr_pairs;

    // For calculating derivatives
    void update_varying_params(double time_indx);

    template <typename time_type>
    void update_varying_params(time_type time_indx);

    template <class vector_type>
    void update_state_params(vector_type const& new_state);

    void run_steady_state_modules();

    template <class vector_type>
    void run_derivative_modules(vector_type& derivs);

    // For generating reports to the user
    int ncalls = 0;
    std::string startup_message;
};

/**
 * @brief Updates the state parameter x with the current values of all
 * the state parameters.
 *
 * @param[out] x An object capable of containing the state,
 * typically either std::vector<double> or boost::numeric::ublas::vector<double>.
 */
template <typename state_type>
void System::get_state(state_type& x) const
{
    x.resize(state_ptr_pairs.size());
    for (size_t i = 0; i < x.size(); i++) x[i] = *(state_ptr_pairs[i].first);
}

/**
 * @brief Updates all quantities (including ones calculated by steady state modules) based on the input state and time
 *
 * @param[in] x the state
 * @param[in] t the time
 */
template <typename state_type, typename time_type>
void System::update(const state_type& x, const time_type& t)
{
    update_varying_params(t);
    update_state_params(x);
    run_steady_state_modules();
}

/**
 * @brief Calculates a derivative given an input state and time. Function signature is set by the boost::odeint library.
 *
 * @param[in] x values of the state parameters
 * @param[out] dxdt derivatives of the state parameters calculated using x and t
 * @param[in] t time value
 */
template <typename state_type, typename time_type>
void System::operator()(const state_type& x, state_type& dxdt, const time_type& t)
{
    ++ncalls;
    update(x, t);
    run_derivative_modules(dxdt);
}

/**
 * @brief Gets values of the varying parameters using a discrete time index (e.g. int or size_t)
 */
template <typename time_type>
void System::update_varying_params(time_type time_indx)
{
    for (auto x : varying_ptr_pairs) {
        *(x.first) = (*(x.second))[time_indx];
    }
}

/**
 * @brief Updates the system's internally stored state parameter values to the new ones in new_state
 */
template <class vector_type>
void System::update_state_params(vector_type const& new_state)
{
    for (size_t i = 0; i < new_state.size(); i++) {
        *(state_ptr_pairs[i].first) = new_state[i];
    }
}

/**
 * @brief Calculates a derivative from the internally stored quantity map by running all the derivative modules
 *
 * @param[out] dxdt a vector for storing the derivative (passed by reference for speed)
 */
template <class vector_type>
void System::run_derivative_modules(vector_type& dxdt)
{
    // Clear the module output map
    for (double* const& x : state_ptrs) {
        *x = 0.0;
    }

    // Reset the derivative vector
    std::fill(dxdt.begin(), dxdt.end(), 0);

    // Run the modules
    for (std::unique_ptr<Module> const& m : derivative_modules) {
        m->run();
    }

    // Store the output in the derivative vector
    for (size_t i = 0; i < dxdt.size(); i++) {
        dxdt[i] += *(state_ptr_pairs[i].second) * (*timestep_ptr);
    }
}

/**
 * @class push_back_state_and_time
 *
 * @brief An observer class used to store state and time values during an odeint simulation.
 *
 * @param[in,out] states new entries will be added to this vector
 * @param[in,out] times new entries will be added to this vector
 * @param[in,out] message additional text will be appended to this string
 *
 */
template <typename state_type>
struct push_back_state_and_time {
   private:
    // Data members
    std::vector<state_type>& states;
    std::vector<double>& times;
    double max_time;
    double threshold = 0;
    double threshold_increment = 0.02;
    std::string& msg;

   public:
    // Constructor
    push_back_state_and_time(
        std::vector<state_type>& states,
        std::vector<double>& times,
        double maximum_time,
        std::string& message) : states(states),
                                times(times),
                                max_time(maximum_time),
                                msg(message) {}

    // Operation
    void operator()(state_type const& x, double t)
    {
        // Add to the message, if required
        if (t >= max_time || t / max_time >= threshold) {
            msg += std::string("Time index = ") +
                   std::to_string(t) +
                   std::string(" (") +
                   std::to_string(100.0 * t / max_time) +
                   std::string("%% done) at clock = ") +
                   std::to_string(clock()) +
                   std::string(" microseconds\n");
            threshold += threshold_increment;
        }

        // Store the new values
        states.push_back(x);
        times.push_back(t);
    }
};

/**
 * @brief Calculates all output parameters for a list of state
 * parameter values and their associated times using the system
 * object's public functions
 */
template <typename state_type, typename time_type>
state_vector_map get_results_from_system(std::shared_ptr<System> sys,
                                         const std::vector<state_type>& x_vec,
                                         const std::vector<time_type>& times)
{
    state_vector_map results;

    // The keys needed for results:
    string_vector output_param_names = sys->get_output_param_names();

    // Initialize all result values to all-zero vectors of the correct
    // length:
    std::vector<double> temporary(x_vec.size());
    for (const std::string& p : output_param_names) {
        results[p] = temporary;
    }

    // Add an entry for ncalls (hopefully there will be a better way
    // to do this someday):
    std::fill(temporary.begin(), temporary.end(), sys->get_ncalls());
    results["ncalls"] = temporary;

    // Get pointers to the output quantities:
    std::vector<const double*> output_param_ptrs =
        sys->get_quantity_access_ptrs(output_param_names);

    // Store the data
    for (size_t i = 0; i < x_vec.size(); ++i) {
        sys->update(x_vec[i], times[i]);
        for (size_t j = 0; j < output_param_names.size(); ++j) {
            (results[output_param_names[j]])[i] = *(output_param_ptrs[j]);
        }
    }

    return results;
}

#endif
