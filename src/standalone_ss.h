#ifndef STANDALONE_SS_H
#define STANDALONE_SS_H

#include <vector>
#include <memory>  // For unique_ptr and shared_ptr
#include <unordered_map>
#include <set>
#include "modules.h"
#include "module_wrapper_factory.h"
#include "system_helper_functions.h"
#include "validate_system.h"

bool validate_standalone_ss_inputs(
    std::string& message,
    string_vector const& ss_module_names,
    std::unordered_map<std::string, const double*> const& input_ptrs,
    std::unordered_map<std::string, double*> const& output_ptrs);

/**
 * @class Standalone_SS
 * 
 * @brief This class defines a standalone steady state module, i.e.,
 * a module that can easily be used without creating a system.
 * It may be formed from one or more steady state modules.
 * Its main intended use is to allow modules to call other modules as
 * part of their operation.
 */
class Standalone_SS
{
   public:
    Standalone_SS(
        string_vector const& ss_module_names,
        std::unordered_map<std::string, const double*> const& input_var_ptrs,
        std::unordered_map<std::string, double*> const& output_var_ptrs);

    Standalone_SS(
        std::vector<std::string> const& ss_module_names,
        std::unordered_map<std::string, const double*> const& input_var_ptrs,
        std::unordered_map<std::string, double*> const& output_var_ptrs,
        bool verb,
        void (*print_fcn_ptr)(char const* format, ...) = void_printf);

    void run() const;

    // Possibly useful functions
    std::set<std::string> get_inputs() const { return unique_module_inputs; }
    std::set<std::string> get_outputs() const { return unique_module_outputs; }
    bool is_adaptive_compatible() const { return adaptive_compatible; }

   private:
    // Members for storing the original inputs
    std::vector<std::string> steady_state_module_names;
    bool verbose;
    void (*print_msg)(char const* format, ...);  // A pointer to a function that takes a pointer to a null-terminated string followed by additional optional arguments, and has no return value
    // Functions for checking and processing inputs when constructing a system
    void process_module_inputs();
    void basic_input_checks();
    void get_variables_from_modules(
        module_wrapper_factory& module_factory,
        std::vector<std::string>& duplicate_output_variables,
        std::vector<std::string>& duplicate_module_names);
    void report_variable_usage();
    void create_modules(
        module_wrapper_factory& module_factory,
        std::vector<std::string>& incorrect_modules,
        std::unordered_map<std::string, double>* quantities,
        std::unordered_map<std::string, double>* module_output_map);
    void get_pointer_pairs_member_function(
        std::unordered_map<std::string, const double*> const& input_var_ptrs,
        std::unordered_map<std::string, double*> const& output_var_ptrs);
        
    std::string startup_message;
    
    // A list of modules
    std::vector<std::unique_ptr<Module>> steady_state_modules;
    // Map for storing the central variable list (used as an input to the modules)
    std::unordered_map<std::string, double> quantities;
    // Map for storing the module outputs
    std::unordered_map<std::string, double> module_output_map;
    // Objects for passing the standalone_ss inputs to the module inputs,
    //  and the module outputs to the standalone_ss outputs
    std::vector<double*> module_output_ptrs;
    std::vector<std::pair<double*, const double*>> module_output_ptr_pairs;
    std::vector<std::pair<double*, const double*>> input_ptr_pairs;
    std::vector<std::pair<const double*, double*>> output_ptr_pairs;
    // Lists of input and output variable names
    std::set<std::string> unique_module_inputs;
    std::set<std::string> unique_module_outputs;
    // Possibly useful
    bool adaptive_compatible;
};

#endif
