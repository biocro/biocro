#include "system_helper_functions.h"
#include "state_map.h"

void get_variables_from_ss_modules(
    std::vector<std::string> const& steady_state_module_names,
    ModuleFactory const& module_factory,
    std::set<std::string>& unique_module_inputs,
    std::set<std::string>& unique_module_outputs,
    std::vector<std::string>& duplicate_output_variables,
    std::vector<std::string>& duplicate_module_names)
{
    std::set<std::string> unique_steady_state_module_names; // Make a container for storing module names
    for (std::string module_name : steady_state_module_names) {
        if (unique_steady_state_module_names.find(module_name) == unique_steady_state_module_names.end()) {
            // Record the module's name
            // Keep track of duplicate names since they will be treated as errors by the system constructor
            unique_steady_state_module_names.insert(module_name);
            // Check through the module's inputs, storing any that have not already appeared as inputs or outputs
            for (std::string p : module_factory.get_inputs(module_name)) {
                if (unique_module_inputs.find(p) == unique_module_inputs.end() && unique_module_outputs.find(p) == unique_module_outputs.end()) unique_module_inputs.insert(p);
            }
            // Check through the module's outputs, storing any that have not already appeared as outputs
            // Keep track of duplicates in the list of all output variables since they will be treated as errors
            //  by the system constructor
            for (std::string p : module_factory.get_outputs(module_name)) {
                if (unique_module_outputs.find(p) == unique_module_outputs.end()) unique_module_outputs.insert(p);
                else duplicate_output_variables.push_back(std::string("Variable '") + p + std::string("' from the '") + module_name + std::string("' module"));
            }
        }
        else duplicate_module_names.push_back(std::string("Steady state module '") + module_name);
    }
}

void get_variables_from_derivative_modules(
    std::vector<std::string> const& derivative_module_names,
    ModuleFactory const& module_factory,
    std::set<std::string>& unique_module_inputs,
    std::set<std::string>& unique_module_outputs,
    std::vector<std::string>& duplicate_module_names)
{
    std::set<std::string> unique_derivative_module_names;   // Make a container for storing module names
    for (std::string module_name : derivative_module_names) {
        if (unique_derivative_module_names.find(module_name) == unique_derivative_module_names.end()) {
            // Record the module's name
            // Keep track of duplicate names since they will be treated as errors by the system constructor
            unique_derivative_module_names.insert(module_name);
            // Check through the module's inputs
            // unique_module_inputs is a set, so it will only store unique values
            for (std::string p : module_factory.get_inputs(module_name)) unique_module_inputs.insert(p);
            // Check through the module's outputs
            // unique_module_outputs is a set, so it will only store unique values
            for (std::string p : module_factory.get_outputs(module_name)) unique_module_outputs.insert(p);
        }
        else duplicate_module_names.push_back(std::string("Derivative module '") + module_name);
    }
}

// This is a simple wrapper for vprintf that returns void instead of an int
// The purpose of this function is to create a printing function
//  having the same output and syntax as Rprintf
void void_printf(char const* format, ...) {
    va_list args;               // Get the input arguments
    va_start (args, format);    // Enable access to the input arguments
    vprintf (format, args);     // Pass the arguments to vprintf, which is just like printf except it doesn't use variadic inputs
    va_end (args);              // Cleanup the input argument list
}

void create_modules_from_names(
    std::vector<std::string> const& module_names,
    bool is_deriv,
    std::vector<std::unique_ptr<Module>>& module_list,
    ModuleFactory const& module_factory,
    std::vector<std::string>& incorrect_modules,
    std::vector<std::string>& adaptive_step_size_incompat,
    bool verbose,
    void (*print_msg) (char const *format, ...))
{
    // Set the content of the error message, depending on whether the modules are supposed to
    //  be derivative or steady state
    std::string error_msg;
    if (is_deriv) error_msg = std::string("' was included in the list of derivative modules, but it does not return a derivative");
    else error_msg = std::string("' was included in the list of steady state modules, but it returns a derivative");
    
    // Create each module and make sure it's correctly classified
    for(std::string module_name : module_names) {
        module_list.push_back(module_factory.create(module_name));
        if (verbose) print_msg("\n  %s", module_list.back()->get_name().c_str());
        if (module_list.back()->is_deriv() != is_deriv) incorrect_modules.push_back(std::string("'") + module_name + error_msg);
        if (!module_list.back()->is_adaptive_compatible()) adaptive_step_size_incompat.push_back(module_name);
    }
}

template<typename list_type>
bool all_are_in_list(list_type find_these, list_type in_this) {
    bool found;
    for (auto const & find_this : find_these) {
        found = false;
        for (auto const & this_it : in_this) {
            if (find_this == this_it) {
                found = true;
                break;
            }
        }
        if (!found) {
            break;
        }
    }
    return found;
}

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
    void (*print_msg) (char const *format, ...))
{
    // Set the content of the error message, depending on whether the modules are supposed to
    //  be derivative or steady state
    std::string error_msg;
    if (is_deriv) error_msg = std::string("' was included in the list of derivative modules, but it does not return a derivative");
    else error_msg = std::string("' was included in the list of steady state modules, but it returns a derivative");
    
    // Create each module and make sure it's correctly classified
    
    auto temp = keys(initial_state);
    std::vector<std::string> defined = keys(initial_state);
    temp = keys(parameters);
    defined.insert(defined.end(), temp.begin(), temp.end()); 
    temp = keys(varying_parameters);
    defined.insert(defined.end(), temp.begin(), temp.end()); 

    for(std::string module_name : module_names) {
        module_list.push_back(module_factory.create(module_name));
        auto required = module_factory.get_inputs(module_name);
        if (!all_are_in_list(required, defined)) {
            throw std::logic_error(std::string("The modules are given in the wrong order. The following module is before a module that provides its input:" + module_name + "\n"));
        }
        auto newly_defined = module_factory.get_outputs(module_name);
        defined.insert(defined.end(), newly_defined.begin(), newly_defined.end());

        if (verbose) print_msg("\n  %s", module_list.back()->get_name().c_str());
        if (module_list.back()->is_deriv() != is_deriv) incorrect_modules.push_back(std::string("'") + module_name + error_msg);
        if (!module_list.back()->is_adaptive_compatible()) adaptive_step_size_incompat.push_back(module_name);
    }
}

// This function either:
//  (1) Adds the error_msg to the total_error_string (verbose == false), or
//  (2) Prints the error_msg, followed by each item in the error_list (verbose == true)
void process_errors(
    std::vector<std::string> const& error_list,
    std::string error_msg,
    std::string& total_error_string,
    bool verbose,
    void (*print_msg) (char const *format, ...))
{
    if(error_list.size() != 0) {
        // The error list size is nonzero, indicating that some errors of this type have occurred
        total_error_string += error_msg + std::string(".\n");   // Add the error message to the total error string
        if(verbose) {
            std::string msg = error_msg + std::string(":\n");
            print_msg(msg.c_str());
            for(std::string s : error_list) print_msg("  %s\n", s.c_str());
            print_msg("\n");
        }
    }
}

// This function throws an exception to let the user know that a serious problem
//  has occurred, where the message is tailored to the verbosity mode
void report_errors(
    std::string& total_error_string,
    bool verbose)
{
    if(total_error_string.size() > 0) {
        // The error string size is nonzero, indicating that some errors have occurred
        if(!verbose) total_error_string += "Rerun the system in verbose mode for more information.\n";
        else total_error_string += "Check the previous messages for more information.\n";
        throw std::logic_error(total_error_string);
    }
}
