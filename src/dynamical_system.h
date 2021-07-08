#ifndef DYNAMICAL_SYSTEM_H
#define DYNAMICAL_SYSTEM_H

#include <vector>
#include <string>
#include <memory>                       // For shared_ptr
#include <utility>                      // For pair
#include "validate_dynamical_system.h"  // For string_set, module_vector, etc
#include "state_map.h"                  // For state_map, state_vector_map, string_vector, etc
#include "modules.h"
#include "dynamical_system_helper_functions.h"

using std::pair;
using std::string;
using std::vector;

/**
 *  @class dynamical_system
 *
 *  @brief Defines a dynamical system which can be solved using an integrator.
 */
class dynamical_system
{
   public:
    dynamical_system(
        state_map const& init_values,
        state_map const& params,
        state_vector_map const& drivers,
        string_vector const& dir_module_names,
        string_vector const& differential_module_names);

    // For integrating via an integrator
    size_t get_ntimes() const
    {
        auto driver = drivers.begin();
        return (driver->second).size();
    }

    /// Check whether the dynamical_system requires a fixed step Euler
    /// integrator based on the modules that it uses.
    bool requires_euler_integrator() const
    {
        return check_euler_requirement(direct_modules) ||
               check_euler_requirement(differential_modules);
    }

    template <typename vector_type>
    void get_differential_quantities(vector_type& x) const;

    // For calculating derivatives
    template <typename vector_type, typename time_type>
    void update_all_quantities(const vector_type& x, const time_type& t);

    template <typename vector_type, typename time_type>
    void calculate_derivative(const vector_type& x, vector_type& dxdt, const time_type& t);

    // For returning the results of a calculation
    vector<const double*> get_quantity_access_ptrs(string_vector quantity_names) const;
    string_vector get_differential_quantity_names() const { return keys(initial_values); }
    string_vector get_output_quantity_names() const;

    // For generating reports to the user
    int get_ncalls() const { return ncalls; }
    void reset_ncalls() { ncalls = 0; }
    string generate_startup_report() const { return startup_message; }

    string generate_usage_report() const
    {
        return std::to_string(ncalls) + string(" derivatives were calculated");
    }

    // For fitting via nlopt
    void reset();

   private:
    // For storing the constructor inputs
    const state_map initial_values;
    const state_map parameters;
    const state_vector_map drivers;
    string_vector direct_module_names;  // These may be re-ordered in the constructor.
    const string_vector differential_module_names;

    // Quantity maps defined during construction
    state_map all_quantities;
    state_map differential_quantities;

    // Module lists defined during construction
    module_vector direct_modules;
    module_vector differential_modules;

    // Pointers to quantity values defined during construction
    double* timestep_ptr;
    vector<pair<double*, const double*>> differential_quantity_ptr_pairs;
    vector<pair<double*, const vector<double>*>> driver_quantity_ptr_pairs;

    // For calculating derivatives
    void update_drivers(double time_indx);

    template <typename time_type>
    void update_drivers(time_type time_indx);

    template <class vector_type>
    void update_differential_quantities(vector_type const& new_values);

    template <class vector_type>
    void run_differential_modules(vector_type& derivs);

    // For generating reports to the user
    int ncalls = 0;
    string startup_message;
};

/**
 *  @brief Updates a vector with the current values of all the differential
 *         quantities
 *
 *  @param[out] x An object capable of containing the values of the differential
 *         quantities, typically either a std::vector<double> or a
 *         boost::numeric::ublas::vector<double>
 */
template <typename vector_type>
void dynamical_system::get_differential_quantities(vector_type& x) const
{
    x.resize(differential_quantity_ptr_pairs.size());
    for (size_t i = 0; i < x.size(); i++) {
        x[i] = *(differential_quantity_ptr_pairs[i].first);
    }
}

/**
 *  @brief Updates all quantities in the internally stored map (including ones
 *         calculated by direct modules) based on supplied values for the
 *         differential quantities and the time
 *
 *  @param[in] x an object containing new values for the differential
 *         quantities, typically either a std::vector<double> or a
 *         boost::numeric::ublas::vector<double>
 *
 *  @param[in] t the time
 */
template <typename vector_type, typename time_type>
void dynamical_system::update_all_quantities(
    const vector_type& x,
    const time_type& t)
{
    update_drivers(t);
    update_differential_quantities(x);
    run_module_list(direct_modules);
}

/**
 *  @brief Calculates derivatives for each of the differential quantities based
 *         on supplied values for the differential quantities and the time. The
 *         function signature is set by the boost::odeint library.
 *
 *  @param[in] x values of the differential quantities
 *
 *  @param[out] dxdt derivatives of the differential quantities calculated using
 *         x and t
 *
 *  @param[in] t the time
 */
template <typename vector_type, typename time_type>
void dynamical_system::calculate_derivative(const vector_type& x, vector_type& dxdt, const time_type& t)
{
    ++ncalls;
    update_all_quantities(x, t);
    run_differential_modules(dxdt);
}

/**
 *  @brief Updates values of the drivers in the internally stored quantity map
 *         based on time expressed as a discrete time index (e.g. int or size_t)
 */
template <typename time_type>
void dynamical_system::update_drivers(time_type time_indx)
{
    for (auto x : driver_quantity_ptr_pairs) {
        *(x.first) = (*(x.second))[time_indx];
    }
}

/**
 *  @brief Updates the system's internally stored values for the differential
 *         quantities to the ones in new_values
 */
template <class vector_type>
void dynamical_system::update_differential_quantities(vector_type const& new_values)
{
    for (size_t i = 0; i < new_values.size(); i++) {
        *(differential_quantity_ptr_pairs[i].first) = new_values[i];
    }
}

/**
 *  @brief Calculates a derivative from the internally stored quantity map by
 *         running all the differential modules
 *
 *  @param[out] dxdt a vector for storing the derivative (passed by reference
 *         for speed)
 */
template <class vector_type>
void dynamical_system::run_differential_modules(vector_type& dxdt)
{
    // Reset the differential quantities
    for (auto& x : differential_quantities) {
        x.second = 0.0;
    }

    // Run the modules
    run_module_list(differential_modules);

    // Store the output in the derivative vector
    for (size_t i = 0; i < dxdt.size(); i++) {
        dxdt[i] = *(differential_quantity_ptr_pairs[i].second) * (*timestep_ptr);
    }
}

/**
 *  @class push_back_state_and_time
 *
 *  @brief An observer class used to store state and time values during a
 *         simulation.
 *
 *  @param[in,out] states new entries will be added to this vector

 *  @param[in,out] times new entries will be added to this vector

 *  @param[in,out] message additional text will be appended to this string
 *
 */
template <typename vector_type>
struct push_back_state_and_time {
   private:
    // Data members
    vector<vector_type>& states;
    vector<double>& times;
    double max_time;
    double threshold = 0;
    double threshold_increment = 0.02;
    string& msg;

   public:
    // Constructor
    push_back_state_and_time(
        vector<vector_type>& states,
        vector<double>& times,
        double maximum_time,
        string& message)
        : states(states),
          times(times),
          max_time(maximum_time),
          msg(message) {}

    // Operation
    void operator()(vector_type const& x, double t)
    {
        // Add to the message, if required
        if (t >= max_time || t / max_time >= threshold) {
            msg += string("Time index = ") +
                   std::to_string(t) +
                   string(" (") +
                   std::to_string(100.0 * t / max_time) +
                   string("%% done) at clock = ") +
                   std::to_string(clock()) +
                   string(" microseconds\n");
            threshold += threshold_increment;
        }

        // Store the new values
        states.push_back(x);
        times.push_back(t);
    }
};

/**
 *  @brief Calculates values for all of the system's output quantities based on
 *         a list of differential quantity values and their associated times
 */
template <typename vector_type, typename time_type>
state_vector_map get_results_from_system(
    std::shared_ptr<dynamical_system> sys,
    const vector<vector_type>& x_vec,
    const vector<time_type>& times)
{
    state_vector_map results;

    // Get the keys needed for results
    string_vector output_param_names = sys->get_output_quantity_names();

    // Initialize all result values to all-zero vectors of the correct length
    vector<double> temporary(x_vec.size());
    for (const string& p : output_param_names) {
        results[p] = temporary;
    }

    // Add an entry for ncalls (hopefully there will be a better way to do this
    // someday)
    std::fill(temporary.begin(), temporary.end(), sys->get_ncalls());
    results["ncalls"] = temporary;

    // Get pointers to the output quantities
    vector<const double*> output_param_ptrs =
        sys->get_quantity_access_ptrs(output_param_names);

    // Store the data
    for (size_t i = 0; i < x_vec.size(); ++i) {
        sys->update_all_quantities(x_vec[i], times[i]);
        for (size_t j = 0; j < output_param_names.size(); ++j) {
            (results[output_param_names[j]])[i] = *(output_param_ptrs[j]);
        }
    }

    return results;
}

#endif
