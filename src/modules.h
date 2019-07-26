#ifndef MODULES_H
#define MODULES_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
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


std::string join_string_vector(std::vector<std::string> const &state_keys);

bool any_key_is_duplicated(std::vector<std::vector<std::string>> const &keys);

#endif
