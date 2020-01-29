#ifndef MODULE_FACTORY_H
#define MODULE_FACTORY_H

#include "../modules.h"
#include <memory>		// For unique_ptr
#include <unordered_map>
#include <vector>
#include <algorithm>	// For sort and transform

template<typename T> std::unique_ptr<Module> createModule(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) { return std::unique_ptr<Module>(new T(input_parameters, output_parameters)); }

class ModuleFactory {
	// This is a factory class that creates module objects and returns info about them
	public:
		ModuleFactory(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters);
    	std::unique_ptr<Module> create(std::string const &module_name) const;				// Returns a pointer to a module object
    	std::vector<std::string> get_inputs(std::string const &module_name) const;			// Returns a module's input parameters
    	std::vector<std::string> get_outputs(std::string const &module_name) const;			// Returns a module's output parameters
		std::string get_description(std::string const &module_name) const;					// Returns a module's description
		std::unordered_map<std::string, std::vector<std::string>> get_all_param() const;	// Returns information about all parameters used by all modules
		std::vector<std::string> get_modules() const;										// Returns a list of all available modules
		static bool cisc(std::string const &a, std::string const &b);	// Simple function for case-insensitive string comparison, used for sorting the list of all module names
	private:
		// Maps for linking module names to their constructors and descriptions
		std::unordered_map<std::string, std::unique_ptr<Module>(*)(const std::unordered_map<std::string, double>*, std::unordered_map<std::string, double>*)> modules;
		std::unordered_map<std::string, std::vector<std::string>> input_parameter_names;
		std::unordered_map<std::string, std::vector<std::string>> output_parameter_names;
		std::unordered_map<std::string, std::string> module_descriptions;
		// Parameter lists required at construction; necessary when creating modules
		const std::unordered_map<std::string, double>* _input_parameters;
		std::unordered_map<std::string, double>* _output_parameters;
};

#endif