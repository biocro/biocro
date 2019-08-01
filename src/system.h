#ifndef SYSTEM_H
#define SYSTEM_H

#include <Rinternals.h>	// For Rprintf
#include <vector>
#include <set>
#include <unordered_map>
#include <memory>		// For unique_ptr and shared_ptr
#include <cmath>		// For fmod
#include <time.h>		// For timing during performance testing
//#include <boost/numeric/ublas/vector.hpp>	// For use with ODEINT
//#include <boost/numeric/ublas/matrix.hpp>	// For use with ODEINT
//#include <boost/numeric/odeint.hpp>		// For use with ODEINT
#include "modules.h"
#include "module_library/ModuleFactory.h"

typedef double time_type;
typedef double value_type;
//typedef boost::numeric::ublas::vector<value_type> state_type;
typedef std::vector<value_type> state_type;

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
	//  - std::vector<double> is significantly faster for non-stiff systems (by a factor of 3-4)
	//  - boost::numeric::ublas::vector<double> is required for stiff systems, since the odeint rosenbrock4 stepper can only use this type
	public:
		System(
			std::unordered_map<std::string, double> const& initial_state,
			std::unordered_map<std::string, double> const& invariant_parameters,
			std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
			std::vector<std::string> const &steady_state_module_names,
			std::vector<std::string> const &derivative_module_names,
			bool const verbose);
		// Possibly helpful functions
		double get_timestep() const {return *timestep_ptr;}
		bool get_state_indx(int& state_indx, const std::string& parameter_name) const;
		bool get_param(double& value, const std::string& parameter_name) const;
		size_t get_ntimes() const {return ntimes;}
		// For fitting via nlopt
		void reset();
		void set_param(const double& value, const std::string& parameter_name);
		void set_param(const std::vector<double>& values, const std::vector<std::string>& parameter_names);
		// For integrating via odeint
		void get_state(std::vector<double>& x) const;
		void operator()(const std::vector<double>& x, std::vector<double>& dxdt, const int& t);
		//template<class time_type> void operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const time_type& t, boost::numeric::ublas::vector<double>& dfdt);
		// For returning the results of a calculation
		std::unordered_map<std::string, std::vector<double>> get_results(const std::vector<std::vector<double>>& x_vec, const std::vector<int>& times);	// Used with ODEINT
		std::vector<std::string> get_output_param_names() const {return output_param_vector;}
		std::vector<const double*> get_output_ptrs() const {return output_ptr_vector;}
		// For testing speed
		template<class vector_type, class time_type> int speed_test(int n, const vector_type& x, vector_type& dxdt, const time_type& t);
		//template<class time_type> int speed_test(int n, const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const time_type& t, boost::numeric::ublas::vector<double>& dfdt);
	private:
		// Map for storing the central parameter list
		std::unordered_map<std::string, double> parameters;
		// Map for storing the initial state
		std::unordered_map<std::string, double> _initial_state;
		// Map for storing the varying parameters
		std::unordered_map<std::string, std::vector<double>> _varying_parameters;
		// Objects for storing module outputs
		std::unordered_map<std::string, double> vector_module_output;
		// Pointers for accessing various inputs and outputs
		double* timestep_ptr;
		std::vector<std::pair<double*, double*>> state_ptrs;
		std::vector<std::pair<double*, double*>> steady_state_ptrs;
		std::vector<std::pair<double*, std::vector<double>*>> varying_ptrs;
		// Functions for updating the central parameter list
		void update_varying_params(int time_indx);		// For integer time
		void update_varying_params(double time_indx);	// For double time
		template<class vector_type> void update_state_params(const vector_type& new_state);
		// Pointers to the modules
		std::vector<std::unique_ptr<Module>> steady_state_modules;
		std::vector<std::unique_ptr<Module>> derivative_modules;
		// For integrating via odeint
		size_t ntimes;
		std::vector<std::string> output_param_vector;
		std::vector<const double*> output_ptr_vector;
		std::vector<std::string> state_parameter_names;
		// For running the modules
		void run_steady_state_modules();
		template<class vector_type> void run_derivative_modules(vector_type& derivs);
		// For testing the modules
		void test_steady_state_modules();
		template<class vector_type> void test_derivative_modules(vector_type& derivs);
		// For numerically calculating derivatives
		const double eps = 1e-11;
		// For user feedback
		bool _verbose;
};

/////////////////////////
// FOR USE WITH ODEINT //
/////////////////////////

class SystemCaller {
	// This is a simple class whose purpose is to prevent odeint from making zillions of copies of an input system
	public:
		SystemCaller(std::shared_ptr<System> sys) : _sys(sys) {}
		template<class vector_type, class time_type> void operator() (const vector_type& x, vector_type& dxdt, const time_type& t) {_sys->operator()(x, dxdt, t);}
		//template<class time_type> void operator() (const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const time_type& t, boost::numeric::ublas::vector<double>& dfdt) {_sys->operator()(x, jacobi, t, dfdt);}
	private:
		std::shared_ptr<System> _sys;
};

// Observer used to store values
struct push_back_state_and_time
{
    std::vector<state_type>& m_states;
    std::vector<time_type>& m_times;
    
    push_back_state_and_time(std::vector<state_type> &states, std::vector<time_type> &times) : m_states(states), m_times(times) {}

    void operator()(const state_type &x, time_type t) {
    	m_states.push_back(x);
    	m_times.push_back(t);
	}
};

// Observer used to store values
struct push_back_state_and_time_verbose
{
    std::vector<state_type>& m_states;
    std::vector<time_type>& m_times;
    double _max_time;
    double threshold = 0;
    
    push_back_state_and_time_verbose(std::vector<state_type> &states, std::vector<time_type> &times, double max_time) : m_states(states), m_times(times), _max_time(max_time) {}

    void operator()(const state_type &x, time_type t) {
    	if(t/_max_time > threshold) {
    		Rprintf("\nTime = %f ( %f%% done) at clock = %u", t, t/_max_time*100.0, (unsigned int) clock());
    		//std::cout << "\nTime = " << t << " (" <<  t/_max_time*100.0 << "% done) at clock = " << clock() << "\n";
    		threshold += 0.02;
		}
    	m_states.push_back(x);
    	m_times.push_back(t);
	}
};

#endif
