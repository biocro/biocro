#ifndef MODULE_FACTORY_H
#define MODULE_FACTORY_H

#include "../modules.h"
#include <memory>	// For unique_ptr

template<typename T> std::unique_ptr<Module> createModule(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) { return std::unique_ptr<Module>(new T(input_parameters, output_parameters)); }

class ModuleFactory {
	// This is a factory class that creates module objects and returns info about them
	public:
		ModuleFactory(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters);
    	std::unique_ptr<Module> create(std::string const &module_name) const;
    	std::vector<std::string> get_inputs(std::string const &module_name) const;
    	std::vector<std::string> get_outputs(std::string const &module_name) const;
	private:
		std::unordered_map<std::string, std::unique_ptr<Module>(*)(const std::unordered_map<std::string, double>*, std::unordered_map<std::string, double>*)> modules;
        std::unordered_map<std::string, std::vector<std::string>> input_parameter_names;
        std::unordered_map<std::string, std::vector<std::string>> output_parameter_names;
        const std::unordered_map<std::string, double>* _input_parameters;
        std::unordered_map<std::string, double>* _output_parameters;
};

std::unique_ptr<Module> ModuleFactory::create(std::string const &module_name) const {
	try {
		return this->modules.at(module_name)(_input_parameters, _output_parameters);
	}
	catch (std::out_of_range) {
		throw std::out_of_range(std::string("'") + module_name + std::string("'") + std::string(" was given as a module name, but the ModuleFactory::create could not find a module with that name.\n"));
	}
	catch (std::logic_error const &e) {
		throw std::logic_error(std::string("Upon construction, module '") + module_name + std::string("'") + std::string(" tried to access a parameter called '") + std::string(e.what()) + std::string("', but this parameter was not in the list. Check for spelling errors!\n"));
	}
}

std::vector<std::string> ModuleFactory::get_inputs(std::string const &module_name) const {
	try {
		return this->input_parameter_names.at(module_name);
	}
	catch (std::out_of_range) {
		throw std::out_of_range(std::string("'") + module_name + std::string("'") + std::string(" was given as a module name, but the ModuleFactory::get_input could not find a module with that name.\n"));
	}
}

std::vector<std::string> ModuleFactory::get_outputs(std::string const &module_name) const {
	try {
		return this->output_parameter_names.at(module_name);
	}
	catch (std::out_of_range) {
		throw std::out_of_range(std::string("'") + module_name + std::string("'") + std::string(" was given as a module name, but the ModuleFactory::get_output could not find a module with that name.\n"));
	}
}

#endif
