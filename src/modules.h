#ifndef MODULES_H
#define MODULES_H

#include <unordered_map>
#include <vector>						// Include this here so all modules will have access to std::vector
#include "state_map.h"					// Include this here so all modules will have access to state_map
#include "module_library/BioCro.h"		// Include this here so all modules will have access to the auxilliary functions
#include "module_library/AuxBioCro.h"	// Include this here so all modules will have access to the auxilliary functions
#include <math.h>						// Include this here so all modules will have access to the standard math functions

class Module {
	// This class represents a component of a larger system of differential equations
	// A module can be one of the following subtypes:
	//   (1) "steady state" (i.e. it returns some parameters based on the current state) or
	//   (2) "derivative" (i.e. it returns the rate of change for some elements of the state)
	// Module and the three derived classes listed above are all abstract, so no objects of these
	//  classes can be instantiated. Instead, concrete (i.e., not abstract) subclasses must be
	//  defined elsewhere. See m_mymodule.h for examples.
	public:
		Module(std::string const& module_name, bool const &is_deriv) :
			_module_name(module_name),
			_is_deriv(is_deriv)
			{}
		// Functions for returning module information
        std::string get_name() const {return _module_name;}
        bool is_deriv() const {return _is_deriv;}
        // Functions for running the module
        void run() const {do_operation();}
	private:
		std::string const _module_name;
		bool const _is_deriv;
        virtual void do_operation() const;
    protected:
    	// Helpful functions for writing concrete module code
    	static const double eps;
    	virtual void update(double* output_ptr, const double& value) const = 0;		// This pure virtual function ensures that module is an abstract class and allows different update functions for steady and deriv modules
        double* get_op(std::unordered_map<std::string, double>* output_parameters, const std::string& name);
        const double* get_ip(const std::unordered_map<std::string, double>* input_parameters, const std::string& name) const;
        double get_val_debug(const double* ptr, const std::string name) const;
};

// If a concrete module has not defined a do_operation method, throw an error if its run method is called
inline void Module::do_operation() const {
	throw std::logic_error(std::string("Module '") + _module_name + std::string("' does not have a 'do_operation()' method defined.\n"));
}

// This is a helping function that returns a pointer to an element of an output map, i.e., an output pointer (op)
// It is designed to simplify the code in a module's initializer list and reduce possible errors
inline double* Module::get_op(std::unordered_map<std::string, double>* output_parameters, const std::string& name) {
	if(output_parameters->find(name) != output_parameters->end()) return &((*output_parameters).at(name));
	else throw std::logic_error(name);
}

// This is a helping function that returns a pointer to an element of an input map, i.e., an input pointer (ip)
// It is designed to simplify the code in a module's initializer list and reduce possible errors
inline const double* Module::get_ip(const std::unordered_map<std::string, double>* input_parameters, const std::string& name) const {
	if(input_parameters->find(name) != input_parameters->end()) return &((*input_parameters).at(name));
	else throw std::logic_error(name);
}

// This derived class represents a steady state module
class SteadyModule : public Module {
	public:
		SteadyModule(const std::string & module_name) : Module(module_name, 0) {}
	protected:
		void update(double* output_ptr, const double& value) const {*output_ptr = value;}	// The output parameters of a steady state module are unique, so we can just overwrite the previously stored value
};

// This derived class represents a derivative module
class DerivModule : public Module {
	public:
		DerivModule(const std::string& module_name) : Module(module_name, 1) {}
	protected:
		void update(double* output_ptr, const double& value) const {*output_ptr += value;}	// The output parameters of a derivative module are not necessarily unique, so we should add a new one to the previously stored value
};

/////////////////////////////////////////////////////
// KEEPING OLD MODEL INFO TO REDUCE COMPILE ERRORS //
/////////////////////////////////////////////////////

class IModule {
    public:
        IModule(std::string const& module_name, std::vector<std::string> const &required_state, std::vector<std::string> const &modified_state) :
            _module_name(module_name),
            _required_state(required_state),
            _modified_state(modified_state)
        {
            requirements_are_met = false;
        }
        inline std::string get_name() const  {return _module_name;};
        std::vector<std::string> list_required_state() const;
        std::vector<std::string> list_modified_state() const;
        std::string list_module_name() const;
        state_map run (state_map const &state) const;
        state_map run (state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const;
        std::vector<std::string> state_requirements_are_met(state_map const &state) const;
        virtual ~IModule() = 0;  // Make the destructor a pure virtual function so that no objects can be made directly from this class.
    private:
        std::string const _module_name;
        std::vector<std::string> const _required_state;
        std::vector<std::string> const _modified_state;
        virtual state_map do_operation(state_map const &state) const {throw std::logic_error("This module cannot call run().\n"); state_map derivs; return derivs;};
        virtual state_map do_operation(state_vector_map const &state_history, state_vector_map const &deriv_history, state_map const &parameters) const;
        bool requirements_are_met;
};

inline IModule::~IModule() {}  // A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header.

std::string join_string_vector(std::vector<std::string> const &state_keys);

bool any_key_is_duplicated(std::vector<std::vector<std::string>> const &keys);

#endif
