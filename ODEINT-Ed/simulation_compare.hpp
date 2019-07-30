#ifndef SIMULATION_COMPARE_H
#define SIMULATION_COMPARE_H

#include "system.hpp"
#include <tuple>

class SimulationCompare {
	// This class facilitates the comparison of a simulation result with reference data
	// It stores a system and all its input parameters, so the system doesn't need to be
	//  re-initialized each time a simulation needs to be run
	// It operates in two modes, which are implemented via two subclasses:
	//  ModelCompare compares a few parameters at each time point in the simulation
	//   this is intended to be used to compare different models, e.g. utilization
	//   vs. partitioning growth
	//  ReferenceCompare compares a few parameters at a few DOY or TTc points
	//   this is intended to be used to fit a model to experimental reference data
	public:
		SimulationCompare(
			std::shared_ptr<System> sys_ptr,
			std::vector<std::string> fitting_param_names,
			std::string timestamp,
			double error_tol);
		bool run(const std::vector<double>& x, std::vector<state_type>& state_vec, std::vector<time_type>& time_vec);
		double compare(const std::vector<double>& x);
		void save(const std::vector<double>& x, std::string filename);
		size_t get_ncalls() {return ncalls;}
		int get_ntimes() {return ntimes;}
		double get_min_error() {return min_error;}
	private:
		std::string _name;
		const std::shared_ptr<System> _sys_ptr;											// A pointer to a system
		const std::vector<std::string> _fitting_param_names;							// A list of parameter names that will be modified during the fitting process
		std::string _timestamp;															// A string specifying when the process started, used to ensure unique filenames
		const double _error_tol;														// Error tolerance for odeint stepper
		SystemCaller syscall;															// A system caller to use with odeint
		double step_size = 1.0;
		size_t ncalls = 0;
		double min_error = HUGE_VAL;
		size_t ntimes;
		virtual double get_error(std::unordered_map<std::string, std::vector<double>>& results) = 0;	// This pure virtual function ensures that SimulationCompare is an abstract class
};

SimulationCompare::SimulationCompare(
	std::shared_ptr<System> sys_ptr,
	std::vector<std::string> fitting_param_names,
	std::string timestamp,
	double error_tol) : 
	_sys_ptr(sys_ptr),
	_fitting_param_names(fitting_param_names),
	_timestamp(timestamp),
	_error_tol(error_tol),
	syscall(SystemCaller(_sys_ptr)),
	ntimes(_sys_ptr->get_ntimes())
{
	// Check to make sure the fitting parameter names are included in the system's parameter list
	double temp;
	for(size_t i = 0; i < _fitting_param_names.size(); i++) {
		if(!_sys_ptr->get_param(temp, _fitting_param_names[i])) throw std::logic_error(std::string("Thrown in SimulationCompare::SimulationCompare -- fitting parameter ") + _fitting_param_names[i] + std::string(" is not present in the system's parameter list\n"));
	}
	
	// Initialize the optimization path log
	std::ofstream myfile;
	std::string filename = _timestamp + std::string("-optimization-path.csv");
	myfile.open (filename, std::ios::out | std::ios::app);
	myfile << "stepNum,error";
	for(size_t i = 0; i < _fitting_param_names.size(); i++) myfile << "," << _fitting_param_names[i];
	myfile << "\n";
	myfile.close();
}

bool SimulationCompare::run(const std::vector<double>& x, std::vector<state_type>& state_vec, std::vector<time_type>& time_vec) {
	// Make sure the input vector has the correct length
	if(x.size() != _fitting_param_names.size()) throw std::logic_error("Thrown in SimulationCompare::run -- input vector x has the wrong length\n");
	
	// Reset all of the system's parameters to their original values
	_sys_ptr->reset();
	
	// Update some of the system's parameters with the new values
	_sys_ptr->set_param(x, _fitting_param_names);
	
	// Get the current state in the correct format
	state_type state;
	_sys_ptr->get_state(state);
	
	// Set the error tolerances
	double abs_err = _error_tol, rel_err = _error_tol;
	
	// Initialize the success indicator
	bool success = true;
	
	// Make the stepper and integrate
	typedef boost::numeric::odeint::rosenbrock4<double> dense_stepper_type;
	try {
		boost::numeric::odeint::integrate_const(
			boost::numeric::odeint::make_dense_output<dense_stepper_type>(abs_err, rel_err),
			std::make_pair(syscall, syscall),
			state,
			0.0,
			(double)ntimes - 1.0,
			step_size,
			push_back_state_and_time_verbose(state_vec, time_vec, (double)ntimes - 1.0)
		);
	}
	catch (std::exception &e) {
		// Something went wrong with the simulation
		success = false;
	}
	
	// Indicate whether the run was successful
	return success;
}

double SimulationCompare::compare(const std::vector<double>& x) {
	// Make vectors to store the observer output
	std::vector<state_type> state_vec;
	std::vector<time_type> time_vec;
	
	// Run the simulation
	bool success = run(x, state_vec, time_vec);
	
	// If the simulation was successful, compare its output to the reference data
	//  Otherwise, arbitrarily set the error to 1e10
	double error;
	if(success) {
		std::unordered_map<std::string, std::vector<double>> results = _sys_ptr->get_results(state_vec, time_vec);
		error = get_error(results);
	}
	else {
		error = 1.0e10;
		std::cout << "\nODEINT simulation failed! Arbitrarily setting the error to 1e10\n";
	}
	
	// Increment the counter
	ncalls++;
	
	// Update the minimum error value
	if(error < min_error) min_error = error;
	
	// Save the result to a file
	std::ofstream myfile;
	std::string filename = _timestamp + std::string("-optimization-path.csv");
	myfile.open (filename, std::ios::out | std::ios::app);
	myfile << ncalls << "," << error;
	for(size_t i = 0; i < x.size(); i++) myfile << "," << x[i];
	myfile << "\n";
	myfile.close();
	
	// Return the error
	return error;
}

void SimulationCompare::save(const std::vector<double>& x, std::string filename) {
	// Make vectors to store the observer output
	std::vector<state_type> state_vec;
	std::vector<time_type> time_vec;
	
	// Run the simulation
	bool success = run(x, state_vec, time_vec);
	
	// Save the simulation result
	if(success) _sys_ptr->print_results(state_vec, time_vec, filename);
	else std::cout << "\nODEINT simulation failed! Not saving any results.\n";
}

class ModelCompare : public SimulationCompare {
	public:
		ModelCompare(
			std::shared_ptr<System> sys_ptr,
			std::vector<std::string> fitting_param_names,
			std::string timestamp,
			double error_tol,
			const std::unordered_map<std::string, std::vector<double>>& compare_to,
			std::vector<std::string> comp_param_names);
	private:
		const std::unordered_map<std::string, std::vector<double>>* const _compare_to;	// The results of a previous simulation
		const std::vector<std::string> _comp_param_names;								// A list of parameter names that will be compared between the simulation output and the comparison data
		double get_error(std::unordered_map<std::string, std::vector<double>>& results);
};

ModelCompare::ModelCompare(
	std::shared_ptr<System> sys_ptr,
	std::vector<std::string> fitting_param_names,
	std::string timestamp,
	double error_tol,
	const std::unordered_map<std::string, std::vector<double>>& compare_to,
	std::vector<std::string> comp_param_names) :
	SimulationCompare(sys_ptr, fitting_param_names, timestamp, error_tol),
	_compare_to(&compare_to),
	_comp_param_names(comp_param_names)
{
	// Check to make sure the system output will have the same length as the comparison data
	size_t ntimes = sys_ptr->get_ntimes();
	if(ntimes != (_compare_to->at("doy")).size()) throw std::logic_error("Thrown in ModelCompare::ModelCompare -- the system simulation output has a different length than the supplied comparison data\n");
	
	// Check to make sure the comparison parameter names are included in the comparison data and the system output
	double temp;
	for(size_t i = 0; i < _comp_param_names.size(); i++) {
		if(_compare_to->find(_comp_param_names[i]) == _compare_to->end()) throw std::logic_error(std::string("Thrown in ModelCompare::ModelCompare -- comparison parameter ") + _comp_param_names[i] + std::string(" is not present in the comparison data\n"));
		if(!sys_ptr->get_param(temp, _comp_param_names[i])) throw std::logic_error(std::string("Thrown in ModelCompare::ModelCompare -- comparison parameter ") + _comp_param_names[i] + std::string(" is not present in the system output\n"));
	}
}

double ModelCompare::get_error(std::unordered_map<std::string, std::vector<double>>& results) {
	// Compare the simulation result to the model reference data
	double error = 0.0;
	std::vector<double> comp_data;
	std::vector<double> sim_data;
	for (std::string param : _comp_param_names) {
		// Get the time series for the parameter from the simulation result and the comparison data
		sim_data = results[param];
		comp_data = _compare_to->at(param);
		// Calculate the error
		for(size_t j = 0; j < sim_data.size(); j++) error += (sim_data[j] - comp_data[j]) * (sim_data[j] - comp_data[j]);	// x * x is faster than std::pow(x, 2). See https://baptiste-wicht.com/posts/2017/09/cpp11-performance-tip-when-to-use-std-pow.html
	}
	return error;
}

class ReferenceCompare : public SimulationCompare {
	public:
		ReferenceCompare(
			std::shared_ptr<System> sys_ptr,
			std::vector<std::string> fitting_param_names,
			std::string timestamp,
			double error_tol,
			std::vector<std::tuple<std::string, double, std::string, double>>& compare_to);
	private:
		const std::vector<std::tuple<std::string, double, std::string, double>>* const _compare_to;
		double get_error(std::unordered_map<std::string, std::vector<double>>& results);
};

ReferenceCompare::ReferenceCompare(
	std::shared_ptr<System> sys_ptr,
	std::vector<std::string> fitting_param_names,
	std::string timestamp,
	double error_tol,
	std::vector<std::tuple<std::string, double, std::string, double>>& compare_to) :
	SimulationCompare(sys_ptr, fitting_param_names, timestamp, error_tol),
	_compare_to(&compare_to)
{
	// Check to make sure that all the parameter names in the comparison data will be included in the system output
	double temp;
	std::string param_name;
	for(size_t i; i < _compare_to->size(); i++) {
		std::tuple<std::string, double, std::string, double> current_tuple = _compare_to->at(i);	// Get the current tuple
		param_name = std::get<0>(current_tuple);													// Get the time parameter name
		// Check to see if it exists in the system output
		if(!sys_ptr->get_param(temp, param_name)) throw std::logic_error(std::string("Thrown in ReferenceCompare::ReferenceCompare -- comparison parameter ") + param_name + std::string(" is not present in the system output\n"));
		param_name = std::get<2>(current_tuple);													// Get the value parameter name
		if(!sys_ptr->get_param(temp, param_name)) throw std::logic_error(std::string("Thrown in ReferenceCompare::ReferenceCompare -- comparison parameter ") + param_name + std::string(" is not present in the system output\n"));
	}
}

double ReferenceCompare::get_error(std::unordered_map<std::string, std::vector<double>>& results) {
	// Compare the simulation results to the experimental reference data
	
	// Create variables to store things
	double error = 0.0;
	std::vector<double> time_vector;
	std::vector<double> value_vector;
	
	// Compute the error by checking each reference value
	for(size_t i = 0; i < _compare_to->size(); i++) {
		// Get the current tuple
		std::tuple<std::string, double, std::string, double> current_tuple = _compare_to->at(i);
		
		// Get the comparison time and value
		double comparison_time;
		double comparison_value;
		comparison_time = std::get<1>(current_tuple);
		comparison_value = std::get<3>(current_tuple);
		
		// Get the corresponding time and value vectors from the simulation output
		time_vector = results[std::get<0>(current_tuple)];
		value_vector = results[std::get<2>(current_tuple)];
		
		// Find the indices of the two time values in the vector that flank the comparison time
		int t1 = -1;	// Index of the smallest time value that's larger than or equal to the reference time
		int t2 = -1;	// Index of the largest time value that's smaller than or equal to the reference time
		double d1 = HUGE_VAL;
		double d2 = -HUGE_VAL;
		for(size_t j = 0; j < time_vector.size(); j++) {
			double diff = time_vector[j] - comparison_time;
			if(diff >= 0 && diff < d1) {
				// diff >= 0 means that time_vector[j] is smaller than or equal to the comparison time
				// diff < d1 means that time_vector[j] is closer to the comparison time than the previous best
				t1 = j;
				d1 = diff;
			}
			if(diff <= 0 && diff > d2) {
				// diff <= 0 means that time_vector[j] is larger than or equal to the comparison time
				// diff > d2 means that time_vector[j] is closer to the comparison time than the previous best
				t2 = j;
				d2 = diff;
			}
		}
		
		// Check to make sure the search was successful
		if(t1 < 0 || t2 < 0) throw std::logic_error(std::string("Thrown in ReferenceCompare::ReferenceCompare -- could not interpolate value parameter ") + std::get<2>(current_tuple) + std::string(" in the simulation output for time parameter ") + std::get<0>(current_tuple) + std::string(" = ") + std::to_string(comparison_time));
		
		// Make a linear interpolation to find the simulated value, if necessary
		double interpolated_value;
		if(t1 == t2) interpolated_value = value_vector[t1];	// In this case, the comparison time value is included in the simulation output and we don't need to interpolate
		else interpolated_value = value_vector[t1] + (comparison_time - time_vector[t1]) * (value_vector[t2] - value_vector[t1]) / (time_vector[t2] - time_vector[t1]);
		
		// Update the error
		//error += pow(comparison_value - interpolated_value, 2);
		error += pow(comparison_value - interpolated_value, 2) / comparison_value;	// Should give more attention to smaller values
	}
	
	// Return the total error
	return error;
}

#endif
