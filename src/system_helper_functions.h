#ifndef SYSTEM_HELPER_FUNCTIONS_H
#define SYSTEM_HELPER_FUNCTIONS_H

#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <memory>       // For unique_ptr
#include <stdio.h>      // For vprintf
#include <stdarg.h>     // For va_list
#include "modules.h"

// Functions for collecting info and reporting errors when constructing a System
//  or another object that requires a list of modules
/*
void get_variables_from_ss_modules(
    std::vector<std::string> const& steady_state_module_names,
    ModuleFactory const& module_factory,
    std::set<std::string>& unique_module_inputs,
    std::set<std::string>& unique_module_outputs,
    std::vector<std::string>& duplicate_output_variables,
    std::vector<std::string>& duplicate_module_names
);

void get_variables_from_derivative_modules(
    std::vector<std::string> const& derivative_module_names,
    ModuleFactory const& module_factory,
    std::set<std::string>& unique_module_inputs,
    std::set<std::string>& unique_module_outputs,
    std::vector<std::string>& duplicate_module_names
);
*/
void void_printf (char const *format, ...);
/*
void create_modules_from_names(
    std::vector<std::string> const& module_names,
    bool is_deriv,
    std::vector<std::unique_ptr<Module>>& module_list,
    ModuleFactory const& module_factory,
    std::vector<std::string>& incorrect_modules,
    std::vector<std::string>& adaptive_step_size_incompat,
    bool verbose,
    void (*print_msg) (char const *format, ...) = void_printf
);

void create_modules_from_names(
    std::vector<std::string> const& module_names,
    bool is_deriv,
    std::vector<std::unique_ptr<Module>>& module_list,
    std::unordered_map<std::string, double> initial_state,
    std::unordered_map<std::string, double> parameters,
    std::unordered_map<std::string, std::vector<double>> varying_parameters,
    ModuleFactory const& module_factory,
    std::vector<std::string>& incorrect_modules,
    std::vector<std::string>& adaptive_step_size_incompat,
    bool verbose,
    void (*print_msg) (char const *format, ...) = void_printf
);

void process_errors(
    std::vector<std::string> const& error_list,
    std::string error_msg,
    std::string& total_error_string,
    bool verbose,
    void (*print_msg) (char const *format, ...) = void_printf
);

void report_errors(
    std::string& total_error_string,
    bool verbose
);
*/
#endif
