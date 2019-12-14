#ifndef MODULES_H
#define MODULES_H

#include <unordered_map>
#include <vector>                       // Include this here so all modules will have access to std::vector
#include "state_map.h"                  // Include this here so all modules will have access to state_map
#include "module_library/BioCro.h"      // Include this here so all modules will have access to the auxilliary functions
#include "module_library/AuxBioCro.h"   // Include this here so all modules will have access to the auxilliary functions
#include <cmath>                        // For log10
#include <sstream>                      // For replicating old sprintf functionality with strings
#include <iomanip>                      // For replicating old sprintf functionality with strings

class Module {
    // This class represents a component of a larger system of differential equations
    // A module can be one of the following subtypes:
    //   (1) "steady state" (i.e. it returns some parameters based on the current state) or
    //   (2) "derivative" (i.e. it returns the rate of change for some elements of the state)
    // A module must also indicate whether or not is compatible with adaptive step size integrators
    //   For most modules, this is the case. However, a few require information from previous
    //   steps and will only work properly with fixed step size methods
    // Module and the derived classes listed above are all abstract, so no objects of these
    //  classes can be instantiated. Instead, concrete (i.e., not abstract) subclasses must be
    //  defined elsewhere.
    public:
        Module(std::string const& module_name, bool const &deriv, bool const &adaptive_compatible) :
            _module_name(module_name),
            _is_deriv(deriv),
            _is_adaptive_compatible(adaptive_compatible)
            {}
        virtual ~Module() = 0;  // Make the destructor a pure virtual function so that no objects can be made directly from this class. Note: clang is unhappy if an abstract class has a non-virtual destructor.
        // Functions for returning module information
        std::string get_name() const {return _module_name;}
        bool is_deriv() const {return _is_deriv;}
        bool is_adaptive_compatible() const {return _is_adaptive_compatible;}
        // Functions for running the module
        void run() const {do_operation();}
    private:
        std::string const _module_name;
        bool const _is_deriv;
        bool const _is_adaptive_compatible;
        virtual void do_operation() const;
    protected:
        // Helpful functions for writing concrete module code
        static const double eps;
        virtual void update(double* output_ptr, const double& value) const = 0;     // This pure virtual function allows different update functions for steady and deriv modules
        double* get_op(std::unordered_map<std::string, double>* output_parameters, const std::string& name);
        const double* get_ip(const std::unordered_map<std::string, double>* input_parameters, const std::string& name) const;
        double get_val_debug(const double* ptr, const std::string name) const;
};

// A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header
inline Module::~Module() {}

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
        SteadyModule(const std::string & module_name, bool adaptive_compatible = true) : Module(module_name, 0, adaptive_compatible) {}
    protected:
        void update(double* output_ptr, const double& value) const {*output_ptr = value;}    // The output parameters of a steady state module are unique, so we can just overwrite the previously stored value
};

// This derived class represents a derivative module
class DerivModule : public Module {
    public:
        DerivModule(const std::string& module_name, bool adaptive_compatible = true) : Module(module_name, 1, adaptive_compatible) {}
    protected:
        void update(double* output_ptr, const double& value) const {*output_ptr += value;}    // The output parameters of a derivative module are not necessarily unique, so we should add a new one to the previously stored value
};

class MultilayerModule {
    // The purpose of this class is to give a module some new functions
    //  used with multilayer parameter lists
    // It is an abstract class with several private static member functions
    public:
        MultilayerModule() {}
        virtual ~MultilayerModule() = 0;
    protected:
        static std::vector<std::string> get_multilayer_param_names(int nlayers, std::vector<std::string> param_names);      // Should be protected so only child classes can access it
        const std::vector<double*> get_multilayer_op(std::unordered_map<std::string, double>* output_parameters, int nlayers, const std::string& name) const;
        const std::vector<const double*> get_multilayer_ip(const std::unordered_map<std::string, double>* input_parameters, int nlayers, const std::string& name) const;
};

// A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header
inline MultilayerModule::~MultilayerModule() {}

#endif
