#ifndef MODULES_H
#define MODULES_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include "BioCro.h"
#include "c3photo.h"
#include "state_map.h"
#include <math.h>

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

class Module_1: public IModule {
    public:
        Module_1()
            : IModule ("Module_1",
                    std::vector<std::string> {},
                    std::vector<std::string> {"A", "B"})
        {}
    private:
        state_map do_operation (state_map const &s) const {
            return { {"A", 0}, {"B", 0} };
        };
};


class Module_2: public IModule {
    public:
        Module_2()
            : IModule ("Module_2",
                    std::vector<std::string> {"B"},
                    std::vector<std::string> {"C", "D"})
        {}
    private:
        state_map do_operation (state_map const &s) const {
            return { {"C", 0}, {"D", 0} };
        };
};


class Module_3: public IModule {
    public:
        Module_3()
            : IModule ("Module_3",
                    std::vector<std::string> {"A", "C"},
                    std::vector<std::string> {"E"})
        {}
    private:
        state_map do_operation (state_map const &s) const {
            return { {"E", 0} };
        };
};

class ICanopy_photosynthesis_module : public IModule {
    public:
        ICanopy_photosynthesis_module(const std::string &module_name, const std::vector<std::string> &required_state, const std::vector<std::string> &modified_state)
            : IModule(module_name, required_state, modified_state)
        {}
};

class leaf_boundary_layer_conductance_nikolov : public IModule {
    public:
        leaf_boundary_layer_conductance_nikolov()
            : IModule("leaf_boundary_layer_conductance_nikolov",
                    std::vector<std::string> {},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation (state_map const &s) const;
};

class ISoil_evaporation_module : public IModule {
    public:
        ISoil_evaporation_module(const std::string &module_name, const std::vector<std::string> &required_state, const std::vector<std::string> &modified_state)
            : IModule(module_name, required_state, modified_state)
        {}
};

class penman_monteith_transpiration : public IModule {
    public:
        penman_monteith_transpiration()
            : IModule("penman_monteith_transpiration",
                    std::vector<std::string> {},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation (state_map const &s) const;
};

class priestley_transpiration : public IModule {
    public:
        priestley_transpiration()
            : IModule("priestley_transpiration",
                    std::vector<std::string> {},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation (state_map const &s) const;
};

class ISenescence_module : public IModule {
    public:
        ISenescence_module(const std::string &module_name, const std::vector<std::string> &required_state, const std::vector<std::string> &modified_state)
            : IModule(module_name, required_state, modified_state)
        {}
};

class IGrowth_module : public IModule {
    public:
        IGrowth_module(const std::string &module_name, const std::vector<std::string> &required_state, const std::vector<std::string> &modified_state)
            : IModule(module_name, required_state, modified_state)
        {}
};

state_vector_map Gro(
        state_map const &initial_state,
        state_map const &invariant_parameters,
        state_vector_map const &varying_parameters,
        std::vector<std::unique_ptr<IModule>> const &steady_state_modules,
        std::vector<std::unique_ptr<IModule>> const &derivative_modules);

state_map Gro(
        state_map state,
        std::vector<std::unique_ptr<IModule>> const &steady_state_modules,
        std::vector<std::unique_ptr<IModule>> const &derivative_modules);

double biomass_leaf_nitrogen_limitation(state_map const &state);
double thermal_leaf_nitrogen_limitation(state_map const &state);


std::string join_string_vector(std::vector<std::string> const &state_keys);

bool any_key_is_duplicated(std::vector<std::vector<std::string>> const &keys);

#endif
