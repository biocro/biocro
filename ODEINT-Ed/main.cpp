#include <map>
#include <iostream>
#include <fstream>
#include <cmath>
#include <boost/numeric/odeint.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include "system.hpp"
#include "simulation_compare.hpp"
#include <nlopt.hpp>

std::unordered_map<std::string, double> read_parameter_list(const std::string& filename) {
	// Make the output map
	std::unordered_map<std::string, double> parameters;
	
	// Open the file
	std::ifstream myfile;
	myfile.open (filename);
	
	// Make a string to store the file entries
	std::string entry;
	
	// Go through the file
	while(std::getline(myfile, entry, ',')) {
		std::string parameter_name = entry;				// The first entry is the parameter name
		std::getline(myfile, entry);					// The second entry is the value and should be the end of the line, so we don't need to specify a delimiter
		parameters[parameter_name] = std::stod(entry);	// Store the info from the line
	}
	
	// Close the file and return the parameters
	myfile.close();
	return parameters;
}

std::unordered_map<std::string, std::vector<double>> read_varying_parameters(const std::string& filename) {
	// Make the output map
	std::unordered_map<std::string, std::vector<double>> varying_parameters;
	
	// Open the file
	std::ifstream myfile;
	myfile.open (filename);
	
	// Make strings to store the file entries
	std::string line;
	std::string entry;
	
	// Go through the file
	while(std::getline(myfile, line)) {
		std::vector<double> parameter_vector;					// Make a vector for this line
		std::istringstream isline(line);						// Make an istream from the line
		std::getline(isline, entry, ',');						// Get the first comma-separated entry
		std::string parameter_name = entry;						// The first entry is the parameter name
		
		while(std::getline(isline, entry, ',')) {
			// The next comma-separated entries in the line are values of that parameter
			try {
				parameter_vector.push_back(std::stod(entry));
			}
			catch (std::exception &e) {
				std::cout << "Encountered error when entry = '" << entry << "' in line = '" << line << "'\n";
				throw std::logic_error(std::string(""));
			}
		}
		varying_parameters[parameter_name] = parameter_vector;	// Store the info from the line
	}
	
	// Close the file and return the parameters
	myfile.close();
	return varying_parameters;
}

std::vector<std::tuple<std::string, double, std::string, double>> read_reference_data(const std::string& filename) {
	// Make the output map
	std::vector<std::tuple<std::string, double, std::string, double>> reference_data;
	
	// Open the file
	std::ifstream myfile;
	myfile.open (filename);
	
	// Make strings to store the file entries
	std::string line;
	std::string entry;
	
	// Read the first line, which contains a header that we can ignore
	std::getline(myfile, line);
	
	// Get the reference data by going through the file
	std::string time_type;
	double time_value;
	std::string param_type;
	double param_value;
	while(std::getline(myfile, line)) {
		std::istringstream isline(line);		// Make an istream from the line
		std::getline(isline, time_type, ',');	// Get the first entry, which is the time type
		std::getline(isline, entry, ',');		// Get the second entry, which is the time value
		time_value = std::stod(entry);
		std::getline(isline, param_type, ',');	// Get the third entry, which is the param type
		std::getline(isline, entry, ',');		// Get the fourth entry, which is the param value
		param_value = std::stod(entry);
		// Save the information as a tuple in the reference data vector
		reference_data.push_back(std::make_tuple(time_type, time_value, param_type, param_value));
	}
	
	// Close the file and return the data
	myfile.close();
	return reference_data;
}

std::unordered_map<std::string, std::vector<double>> read_simulation_record(const std::string& filename) {
	// Make the output map
	std::unordered_map<std::string, std::vector<double>> simulation_record;
	
	// Open the file
	std::ifstream myfile;
	myfile.open (filename);
	
	// Make strings to store the file entries
	std::string line;
	std::string entry;
	
	// Read the first line, which contains all the column names
	std::getline(myfile, line);
	
	// Make an istream from the line
	std::istringstream isline(line);
	
	// Get the parameter names, storing them in a vector and making entries in the output map
	std::vector<std::string> parameter_names;
	while(std::getline(isline, entry, ',')) {
		entry = entry.substr(1, entry.size()-2);	// The parameter name is initially wrapped in quotes, so remove them
		parameter_names.push_back(entry);
		std::vector<double> temp;
		simulation_record[entry] = temp;
	}
	
	// Go through the rest of the file, storing the data values in the output map
	while(std::getline(myfile, line)) {
		std::istringstream isline(line);											// Make an istream from the line
		for(size_t i = 0; i < parameter_names.size(); i++) {
			std::getline(isline, entry, ',');										// Get a comma-separated entry from the line
			(simulation_record.at(parameter_names[i])).push_back(std::stod(entry));	// Store the value in the map
		}
	}
	
	// Close the file and return the parameters
	myfile.close();
	return simulation_record;
}

double f(const std::vector<double> &x, std::vector<double> &grad, void* f_data) {
	
	// A function to be passed into the NLOPT optimization routine
	// Must be defined as:
	//  double f(const std::vector<double> &x, std::vector<double> &grad, void* f_data)
	// Here we are not considering methods that use a derivative
	// f_data should be a shared pointer to a system comparison object
	
	if(grad.size() > 0) throw std::logic_error(std::string("No derivatives allowed! Try a different optimization algorithm\n"));
	
	// Unpack the pointer
	std::shared_ptr<SimulationCompare> sim_comp = *(static_cast<std::shared_ptr<SimulationCompare>*>(f_data));
	
	// Run the simulation and calculate the error
	clock_t t = clock();
	double error = sim_comp->compare(x);
	t = clock() - t;
	
	// Give the user an update
	char buff[128];
	sprintf(buff, "\nFor step %8llu:\terror = %10e.\tCompare to min_error = %10e.\tRequired %8i clock cycles\n", sim_comp->get_ncalls(), error, sim_comp->get_min_error(), (int)t);
	std::cout << buff;
	
	// Return the error
	return error;
}

bool find_tracker_phase_offsets(
				int start_step,
				double& phase_1_dawn, double& time_1_dawn,
				double& phase_1_dusk, double& time_1_dusk,
				double& ref_dawn150_phase, double& ref_dusk150_phase,
				double& dawn150_phase, double& dusk150_phase) {
		// Initialize the system inputs
		std::unordered_map<std::string, double> initial_state;
		std::unordered_map<std::string, double> invariant_parameters;
		std::unordered_map<std::string, std::vector<double>> varying_parameters;
		std::vector<std::string> steady_state_modules;
		std::vector<std::string> derivative_modules;
		
		// Set the initial state
		initial_state = {
			{"night_tracker",	1.0},
			{"day_tracker",		0.0},
			{"dawn_x",			0.0},
			{"dawn_v",			1.0},
			{"dusk_x",			0.0},
			{"dusk_v",			1.0},
			{"ref_x",			0.0},
			{"ref_v",			1.0},
			{"allday",			0.0}
		};
		
		// Make a steady state module list
	  	steady_state_modules = {
	  		"fake_solar",
	  		"circadian_clock_calculator"
		};
	  	
	  	// Make a derivative module list
	  	derivative_modules = {
	  		"circadian_clock"
		};
		
		// Set the invariant parameters
		double target_doy_dbl = 141.0 + start_step * 12.0 / (24.0 * 60.0);	// Use 12 min step sizes for setting midday
		invariant_parameters = {
			{"timestep",		1.0},
			{"target_doy_dbl",	target_doy_dbl}
		};
		
		std::cout << "For start_step = " << start_step << ", target_doy_dbl = " << target_doy_dbl << "\n";
		
		// Get the varying parameters
		varying_parameters = read_varying_parameters("weather01-blank.csv");
			
		// Make a system and store a smart pointer to it
		std::shared_ptr<System> sys(new System(initial_state, invariant_parameters, varying_parameters, steady_state_modules, derivative_modules, true));
		
		// Make a system caller
		SystemCaller syscall(sys);
		
		// Get the current state in the correct format
		state_type state;
		sys->get_state(state);
		
		// Get the number of time points
		int ntimes = sys->get_ntimes();
		
		// Make vectors to store the observer output
		std::vector<state_type> state_vec;
		std::vector<time_type> time_vec;
		
		// Set the error tolerance to use for odeint simulations
		double error_tol = 1.0e-4;
		
		// Set the step size to use for odeint simulations
		double step_size = 0.1;
		
		// Run the simulation
		double abs_err = error_tol, rel_err = error_tol;
		typedef boost::numeric::odeint::rosenbrock4<double> dense_stepper_type;
		boost::numeric::odeint::integrate_const(
			boost::numeric::odeint::make_dense_output<dense_stepper_type>(abs_err, rel_err),
			std::make_pair(syscall, syscall),
			state,
			0.0,
			(double)ntimes - 1.0,
			step_size,
			push_back_state_and_time_verbose(state_vec, time_vec, (double)ntimes - 1.0)
		);
		
		// Get the results
		std::unordered_map<std::string, std::vector<double>> results = sys->get_results(state_vec, time_vec);
		
		// Reset the outputs
		phase_1_dawn = 0;
		time_1_dawn = 0;
		phase_1_dusk = 0;
		time_1_dusk = 0;
		ref_dawn150_phase = 0;
		dawn150_phase = 0;
		ref_dusk150_phase = 0;
		dusk150_phase = 0;
		
		// Iterate through the results to find the phase and time values when light exceeds 0.5, and then when it falls below 0.5
		for(size_t i = 0; i < results["light"].size(); i++) {
			// Get the current light value
			double curr_light = results["light"][i];
			// Check its value
			if(curr_light > 0.5) {
				std::cout << "\nLight becomes nonzero at i = " << i << "\n";
				phase_1_dawn = results["ref_phase"][i];
				time_1_dawn = results["doy_dbl"][i];
				std::cout << "Reference phase is currently " << phase_1_dawn << "\n";
				std::cout << "DOY is currently " << time_1_dawn << "\n";
				for(size_t j = i; j < results["light"].size(); j++) {
					// Get the current light value
					curr_light = results["light"][j];
					// Check its value
					if(curr_light < 0.5) {
						std::cout << "\nLight becomes zero at i = " << j << "\n";
						phase_1_dusk = results["ref_phase"][j];
						time_1_dusk = results["doy_dbl"][j];
						std::cout << "Reference phase is currently " << phase_1_dusk << "\n";
						std::cout << "DOY is currently " << time_1_dusk << "\n";
						break;
					}
				}
				break;
			}
		}
		if(time_1_dawn == 0) return false;
		
		// Iterate through the results to find the reference and altered phases after 150 more cycles
		for(size_t i = 0; i < results["doy_dbl"].size(); i++) {
			// Get the current doy_dbl value
			double curr_doy_dbl = results["doy_dbl"][i];
			// Check its value
			if(curr_doy_dbl >= time_1_dawn + 150.0 * 25.0 / 24.0) {
				std::cout << "150 periods later, i = " << i << "\n";
				ref_dawn150_phase = results["ref_phase"][i];
				dawn150_phase = results["dawn_phase"][i];
				std::cout << "DOY is currently " << curr_doy_dbl << "\n";
				std::cout << "Reference phase is currently " << ref_dawn150_phase << "\n";
				std::cout << "Dawn phase is currently " << dawn150_phase << "\n";
				for(size_t j = i; j < results["light"].size(); j++) {
					// Get the current doy_dbl value
					double curr_doy_dbl = results["doy_dbl"][j];
					// Check its value
					if(curr_doy_dbl >= time_1_dusk + 150.0 * 25.0 / 24.0) {
						std::cout << "150 periods later, i = " << j << "\n";
						ref_dusk150_phase = results["ref_phase"][i];
						dusk150_phase = results["dusk_phase"][i];
						std::cout << "DOY is currently " << curr_doy_dbl << "\n";
						std::cout << "Reference phase is currently " << ref_dusk150_phase << "\n";
						std::cout << "Dusk phase is currently " << dusk150_phase << "\n\n";
						break;
					}
				}
				break;
			}
		}
		
		// Finish
		return true;
}

bool find_clock_phase_offset(
				int start_step, double A,
				double& phase_1, double& time_1,
				double& ref1_phase, double& new1_phase,
				double& ref2_phase, double& new2_phase,
				double& ref150_phase, double& new150_phase) {
		// Define the initial oscillator state
		double E_0 = 0.1;
		double theta_0 = 0.0;
		
		// Define the oscillator restoring strength
		double gamma = 1.0;
		
		// Make the timestep equal to 0.01 of the oscillator's intrinsic period of 2 * pi
		double timestep = 2.0 * 3.14159265358979323846 / 100.0;
		
		// Define the driving force parameters (run the force for one timestep, starting at an integer timestep, during the 200th period)
		double T = 6.0;
		double F_start = start_step * timestep;
		double F_end = F_start + timestep;
		double max_F_per = 200;
		
		std::cout << "For start_step = " << start_step << ", F_start = " << F_start << " and F_end = " << F_end << "\n";
		
		// Initialize the system inputs
		std::unordered_map<std::string, double> initial_state;
		std::unordered_map<std::string, double> invariant_parameters;
		std::vector<std::string> steady_state_modules;
		std::vector<std::string> derivative_modules;
		
		// Set the initial state
		initial_state = {
			{"x",			sqrt(E_0) * cos(theta_0)},
			{"v",			sqrt(E_0) * sin(theta_0)},
			{"E",			E_0},
			{"theta",		theta_0},
			{"max_F_per",	max_F_per}
		};
		
		// Make a steady state module list
	  	steady_state_modules = {
	  		"clock_testing_calc"
		};
	  	
	  	// Make a derivative module list
	  	derivative_modules = {
	  		"clock_testing"
		};
		
		// Use a smaller timestep for running the simulation
		timestep /= 2.0;
		
		// Set the invariant parameters
		invariant_parameters = {
			{"timestep",	timestep},
			{"gamma",		gamma},
			{"T",			T},
			{"A",			A},
			{"F_start",		F_start},
			{"F_end",		F_end}
		};
		
		// Define the varying parameters, i.e., make a list of times
		int nperiods = 400;
		int ntimes = 2 * 100 * nperiods + 1;
		std::vector<double> time_vector;
		for (int i = 0; i < ntimes; i++) time_vector.push_back(i*timestep);
		std::unordered_map<std::string, std::vector<double>> varying_parameters {
			{"time", time_vector},
			{"doy", time_vector},
			{"hour", time_vector}
		};
		
		// Make a system and store a smart pointer to it
		std::shared_ptr<System> sys(new System(initial_state, invariant_parameters, varying_parameters, steady_state_modules, derivative_modules, true));
		
		// Make a system caller
		SystemCaller syscall(sys);
		
		// Get the current state in the correct format
		state_type state;
		sys->get_state(state);
		
		// Make vectors to store the observer output
		std::vector<state_type> state_vec;
		std::vector<time_type> time_vec;
		
		// Set the error tolerance to use for odeint simulations
		double error_tol = 1.0e-6;
		
		// Set the step size to use for odeint simulations
		double step_size = 1.0;
		
		// Run the simulation
		double abs_err = error_tol, rel_err = error_tol;
		typedef boost::numeric::odeint::rosenbrock4<double> dense_stepper_type;
		boost::numeric::odeint::integrate_const(
			boost::numeric::odeint::make_dense_output<dense_stepper_type>(abs_err, rel_err),
			std::make_pair(syscall, syscall),
			state,
			0.0,
			(double)ntimes - 1.0,
			step_size,
			push_back_state_and_time_verbose(state_vec, time_vec, (double)ntimes - 1.0)
		);
		
		// Get the results
		std::unordered_map<std::string, std::vector<double>> results = sys->get_results(state_vec, time_vec);
		
		// Reset the outputs
		phase_1 = 0;
		time_1 = 0;
		ref1_phase = 0;
		new1_phase = 0;
		ref2_phase = 0;
		new2_phase = 0;
		ref150_phase = 0;
		new150_phase = 0;
		
		// Iterate through the results to find the phase and time values when the force is applied
		for(size_t i = 0; i < results["F"].size(); i++) {
			// Get the current force value
			double curr_force = results["F"][i];
			// Check its value
			if(curr_force > 0) {
				std::cout << "\nForce becomes nonzero at i = " << i << "\n";
				phase_1 = results["theta"][i];
				time_1 = results["time"][i];
				std::cout << "Theta is currently " << phase_1 << " = " << fmod(phase_1, 2 * 3.14159265358979323846) + 2 * 3.14159265358979323846 << "\n";
				std::cout << "Time is currently " << time_1 << "\n";
				break;
			}
		}
		if(time_1 == 0) return false;
		
		// Iterate through the results to find the reference and altered phases after 1 more cycle
		for(size_t i = 0; i < results["time"].size(); i++) {
			// Get the current time value
			double curr_time = results["time"][i];
			// Check its value
			if(curr_time >= time_1 + 2 * 3.14159265358979323846) {
				std::cout << "1 period later, i = " << i << "\n";
				ref1_phase = results["theta"][i];
				new1_phase = results["phase"][i];
				std::cout << "Time is currently " << curr_time << "\n";
				std::cout << "Theta is currently " << ref1_phase << " = " << fmod(ref1_phase, 2 * 3.14159265358979323846) + 2 * 3.14159265358979323846 << "\n";
				std::cout << "Altered phase is currently " << new1_phase << "\n\n";
				break;
			}
		}
		
		// Iterate through the results to find the reference and altered phases after 2 more cycles
		for(size_t i = 0; i < results["time"].size(); i++) {
			// Get the current time value
			double curr_time = results["time"][i];
			// Check its value
			if(curr_time >= time_1 + 2.0 * 2 * 3.14159265358979323846) {
				std::cout << "2 periods later, i = " << i << "\n";
				ref2_phase = results["theta"][i];
				new2_phase = results["phase"][i];
				std::cout << "Time is currently " << curr_time << "\n";
				std::cout << "Theta is currently " << ref2_phase << " = " << fmod(ref2_phase, 2 * 3.14159265358979323846) + 2 * 3.14159265358979323846 << "\n";
				std::cout << "Altered phase is currently " << new2_phase << "\n\n";
				break;
			}
		}
		
		// Iterate through the results to find the reference and altered phases after 150 more cycles
		for(size_t i = 0; i < results["time"].size(); i++) {
			// Get the current time value
			double curr_time = results["time"][i];
			// Check its value
			if(curr_time >= time_1 + 150.0 * 2 * 3.14159265358979323846) {
				std::cout << "150 periods later, i = " << i << "\n";
				ref150_phase = results["theta"][i];
				new150_phase = results["phase"][i];
				std::cout << "Time is currently " << curr_time << "\n";
				std::cout << "Theta is currently " << ref150_phase << " = " << fmod(ref150_phase, 2 * 3.14159265358979323846) + 2 * 3.14159265358979323846 << "\n";
				std::cout << "Altered phase is currently " << new150_phase << "\n\n";
				break;
			}
		}
		
		// Finish
		return true;
	
}

int main() {
	
	bool stopearly = false;
	bool singlerun = true;
	bool circadian = false;
	bool circadian2 = false;
	bool clock_test = false;
	bool phase_response = false;
	bool phase_response_light = false;
	
	if(phase_response_light) {		
		// Make a list of start steps
		std::vector<int> start_steps;
		for(int i = 0; i < 125; i++) start_steps.push_back(i);	// Max: i = 125
		
		// Make vectors to hold the simulation output
		std::vector<double> phase_1_dawn_vector;
		std::vector<double> time_1_dawn_vector;
		std::vector<double> phase_1_dusk_vector;
		std::vector<double> time_1_dusk_vector;
		std::vector<double> ref_dawn150_phase_vector;
		std::vector<double> ref_dusk150_phase_vector;
		std::vector<double> dawn150_phase_vector;
		std::vector<double> dusk150_phase_vector;
		
		// Make variables to store the results of each simulation
		double phase_1_dawn, time_1_dawn;
		double phase_1_dusk, time_1_dusk;
		double ref_dawn150_phase, ref_dusk150_phase;
		double dawn150_phase, dusk150_phase;
		
		// Run a bunch of simulations
		for(size_t i = 0; i < start_steps.size(); i++) {
			int current_step = start_steps[i];
			bool success = find_tracker_phase_offsets(current_step, phase_1_dawn, time_1_dawn, phase_1_dusk, time_1_dusk, ref_dawn150_phase, ref_dusk150_phase, dawn150_phase, dusk150_phase);
			if(success) {
				phase_1_dawn_vector.push_back(phase_1_dawn);
				time_1_dawn_vector.push_back(time_1_dawn);
				phase_1_dusk_vector.push_back(phase_1_dusk);
				time_1_dusk_vector.push_back(time_1_dusk);
				ref_dawn150_phase_vector.push_back(ref_dawn150_phase);
				ref_dusk150_phase_vector.push_back(ref_dusk150_phase);
				dawn150_phase_vector.push_back(dawn150_phase);
				dusk150_phase_vector.push_back(dusk150_phase);
			}
		}
		
		// Store the results
		std::string name = std::string("PRC_light_weak.csv");
		std::ofstream myfile;
	  	myfile.open (name, std::ios::out | std::ios::trunc);
	  	myfile << "phase_1_dawn,time_1_dawn,phase_1_dusk,time_1_dusk,ref_dawn150_phase,ref_dusk150_phase,dawn150_phase,dusk150_phase\n";
	  	for(size_t i = 0; i < phase_1_dawn_vector.size(); i++) myfile << phase_1_dawn_vector[i] << "," << time_1_dawn_vector[i] << "," << phase_1_dusk_vector[i] << "," << time_1_dusk_vector[i] << "," << ref_dawn150_phase_vector[i] << "," << ref_dusk150_phase_vector[i] << "," << dawn150_phase_vector[i] << "," << dusk150_phase_vector[i] << "\n"; 
		myfile.close();
		
		// Stop doing things
		return 0;
	}
	
	if(phase_response) {
		// Define the force amplitude
		double A = 100.0;
		
		// Make a list of start steps
		std::vector<int> start_steps;
		for(int i = 0; i < 100; i++) start_steps.push_back(i);
		
		// Make vectors to hold the simulation output
		std::vector<double> phase_1_vector;
		std::vector<double> time_1_vector;
		std::vector<double> ref1_phase_vector;
		std::vector<double> new1_phase_vector;
		std::vector<double> ref2_phase_vector;
		std::vector<double> new2_phase_vector;
		std::vector<double> ref150_phase_vector;
		std::vector<double> new150_phase_vector;
		
		// Make variables to store the results of each simulation
		double phase_1, time_1;
		double ref1_phase, new1_phase;
		double ref2_phase, new2_phase;
		double ref150_phase, new150_phase;
		
		// Run a bunch of simulations
		for(size_t i = 0; i < start_steps.size(); i++) {
			int current_step = start_steps[i];
			bool success = find_clock_phase_offset(current_step, A, phase_1, time_1, ref1_phase, new1_phase, ref2_phase, new2_phase, ref150_phase, new150_phase);
			if(success) {
				phase_1_vector.push_back(phase_1);
				time_1_vector.push_back(time_1);
				ref1_phase_vector.push_back(ref1_phase);
				new1_phase_vector.push_back(new1_phase);
				ref2_phase_vector.push_back(ref2_phase);
				new2_phase_vector.push_back(new2_phase);
				ref150_phase_vector.push_back(ref150_phase);
				new150_phase_vector.push_back(new150_phase);
			}
		}
		
		// Store the results
		std::string name = std::string("PRC_poin2_A_") + std::to_string(A) + std::string(".csv");
		std::ofstream myfile;
	  	myfile.open (name, std::ios::out | std::ios::trunc);
	  	myfile << "phase_1,time_1,ref1_phase,new1_phase,ref2_phase,new2_phase,ref150_phase,new150_phase\n";
	  	for(size_t i = 0; i < phase_1_vector.size(); i++) myfile << phase_1_vector[i] << "," << time_1_vector[i] << "," << ref1_phase_vector[i] << "," << new1_phase_vector[i] << "," << ref2_phase_vector[i] << "," << new2_phase_vector[i] << "," << ref150_phase_vector[i] << "," << new150_phase_vector[i] << "\n"; 
		myfile.close();
		
		// Stop doing things
		return 0;
	}
	
	if(clock_test) {
		// Define the initial oscillator state
		double E_0 = 0.1;
		double theta_0 = 0.0;
		
		// Define the oscillator restoring strength
		double gamma = 1.0;
		
		// Make the timestep equal to 0.01 of the oscillator's intrinsic period of 2 * pi
		double timestep = 2.0 * 3.14159265358979323846 / 100.0;
		
		// Define the driving force parameters (run the force for one timestep, starting at an integer timestep)
		double T = 6.0;
		double A = 10.0;
		double F_start = 0.0 * timestep;
		double F_end = F_start + timestep;
		double max_F_per = 200;
		
		// Make the filename
		std::string filename = std::string("clock_testing_poin2_E0_") + std::to_string(E_0) + std::string("clock_testing_T0_") + std::to_string(theta_0) + std::string("_gamma_") + std::to_string(gamma) + std::string("_A_") + std::to_string(A) + std::string(".csv");
		
		// Initialize the system inputs
		std::unordered_map<std::string, double> initial_state;
		std::unordered_map<std::string, double> invariant_parameters;
		std::vector<std::string> steady_state_modules;
		std::vector<std::string> derivative_modules;
		
		// Set the initial state
		initial_state = {
			{"x",			sqrt(E_0) * cos(theta_0)},
			{"v",			sqrt(E_0) * sin(theta_0)},
			{"E",			E_0},
			{"theta",		theta_0},
			{"max_F_per",	max_F_per}
		};
		
		// Make a steady state module list
	  	steady_state_modules = {
	  		"clock_testing_calc"
		};
	  	
	  	// Make a derivative module list
	  	derivative_modules = {
	  		"clock_testing"
		};
		
		// Set the invariant parameters
		invariant_parameters = {
			{"timestep",	timestep},
			{"gamma",		gamma},
			{"T",			T},
			{"A",			A},
			{"F_start",		F_start},
			{"F_end",		F_end}
		};
		
		// Define the varying parameters, i.e., make a list of times
		int nperiods = 400;
		int ntimes = 100 * nperiods + 1;
		std::vector<double> time_vector;
		for (int i = 0; i < ntimes; i++) time_vector.push_back(i*timestep);
		std::unordered_map<std::string, std::vector<double>> varying_parameters {
			{"time", time_vector},
			{"doy", time_vector},
			{"hour", time_vector}
		};
		
		// Make a system and store a smart pointer to it
		std::shared_ptr<System> sys(new System(initial_state, invariant_parameters, varying_parameters, steady_state_modules, derivative_modules, true));
		
		// Make a system caller
		SystemCaller syscall(sys);
		
		// Get the current state in the correct format
		state_type state;
		sys->get_state(state);
		
		// Make vectors to store the observer output
		std::vector<state_type> state_vec;
		std::vector<time_type> time_vec;
		
		// Set the error tolerance to use for odeint simulations
		double error_tol = 1.0e-4;
		
		// Set the step size to use for odeint simulations
		double step_size = 1.0;
		
		// Run the simulation
		double abs_err = error_tol, rel_err = error_tol;
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
			// Just save whatever progresss we have made
			sys->print_results(state_vec, time_vec, filename);
		}
		
		// Save the results
		sys->print_results(state_vec, time_vec, filename);
		
		// Stop doing things
		return 0;
	}
	
	if(circadian) {
		// Initialize the system inputs
		std::unordered_map<std::string, double> initial_state;
		std::unordered_map<std::string, double> invariant_parameters;
		std::unordered_map<std::string, std::vector<double>> varying_parameters;
		std::vector<std::string> steady_state_modules;
		std::vector<std::string> derivative_modules;
		
		/*
		// Set the initial state
		initial_state = {
			{"LHY_mRNA",			0.0},
			{"P",					0.0},
			{"GI_ZTL",				0.0},
			{"GI_ELF3_cytoplasm",	0.0},
			{"LHY_prot",			0.0},
			{"TOC1_mRNA",			0.0},
			{"PRR9_prot",			0.0},
			{"PRR5_NI_mRNA",		0.0},
			{"PRR5_NI_prot",		0.0},
			{"GI_prot_cytoplasm",	0.0},
			{"TOC1_prot",			0.0},
			{"ZTL",					0.0},
			{"EC",					0.0},
			{"GI_mRNA",				0.0},
			{"PRR9_mRNA",			0.0},
			{"PRR7_mRNA",			0.0},
			{"PRR7_prot",			0.0},
			{"ELF4_mRNA",			0.0},
			{"ELF4_prot",			0.0},
			{"LHY_prot_modif",		0.0},
			{"HY5",					0.0},
			{"HFR1",				0.0},
			{"ELF3_mRNA",			0.0},
			{"ELF3_cytoplasm",		0.0},
			{"ELF3_nuclear",		0.0},
			{"COP1_nuclear_night",	0.0},
			{"COP1_nuclear_day",	0.0},
			{"LUX_mRNA",			0.0},
			{"LUX_prot",			0.0},
			{"COP1_cytoplasm",		0.0}
		};
	  	
	  	// Make a derivative module list
	  	derivative_modules = {
	  		"pokhilko_circadian_clock"
		};
		*/
		
		/*
		// Set the initial state
		initial_state = {
			{"cLm", 	0.539},
			{"cLc", 	0.015},
			{"cLn", 	0.0855},
			{"cTm", 	0.456},
			{"cTc", 	8.7},
			{"cTn", 	0.4},
			{"cXm", 	0.08},
			{"cXc", 	1.34},
			{"cXn", 	0.4},
			{"cYm", 	0.037},
			{"cYc", 	0.004},
			{"cYn", 	0.013},
			{"cPn", 	0.835}
		};
		
		// Make a steady state module list
	  	steady_state_modules = {
		};
	  	
	  	// Make a derivative module list
	  	derivative_modules = {
	  		"song_flowering"
		};
		*/
		
		// Set the initial state
		initial_state = {
			{"night_tracker",	1.0},
			{"day_tracker",		0.0},
			{"dawn_x",			0.0},
			{"dawn_v",			1.0},
			{"dusk_x",			0.0},
			{"dusk_v",			-1.0},
			{"ref_x",			0.0},
			{"ref_v",			1.0},
			{"allday",			0.0}
		};
		
		// Make a steady state module list
	  	steady_state_modules = {
	  		//"fake_solar",	// Use with weather01-blank.csv varying parameters
	  		"circadian_clock_calculator"
		};
	  	
	  	// Make a derivative module list
	  	derivative_modules = {
	  		"circadian_clock"
		};
		
		// Set the invariant parameters
		invariant_parameters = {
			{"timestep",		1.0},
			{"target_doy_dbl",	141.0}
		};
		
		// Get the varying parameters
		varying_parameters = read_varying_parameters("weather01-dst.csv");		// Use without the fake_solar module
		//varying_parameters = read_varying_parameters("weather01-blank.csv");	// Use with the fake_solar module
			
		// Make a system and store a smart pointer to it
		std::shared_ptr<System> sys(new System(initial_state, invariant_parameters, varying_parameters, steady_state_modules, derivative_modules, true));
		
		// Make a system caller
		SystemCaller syscall(sys);
		
		// Get the current state in the correct format
		state_type state;
		sys->get_state(state);
		
		// Get the number of time points
		int ntimes = sys->get_ntimes();
		
		// Make vectors to store the observer output
		std::vector<state_type> state_vec;
		std::vector<time_type> time_vec;
		
		// Set the error tolerance to use for odeint simulations
		double error_tol = 1.0e-4;
		
		// Set the step size to use for odeint simulations
		double step_size = 1.0;
		
		// Run the simulation
		double abs_err = error_tol, rel_err = error_tol;
		typedef boost::numeric::odeint::rosenbrock4<double> dense_stepper_type;
		try {
			//boost::numeric::odeint::integrate_const(
			boost::numeric::odeint::integrate_adaptive(
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
			// Just save whatever progresss we have made
			sys->print_results(state_vec, time_vec, "circadian_clock.csv");
		}
		
		// Save the results
		sys->print_results(state_vec, time_vec, "circadian_clock.csv");
		
		// Stop doing things
		return 0;
	}
	
	if(circadian2) {
		// Make the filename
		std::string filename = std::string("clock_testing2.csv");
		
		// Initialize the system inputs
		std::unordered_map<std::string, double> initial_state;
		std::unordered_map<std::string, double> invariant_parameters;
		std::unordered_map<std::string, std::vector<double>> varying_parameters;
		std::vector<std::string> steady_state_modules;
		std::vector<std::string> derivative_modules;
		
		// Set the initial state
		initial_state = {
			{"phi",			0.0}
		};
		
		// Make a steady state module list
	  	steady_state_modules = {
	  		"fake_solar",	// Use with weather01-blank.csv varying parameters
	  		"circadian_clock_calculator2"
		};
	  	
	  	// Make a derivative module list
	  	derivative_modules = {
	  		"circadian_clock2"
		};
		
		// Set the invariant parameters
		invariant_parameters = {
			{"timestep",	1.0},
			{"target_doy_dbl",	141.0}
		};
		
		// Get the varying parameters
		varying_parameters = read_varying_parameters("weather01-blank.csv");		// Use with the fake_solar module
		//varying_parameters = read_varying_parameters("weather01-dst.csv");			// Use without the fake_solar module
		
		// Make a system and store a smart pointer to it
		std::shared_ptr<System> sys(new System(initial_state, invariant_parameters, varying_parameters, steady_state_modules, derivative_modules, true));
		
		// Make a system caller
		SystemCaller syscall(sys);
		
		// Get the current state in the correct format
		state_type state;
		sys->get_state(state);
		
		// Get the number of time points
		int ntimes = sys->get_ntimes();
		
		// Make vectors to store the observer output
		std::vector<state_type> state_vec;
		std::vector<time_type> time_vec;
		
		// Set the error tolerance to use for odeint simulations
		double error_tol = 1.0e-4;
		
		// Set the step size to use for odeint simulations
		double step_size = 1.0;
		
		// Run the simulation
		double abs_err = error_tol, rel_err = error_tol;
		typedef boost::numeric::odeint::rosenbrock4<double> dense_stepper_type;
		boost::numeric::odeint::integrate_const(
			boost::numeric::odeint::make_dense_output<dense_stepper_type>(abs_err, rel_err),
			std::make_pair(syscall, syscall),
			state,
			0.0,
			(double)ntimes - 1.0,
			step_size,
			push_back_state_and_time_verbose(state_vec, time_vec, (double)ntimes - 1.0)
		);
		
		// Save the results
		sys->print_results(state_vec, time_vec, filename);
		
		// Stop doing things
		return 0;
	}
	
	// Choose a name for the optimization
	std::string name = std::string("glycine_max_01_CRS_143_V2");
	std::cout << "Beginning optimization called " << name << "\n";
	
	// Get the starting time
    char timestampString[32];
    time_t rawtime = time(NULL);
    strftime(timestampString, 32, "%Y-%m-%d-%H-%M-%S", localtime(&rawtime));
    
    // Get the reference data
    std::vector<std::tuple<std::string, double, std::string, double>> reference_data = read_reference_data("glycine_max_01_reference.csv");
	
	// Get the BioCro simulation result
	//std::unordered_map<std::string, std::vector<double>> biocro_results;
	//biocro_results = read_simulation_record("biocro_miscanthus_05.csv");
	
	// Initialize the system inputs
	std::unordered_map<std::string, double> initial_state;
	std::unordered_map<std::string, double> invariant_parameters;
	std::unordered_map<std::string, std::vector<double>> varying_parameters;
	std::vector<std::string> steady_state_modules;
	std::vector<std::string> derivative_modules;
	
	// Get the initial state
	initial_state = read_parameter_list("glycine_max_initial_state.csv");
	
	// Add aditional parameters to the initial state
	initial_state["night_tracker"] = 1.0;
	initial_state["day_tracker"] = 0.0;
	initial_state["dawn_x"] = 0.0;
	initial_state["dawn_v"] = 1.0;
	initial_state["dusk_x"] = 0.0;
	initial_state["dusk_v"] = -1.0;
	initial_state["ref_x"] = 0.0;
	initial_state["ref_v"] = 1.0;
	initial_state["allday"] = 0.0;
	
	// Get the invariant parameters
	invariant_parameters = read_parameter_list("glycine_max_parameters.csv");
	
	// Get the varying parameters
	//varying_parameters = read_varying_parameters("weather01-wholeseason.csv");	// Has some weird points due to switching to and from DST. Not usable.
	//varying_parameters = read_varying_parameters("weather01-dst.csv");			// Contains only the weather data during DST
	varying_parameters = read_varying_parameters("weather01-dst-143.csv");		// Contains only the weather data during DST after day 143, when the soybeans were planted
	
	// Make a steady state module list
  	steady_state_modules = {
  		"circadian_clock_calculator",
	  	"soil_type_selector",
	  	"stomata_water_stress_linear",
	  	"leaf_water_stress_exponential",
		"biomass_leaf_n_limitation",
		"parameter_calculator",
		"soil_evaporation",
		"c3_canopy",
		"utilization_growth_calculator",
		"utilization_senescence_calculator"
	};
  	
  	// Make a derivative module list
  	derivative_modules = {
  		"circadian_clock",
  		"utilization_growth",
  		"utilization_senescence",
  		"thermal_time_accumulator",
	  	"one_layer_soil_profile"
	};
		
	// Make a system and store a smart pointer to it
	std::shared_ptr<System> sys(new System(initial_state, invariant_parameters, varying_parameters, steady_state_modules, derivative_modules, true));
	
	// Maybe stop here
	if (stopearly) return 0;
	
	// Make a list of parameter names to vary during the optimization process
	std::vector<std::string> fitting_param_names = {
		"rate_constant_leaf",
		"rate_constant_stem",
		"rate_constant_root",
		"rate_constant_grain",
		"KmLeaf",
		"KmStem",
		"KmRoot",
		"KmGrain",
		"resistance_leaf_to_stem",
		"resistance_stem_to_root",
		"resistance_stem_to_grain",
		"rate_constant_leaf_senescence",
		"rate_constant_stem_senescence",
		"rate_constant_root_senescence",
		//"substrate_pool_leaf",
		//"substrate_pool_stem",
		//"substrate_pool_root",
		//"Leaf",
		//"Stem",
		//"Root",
		"grain_TTc",
		"iSp"
	};
	
	// Set their initial values
	
	/*
	// Original guess for soybeans
	double xvals[] = {
		0.5, 1.2, 0.8, 3,				// growth rate constants
		0.63, 0.71, 0.77, 0.84,			// Km values
		0.16, 0.16, 8,					// resistance values
		0.0025, 0.004, 0.0018,			// senescence rates
		0.03, 0.005, 0.005,				// initial substrate pools
		0.06, 0.01, 0.01,				// initial tissue mass
		950, 2.5						// grain_TTc and iSp
	};
	std::vector<double> x(xvals, xvals + sizeof(xvals) / sizeof(double));
	*/
	
	/*
	// First optimization result for soybeans
	double xvals[] = {
		0.500176, 1.06352, 0.65, 4.14157,		// growth rate constants
		0.630237, 0.710666, 0.817691, 0.84793,	// Km values
		0.106, 0.088, 10.1837,					// resistance values
		0.00242405, 0.00398209, 0.00195119,		// senescence rates
		0.249023, 0.00868262, 0.00896094,		// initial substrate pools
		0.0554437, 0.00741755, 0.198971,		// initial tissue mass
		942.755, 2.07073						// grain_TTc and iSp
	};
	std::vector<double> x(xvals, xvals + sizeof(xvals) / sizeof(double));
	*/
	
	/*
	// Second optimization result for soybeans
	double xvals[] = {
		0.498271, 0.963817, 0.59329, 4.8736,
		0.630237, 0.713425, 0.817691, 0.84793,
		0.09805, 0.088, 7.62549,
		0.00242406, 0.00394432, 0.00197517,
		//0.0415866, 0.0151946, 0.0127727,
		//0.0602811, 0.00741755, 0.198971,
		967.762, 0.915021	
	};
	std::vector<double> x(xvals, xvals + sizeof(xvals) / sizeof(double));
	*/
	
	/*
	// Current best during global CRS2 soybean optimization
	double xvals[] = {
		3.08706, 0.1, 8.51131, 4.27792, 7.6481, 9.21094, 6.9677, 4.40767, 1.25027, 9.76089, 0.370101, 0.00227059, 0.00956631, 0.01, 900, 0.589663
	};
	std::vector<double> x(xvals, xvals + sizeof(xvals) / sizeof(double));
	*/
	
	/*
	// Current best during global CRS2 soybean optimization
	double xvals[] = {
		1.08687, 1.26983, 2.83636, 5.811, 9.29031, 5.9716, 8.37707, 9.89279, 6.0466, 8.56714, 2.74792, 0.00228144, 0.0080574, 0.00955471, 950.234, 1.01394
	};
	std::vector<double> x(xvals, xvals + sizeof(xvals) / sizeof(double));
	*/
	
	/*
	// Best values from global CRS2 soybean optimization
	double xvals[] = {
		1.09443, 0.769573, 3.69391, 4.20917,	// growth rate constants
		9.84196, 5.54928, 9.86722, 8.77767,		// Km values
		5.57204, 6.84723, 8.41178,				// resistance values
		0.0022613, 0.00592988, 0.00998683,		// senescence rates
		974.823, 0.996066						// grain_TTc and iSp
	};
	std::vector<double> x(xvals, xvals + sizeof(xvals) / sizeof(double));
	*/
	
	// Current best during CRS2 soybean optimization
	double xvals[] = {
		5.19058, 0.0100501, 0.0349419, 29.3153, 35.4688, 1.41893, 5.37002, 7.43035, 0.0226361, 0.190704, 0.00351716, 0.00229215, 0.000298893, 0.00016656, 1145.29, 1.8128
	};
	std::vector<double> x(xvals, xvals + sizeof(xvals) / sizeof(double));
	
	/*
	// Set their lower bounds (all 0)
	double lbvals[] = {
		0.0, 0.0, 0.0, 0.0,		// rate constants
		0.0, 0.0, 0.0, 0.0,		// Km values
		0.0, 0.0, 0.0,			// resistance values
		0.0, 0.0, 0.0,			// senescence values
		//0.0, 0.0, 0.0,			// initial substrate pools
		//0.0, 0.0, 0.0,			// initial tissue mass
		0.0, 0.0				// grain_TTc and iSp
	};
	std::vector<double> lb(lbvals, lbvals + sizeof(lbvals) / sizeof(double));
	*/
	
	// Set their lower bounds
	double lbvals[] = {
		0.01, 0.01, 0.01, 0.01,	// rate constants
		0.01, 0.01, 0.01, 0.01,	// Km values
		0.001, 0.001, 0.001,	// resistance values
		0.0001, 0.0001, 0.0001,	// senescence values
		//0.0, 0.0, 0.0,		// initial substrate pools
		//0.0, 0.0, 0.0,		// initial tissue mass
		500.0, 0.05				// grain_TTc and iSp
	};
	std::vector<double> lb(lbvals, lbvals + sizeof(lbvals) / sizeof(double));
	
	/*
	// Set their upper bounds (all inf)
	double ubvals[] = {
		HUGE_VAL, HUGE_VAL, HUGE_VAL, HUGE_VAL,	// rate constants
		HUGE_VAL, HUGE_VAL, HUGE_VAL, HUGE_VAL,	// Km values
		HUGE_VAL, HUGE_VAL, HUGE_VAL,			// resistance values
		HUGE_VAL, HUGE_VAL, HUGE_VAL,			// senescence values
		//HUGE_VAL, HUGE_VAL, HUGE_VAL,			// initial substrate pools
		//HUGE_VAL, HUGE_VAL, HUGE_VAL,			// initial tissue mass
		HUGE_VAL, HUGE_VAL						// grain_TTc and iSp
	};
	std::vector<double> ub(ubvals, ubvals + sizeof(ubvals) / sizeof(double));
	*/
	
	// Set their upper bounds
	double ubvals[] = {
		100.0, 100.0, 100.0, 100.0,	// rate constants
		100.0, 100.0, 100.0, 100.0,	// Km values
		100.0, 100.0, 100.0,		// resistance values
		0.1, 0.1, 0.1,				// senescence values
		//HUGE_VAL, HUGE_VAL, HUGE_VAL,	// initial substrate pools
		//HUGE_VAL, HUGE_VAL, HUGE_VAL,	// initial tissue mass
		1500.0, 50.0				// grain_TTc and iSp
	};
	std::vector<double> ub(ubvals, ubvals + sizeof(ubvals) / sizeof(double));
	
	// Set the error tolerance to use for odeint simulations
	double error_tol = 1.0e-4;
	
	// Make a comparison object and store a smart pointer to it
	std::shared_ptr<SimulationCompare> sim_comp(new ReferenceCompare(sys, fitting_param_names, std::string(timestampString), error_tol, reference_data));
	
	// Run a simulation if we are in single-run mode
	if(singlerun) {
		sim_comp->save(x, std::string("singlerun.csv"));
		return 0;
	}
	
	// Create an NLOPT "opt" object via:
	//  nlopt::opt(algorithm a, unsigned n)
	// Here we want to use derivative-free algorithms
	// NLOPT offers the following local algorithms, that nevertheless support bounds:
	//  LN_COBYLA
	//  LN_BOBYQA
	//  LN_PRAXIS
	//  LN_NELDERMEAD
	//  LN_SBPLX
	// There are also global search algorithms that require finite bounds, including
	//  the following:
	//  GN_DIRECT
	//  GN_CRS2_LM
	// I think these will take a long time to run
	unsigned n = (unsigned) x.size();
	//nlopt::opt optimizer(nlopt::LN_COBYLA, n);
	//nlopt::opt optimizer(nlopt::LN_BOBYQA, n);
	//nlopt::opt optimizer(nlopt::LN_PRAXIS, n);
	//nlopt::opt optimizer(nlopt::LN_NELDERMEAD, n);
	//nlopt::opt optimizer(nlopt::LN_SBPLX, n);
	//nlopt::opt optimizer(nlopt::GN_DIRECT, n);
	nlopt::opt optimizer(nlopt::GN_CRS2_LM, n);
	
	// Specify the minimization target via:
	//  nlopt::opt::set_min_objective(nlopt::vfunc f, void* f_data)
	optimizer.set_min_objective(f, &sim_comp);
	
	// Specify the upper and lower bounds via:
	//  void nlopt::opt::set_lower_bounds(const std::vector<double> &lb)
	//  void nlopt::opt::set_upper_bounds(const std::vector<double> &ub)
	optimizer.set_lower_bounds(lb);
	optimizer.set_upper_bounds(ub);
	
	// Specify the stopping conditions
	// The main condition is set via:
	//  void nlopt::opt::set_stopval(double stopval)
	//  In this case, the optimization will end when the value falls below stopval
	//  But we can also specify other conditions to prevent the optimizer from making small
	//   improvements near the optimal value
	// Here we'll specify an absolute tolerance via:
	//  void nlopt::opt::set_ftol_abs(double tol);
	//  In this case, the optimization will end if a step changes the optimum value by less than tol
	// We'll also specify a relative tolerance via:
	//  void nlopt::opt::set_xtol_rel(double tol);
	//  In this case, the optimization will end if a step changes the optimum value by less than tol multiplied by the optimum value
	//  Note: this method won't work well for an optimal value of 0, so it's good to use the absolute tolerance as well
	// Alternatively, remove all the stopping criteria to just let the optimization run (probably) forever. We can
	//  just end the process when we decide that progress has stalled
	double stopval = 0.0;
	double abs = 0.0;		// Set to 0 to disable
	double rel = 0.0;		// Set to 0 to disable
	optimizer.set_stopval(stopval);
	optimizer.set_ftol_abs(abs);
	optimizer.set_xtol_rel(rel);
	
	// Specify the initial step size via:
	//  void nlopt::opt::set_initial_step(double dx)
	double dx = 0.0;
	if (dx > 0.0) optimizer.set_initial_step(dx);
	
	// Make a variable to hold the optimimum value
	double opt_f;
	
	// Run the optimization via:
	//  nlopt::result nlopt::opt::optimize(std::vector<double> &x, double &opt_f);
	nlopt::result res;
	clock_t t = clock();
	try {
		res = optimizer.optimize(x, opt_f);
	}
	catch (std::exception &e) {
		std::cout << e.what() << "\n";
	}
	t = clock() - t;
	
	// Print the important results
	std::cout << "\nOptimization name was " << name << "\n\n";
	std::cout << "Stopping criteria were: stopval = " << stopval << ", abs = " << abs << ", rel = " << rel << "\n\n";
	if (dx > 0.0) std::cout << "Initial step size was: dx = " << dx << "\n\n";
	else std::cout << "Default step sizes were used\n\n";
	std::cout << "Initial values and bounds were:\n";
	for(size_t i = 0; i < fitting_param_names.size(); i++) std::cout << fitting_param_names[i] << " = " << xvals[i] << " (" << lbvals[i] << "," << ubvals[i] << ")\n";
	std::cout << "\nOptimization has finished after " << t << " clock cycles\n\n";
	std::cout << "Result indicates: ";
	switch(res) {
	    case nlopt::FAILURE : std::cout << "FAILURE\n"; break;
	    case nlopt::INVALID_ARGS : std::cout << "INVALID_ARGS\n"; break;
	    case nlopt::OUT_OF_MEMORY : std::cout << "OUT_OF_MEMORY\n"; break;
	    case nlopt::ROUNDOFF_LIMITED : std::cout << "ROUNDOFF_LIMITED\n"; break;
	    case nlopt::FORCED_STOP : std::cout << "FORCED_STOP\n"; break;
	    case nlopt::SUCCESS : std::cout << "SUCCESS\n"; break;
	    case nlopt::STOPVAL_REACHED : std::cout << "STOPVAL_REACHED\n"; break;
	    case nlopt::FTOL_REACHED : std::cout << "FTOL_REACHED (abs)\n"; break;
	    case nlopt::XTOL_REACHED : std::cout << "XTOL_REACHED (rel)\n"; break;
	    case nlopt::MAXEVAL_REACHED : std::cout << "MAXEVAL_REACHED\n"; break;
	    case nlopt::MAXTIME_REACHED : std::cout << "MAXTIME_REACHED\n"; break;
	    default : std::cout << "???\n";
  	}
  	std::cout << "\nOptimal value: f = " << opt_f << "\n\n";
  	std::cout << "Optimal parameters:\n";
  	for(size_t i = 0; i < x.size(); i++) std::cout << fitting_param_names[i] << " = " << x[i] << "\n";
	
	// Save them to a text file too, with a filename based on the current time so nothing gets overwritten
	std::string filename = std::string(timestampString) + std::string("-optimization-result.txt");
	std::ofstream myfile;
  	myfile.open (filename, std::ios::out | std::ios::trunc);
	myfile << "Optimization name was " << name << "\n\n";
	myfile << "Stopping criteria were: stopval = " << stopval << ", abs = " << abs << ", rel = " << rel << "\n\n";
	if (dx > 0.0) myfile << "Initial step size was: dx = " << dx << "\n\n";
	else myfile << "Default step sizes were used\n\n";
	myfile << "Initial values and bounds were:\n";
	for(size_t i = 0; i < fitting_param_names.size(); i++) myfile << fitting_param_names[i] << " = " << xvals[i] << " (" << lbvals[i] << "," << ubvals[i] << ")\n";
  	myfile << "\nOptimization has finished after " << t << " clock cycles\n\n";
	myfile << "Result indicates: ";
	switch(res) {
	    case nlopt::FAILURE : myfile << "FAILURE\n"; break;
	    case nlopt::INVALID_ARGS : myfile << "INVALID_ARGS\n"; break;
	    case nlopt::OUT_OF_MEMORY : myfile << "OUT_OF_MEMORY\n"; break;
	    case nlopt::ROUNDOFF_LIMITED : myfile << "ROUNDOFF_LIMITED\n"; break;
	    case nlopt::FORCED_STOP : myfile << "FORCED_STOP\n"; break;
	    case nlopt::SUCCESS : myfile << "SUCCESS\n"; break;
	    case nlopt::STOPVAL_REACHED : myfile << "STOPVAL_REACHED\n"; break;
	    case nlopt::FTOL_REACHED : myfile << "FTOL_REACHED (abs)\n"; break;
	    case nlopt::XTOL_REACHED : myfile << "XTOL_REACHED (rel)\n"; break;
	    case nlopt::MAXEVAL_REACHED : myfile << "MAXEVAL_REACHED\n"; break;
	    case nlopt::MAXTIME_REACHED : myfile << "MAXTIME_REACHED\n"; break;
	    default : myfile << "???\n";
  	}
  	myfile << "\nOptimal value: f = " << opt_f << "\n\n";
  	myfile << "Optimal parameters:\n";
  	for(size_t i = 0; i < x.size(); i++) myfile << fitting_param_names[i] << " = " << x[i] << "\n";
	filename = std::string(timestampString) + std::string(" ") + name + std::string(".csv");
  	myfile << "Saving simulation output to " << filename;
	myfile.close();
	
	// Also save the final simulation output
	std::cout << "Redoing the final simulation and saving the result...";
	sim_comp->save(x, filename);
	std::cout << "done!\n";
	
	return 0;
	
}
