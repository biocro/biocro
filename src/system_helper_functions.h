#ifndef SYSTEM_HELPER_FUNCTIONS_H
#define SYSTEM_HELPER_FUNCTIONS_H

#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <memory>    // For unique_ptr
#include <stdio.h>   // For vprintf
#include <stdarg.h>  // For va_list
#include <time.h>
#include "modules.h"
#include "module_wrapper_factory.h"

/**
 * Returns pairs of pointers that each point to the same named quantity in each of the two input maps.
 * Here it is essential that map_1 and map_2 are passed by reference; otherwise the pointers will be
 * useless.
 */
template <typename name_list_type, typename value_type_1, typename value_type_2>
std::vector<std::pair<value_type_1*, value_type_2*>> get_pointer_pairs(
    name_list_type quantity_names,
    std::unordered_map<std::string, value_type_1>& map_1,
    std::unordered_map<std::string, value_type_2>& map_2)
{
    std::vector<std::pair<value_type_1*, value_type_2*>> pointer_pairs;

    for (std::string const& name : quantity_names) {
        std::pair<value_type_1*, value_type_2*> temporary(&map_1.at(name), &map_2.at(name));
        pointer_pairs.push_back(temporary);
    }

    return pointer_pairs;
}



// Functions for collecting info and reporting errors when constructing a System
//  or another object that requires a list of modules

void get_variables_from_ss_modules(
    std::vector<std::string> const& steady_state_module_names,
    module_wrapper_factory const& module_factory,
    std::set<std::string>& unique_module_inputs,
    std::set<std::string>& unique_module_outputs,
    std::vector<std::string>& duplicate_output_variables,
    std::vector<std::string>& duplicate_module_names);

void get_variables_from_derivative_modules(
    std::vector<std::string> const& derivative_module_names,
    module_wrapper_factory const& module_factory,
    std::set<std::string>& unique_module_inputs,
    std::set<std::string>& unique_module_outputs,
    std::vector<std::string>& duplicate_module_names);

void void_printf(char const* format, ...);

void create_modules_from_names(
    std::vector<std::string> const& module_names,
    bool is_deriv,
    std::vector<std::unique_ptr<Module>>& module_list,
    module_wrapper_factory const& module_factory,
    std::unordered_map<std::string, double>* quantities,
    std::unordered_map<std::string, double>* module_output_map,
    std::vector<std::string>& incorrect_modules,
    std::vector<std::string>& adaptive_step_size_incompat,
    bool verbose,
    void (*print_msg)(char const* format, ...) = void_printf);

void create_modules_from_names(
    std::vector<std::string> const& module_names,
    bool is_deriv,
    std::vector<std::unique_ptr<Module>>& module_list,
    std::unordered_map<std::string, double> initial_state,
    std::unordered_map<std::string, double> parameters,
    std::unordered_map<std::string, std::vector<double>> varying_parameters,
    module_wrapper_factory const& module_factory,
    std::unordered_map<std::string, double>* quantities,
    std::unordered_map<std::string, double>* module_output_map,
    std::vector<std::string>& incorrect_modules,
    std::vector<std::string>& adaptive_step_size_incompat,
    bool verbose,
    void (*print_msg)(char const* format, ...) = void_printf);

void process_errors(
    std::vector<std::string> const& error_list,
    std::string error_msg,
    std::string& total_error_string,
    bool verbose,
    void (*print_msg)(char const* format, ...) = void_printf);

void report_errors(
    std::string& total_error_string,
    bool verbose);

#endif
