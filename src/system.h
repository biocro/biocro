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
 * Defines a system of differential equations. Intended to be passed to a system_solver object.
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
    bool is_adaptive_compatible() const { return adaptive_compatible; }
    size_t get_ntimes() const { return ntimes; }

    template <typename state_type>
    void get_state(state_type& x) const;

    // For calculating derivatives
    template <typename state_type, typename time_type>
    void operator()(const state_type& x, state_type& dxdt, const time_type& t);

    template <typename state_type, typename jacobi_type, typename time_type>
    void operator()(const state_type& x, jacobi_type& jacobi, const time_type& t, state_type& dfdt);

    // For returning the results of a calculation
    string_vector get_state_parameter_names() const { return keys(initial_state); }
    string_vector get_output_param_names() const { return output_param_vector; }
    std::vector<const double*> get_output_ptrs() const { return output_ptr_vector; }

    template <typename state_type, typename time_type>
    state_vector_map get_results(const std::vector<state_type>& x_vec, const std::vector<time_type>& times);

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
    state_vector_map varying_parameters;
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
    std::vector<std::pair<double*, double*>> state_ptrs;
    std::vector<std::pair<double*, double*>> steady_state_ptrs;
    std::vector<std::pair<double*, std::vector<double>*>> varying_ptrs;

    // For integrating via a system_solver
    bool adaptive_compatible;
    size_t ntimes;

    // For calculating derivatives
    void update_varying_params(int time_indx);     // For integer time
    void update_varying_params(size_t time_indx);  // For size_t time
    void update_varying_params(double time_indx);  // For double time

    template <class vector_type>
    void update_state_params(const vector_type& new_state);

    void run_steady_state_modules();

    template <class vector_type>
    void run_derivative_modules(vector_type& derivs);

    // For returning the results of a calculation
    string_vector output_param_vector;
    std::vector<const double*> output_ptr_vector;

    // For generating reports to the user
    int ncalls;
    std::string startup_message;
};

/**
 * Returns the current values of all the state parameters.
 * 
 * @param[out] x An object capable of containing the state,
 * typically either std::vector<double> or boost::numeric::ublas::vector<double>.
 */
template <typename state_type>
void System::get_state(state_type& x) const
{
    x.resize(state_ptrs.size());
    for (size_t i = 0; i < x.size(); i++) x[i] = *(state_ptrs[i].first);
}

/**
 * Calculates a derivative given an input state and time. Function signature is set by the boost::odeint library.
 * 
 * @param[in] x values of the state parameters
 * @param[in] t time value
 * @param[out] dxdt derivatives of the state parameters calculated using x and t
 */
template <typename state_type, typename time_type>
void System::operator()(const state_type& x, state_type& dxdt, const time_type& t)
{
    ++ncalls;
    update_varying_params(t);
    update_state_params(x);
    run_steady_state_modules();
    run_derivative_modules(dxdt);
}

/**
 * @brief Numerically compute the Jacobian matrix. Function signature is set by the boost::odeint library.
 * 
 * @param[in] x values of the state parameters
 * @param[in] t time value
 * @param[out] jacobi jacobian matrix
 * @param[out] dfdt time dependence of derivatives (which is not included in the jacobian)
 * 
 * The odeint Rosenbrock stepper requires the use of UBLAS vectors and matrices and the Jacobian is only required when using this
 *  stepper, so we can restrict the state vector type to be UBLAS
 * Discussion of step size from http://www.iue.tuwien.ac.at/phd/khalil/node14.html:
 *  "It is known that numerical differentiation is an unstable procedure prone to truncation and subtractive cancellation errors.
 *  Decreasing the step size will reduce the truncation error.
 *  Unfortunately a smaller step has the opposite effect on the cancellation error.
 *  Selecting the optimal step size for a certain problem is computationally expensive and the benefits achieved are not justifiable
 *   as the effect of small errors in the values of the elements of the Jacobian matrix is minor.
 *  For this reason, the sizing of the finite difference step is not attempted and a constant increment size is used in evaluating the gradient."
 * In BioCro, we fix a step size and only evaluate the forward perturbation to reduce calculation costs
 *  In other words:
 *   (1) We calculate dxdt using the input (x,t) (called dxdt_c for current)
 *   (2) We make a forward perturbation by adding h to one state variable and calculating the time derivatives (called dxdt_p for perturbation)
 *   (3) We calculate the rate of change for each state variable according to (dxdt_p[i] - dxdt_c[i])/h
 *   (4) We repeat steps (2) and (3) for each state variable
 *  The alternative method would be:
 *   (1) We make a backward perturbation by substracting h from one state variable and calculating the time derivatives (called dxdt_b for backward)
 *   (2) We make a forward perturbation by adding h to the same state variable and calculating the time derivatives (called dxdt_f for forward)
 *   (3) We calculate the rate of change for each state variable according to (dxdt_f[i] - dxdt_b[i])/(2*h)
 *   (4) We repeat steps (1) through (3) for each state variable
 *  In the simpler scheme, we make N + 1 derivative evaluations, where N is the number of state variables
 *  In the other scheme, we make 2N derivative evaluations
 *  The improvement in accuracy does not seem to outweigh the cost of additional calculations, since BioCro derivatives are expensive
 *  Likewise, higher-order numerical derivative calculations are also not worthwhile
 */
template <typename state_type, typename jacobi_type, typename time_type>
void System::operator()(const state_type& x, jacobi_type& jacobi, const time_type& t, state_type& dfdt)
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
    if (t + h <= (double)ntimes - 1.0) {
        operator()(x, dxdt_p, t + h);
        for (size_t j = 0; j < n; j++) dfdt[j] = (dxdt_p[j] - dxdt_c[j]) / h;
    } else {
        operator()(x, dxdt_p, t - h);
        for (size_t j = 0; j < n; j++) dfdt[j] = (dxdt_c[j] - dxdt_p[j]) / h;
    }
}

/**
 * Calculates all output parameters for a list of state parameter values and their associated times
 */
template <typename state_type, typename time_type>
std::unordered_map<std::string, std::vector<double>> System::get_results(const std::vector<state_type>& x_vec, const std::vector<time_type>& times)
{
    std::unordered_map<std::string, std::vector<double>> results;

    // Initialize the parameter names
    std::vector<double> temp(x_vec.size());
    for (std::string const& p : output_param_vector) results[p] = temp;

    std::fill(temp.begin(), temp.end(), ncalls);
    results["ncalls"] = temp;

    // Store the data
    for (size_t i = 0; i < x_vec.size(); ++i) {
        state_type current_state = x_vec[i];
        time_type current_time = times[i];
        // Get the corresponding parameter list
        update_varying_params(current_time);
        update_state_params(current_state);
        run_steady_state_modules();
        // Add the list to the results map
        for (size_t j = 0; j < output_param_vector.size(); ++j) (results[output_param_vector[j]])[i] = quantities[output_param_vector[j]];
    }

    return results;
}

/**
 * Updates the system's internally stored state parameter values to the new ones in new_state
 */
template <class vector_type>
void System::update_state_params(const vector_type& new_state)
{
    for (size_t i = 0; i < new_state.size(); i++) {
        *(state_ptrs[i].first) = new_state[i];
    }
}

/**
 * Calculates a derivative from the internally stored quantity map by running all the derivative modules
 * 
 * @param[out] dxdt a vector for storing the derivative (passed by reference for speed)
 */
template <class vector_type>
void System::run_derivative_modules(vector_type& dxdt)
{
    for (auto const& x : state_ptrs) {
        *x.second = 0.0;  // Reset the module output map
    }
    std::fill(dxdt.begin(), dxdt.end(), 0);  // Reset the derivative vector
    for (auto it = derivative_modules.begin(); it != derivative_modules.end(); ++it) {
        (*it)->run();  // Run the modules
    }
    for (size_t i = 0; i < dxdt.size(); i++) {
        dxdt[i] += *(state_ptrs[i].second) * (*timestep_ptr);  // Store the output in the derivative vector
    }
}

/**
 * @class SystemPointerWrapper
 * 
 * This is a simple wrapper class that prevents odeint from making zillions of copies of an input system.
 */
class SystemPointerWrapper
{
   public:
    SystemPointerWrapper(std::shared_ptr<System> sys) : sys(sys) {}

    template <typename state_type, typename time_type>
    void operator()(const state_type& x, state_type& dxdt, const time_type& t)
    {
        sys->operator()(x, dxdt, t);
    }

    template <typename state_type, typename jacobi_type, typename time_type>
    void operator()(const state_type& x, jacobi_type& jacobi, const time_type& t, state_type& dfdt)
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
 * This is a simple class that allows the same object to be used as inputs to integrate_const with
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
 * An observer class used to store state and time values during an odeint simulation
 */
template <typename state_type>
struct push_back_state_and_time {
    std::vector<state_type>& m_states;
    std::vector<double>& m_times;
    double _max_time;
    double threshold = 0;
    bool _verbose;
    void (*print_msg)(char const* format, ...);  // A pointer to a function that takes a pointer to a null-terminated string followed by additional optional arguments, and has no return value

    push_back_state_and_time(
        std::vector<state_type>& states,
        std::vector<double>& times,
        double max_time,
        bool verbose,
        void (*print_fcn_ptr)(char const* format, ...) = void_printf) : m_states(states),
                                                                        m_times(times),
                                                                        _max_time(max_time),
                                                                        _verbose(verbose),
                                                                        print_msg(print_fcn_ptr) {}

    void operator()(const state_type& x, double t)
    {
        if (_verbose) {
            if (t >= _max_time)
                print_msg("Timestep = %f (%f%% done) at clock = %u microseconds\n", t, t / _max_time * 100.0, (unsigned int)clock());
            else if (t / _max_time >= threshold) {
                print_msg("Timestep = %f (%f%% done) at clock = %u microseconds\n", t, t / _max_time * 100.0, (unsigned int)clock());
                threshold += 0.02;
            }
        }
        m_states.push_back(x);
        m_times.push_back(t);
    }
};

#endif
