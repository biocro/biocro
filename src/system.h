#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>
#include <string>
#include <set>
#include <unordered_map>
#include <memory>       // For unique_ptr and shared_ptr
#include <cmath>        // For fmod
#include "constants.h"  // For eps_deriv
#include "modules.h"
#include "module_wrapper_factory.h"
#include "validate_system.h"
#include "system_helper_functions.h"
#include "state_map.h"

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

    bool is_adaptive_compatible() const
    {
        return check_adaptive_compatible(&steady_state_modules) * check_adaptive_compatible(&derivative_modules);
    }

    template <typename state_type>
    void get_state(state_type& x) const;

    // For calculating derivatives
    template <typename state_type, typename time_type>
    void update(state_type const& x, time_type const& t);

    template <typename state_type, typename time_type>
    void operator()(state_type const& x, state_type& dxdt, time_type const& t);

    template <typename state_type, typename jacobi_type, typename time_type>
    void operator()(state_type const& x, jacobi_type& jacobi, time_type const& t, state_type& dfdt);

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
    const string_vector steady_state_module_names;
    const string_vector derivative_module_names;

    // Quantity maps defined during construction
    state_map quantities;
    state_map module_output_map;

    // Module lists defined during construction
    module_vector steady_state_modules;
    module_vector derivative_modules;

    // Pointers to quantity values defined during construction
    double* timestep_ptr;
    std::vector<double*> state_ptrs;
    std::vector<double*> steady_state_ptrs;
    std::vector<std::pair<double*, const double*>> state_ptr_pairs;
    std::vector<std::pair<double*, const double*>> steady_state_ptr_pairs;
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
 * @brief Returns the current values of all the state parameters.
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
void System::update(state_type const& x, time_type const& t)
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
void System::operator()(state_type const& x, state_type& dxdt, time_type const& t)
{
    ++ncalls;
    update(x, t);
    run_derivative_modules(dxdt);
}

/**
 * @brief Numerically compute the Jacobian matrix. Function signature is set by the boost::odeint library.
 * 
 * @param[in] x values of the state parameters
 * @param[out] jacobi jacobian matrix (i.e., derivatives of each derivative with respect to each state variable)
 * @param[in] t time value
 * @param[out] dfdt derivatives of each derivative with respect to time (not included in the jacobian)
 * 
 * Discussion of step size from <a href="http://www.iue.tuwien.ac.at/phd/khalil/node14.html">Nadim Khalil's thesis</a>:
 * <BLOCKQUOTE>
 * It is known that numerical differentiation is an unstable procedure prone to truncation and subtractive cancellation errors.
 * Decreasing the step size will reduce the truncation error.
 * Unfortunately a smaller step has the opposite effect on the cancellation error.
 * Selecting the optimal step size for a certain problem is computationally expensive and the benefits achieved are not justifiable
 *  as the effect of small errors in the values of the elements of the Jacobian matrix is minor.
 * For this reason, the sizing of the finite difference step is not attempted and a constant increment size is used in evaluating the gradient.
 * </BLOCKQUOTE>
 * 
 * In BioCro, we fix a step size and only evaluate the forward perturbation to reduce calculation costs.
 *  In other words:
 *   - (1) We calculate dxdt using the input (x,t) (called dxdt_c for current)
 *   - (2) We make a forward perturbation by adding h to one state variable and calculating the time derivatives (called dxdt_p for perturbation)
 *   - (3) We calculate the rate of change for each state variable according to (dxdt_p[i] - dxdt_c[i])/h
 *   - (4) We repeat steps (2) and (3) for each state variable
 * 
 *  The alternative method would be:
 *   - (1) We make a backward perturbation by substracting h from one state variable and calculating the time derivatives (called dxdt_b for backward)
 *   - (2) We make a forward perturbation by adding h to the same state variable and calculating the time derivatives (called dxdt_f for forward)
 *   - (3) We calculate the rate of change for each state variable according to (dxdt_f[i] - dxdt_b[i])/(2*h)
 *   - (4) We repeat steps (1) through (3) for each state variable
 * 
 *  In the simpler scheme, we make N + 1 derivative evaluations, where N is the number of state variables.
 *  In the other scheme, we make 2N derivative evaluations.
 *  The improvement in accuracy does not seem to outweigh the cost of additional calculations, since BioCro derivatives are expensive.
 *  Likewise, higher-order numerical derivative calculations are also not worthwhile.
 */
template <typename state_type, typename jacobi_type, typename time_type>
void System::operator()(state_type const& x, jacobi_type& jacobi, time_type const& t, state_type& dfdt)
{
    size_t n = x.size();

    // Make vectors to store the current and perturbed dxdt
    state_type dxdt_c(n);
    state_type dxdt_p(n);

    // Get the current dxdt
    operator()(x, dxdt_c, t);

    // Perturb each state variable and find the corresponding change in the derivative
    double h;
    state_type xperturb = x;
    for (size_t i = 0; i < n; i++) {
        // Ensure that the step size h is close to eps_deriv but is exactly representable
        //  (see Numerical Recipes in C, 2nd ed., Section 5.7)
        h = calculation_constants::eps_deriv;
        double temp = x[i] + h;
        h = temp - x[i];

        // Calculate the new derivatives
        xperturb[i] = x[i] + h;           // Add h to the ith state variable
        operator()(xperturb, dxdt_p, t);  // Calculate dxdt_p

        // Store the results in the Jacobian matrix
        for (size_t j = 0; j < n; j++) jacobi(j, i) = (dxdt_p[j] - dxdt_c[j]) / h;

        // Reset the ith state variable
        xperturb[i] = x[i];  // Reset the ith state variable
    }

    // Perturb the time and find the corresponding change in dxdt
    // Use a forward step whenever possible
    h = calculation_constants::eps_deriv;
    double temp = t + h;
    h = temp - t;
    if (t + h <= (double)this->get_ntimes() - 1.0) {
        operator()(x, dxdt_p, t + h);
        for (size_t j = 0; j < n; j++) dfdt[j] = (dxdt_p[j] - dxdt_c[j]) / h;
    } else {
        operator()(x, dxdt_p, t - h);
        for (size_t j = 0; j < n; j++) dfdt[j] = (dxdt_c[j] - dxdt_p[j]) / h;
    }
}

/**
 * @brief Gets values of the varying parameters using a discrete time index (e.g. int or size_t)
 */
template <typename time_type>
void System::update_varying_params(time_type time_indx)
{
    for (auto x : varying_ptr_pairs) *(x.first) = (*(x.second))[time_indx];
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
 * @class SystemPointerWrapper
 * 
 * @brief This is a simple wrapper class that prevents odeint from making zillions of copies of an input system.
 */
class SystemPointerWrapper
{
   public:
    SystemPointerWrapper(std::shared_ptr<System> sys) : sys(sys) {}

    template <typename state_type, typename time_type>
    void operator()(state_type const& x, state_type& dxdt, time_type const& t)
    {
        sys->operator()(x, dxdt, t);
    }

    template <typename state_type, typename jacobi_type, typename time_type>
    void operator()(state_type const& x, jacobi_type& jacobi, time_type const& t, state_type& dfdt)
    {
        sys->operator()(x, jacobi, t, dfdt);
    }

    size_t get_ntimes() const { return sys->get_ntimes(); }

   private:
    std::shared_ptr<System> sys;
};

/**
 * @class SystemCaller
 * 
 * @brief This is a simple class that allows the same object to be used as inputs to integrate_const with
 * explicit and implicit steppers
 */
class SystemCaller : public SystemPointerWrapper
{
   public:
    SystemCaller(std::shared_ptr<System> sys) : SystemPointerWrapper(sys),
                                                first(SystemPointerWrapper(sys)),
                                                second(SystemPointerWrapper(sys)) {}

    // Provide the member types and variables that a std::pair would have
    typedef SystemPointerWrapper first_type;
    typedef first_type second_type;
    first_type first;
    second_type second;
};

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
    // Data members
    std::vector<state_type>& m_states;
    std::vector<double>& m_times;
    double max_time;
    double threshold = 0;
    double threshold_increment = 0.02;
    std::string& msg;

    // Constructor
    push_back_state_and_time(
        std::vector<state_type>& states,
        std::vector<double>& times,
        double maximum_time,
        std::string& message) : m_states(states),
                                m_times(times),
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
        m_states.push_back(x);
        m_times.push_back(t);
    }
};

/**
 * @brief Calculates all output parameters for a list of state parameter values and their associated times using the system object's public functions
 */
template <typename state_type, typename time_type>
state_vector_map get_results_from_system(std::shared_ptr<System> sys, std::vector<state_type> const& x_vec, std::vector<time_type> const& times)
{
    std::unordered_map<std::string, std::vector<double>> results;

    // Initialize the quantity names and their associated time series
    std::vector<double> temporary(x_vec.size());
    string_vector output_param_names = sys->get_output_param_names();
    for (std::string const& p : output_param_names) results[p] = temporary;

    // Add an entry for ncalls (hopefully there will be a better way to do this someday)
    std::fill(temporary.begin(), temporary.end(), sys->get_ncalls());
    results["ncalls"] = temporary;

    // Get pointers to the output quantities
    std::vector<const double*> output_param_ptrs = sys->get_quantity_access_ptrs(output_param_names);

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
