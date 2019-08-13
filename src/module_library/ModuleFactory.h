#ifndef MODULE_FACTORY_H
#define MODULE_FACTORY_H

#include "../modules.h"
#include <memory>		// For unique_ptr
#include <unordered_map>
#include <vector>
#include <algorithm>	// For sort

template<typename T> std::unique_ptr<Module> createModule(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) { return std::unique_ptr<Module>(new T(input_parameters, output_parameters)); }

class ModuleFactory {
	// This is a factory class that creates module objects and returns info about them
	public:
		ModuleFactory(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters);
    	std::unique_ptr<Module> create(std::string const &module_name) const;
    	std::vector<std::string> get_inputs(std::string const &module_name) const;
    	std::vector<std::string> get_outputs(std::string const &module_name) const;
		std::vector<std::string> get_modules() const;
	private:
		std::unordered_map<std::string, std::unique_ptr<Module>(*)(const std::unordered_map<std::string, double>*, std::unordered_map<std::string, double>*)> modules;
        std::unordered_map<std::string, std::vector<std::string>> input_parameter_names;
        std::unordered_map<std::string, std::vector<std::string>> output_parameter_names;
        const std::unordered_map<std::string, double>* _input_parameters;
        std::unordered_map<std::string, double>* _output_parameters;
};

#endif
