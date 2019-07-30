#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>		// For saving results to a file
#include <vector>
#include <set>
#include <memory>		// For unique_ptr and shared_ptr
#include <cmath>		// For fmod
#include <time.h>		// For timing during performance testing
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/odeint.hpp>
#include "module.hpp"
#include "module_library/module_factory.hpp"

typedef double time_type;
typedef double value_type;
typedef boost::numeric::ublas::vector<value_type> state_type;

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

struct push_back_state_and_time_verbose
{
    std::vector<state_type>& m_states;
    std::vector<time_type>& m_times;
    double _max_time;
    double threshold = 0;
    
    push_back_state_and_time_verbose(std::vector<state_type> &states, std::vector<time_type> &times, double max_time) : m_states(states), m_times(times), _max_time(max_time) {}

    void operator()(const state_type &x, time_type t) {
    	if(t/_max_time > threshold) {
    		std::cout << "\nTime = " << t << " (" <<  t/_max_time*100.0 << "% done) at clock = " << clock() << "\n";
    		threshold += 0.02;
		}
    	m_states.push_back(x);
    	m_times.push_back(t);
	}
};

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
		template<class vector_type> void get_state(vector_type& x) const;
		template<class vector_type, class time_type> void operator()(const vector_type& x, vector_type& dxdt, const time_type& t);
		template<class time_type> void operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const time_type& t, boost::numeric::ublas::vector<double>& dfdt);
		// For saving or returning the results of an odeint calculation
		template<class vector_type, class time_type> void print_results(const std::vector<vector_type>& x_vec, const std::vector<time_type>& times, const std::string& name);
		template<class vector_type, class time_type> std::unordered_map<std::string, std::vector<double>> get_results(const std::vector<vector_type>& x_vec, const std::vector<time_type>& times);
		// For testing speed
		template<class vector_type, class time_type> int speed_test(int n, const vector_type& x, vector_type& dxdt, const time_type& t);
		template<class time_type> int speed_test(int n, const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const time_type& t, boost::numeric::ublas::vector<double>& dfdt);
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

class SystemCaller {
	// This is a simple class whose purpose is to prevent odeint from making zillions of copies of an input system
	public:
		SystemCaller(std::shared_ptr<System> sys) : _sys(sys) {}
		template<class vector_type, class time_type> void operator() (const vector_type& x, vector_type& dxdt, const time_type& t) {_sys->operator()(x, dxdt, t);}
		template<class time_type> void operator() (const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const time_type& t, boost::numeric::ublas::vector<double>& dfdt) {_sys->operator()(x, jacobi, t, dfdt);}
	private:
		std::shared_ptr<System> _sys;
};

template<class vector_type, class time_type> void System::operator()(const vector_type& x, vector_type& dxdt, const time_type& t) {
	// Update the internally stored parameter list and use it to calculate a derivative
	update_varying_params(t);
	update_state_params(x);
	run_steady_state_modules();
	run_derivative_modules(dxdt);
}

template<class time_type> void System::operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const time_type& t, boost::numeric::ublas::vector<double>& dfdt) {
	// Numerically compute the Jacobian matrix
	//  The odeint Rosenbrock stepper requires the use of UBLAS vectors and matrices and the Jacobian is only required when using this
	//    stepper, so we can restrict the state vector type to be UBLAS
	// Discussion of step size from http://www.iue.tuwien.ac.at/phd/khalil/node14.html:
	//  It is also known that numerical differentiation is an unstable procedure prone to truncation and subtractive cancellation errors.
	//  Decreasing the step size will reduce the truncation error.
	//  Unfortunately a smaller step has the opposite effect on the cancellation error.
	//  Selecting the optimal step size for a certain problem is computationally expensive and the benefits achieved are not justifiable
	//    as the effect of small errors in the values of the elements of the Jacobian matrix is minor.
	//  For this reason, the sizing of the finite difference step is not attempted and a constant increment size is used in evaluating the gradient.
	// In BioCro, we only evaluate the forward perturbation to reduce calculation costs
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
	
	int n = x.size();
	
	// Make vectors to store the current and perturbed dxdt
	boost::numeric::ublas::vector<double> dxdt_c(n);
	boost::numeric::ublas::vector<double> dxdt_p(n);
	
	// Get the current dxdt
	operator()(x, dxdt_c, t);
	
	// Perturb each state variable and find the corresponding change in the derivative
	double h;
	boost::numeric::ublas::vector<double> xperturb = x;
	for(int i = 0; i < n; i++) {
		// Ensure that the step size h is close to eps but is exactly representable
		//  (see Numerical Recipes in C, 2nd ed., Section 5.7)
		h = eps;
		double temp = x[i] + h;
		h = temp - x[i];
		
		// Calculate the new derivatives
		xperturb[i] = x[i] + h;				// Add h to the ith state variable
		operator()(xperturb, dxdt_p, t);	// Calculate dxdt_p
		
		// Store the results in the Jacobian matrix
		for(int j = 0; j < n; j++) jacobi(j,i) = (dxdt_p[j] - dxdt_c[j])/h;
		
		// Reset the ith state variable
		xperturb[i] = x[i];				// Reset the ith state variable
	}
	
	// Perturb the time and find the corresponding change in dxdt
	// Use a forward step whenever possible
	h = eps;
	double temp = t + h;
	h = temp - t;
	if(t + h <= (double)ntimes - 1.0) {
		operator()(x, dxdt_p, t + h);
		for(int j = 0; j < n; j++) dfdt[j] = (dxdt_p[j] - dxdt_c[j])/h;
	}
	else {
		operator()(x, dxdt_p, t - h);
		for(int j = 0; j < n; j++) dfdt[j] = (dxdt_c[j] - dxdt_p[j])/h;
	}
}

System::System(
	std::unordered_map<std::string, double> const& initial_state,
	std::unordered_map<std::string, double> const& invariant_parameters,
	std::unordered_map<std::string, std::vector<double>> const &varying_parameters,
	std::vector<std::string> const &steady_state_module_names,
	std::vector<std::string> const &derivative_module_names,
	bool const verbose) :
	_initial_state(initial_state),
	_varying_parameters(varying_parameters),
	_verbose(verbose)
{
	// A note about parameters:
	//  The system stores an unordered_map of parameters, which includes several types:
	//   - The state variables that evolve according to differential equations and whose initial values are an input to the system
	//       (e.g. position and velocity for a mass-on-a-spring harmonic oscillator)
	//   - The invariant parameters that remain constant throughout the calculation
	//       (e.g. mass and spring constant for a mass-on-a-spring harmonic oscillator)
	//   - The varying parameters that change throughout the calculation in a way that is known beforehand
	//       (e.g. the temperature throughout a growing season)
	//   - The 'steady state' parameters that are calculated from other parameters at each time step
	//       (e.g. total biomass calculated by adding several state variables together)
	//
	// Rules for parameters that are enforced while building the system:
	//  - The invariant parameters must include one called 'timestep'
	//  - The parameters in the initial state, invariant parameters, and varying parameters must be unique
	//  - A module's input parameters must be included in the state, invariant parameters, or varying parameters OR be output by a previous module
	//      (i.e., a module's input must be defined before it runs)
	//  - A steady state module's output parameters must not be included in the state, invariant parameters, or varying parameters AND must not be the output of a previous module
	//      (i.e., all steady state module outputs must be unique in the system)
	//  - A derivative module's output parameters must be included in the state
	//      (i.e., derivatives can only be defined for state variables)
	
	// Lists for collecting parameters and modules
	std::set<std::string> unique_parameter_names;					// All parameter names
	std::set<std::string> unique_steady_state_module_names;			// All steady state module names
	std::set<std::string> unique_derivative_module_names;			// All derivative module names
	std::set<std::string> unique_steady_state_parameter_names;		// All parameters output by steady state modules
	std::set<std::string> unique_derivative_outputs;				// All parameters output by derivative modules
	std::set<std::string> unique_module_inputs;						// All parameters used as inputs to modules
	std::set<std::string> unique_changing_parameters;				// All parameters that change throughout a simulation (used for saving/returning results, since we shouldn't include invariant parameters)
	// Lists for describing problems with the inputs
	std::vector<std::string> duplicate_parameter_names;				// A list of parameter names that are duplicated in the initial state, invariant parameters, and varying parameters
	std::vector<std::string> duplicate_module_names;				// A list of module names that are duplicated
	std::vector<std::string> duplicate_output_parameters;			// A list of parameter names that are duplicated in the output of steady state modules
	std::vector<std::string> undefined_input_parameters;			// A list of module input parameter names that are not defined when the module runs
	std::vector<std::string> illegal_output_parameters;				// A list of derivative module output parameter names that are not state variables
	std::vector<std::string> incorrect_modules;						// A list of mischaracterized modules, e.g., derivative modules included in the steady state module list
	std::string error_string;										// A message to send to the user about any issues that were discovered during the system setup
	
	// Open an output file
	std::ofstream myfile;
	if(_verbose) {
	  	myfile.open ("system_startup_record.txt", std::ios::out | std::ios::trunc);
		myfile << "Starting to apply checks and build a system...\n\n";
	}
	
	// Check to make sure at least one module was supplied
	if(_verbose) myfile << "Checking to make sure at least one module was specified... ";
	if(steady_state_module_names.size() == 0 && derivative_module_names.size() == 0) {
		throw std::logic_error(std::string("No input modules were found! A system requires at least one module.\n"));
	}
	if(_verbose) myfile << "done!\n\n";
	
	// Check to make sure the timing information has been properly supplied
	if(_verbose) myfile << "Checking to make sure the time parameters are properly defined... ";
	if(invariant_parameters.find("timestep") == invariant_parameters.end()) {
		throw std::logic_error(std::string("The 'timestep' parameter was not defined in the invariant parameters. This is a required parameter for any system.\n"));
	}
	unique_module_inputs.insert("timestep");
	if(_varying_parameters.find("doy") == _varying_parameters.end()) {
		throw std::logic_error(std::string("The 'doy' parameter was not defined in the varying parameters. This is a required parameter for any system.\n"));
	}
	if(_varying_parameters.find("hour") == _varying_parameters.end()) {
		throw std::logic_error(std::string("The 'hour' parameter was not defined in the varying parameters. This is a required parameter for any system.\n"));
	}
	if(_varying_parameters.find("doy_dbl") != _varying_parameters.end() || invariant_parameters.find("doy_dbl") != invariant_parameters.end() || initial_state.find("doy_dbl") != initial_state.end()) {
		throw std::logic_error(std::string("'doy_dbl' is a reserved parameter name. Please remove or rename this parameter.\n"));
	}
	std::vector<double> hour_vec = _varying_parameters.at("hour");
	for(size_t i = 1; i < hour_vec.size(); i++ ) {
		if(abs(hour_vec[i] - hour_vec[i-1] - 24.0 * floor((hour_vec[i] - hour_vec[i-1]) / 24.0) - invariant_parameters.at("timestep")) > 0.01) {
			throw std::logic_error(std::string("At least one pair of sequential values of 'hour' are not separated by the value of 'timestep'. Please check over the inputs.\n"));
		}
	}
	std::vector<double> doy_vec = _varying_parameters.at("doy");
	std::vector<double> doy_dbl_vec(doy_vec.size());
	for(size_t i = 0; i < doy_dbl_vec.size(); i++) doy_dbl_vec[i] = hour_vec[i]/24.0 + doy_vec[i];		// Make a new parameter expressing the day of year as a double (note: might have trouble with multi-year simulations)
	_varying_parameters["doy_dbl"] = doy_dbl_vec;
	_varying_parameters.erase("doy");
	_varying_parameters.erase("hour");
	if(_verbose) myfile << "done!\n\n";
	
	// Start collecting parameter names and check to make sure all the requirements are met
	if(_verbose) myfile << "Building list of parameters...\n\n";
	
	// Go through the initial state, invariant parameters, and varying parameters
	if(_verbose) myfile << "Varying parameters:\n";
	for(auto x : _varying_parameters) {
		if(unique_parameter_names.find(x.first) == unique_parameter_names.end()) {
			unique_parameter_names.insert(x.first);
			unique_changing_parameters.insert(x.first);
			parameters[x.first] = x.second[0];
			if(_verbose) myfile << "  " << x.first << "[0] = " << x.second[0] << "\n";
		}
		else duplicate_parameter_names.push_back(std::string("Parameter '") + x.first + std::string("' from the varying parameters"));
	}
	if(_verbose) myfile << "State variables:\n";
	for(auto x : initial_state) {
		if(unique_parameter_names.find(x.first) == unique_parameter_names.end()) {
			unique_parameter_names.insert(x.first);
			unique_changing_parameters.insert(x.first);
			parameters[x.first] = x.second;
			if(_verbose) myfile << "  " << x.first << "[0] = " << x.second << "\n";
		}
		else duplicate_parameter_names.push_back(std::string("Parameter '") + x.first + std::string("' from the initial state"));
	}
	if(_verbose) myfile << "Invariant parameters:\n";
	for(auto x : invariant_parameters) {
		if(unique_parameter_names.find(x.first) == unique_parameter_names.end()) {
			unique_parameter_names.insert(x.first);
			parameters[x.first] = x.second;
			if(_verbose) myfile << "  " << x.first << " = " << x.second << "\n";
		}
		else duplicate_parameter_names.push_back(std::string("Parameter '") + x.first + std::string("' from the invariant parameters"));
	}
	
	// Initialize a module factory
	// Note: the inputs to the module factory have not been fully initialized yet. We can
	//  get the module input/output parameters right now, but any attempt to create a module
	//  will fail
	ModuleFactory module_factory(&parameters, &vector_module_output);
	
	// Continue collecting parameter names from the steady state modules
	if(_verbose) myfile << "Steady state parameters:\n";
	for(std::string module_name : steady_state_module_names) {
		if(unique_steady_state_module_names.find(module_name) == unique_steady_state_module_names.end()) {
			unique_steady_state_module_names.insert(module_name);
			for(std::string p : module_factory.get_inputs(module_name)) {
				if(unique_parameter_names.find(p) == unique_parameter_names.end()) undefined_input_parameters.push_back(std::string("Parameter '") + p + std::string("' for the '") + module_name + std::string("' steady state module"));
				if(unique_module_inputs.find(p) == unique_module_inputs.end()) unique_module_inputs.insert(p);
			}
			for(std::string p : module_factory.get_outputs(module_name)) {
				if(unique_parameter_names.find(p) == unique_parameter_names.end()) {
					unique_parameter_names.insert(p);
					unique_steady_state_parameter_names.insert(p);
					unique_changing_parameters.insert(p);
					parameters[p] = 0.0;
					if(_verbose) myfile << "  " << p << "\n";
				}
				else duplicate_output_parameters.push_back(std::string("Parameter '") + p + std::string("' from the '") + module_name + std::string("' module"));
			}
		}
		else duplicate_module_names.push_back(std::string("Steady state module '") + module_name);
	}
	if(_verbose) myfile << "\n...done building list of parameters!\n\n";
	
	// Check the derivative modules
	if(_verbose) myfile << "Checking the derivative module input and output parameters... ";
	for(std::string module_name : derivative_module_names) {
		if(unique_derivative_module_names.find(module_name) == unique_derivative_module_names.end()) {
			for(std::string p : module_factory.get_inputs(module_name)) {
				if(unique_parameter_names.find(p) == unique_parameter_names.end()) undefined_input_parameters.push_back(std::string("Parameter '") + p + std::string("' for the '") + module_name + std::string("' derivative module"));
				if(unique_module_inputs.find(p) == unique_module_inputs.end()) unique_module_inputs.insert(p);
			}
			for(std::string p : module_factory.get_outputs(module_name)) {
				if(initial_state.find(p) == initial_state.end()) {
					illegal_output_parameters.push_back(std::string("Parameter '") + p + std::string("' from the '") + module_name + std::string("' module"));
				}
				else if(unique_derivative_outputs.find(p) == unique_derivative_outputs.end()) unique_derivative_outputs.insert(p);
			}
		}
		else duplicate_module_names.push_back(std::string("Derivative module '") + module_name);
		
	}
	if(_verbose) myfile << "done!\n\n";
	
	// Collect information about any errors that may have occurred while checking the parameters
	if(duplicate_parameter_names.size() != 0) {
		error_string += "Some parameters in the initial state, invariant parameters, and/or varying parameters were duplicated.\n";
		if(_verbose) {
			myfile << "The following parameters in the initial state, invariant parameters, and/or varying parameters were duplicated:\n";
			for(std::string s : duplicate_parameter_names) myfile << s << "\n";
			myfile << "\n";
		}
	}
	if(duplicate_module_names.size() != 0) {
		error_string += "Some modules were duplicated in the steady state and/or derivative module lists.\n";
		if(_verbose) {
			myfile << "The following modules were duplicated:\n";
			for(std::string s : duplicate_module_names) myfile << s << "\n";
			myfile << "\n";
		}
	}
	if(duplicate_output_parameters.size() != 0) {
		error_string += "Some steady state module output parameters were already included in the initial state, invariant parameters, varying parameters, or previous steady state modules.\n";
		if(_verbose) {
			myfile << "The following steady state output parameters were already included in the initial state, invariant parameters, varying parameters, or previous steady state modules:\n";
			for(std::string s : duplicate_output_parameters) myfile << s << "\n";
			myfile << "\n";
		}
	}
	if(undefined_input_parameters.size() != 0) {
		error_string += "Some modules required inputs parameters that were not defined by the initial state, invariant parameters, varying parameters, or previous steady state modules.\n";
		if(_verbose) {
			myfile << "The following module input parameters were not defined by the initial state, invariant parameters, varying parameters, or previous steady state modules:\n";
			for(std::string s : undefined_input_parameters) myfile << s << "\n";
			myfile << "\n";
		}
	}
	if(illegal_output_parameters.size() != 0) {
		error_string += "Some derivative modules returned derivatives for parameters that were not included in the initial state.\n";
		if(_verbose) {
			myfile << "The following parameters were output by derivative modules but not included in the initial state:\n";
			for(std::string s : illegal_output_parameters) myfile << s << "\n";
			myfile << "\n";
		}
	}
	if(unique_derivative_outputs.size() != initial_state.size()) {
		if(_verbose) {
			myfile << "No derivatives were supplied for the following state variables:\n";
			for(auto x : initial_state) {
				if(unique_derivative_outputs.find(x.first) == unique_derivative_outputs.end()) myfile << x.first << "\n";
			}
			myfile << "These variables will not change with time. You may want to consider adding one or more derivative modules that describe them.\n\n";
		}
	}
	if(_verbose) {
		bool found_unused_invariant_parameter = false;
		for(auto x : invariant_parameters) {
			if(unique_module_inputs.find(x.first) == unique_module_inputs.end()) {
				if(found_unused_invariant_parameter == false) {
					myfile << "The following invariant parameters were not used as inputs to any module:\n" << x.first << "\n";
					found_unused_invariant_parameter = true;
				}
				else myfile << x.first << "\n";
			}
		}
		if(found_unused_invariant_parameter) myfile << "You may want to consider removing them from the input list.\n\n";
		else myfile << "All invariant parameters were used as module inputs.\n\n";
	}
	if(_verbose) {
		myfile << "The following parameters were used as inputs to at least one module:\n";
		for(std::string p : unique_module_inputs) myfile << p << "\n";
		myfile << "\n";
	}
	
	// If any errors occurred, notify the user
	if(error_string.size() > 0) {
		if(!_verbose) error_string += "Rerun the system in verbose mode for more information.\n";
		else error_string += "Check the system startup record for more information.\n";
		myfile.close();
		throw std::logic_error(error_string);
	}
	
	// Initialize the steady state and derivative module output map
	vector_module_output = parameters;
	
	// Get a pointer to the timestep
	timestep_ptr = &parameters.at("timestep");
	
	// Get pointers to the state variables in the parameter and module output maps
	for(auto x : initial_state) {
		std::pair<double*, double*> temp(&parameters.at(x.first), &vector_module_output.at(x.first));
		state_parameter_names.push_back(x.first);
		state_ptrs.push_back(temp);
	}
	
	// Get pointers to the steady state parameters in the parameter and module output maps
	for(std::string p : unique_steady_state_parameter_names) {
		std::pair<double*, double*> temp(&parameters.at(p), &vector_module_output.at(p));
		steady_state_ptrs.push_back(temp);
	}
	
	// Get pointers to the varying parameters in the parameter and varying parameter maps
	for(auto x : _varying_parameters) {
		std::pair<double*, std::vector<double>*> temp(&parameters.at(x.first), &_varying_parameters.at(x.first));
		varying_ptrs.push_back(temp);
	}
	
	// Create a vector of parameter names (useful for saving the output of a calculation)
	output_param_vector.resize(unique_changing_parameters.size());
	std::copy(unique_changing_parameters.begin(), unique_changing_parameters.end(), output_param_vector.begin());
	
	// Create the modules
	if(_verbose) myfile << "Creating the steady state modules from the list and making sure the list only includes steady state modules... ";
	for(std::string module_name : steady_state_module_names) {
		steady_state_modules.push_back(module_factory.create(module_name));
		if(steady_state_modules.back()->is_deriv()) incorrect_modules.push_back(std::string("'") + module_name + std::string("' was included in the list of steady state modules, but it returns a derivative"));
	}
	if(_verbose) myfile << "done!\n\n";
	if(_verbose) myfile << "Creating the derivative modules from the list and making sure the list only includes derivative modules... ";
	for(std::string module_name : derivative_module_names) {
		derivative_modules.push_back(module_factory.create(module_name));
		if(!derivative_modules.back()->is_deriv()) incorrect_modules.push_back(std::string("'") + module_name + std::string("' was included in the list of derivative modules, but it does not return a derivative"));
	}
	if(_verbose) myfile << "done!\n\n";
	
	// Collect information about any errors that may have occurred while creating the modules
	if(incorrect_modules.size() != 0) {
		error_string += "Some modules were mischaracterized in the input lists.\n";
		if(_verbose) {
			myfile << "The following modules were mischaracterized:\n";
			for(std::string s : incorrect_modules) myfile << s << "\n";
			myfile << "\n";
		}
	}
	
	// If any errors occurred, notify the user
	if(error_string.size() > 0) {
		if(!_verbose) error_string += "Rerun the system in verbose mode for more information.\n";
		else error_string += "Check the system startup record for more information.\n";
		myfile.close();
		throw std::logic_error(error_string);
	}
	
	// Get the number of time points
	ntimes = (_varying_parameters.at("doy_dbl")).size();
	
	// Fill in the initial values and test the modules
	if(_verbose) myfile << "Trying to run all the modules... ";
	try {test_steady_state_modules();}
	catch (const std::exception& e) {error_string += e.what();}
	std::vector<double> temp_vec(state_ptrs.size());
	test_derivative_modules(temp_vec);
	if(_verbose) myfile << "done!\n\n";
	
	// If any errors occurred, notify the user
	if(error_string.size() > 0) {
		if(!_verbose) error_string += "Rerun the system in verbose mode for more information.\n";
		else error_string += "Check the system startup record for more information.\n";
		myfile.close();
		throw std::logic_error(error_string);
	}
	
	// Otherwise, we are done!
	if(_verbose) {
		myfile << "Done applying checks and building the system!\n\n";
		myfile.close();
	}
}

bool System::get_state_indx(int& state_indx, const std::string& parameter_name) const {
	for(size_t i = 0; i < state_parameter_names.size(); i++) {
		if(state_parameter_names[i] == parameter_name) {
			state_indx = i;
			return true;
		}
	}
	return false;
}

bool System::get_param(double& value, const std::string& parameter_name) const {
	if(parameters.find(parameter_name) != parameters.end()) {
		value = parameters.at(parameter_name);
		return true;
	}
	return false;
}

void System::reset() {
	// Put all parameters back to their original values
	int t = 0;
	update_varying_params(t);
	for(auto x : _initial_state) parameters[x.first] = x.second;
	run_steady_state_modules();
}

void System::set_param(const double& value, const std::string& parameter_name) {
	if(parameters.find(parameter_name) != parameters.end()) parameters[parameter_name] = value;
	else throw std::logic_error(std::string("Thrown by System::set_param - could not find a parameter called ") + parameter_name);
}

void System::set_param(const std::vector<double>& values, const std::vector<std::string>& parameter_names) {
	if(values.size() != parameter_names.size()) throw std::logic_error("Thrown by System::set_param - input vector lengths are different\n");
	for(size_t i = 0; i < values.size(); i++) set_param(values[i], parameter_names[i]);
}

template<class vector_type> void System::get_state(vector_type& x) const {
	x.resize(state_ptrs.size());
	for(size_t i = 0; i < x.size(); i++) x[i] = *(state_ptrs[i].first);
}

template<class vector_type, class time_type> void System::print_results(const std::vector<vector_type>& x_vec, const std::vector<time_type>& times, const std::string& name) {
	// Get ready to store the index of doy_dbl
	size_t doy_dbl_indx;
	
	// Open a text file
	std::ofstream myfile;
  	myfile.open (name, std::ios::out | std::ios::trunc);
  	
  	// Write the parameter names
  	for(size_t i = 0; i < output_param_vector.size(); i++) {
		if (i) myfile << ",";
  		myfile << output_param_vector[i];
  		if(output_param_vector[i] == std::string("doy_dbl")) {
  			doy_dbl_indx = i;
  			myfile << ",doy,hour";
		}
	}
	myfile << "\n";
	
	// Write the data
	for(size_t i = 0; i < x_vec.size(); i++) {
		// Unpack the latest time and state from the calculation results
		vector_type current_state = x_vec[i];
		time_type current_time = times[i];
		// Get the corresponding parameter list
		update_varying_params(current_time);
		update_state_params(current_state);
		run_steady_state_modules();
		// Save it
		for(size_t j = 0; j < output_param_vector.size(); j++) {
			if(j) myfile << ",";
			myfile << parameters[output_param_vector[j]];
			if(j == doy_dbl_indx) {
				double doy_dbl = parameters[output_param_vector[j]];
				int doy = floor(doy_dbl);
				double hour = 24.0 * (doy_dbl - doy);
				myfile << "," << doy << "," << hour;
			}
		}
		if(i < x_vec.size() - 1) myfile << "\n";
	}
  	
  	// Close the file
	myfile.close();
}

template<class vector_type, class time_type> std::unordered_map<std::string, std::vector<double>> System::get_results(const std::vector<vector_type>& x_vec, const std::vector<time_type>& times) {
	// Make the result map
	std::unordered_map<std::string, std::vector<double>> results;
	
	// Initialize the parameter names
	std::vector<double> temp(x_vec.size());
	for(std::string p : output_param_vector) results[p] = temp;
	results["doy"] = temp;
	results["hour"] = temp;
	
	// Store the data
	for(size_t i = 0; i < x_vec.size(); i++) {
		// Unpack the latest time and state from the calculation results
		vector_type current_state = x_vec[i];
		time_type current_time = times[i];
		// Get the corresponding parameter list
		update_varying_params(current_time);
		update_state_params(current_state);
		run_steady_state_modules();
		// Add the list to the results map
		for(size_t j = 0; j < output_param_vector.size(); j++) {
			(results[output_param_vector[j]])[i] = parameters[output_param_vector[j]];
			if(output_param_vector[j] == std::string("doy_dbl")) {
				double doy_dbl = parameters[output_param_vector[j]];
				int doy = floor(doy_dbl);
				double hour = 24.0 * (doy_dbl - doy);
				results["doy"][i] = doy;
				results["hour"][i] = hour;
			}
		}
	}
	
	// Return the result map
	return results;
}

// For integer time
void System::update_varying_params(int time_indx) {
	for(auto x : varying_ptrs) *(x.first) = (*(x.second))[time_indx];
}

// For double time
void System::update_varying_params(double time_indx) {
	// Find the two closest integers
	int t1 = (int)(time_indx + 0.5);
	int t2 = (t1 > time_indx) ? (t1 - 1) : (t1 + 1);
	// Make a linear interpolation
	for(auto x : varying_ptrs) *(x.first) = (*(x.second))[t1] + (time_indx - t1) * ((*(x.second))[t2] - (*(x.second))[t1]) / (t2 - t1);
}

template<class vector_type> void System::update_state_params(const vector_type& new_state) {
	for(size_t i = 0; i < new_state.size(); i++) *(state_ptrs[i].first) = new_state[i];
}

void System::run_steady_state_modules() {
	for(auto x : steady_state_ptrs) *x.second = 0.0;														// Clear the module output map
	for(auto it = steady_state_modules.begin(); it != steady_state_modules.end(); ++it) {
		(*it)->run();																						// Run the module
		for(auto x : steady_state_ptrs) *x.first = *x.second;												// Store its output in the main parameter map
	}
}

template<class vector_type> void System::run_derivative_modules(vector_type& dxdt) {
	for(auto x : state_ptrs) *x.second = 0.0;																// Reset the module output map
	std::fill(dxdt.begin(), dxdt.end(), 0);																	// Reset the derivative vector
	for(auto it = derivative_modules.begin(); it != derivative_modules.end(); ++it) (*it)->run();			// Run the modules
	for(size_t i = 0; i < dxdt.size(); i++) dxdt[i] += *(state_ptrs[i].second)*(*timestep_ptr);				// Store the output in the derivative vector
}

void System::test_steady_state_modules() {
	// Identical to run_steady_state_modules except for a try-catch block
	for(auto x : steady_state_ptrs) *x.second = 0.0;
	for(auto it = steady_state_modules.begin(); it != steady_state_modules.end(); ++it) {
		try {(*it)->run();}
		catch (const std::exception& e) {
			for(auto x : parameters) std::cout << x.first << ": " << x.second << "\n";
			throw std::logic_error(std::string("Steady state module '") + (*it)->get_name() + std::string("' generated an exception while calculating steady state parameters: ") + e.what() + std::string("\n"));
		}
		for(auto x : steady_state_ptrs) *x.first = *x.second;
	}
}

template<class vector_type> void System::test_derivative_modules(vector_type& dxdt) {
	// Identical to run_derivative_modules except for a try-catch block
	for(auto x : state_ptrs) *x.second = 0.0;
	std::fill(dxdt.begin(), dxdt.end(), 0);
	for(auto it = derivative_modules.begin(); it != derivative_modules.end(); ++it) {
		try {(*it)->run();}
		catch (const std::exception& e) {
			throw std::logic_error(std::string("Derivative module '") + (*it)->get_name() + std::string("' generated an exception while calculating time derivatives: ") + e.what() + std::string("\n"));
		}
	}
	for(size_t i = 0; i < dxdt.size(); i++) dxdt[i] += *(state_ptrs[i].second)*(*timestep_ptr);
}

template<class vector_type, class time_type> int System::speed_test(int n, const vector_type& x, vector_type& dxdt, const time_type& t) {
	// Run the system operator n times
	clock_t ct = clock();
	for(int i = 0; i < n; i++) operator()(x, dxdt, t);
	ct = clock() - ct;
	return (int)ct;
}

template<class time_type> int System::speed_test(int n, const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::matrix<double>& jacobi, const time_type& t, boost::numeric::ublas::vector<double>& dfdt) {
	// Run the system operator n times
	clock_t ct = clock();
	for(int i = 0; i < n; i++) operator()(x, jacobi, t, dfdt);
	ct = clock() - ct;
	return (int)ct;
}

#endif
