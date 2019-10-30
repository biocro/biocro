#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>
#include <set>
#include <unordered_map>
#include <memory>       // For unique_ptr and shared_ptr
#include <cmath>        // For fmod
#include <time.h>       // For timing during performance testing
#include <boost/numeric/ublas/vector.hpp>   // For use with ODEINT
#include <boost/numeric/ublas/matrix.hpp>   // For use with ODEINT
#include <boost/numeric/odeint.hpp>         // For use with ODEINT
#include "modules.h"
#include "module_library/ModuleFactory.h"
#include "system_helper_functions.h"

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
        // Possibly helpful functions
        double get_timestep() const {return *timestep_ptr;}
        bool get_state_indx(int& state_indx, const std::string& parameter_name) const;
        bool get_param(double& value, const std::string& parameter_name) const;
        size_t get_ntimes() const {return ntimes;}
        bool is_adaptive_compatible() const {return adaptive_compatible;}
        // For fitting via nlopt
        void reset();
        void set_param(const double& value, const std::string& parameter_name);
        void set_param(const std::vector<double>& values, const std::vector<std::string>& parameter_names);
        // For integrating
        // Note: I can't seem use a template for get_state(x) or operator(x, dxdt, t). I'm not sure why.
        void get_state(std::vector<double>& x) const;
        void get_state(boost::numeric::ublas::vector<double>& x) const;
        void operator()(const std::vector<double>& x, std::vector<double>& dxdt, const int& t);
        void operator()(const std::vector<double>& x, std::vector<double>& dxdt, const double& t);
        void operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const int& t);
        void operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t);
        void operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const double& t, boost::numeric::ublas::vector<double>& dfdt);
        // For returning the results of a calculation
        std::unordered_map<std::string, std::vector<double>> get_results(const std::vector<std::vector<double>>& x_vec, const std::vector<int>& times);
        std::unordered_map<std::string, std::vector<double>> get_results(const std::vector<boost::numeric::ublas::vector<double>>& x_vec, const std::vector<double>& times);
        std::vector<std::string> get_output_param_names() const {return output_param_vector;}
        std::vector<const double*> get_output_ptrs() const {return output_ptr_vector;}
        std::vector<std::string> get_state_parameter_names() const {return state_parameter_names;}
        // For testing speed
        template<class vector_type, class time_type> int speed_test(int n, const vector_type& x, vector_type& dxdt, const time_type& t);
        int speed_test(int n, const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const double& t, boost::numeric::ublas::vector<double>& dfdt);
    private:
        // Members for storing the original inputs
        std::unordered_map<std::string, double> initial_state;
        std::unordered_map<std::string, double> invariant_parameters;
        std::unordered_map<std::string, std::vector<double>> varying_parameters;
        std::vector<std::string> steady_state_module_names;
        std::vector<std::string> derivative_module_names;
        bool verbose;
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
            ModuleFactory& module_factory,
            std::set<std::string>& unique_steady_state_module_names,
            std::set<std::string>& unique_steady_state_parameter_names,
            std::set<std::string>& unique_derivative_module_names,
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
            ModuleFactory& module_factory,
            std::vector<std::string>& incorrect_modules
        );
        void get_pointer_pairs(std::set<std::string> const& unique_steady_state_parameter_names);
        void test_all_modules(std::string& total_error_string);
        void get_simulation_info(std::set<std::string> const& unique_changing_parameters);
        void process_errors(std::vector<std::string> const& error_list, std::string error_msg, std::string& total_error_string) const;
        void report_errors(std::string& total_error_string) const;
        // Map for storing the central parameter list
        std::unordered_map<std::string, double> parameters;
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
        std::vector<std::string> state_parameter_names;
        bool adaptive_compatible;
        // For running the modules
        void run_steady_state_modules();
        template<class vector_type> void run_derivative_modules(vector_type& derivs);
        // For testing the modules
        void test_steady_state_modules();
        template<class vector_type> void test_derivative_modules(vector_type& derivs);
        // For numerically calculating derivatives
        const double eps_deriv = 1e-11;
};

/////////////////////////
// FOR USE WITH ODEINT //
/////////////////////////

class SystemCaller {
    // This is a simple class whose purpose is to prevent odeint from making zillions of copies of an input system
    public:
        SystemCaller(std::shared_ptr<System> sys) : _sys(sys) {}
        void operator() (const std::vector<double>& x, std::vector<double>& dxdt, const int& t) {_sys->operator()(x, dxdt, t);}
        void operator() (const std::vector<double>& x, std::vector<double>& dxdt, const double& t) {_sys->operator()(x, dxdt, t);}
        void operator() (const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const int& t) {_sys->operator()(x, dxdt, t);}
        void operator() (const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t) {_sys->operator()(x, dxdt, t);}
        void operator() (const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const double& t, boost::numeric::ublas::vector<double>& dfdt) {_sys->operator()(x, jacobi, t, dfdt);}
    private:
        std::shared_ptr<System> _sys;
};

// Observer used to store values
struct push_back_state_and_time_rsnbrk
{
    std::vector<boost::numeric::ublas::vector<double>>& m_states;
    std::vector<double>& m_times;
    double _max_time;
    double threshold = 0;
    bool _verbose;
    void (*print_msg) (char const *format, ...);    // A pointer to a function that takes a pointer to a null-terminated string followed by additional optional arguments, and has no return value
    
    push_back_state_and_time_rsnbrk(
        std::vector<boost::numeric::ublas::vector<double>> &states,
        std::vector<double> &times, double max_time,
        bool verbose,
        void (*print_fcn_ptr) (char const *format, ...) = void_printf) :
        m_states(states),
        m_times(times),
        _max_time(max_time),
        _verbose(verbose),
        print_msg(print_fcn_ptr)
    {}
    
    void operator()(const boost::numeric::ublas::vector<double> &x, double t) {
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
