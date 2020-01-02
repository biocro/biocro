#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>
#include <set>
#include <unordered_map>
#include <memory>       // For unique_ptr and shared_ptr
#include <cmath>        // For fmod
#include <time.h>       // For timing during performance testing
#include "modules.h"
#include "module_wrapper_factory.h"
#include "system_helper_functions.h"
#include "state_map.h"

class System {
    // This class defines a system of differential equations by storing a state, a list of modules, and all of their input/output parameters
    // It is designed to be passed to a boost::odeint integrator via a SystemCaller
    // The time input to operator() can be integer or double (any other type will throw an error):
    //  - For integer time, the varying parameters are extracted using the time as an index, i.e. varying_parameters[time]. This method can only work
    //    with fixed-step-size integration methods such as the Euler method.
    //  - For double time, the varying parameters are extracted by interpolating between two indices, e.g. between varying_parameters[floor(time)] and
    //    varying_parameters[ceil(time)]. This method is applicable for adaptive integration methods such as Runge-Kutta-Cash-Karp or fixed-step
    //    integration methods that evaluate derivatives at fractional time steps such as 4th Order Runge-Kutta
    // The state vector (and its derivative) can be either std::vector<double> or boost::numeric::ublas::vector<double>
    //  - boost::numeric::ublas::vector<double> is required for stiff systems, since the odeint rosenbrock4 stepper can only use this type
    public:
        System(
            std::unordered_map<std::string, double> const& init_state,
            std::unordered_map<std::string, double> const& invariant_params,
            std::unordered_map<std::string, std::vector<double>> const &varying_params,
            std::vector<std::string> const &ss_module_names,
            std::vector<std::string> const &deriv_module_names,
            bool const verb,
            void (*print_fcn_ptr) (char const *format, ...) = void_printf);

        System(
            std::unordered_map<std::string, double> const& init_state,
            std::unordered_map<std::string, double> const& invariant_params,
            std::unordered_map<std::string, std::vector<double>> const &varying_params,
            std::vector<std::string> const &ss_module_names,
            std::vector<std::string> const &deriv_module_names,
            void (*print_fcn_ptr) (char const *format, ...) = void_printf);
        // Possibly helpful functions
        double get_timestep() const {return *timestep_ptr;}
        size_t get_ntimes() const {return ntimes;}
        bool is_adaptive_compatible() const {return adaptive_compatible;}

        // For fitting via nlopt
        void reset();

        // For integrating
        template<typename state_type>
            void get_state(state_type& x) const;

        template<typename state_type, typename time_type>
            void operator()(const state_type& x, state_type& dxdt, const time_type& t);

        template<typename state_type, typename jacobi_type, typename time_type>
            void operator()(const state_type& x, jacobi_type& jacobi, const time_type& t, state_type& dfdt);

        // For returning the results of a calculation
        template<typename state_type, typename time_type>
            std::unordered_map<std::string, std::vector<double>> get_results(const std::vector<state_type>& x_vec, const std::vector<time_type>& times);

        std::vector<std::string> get_output_param_names() const {return output_param_vector;}
        std::vector<const double*> get_output_ptrs() const {return output_ptr_vector;}
        std::vector<std::string> get_state_parameter_names() const {return keys(initial_state);}

        // For performance testing
        int get_ncalls() const {return ncalls;}
        template<class vector_type, class time_type> int speed_test(int n, const vector_type& x, vector_type& dxdt, const time_type& t);

    private:
        // Members for storing the original inputs
        std::unordered_map<std::string, double> initial_state;
        std::unordered_map<std::string, double> invariant_parameters;
        std::unordered_map<std::string, std::vector<double>> varying_parameters;
        const std::vector<std::string> steady_state_module_names;
        const std::vector<std::string> derivative_module_names;
        bool verbose = false;
        void (*print_msg) (char const *format, ...);    // A pointer to a function that takes a pointer to a null-terminated string followed by additional optional arguments, and has no return value

        // Functions for checking and processing inputs when constructing a system
        void process_variable_and_module_inputs(
            std::set<std::string>& unique_steady_state_parameter_names,
            std::set<std::string>& unique_changing_parameters);
        void basic_input_checks();
        void get_variables_from_input_lists(
            std::set<std::string>& unique_variable_names,
            std::set<std::string>& unique_changing_parameters,
            std::vector<std::string>& duplicate_parameter_names
        );
        void get_variables_from_modules(
            module_wrapper_factory& module_factory,
            std::set<std::string>& unique_steady_state_parameter_names,
            std::set<std::string>& unique_derivative_outputs,
            std::set<std::string>& unique_variable_names,
            std::set<std::string>& unique_module_inputs,
            std::set<std::string>& unique_changing_parameters,
            std::vector<std::string>& undefined_input_variables,
            std::vector<std::string>& duplicate_output_variables,
            std::vector<std::string>& duplicate_module_names,
            std::vector<std::string>& illegal_output_variables
        );
        void check_variable_usage(
            std::set<std::string> const& unique_derivative_outputs,
            std::set<std::string> const& unique_module_inputs
        ) const;
        void create_modules(
            module_wrapper_factory& module_factory,
            std::vector<std::string>& incorrect_modules
        );
        template<typename name_list>
        void get_pointer_pairs(name_list const& unique_steady_state_parameter_names);

        void test_all_modules(std::string& total_error_string);
        void get_simulation_info(std::set<std::string> const& unique_changing_parameters);

        // Map for storing the central quantities list
        std::unordered_map<std::string, double> quantities;

        // Map for storing module outputs
        std::unordered_map<std::string, double> module_output_map;

        // Pointers for accessing various inputs and outputs
        double* timestep_ptr;
        std::vector<std::pair<double*, double*>> state_ptrs;
        std::vector<std::pair<double*, double*>> steady_state_ptrs;
        std::vector<std::pair<double*, std::vector<double>*>> varying_ptrs;

        // Functions for updating the central parameter list
        void update_varying_params(int time_indx);      // For integer time
        void update_varying_params(double time_indx);   // For double time
        template<class vector_type> void update_state_params(const vector_type& new_state);

        // Lists of modules
        std::vector<std::unique_ptr<Module>> steady_state_modules;
        std::vector<std::unique_ptr<Module>> derivative_modules;

        // For integrating via odeint or other methods
        size_t ntimes;
        std::vector<std::string> output_param_vector;
        std::vector<const double*> output_ptr_vector;
        bool adaptive_compatible;

        // For running the modules
        void run_steady_state_modules();
        template<class vector_type> void run_derivative_modules(vector_type& derivs);

        // For testing the modules
        void test_steady_state_modules();
        template<class vector_type> void test_derivative_modules(vector_type& derivs);

        // For performance testing
        int ncalls;

        // For numerically calculating derivatives
        const double eps_deriv = 1e-11;
};

template<typename state_type>
void System::get_state(state_type& x) const {
    x.resize(state_ptrs.size());
    for(size_t i = 0; i < x.size(); i++) x[i] = *(state_ptrs[i].first);
}

template<typename state_type, typename time_type>
void System::operator()(const state_type& x, state_type& dxdt, const time_type& t)
{
    ++ncalls;
    update_varying_params(t);
    update_state_params(x);
    run_steady_state_modules();
    run_derivative_modules(dxdt);
}

template<typename state_type, typename jacobi_type, typename time_type>
void System::operator()(const state_type& x, jacobi_type& jacobi, const time_type& t, state_type& dfdt)
{
    // Numerically compute the Jacobian matrix
    //  The odeint Rosenbrock stepper requires the use of UBLAS vectors and matrices and the Jacobian is only required when using this
    //    stepper, so we can restrict the state vector type to be UBLAS
    // Discussion of step size from http://www.iue.tuwien.ac.at/phd/khalil/node14.html:
    //  "It is known that numerical differentiation is an unstable procedure prone to truncation and subtractive cancellation errors.
    //  Decreasing the step size will reduce the truncation error.
    //  Unfortunately a smaller step has the opposite effect on the cancellation error.
    //  Selecting the optimal step size for a certain problem is computationally expensive and the benefits achieved are not justifiable
    //    as the effect of small errors in the values of the elements of the Jacobian matrix is minor.
    //  For this reason, the sizing of the finite difference step is not attempted and a constant increment size is used in evaluating the gradient."
    // In BioCro, we fix a step size and only evaluate the forward perturbation to reduce calculation costs
    //  In other words:
    //    (1) We calculate dxdt using the input (x,t) (called dxdt_c for current)
    //    (2) We make a forward perturbation by adding h to one state variable and calculating the time derivatives (called dxdt_p for perturbation)
    //    (3) We calculate the rate of change for each state variable according to (dxdt_p[i] - dxdt_c[i])/h
    //    (4) We repeat steps (2) and (3) for each state variable
    //  The alternative method would be:
    //    (1) We make a backward perturbation by substracting h from one state variable and calculating the time derivatives (called dxdt_b for backward)
    //    (2) We make a forward perturbation by adding h to the same state variable and calculating the time derivatives (called dxdt_f for forward)
    //    (3) We calculate the rate of change for each state variable according to (dxdt_f[i] - dxdt_b[i])/(2*h)
    //    (4) We repeat steps (1) through (3) for each state variable
    //  In the simpler scheme, we make N + 1 derivative evaluations, where N is the number of state variables
    //  In the other scheme, we make 2N derivative evaluations
    //  The improvement in accuracy does not seem to outweigh the cost of additional calculations, since BioCro derivatives are expensive
    //  Likewise, higher-order numerical derivative calculations are also not worthwhile
    
    size_t n = x.size();
    
    // Make vectors to store the current and perturbed dxdt
    state_type dxdt_c(n);
    state_type dxdt_p(n);
    
    // Get the current dxdt
    operator()(x, dxdt_c, t);
    
    // Perturb each state variable and find the corresponding change in the derivative
    double h;
    state_type xperturb = x;
    for(size_t i = 0; i < n; i++) {
        // Ensure that the step size h is close to eps_deriv but is exactly representable
        //  (see Numerical Recipes in C, 2nd ed., Section 5.7)
        h = eps_deriv;
        double temp = x[i] + h;
        h = temp - x[i];
        
        // Calculate the new derivatives
        xperturb[i] = x[i] + h;             // Add h to the ith state variable
        operator()(xperturb, dxdt_p, t);    // Calculate dxdt_p
        
        // Store the results in the Jacobian matrix
        for(size_t j = 0; j < n; j++) jacobi(j,i) = (dxdt_p[j] - dxdt_c[j])/h;
        
        // Reset the ith state variable
        xperturb[i] = x[i];                 // Reset the ith state variable
    }
    
    // Perturb the time and find the corresponding change in dxdt
    // Use a forward step whenever possible
    h = eps_deriv;
    double temp = t + h;
    h = temp - t;
    if(t + h <= (double)ntimes - 1.0) {
        operator()(x, dxdt_p, t + h);
        for(size_t j = 0; j < n; j++) dfdt[j] = (dxdt_p[j] - dxdt_c[j])/h;
    }
    else {
        operator()(x, dxdt_p, t - h);
        for(size_t j = 0; j < n; j++) dfdt[j] = (dxdt_c[j] - dxdt_p[j])/h;
    }
}

template<typename state_type, typename time_type>
std::unordered_map<std::string, std::vector<double>> System::get_results(const std::vector<state_type>& x_vec, const std::vector<time_type>& times)
{
    std::unordered_map<std::string, std::vector<double>> results;
    
    // Initialize the parameter names
    std::vector<double> temp(x_vec.size());
    for (std::string const & p : output_param_vector) results[p] = temp;
    
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

template<class vector_type>
void System::update_state_params(const vector_type& new_state) {
    for(size_t i = 0; i < new_state.size(); i++) {
        *(state_ptrs[i].first) = new_state[i];
    }
}

template<class vector_type>
void System::run_derivative_modules(vector_type& dxdt) {
    for(auto const & x : state_ptrs) {
        *x.second = 0.0;                                                               // Reset the module output map
    }
    std::fill(dxdt.begin(), dxdt.end(), 0);                                                                 // Reset the derivative vector
    for (auto it = derivative_modules.begin(); it != derivative_modules.end(); ++it) {
        (*it)->run();           // Run the modules
    }
    for(size_t i = 0; i < dxdt.size(); i++) {
        dxdt[i] += *(state_ptrs[i].second)*(*timestep_ptr);             // Store the output in the derivative vector
    }
}

template<class vector_type>
void System::test_derivative_modules(vector_type& dxdt) {
    // Identical to run_derivative_modules except for a try-catch block
    for(auto const & x : state_ptrs) *x.second = 0.0;
    std::fill(dxdt.begin(), dxdt.end(), 0);
    for(auto it = derivative_modules.begin(); it != derivative_modules.end(); ++it) {
        try {(*it)->run();}
        catch (const std::exception& e) {
            throw std::logic_error(std::string("Derivative module '") + (*it)->get_name() + std::string("' generated an exception while calculating time derivatives: ") + e.what() + std::string("\n"));
        }
    }
    for(size_t i = 0; i < dxdt.size(); i++) dxdt[i] += *(state_ptrs[i].second)*(*timestep_ptr);
}

template<class vector_type, class time_type>
int System::speed_test(int n, const vector_type& x, vector_type& dxdt, const time_type& t) {
    // Run the system operator n times
    clock_t ct = clock();
    for(int i = 0; i < n; i++) operator()(x, dxdt, t);
    ct = clock() - ct;
    return (int)ct;
}

/////////////////////////
// FOR USE WITH ODEINT //
/////////////////////////

class SystemCaller {
    // This is a simple class whose purpose is to prevent odeint from making zillions of copies of an input system
    public:
        SystemCaller(std::shared_ptr<System> sys) : _sys(sys) {}
        template<typename state_type, typename time_type>
        void operator() (const state_type& x, state_type& dxdt, const time_type& t) {_sys->operator()(x, dxdt, t);}

        template<typename state_type, typename jacobi_type, typename time_type>
        void operator() (const state_type& x, jacobi_type& jacobi, const time_type& t, state_type& dfdt) {_sys->operator()(x, jacobi, t, dfdt);}

    private:
        std::shared_ptr<System> _sys;
};

// Observer used to store values

template <typename state_type>
struct push_back_state_and_time
{
    std::vector<state_type>& m_states;
    std::vector<double>& m_times;
    double _max_time;
    double threshold = 0;
    bool _verbose;
    void (*print_msg) (char const *format, ...);    // A pointer to a function that takes a pointer to a null-terminated string followed by additional optional arguments, and has no return value
    
    push_back_state_and_time(
        std::vector<state_type>& states,
        std::vector<double> &times, double max_time,
        bool verbose,
        void (*print_fcn_ptr) (char const *format, ...) = void_printf) :
        m_states(states),
        m_times(times),
        _max_time(max_time),
        _verbose(verbose),
        print_msg(print_fcn_ptr)
    {}
    
    void operator()(const state_type & x, double t) {
        if(_verbose) {
            if(t >= _max_time) print_msg("Timestep = %f (%f%% done) at clock = %u microseconds\n", t, t/_max_time*100.0, (unsigned int) clock());
            else if(t/_max_time >= threshold) {
                print_msg("Timestep = %f (%f%% done) at clock = %u microseconds\n", t, t/_max_time*100.0, (unsigned int) clock());
                threshold += 0.02;
            }
        }
        m_states.push_back(x);
        m_times.push_back(t);
    }
};

#endif
