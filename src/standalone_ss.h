#ifndef STANDALONE_SS_H
#define STANDALONE_SS_H

#include <vector>
#include <memory>		// For unique_ptr and shared_ptr
#include <set>
#include <Rinternals.h>	// For Rprintf
#include "modules.h"
#include "module_library/ModuleFactory.h"

class Standalone_SS {
	// This class defines a standalone steady state module, i.e., a module that can easily be used without creating a system
	// It may be formed from one or more basic steady state modules
	public:
		Standalone_SS(
			std::vector<std::string> const &steady_state_module_names,
			std::unordered_map<std::string, const double*> const &input_param_ptrs,
			std::unordered_map<std::string, double*> const &output_param_ptrs,
			bool verbose);
		void run();
	private:
		// Pointers to the modules
		std::vector<std::unique_ptr<Module>> steady_state_modules;
		// Map for storing the central parameter list
		std::unordered_map<std::string, double> parameters;
		// Objects for storing module outputs
		std::unordered_map<std::string, double> module_outputs;
		// Objects for getting module inputs and outputs
		std::vector<std::pair<double*, double*>> steady_state_ptrs;
		std::vector<std::pair<double*, const double*>> input_ptrs;
		std::vector<std::pair<double*, double*>> output_ptrs;
		// For user feedback
		bool _verbose;
};

#endif
