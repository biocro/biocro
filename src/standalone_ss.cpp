#include "standalone_ss.h"

Standalone_SS::Standalone_SS(
	std::vector<std::string> const &steady_state_module_names,
	std::unordered_map<std::string, const double*> const &input_param_ptrs,
	std::unordered_map<std::string, double*> const &output_param_ptrs,
	bool verbose) :
	_verbose(verbose)
{
	// Lists for collecting parameters and modules
	std::set<std::string> unique_parameter_names;					// All parameter names
	std::set<std::string> unique_steady_state_module_names;			// All steady state module names
	std::set<std::string> unique_steady_state_parameter_names;		// All parameters output by steady state modules
	std::set<std::string> unique_module_inputs;						// All parameters used as inputs to modules
	std::set<std::string> unique_module_outputs;					// All parameters used as inputs to modules
	// Lists for describing problems with the inputs
	std::vector<std::string> duplicate_module_names;				// A list of module names that are duplicated
	std::vector<std::string> duplicate_output_parameters;			// A list of parameter names that are duplicated in the output of steady state modules
	std::vector<std::string> incorrect_modules;						// A list of mischaracterized modules, e.g., derivative modules included in the steady state module list
	std::vector<std::string> bad_input_names;						// A list of supplied input parameter names that don't match the input parameter list
	std::vector<std::string> bad_output_names;						// A list of supplied output parameter names that don't match the output parameter list
	std::string error_string;										// A message to send to the user about any issues that were discovered during the system setup
	
	// Begin to initialize
	if(_verbose) Rprintf("\nStarting to initialize a standalone_ss (standalone steady state module combination)\n\n");
	
	// Check to make sure at least one module was supplied
	if(_verbose) Rprintf("Checking to make sure at least one module was specified... ");
	if(steady_state_module_names.size() == 0) {
		throw std::logic_error(std::string("No input modules were found! A standalone_ss requires at least one module.\n"));
	}
	if(_verbose) Rprintf("done!\n\n");
	
	// Start collecting parameter names and check to make sure all the requirements are met
	if(_verbose) Rprintf("Building list of parameters...");
	
	// Initialize a module factory
	// Note: the inputs to the module factory have not been fully initialized yet. We can
	//  get the module input/output parameters right now, but any attempt to create a module
	//  will fail
	ModuleFactory module_factory(&parameters, &module_outputs);
	
	// Collect parameters from the first module
	std::string module_name = steady_state_module_names[0];		// Get the first module name
	unique_steady_state_module_names.insert(module_name);		// Add it to the list
	for(std::string p : module_factory.get_inputs(module_name)) {
		if(unique_module_inputs.find(p) == unique_module_inputs.end()) unique_module_inputs.insert(p);			// Store the module's unique input parameters in the list of inputs
		if(unique_parameter_names.find(p) == unique_parameter_names.end()) unique_parameter_names.insert(p);	// Store the module's unique input parameters in the list of all parameters
	}
	for(std::string p : module_factory.get_outputs(module_name)) {
		if(unique_parameter_names.find(p) == unique_parameter_names.end()) {
			unique_parameter_names.insert(p);	// Store the module's unique output parameters in the list of all parameters
			unique_module_outputs.insert(p);	// Store the module's unique output parameters in the list of output parameters
		}
		else duplicate_output_parameters.push_back(std::string("Parameter '") + p + std::string("' from the '") + module_name + std::string("' module"));
	}
	
	// Continue collecting and checking parameters from the other modules
	for(size_t i = 1; i < steady_state_module_names.size(); i++) {
		module_name = steady_state_module_names[i];
		if(unique_steady_state_module_names.find(module_name) == unique_steady_state_module_names.end()) {
			unique_steady_state_module_names.insert(module_name);
			for(std::string p : module_factory.get_inputs(module_name)) {
				if(unique_parameter_names.find(p) == unique_parameter_names.end()) {
					// This input parameter was not required or produced by any previous module
					unique_module_inputs.insert(p);		// Add the parameter to the input list
					unique_parameter_names.insert(p);	// Add the parameter to the list of all parameters
				}
			}
			for(std::string p : module_factory.get_outputs(module_name)) {
				if(unique_parameter_names.find(p) == unique_parameter_names.end()) {
					unique_parameter_names.insert(p);	// Store the module's unique output parameters in the list of all parameters
					unique_module_outputs.insert(p);	// Store the module's unique output parameters in the list of output parameters
				}
				else duplicate_output_parameters.push_back(std::string("Parameter '") + p + std::string("' from the '") + module_name + std::string("' module"));
			}
		}
		else duplicate_module_names.push_back(std::string("Steady state module '") + module_name);
	}
	if(_verbose) Rprintf(" done!\n\n");
	
	// Let the user know which inputs are required
	if(_verbose) {
		Rprintf("The following parameters are required as inputs to at least one module:\n");
		for(std::string p : unique_module_inputs) Rprintf("%s\n", p.c_str());
		Rprintf("\n");
	}
	
	// Let the user know which parameters are output
	if(_verbose) {
		Rprintf("The following parameters are produced as outputs:\n");
		for(std::string p : unique_module_outputs) Rprintf("%s\n", p.c_str());
		Rprintf("\n");
	}
	
	// Collect information about any errors that may have occurred while checking the parameters
	if(duplicate_module_names.size() != 0) {
		error_string += "Some modules were duplicated in the input list.\n";
		if(_verbose) {
			Rprintf("The following modules were duplicated:\n");
			for(std::string s : duplicate_module_names) Rprintf("%s\n", s.c_str());
			Rprintf("\n");
		}
	}
	if(duplicate_output_parameters.size() != 0) {
		error_string += "Some steady state module output parameters were already included by previous steady state modules.\n";
		if(_verbose) {
			Rprintf("The following steady state output parameters were already included in the initial state, invariant parameters, varying parameters, or previous steady state modules:\n");
			for(std::string s : duplicate_output_parameters) Rprintf("%s\n", s.c_str());
			Rprintf("\n");
		}
	}
	
	// If any errors occurred, notify the user
	if(error_string.size() > 0) {
		if(!_verbose) error_string += "Rerun the system in verbose mode for more information.\n";
		else error_string += "Check the previous messages for more information.\n";
		throw std::logic_error(error_string);
	}
	
	// Build the parameter lists
	for(std::string p : unique_parameter_names) {
		parameters[p] = 0.0;
		module_outputs[p] = 0.0;
	}
	
	// Get pointers to the steady state parameters in the parameter and module output maps
	for(std::string p : unique_module_outputs) {
		std::pair<double*, double*> temp(&parameters.at(p), &module_outputs.at(p));
		steady_state_ptrs.push_back(temp);
	}
	
	// Create the modules
	if(_verbose) Rprintf("Creating the steady state modules from the list and making sure the list only includes steady state modules... ");
	for(std::string module_name : steady_state_module_names) {
		steady_state_modules.push_back(module_factory.create(module_name));
		if(steady_state_modules.back()->is_deriv()) incorrect_modules.push_back(std::string("'") + module_name + std::string("' was included in the list of steady state modules, but it returns a derivative"));
	}
	if(_verbose) Rprintf("done!\n\n");
	
	// Collect information about any errors that may have occurred while creating the modules
	if(incorrect_modules.size() != 0) {
		error_string += "Some modules were mischaracterized in the input lists.\n";
		if(_verbose) {
			Rprintf("The following modules were mischaracterized:\n");
			for(std::string s : incorrect_modules) Rprintf("%s\n", s.c_str());
			Rprintf("\n");
		}
	}
	
	// If any errors occurred, notify the user
	if(error_string.size() > 0) {
		if(!_verbose) error_string += "Rerun the system in verbose mode for more information.\n";
		else error_string += "Check the previous messages for more information.\n";
		throw std::logic_error(error_string);
	}
	
	// Create the input pointer map, which contains pairs of pointers
	// In each pair, the first points to an entry in the parameters map,
	//  while the second points to an input variable outside the class
	// If the input_param_ptrs map has the wrong size, throw a length_error.
	// This will allow R_get_standalone_ss_info to disregard the problem.
	if(input_param_ptrs.size() == unique_module_inputs.size()) {
		for(auto x : input_param_ptrs) {
			if(unique_module_inputs.find(x.first) == unique_module_inputs.end()) bad_input_names.push_back(x.first);
			else {
				std::pair<double*, const double*> temp(&parameters.at(x.first), x.second);
				input_ptrs.push_back(temp);
			}
		}
	}
	else throw std::length_error(std::string("Thrown by standalone_ss: the supplied input pointer map does not have the correct length. Are all required parameters defined? Are too many defined?\n"));
	
	// Create the output pointer map, which contains pairs of pointers
	// In each pair, the first points to an entry in the module_output map,
	//  while the second points to an output variable outside the class
	if(output_param_ptrs.size() > 0) {
		for(auto x : output_param_ptrs) {
			if(unique_module_outputs.find(x.first) == unique_module_inputs.end()) bad_output_names.push_back(x.first);
			else {
				std::pair<double*, double*> temp(&module_outputs.at(x.first), x.second);
				output_ptrs.push_back(temp);
			}
		}
	}
	else error_string += "The supplied output pointer map is empty.\n";
	
	// Collect information about any errors that may have occurred while creating the modules
	if(bad_input_names.size() != 0) {
		error_string += "Some of the supplied input parameters were not required by the modules.\n";
		if(_verbose) {
			Rprintf("The following input parameters were not required:\n");
			for(std::string s : bad_input_names) Rprintf("%s\n", s.c_str());
			Rprintf("\n");
		}
	}
	if(bad_output_names.size() != 0) {
		error_string += "Some of the supplied output parameters were not produced by the modules.\n";
		if(_verbose) {
			Rprintf("The following output parameters were not valid:\n");
			for(std::string s : bad_output_names) Rprintf("%s\n", s.c_str());
			Rprintf("\n");
		}
	}
	
	// If any errors occurred, notify the user
	if(error_string.size() > 0) {
		if(!_verbose) error_string += "Rerun the system in verbose mode for more information.\n";
		else error_string += "Check the previous messages for more information.\n";
		throw std::logic_error(error_string);
	}
	
	// Otherwise, we are done!
	if(_verbose) Rprintf("Done applying checks and building the standalone module set!\n\n");
}

void Standalone_SS::run() {
	Rprintf("Running standalone_ss!\n\n");
	
	Rprintf("Input parameter addresses:");
	for(size_t i = 0; i < input_ptrs.size(); i++) Rprintf("\n  %i: %p", i, (input_ptrs[i]).second);
	Rprintf("\n\nOutput parameter addresses:");
	for(size_t i = 0; i < output_ptrs.size(); i++) Rprintf("\n  %i: %p", i, (output_ptrs[i]).second);
	
	/*
	// First get the input parameter values
	for(auto x : input_ptrs) *(x.first) = *(x.second);
	
	// Then clear the module output map
	for(auto x : steady_state_ptrs) *x.second = 0.0;
	
	// Now go through the modules
	for(auto it = steady_state_modules.begin(); it != steady_state_modules.end(); ++it) {
		(*it)->run();												// Run the module
		for(auto x : steady_state_ptrs) *(x.first) = *(x.second);	// Store its output in the main parameter map
	}
	
	// Finally, export the output parameter values
	for(auto x : output_ptrs) *(x.second) = *(x.first);
	*/
}