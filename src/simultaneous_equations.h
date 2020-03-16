#ifndef SIMULTANEOUS_EQUATIONS_H
#define SIMULTANEOUS_EQUATIONS_H

#include "state_map.h"
#include "validate_system.h"

bool validate_simultanous_equations_inputs(
    std::string& message,
    state_map const& known_quantities,
    string_vector const& unknown_quantities,
    string_vector const& independent_quantities,
    string_vector const& ss_module_names);

std::string analyze_simultanous_equations_inputs(
    state_map const& known_quantities,
    string_vector const& unknown_quantities,
    string_vector const& independent_quantities,
    string_vector const& ss_module_names);

class simultaneous_equations
{
   public:
    simultaneous_equations(
        state_map const& known_quantities,
        string_vector const& unknown_quantities,
        string_vector const& independent_quantities,
        string_vector const& ss_module_names) {}
};

string_vector get_unknown_quantities(std::vector<string_vector> module_name_vector);

string_vector get_independent_quantities(std::vector<string_vector> module_name_vector);

#endif