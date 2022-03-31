#ifndef DYNAMICAL_SYSTEM_H
#define DYNAMICAL_SYSTEM_H

#include <vector>
#include <string>
#include <memory>            // For std::shared_ptr
#include <utility>           // For std::pair
#include "state_map.h"       // For state_map, state_vector_map, string_vector
#include "module_creator.h"  // For mc_vector
#include "module.h"          // For module_vector
#include "validate_dynamical_system.h"
#include "dynamical_system_helper_functions.h"

using std::pair;
using std::string;
using std::vector;

/**
 *  @class dynamical_system
 *
 *  @brief Defines a dynamical system which can be solved using an ode_solver.
 *
 *  A dynamical system requires three elements:
 *
 *  - A state space, which may generally be thought of as consisting of one or
 *    more quantities; here we use the term _quantity_ to refer to a named
 *    variable that represents some particular attribute of the state.
 *
 *  - An independent quantity upon which the value of the state depends, which
 *    is typically taken to be the time.
 *
 *  - An evolution rule which describes how to determine the value of the state
 *    at a future time given its value at the current time.
 *
 *  This class (together with a differential equation solver) represents a
 *  dynamical system. It defines the following:
 *
 *  - The quantities that comprise the state. These are determined from the
 *    parameters, drivers, direct modules, and differential modules.
 *
 *  - The independent quantity. This is assumed to be _time_.
 *
 *  - The equations required to calculate the values of the parameters, drivers,
 *    and direct quantities (which are all elements of the state), given values
 *    of the differential quantities (another subset of the state) and the time.
 *    The equations for the parameters set them equal to constants. The
 *    equations for the drivers are determined by interpolating the supplied
 *    discrete values. The equations for the direct quantities are supplied by
 *    the direct modules.
 *
 *  - The equations required to calculate the derivatives of the differential
 *    quantities, given the values of the state and the time. These equations
 *    are supplied by the differential modules.
 *
 *  - The initial values of the differential quantities.
 *
 *  To fully define the evolution rule, it is also necessary to supply a
 *  differential equation solver in addition to the equations mentioned above.
 *  Such solvers are provided as another class: the `ode_solver`.
 *
 *  ----------------------------------------------------------------------------
 *
 *  One key property of this class is its private data member `all_quantities`.
 *  This `state_map` represents the current state of a dynamical system, i.e.,
 *  it specifies the names and values of all quantities that comprise the state.
 *  When a `dynamical_system` object's direct modules are run, they read
 *  quantity values from `all_quantities` and store calculated quantity values
 *  in it; when a `dynamical_system` object's differential modules are run, they
 *  also read quantity values from `all_quantities`. In that sense, it is a sort
 *  of "central clearing house" for quantities. It is sometimes referred to as
 *  the "internally stored quantity map" or "internally stored map." The
 *  individual quantities that comprise `all_quantities` are sometimes referred
 *  to as "internally stored quantities," and their values may be called
 *  "internally stored quantity values."
 *
 *  In addition to the `all_quantities` data member, there is a separate private
 *  data member for storing the outputs from the differential modules; this
 *  `state_map` is called `differential_quantity_derivatives`. This member is
 *  sometimes also referred to as the "internally stored map," and it should be
 *  clear from context that derivatives of quantity values, rather than quantity
 *  values themselves, are being referenced.
 *
 *  ----------------------------------------------------------------------------
 *
 *  Public methods are provided that allow access to the values or names of
 *  subsets of `all_quantities` and `differential_quantity_derivatives`:
 *
 *  - `get_differential_quantity_names` returns the names of the differential
 *    quantities
 *
 *  - `get_differential_quantities` returns the values of the differential
 *    quantities
 *
 *  - `calculate_derivative` calculates the derivatives of each of the
 *    differential quantities given values for the time and the differential
 *    quantities
 *
 *  - `get_output_quantity_names` returns the names of all quantities that are
 *    expected to change throughout a simulation, i.e., the drivers, direct
 *    quantities, and differential quantities (but not the parameters)
 *
 *  - `get_quantity_access_ptrs` returns pointers to elements of all_quantities
 *    that correspond to quantity names that are supplied in the input argument
 *
 *  - `update_all_quantities` determines the values of all state quantities from
 *    input values of time and the differential quantities; this function
 *    modifies `all_quantities` but has no return value
 *
 *  - `reset` returns all quantities to their initial values, as if the
 *    `dynamical_system` object had just been created; this may be helpful if an
 *    object is to be reused for multiple simulations; this function
 *    modifies `all_quantities` but has no return value
 *
 *  When using a differential equation solver to determine the time evolution of
 *  a dynamical system's state, it is typically necessary to treat the values of
 *  the differential quantities as a vector where they take a particular order,
 *  in contrast to our usual method of storing quantities in `state_map` objects
 *  where their values are accessed via their names and do not have a defined
 *  order. In this situation, the `get_differential_quantity_names` and
 *  `get_differential_quantities` methods are essential, since they return the
 *  names and values of the differential quantities in a defined order. When the
 *  `calculate_derivative` method is called, the values of the differential
 *  quantities are expected to be supplied in the same order defined by
 *  `get_differential_quantity_names`, and the derivatives are also returned in
 *  that order.
 *
 *  Once a time sequence of differential quantity values has been determined by
 *  a solver, it is typically necessary to retrieve the values of all state
 *  quantities that change during a simulation. This can be achieved using
 *  additional public methods. First, the names of all such quantities can be
 *  determined using the `get_output_quantity_names` method. Then, pointers to
 *  the values of these quantities in the central map can be obtained using the
 *  `get_quantity_access_ptrs` method. Finally, for each value of time and the
 *  differential quantities, the `update_all_quantities` method can be called to
 *  update `all_quantities`, and the access pointers can be used to retrieve the
 *  updated values from `all_quantities`. Rather than explicitly calling these
 *  methods, this procedure can be performed using the `get_results_from_system`
 *  free function.
 */
class dynamical_system
{
   public:
    dynamical_system(
        state_map const& init_values,
        state_map const& params,
        state_vector_map const& drivers,
        mc_vector const& dir_mcs,
        mc_vector const& differential_mcs);

    // For integrating via an ode_solver
    size_t get_ntimes() const
    {
        auto driver = drivers.begin();
        return (driver->second).size();
    }

    /// Check whether the dynamical_system requires a fixed step Euler
    /// ODE solver based on the modules that it uses.
    bool requires_euler_ode_solver() const
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
    mc_vector direct_mcs;  // These may be re-ordered in the constructor.
    const mc_vector differential_mcs;

    // Quantity maps defined during construction
    state_map all_quantities;
    state_map differential_quantity_derivatives;

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
 *         to match their values in the internally stored drivers table at time
 *         `time_index`, which should be a discrete index such as an `int` or
 *         `size_t`.
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
    // Reset the derivatives of the differential quantities
    for (auto& x : differential_quantity_derivatives) {
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

/**
 * @brief Define `evaluation` behavior for a set of equations defined by a
 * `dynamical_system` object. This is required to use `calculate_jacobian()`
 * with a `simultaneous_equations` object.
 *
 * @param[in] sys a shared pointer to a dynamical_system object.
 *
 * @param[in] x a vector to be passed to the dynamical_system as an input. Since
 *            we are evaluating a dynamical_system, x contains values of the
 *            dynamical_system's differential quantities.
 *
 * @param[in] t a time to be passed to the dynamical_system as an input.
 *
 * @param[out] y the vector output. Since we are evaluating a dynamical_system
 *             object, y contains the time derivative of each differential
 *             quantity.
 */
template <typename vector_type, typename time_type>
void evaluate_equations(
    std::shared_ptr<dynamical_system> const& sys,
    vector_type const& x,
    time_type t,
    vector_type& y)
{
    sys->calculate_derivative(x, y, t);
}

#endif
