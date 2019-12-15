#include "standalone_ss.h"

Standalone_SS::Standalone_SS(
    std::vector<std::string> const &ss_module_names,
    std::unordered_map<std::string, const double*> const &input_var_ptrs,
    std::unordered_map<std::string, double*> const &output_var_ptrs,
    bool verb,
    void (*print_fcn_ptr) (char const *format, ...)) :
    steady_state_module_names(ss_module_names),
    verbose(verb),
    print_msg(print_fcn_ptr)
{
    // Lists for collecting variables
    std::set<std::string> unique_module_inputs;                     // All variables used as inputs to modules
    std::set<std::string> unique_module_outputs;                    // All variables output by modules
    
    // Begin to initialize
    if(verbose) print_msg("\nStarting to initialize a standalone_ss (standalone steady state module combination)\n\n");
    
    // Check through the input module list, making sure everything is consistent.
    // If everything is okay, generate a central variable map, a module output map, lists
    //  of modules, and lists of inputs and output variable names
    // This function will throw an exception if serious errors are found with the inputs
    process_module_inputs();
    
    // Get pairs of pointers that are required for the standalone_ss to access inputs
    //  and report ouputs
    get_pointer_pairs(input_var_ptrs, output_var_ptrs);
    
    // Now we are done!
    if(verbose) print_msg("Done applying checks and building the standalone steady state module combination!\n\n");
}

void Standalone_SS::process_module_inputs() {
    // Lists for describing problems with the inputs
    std::vector<std::string> duplicate_module_names;                // A list of module names that are duplicated
    std::vector<std::string> duplicate_output_variables;            // A list of parameter names that are duplicated in the output of steady state modules
    std::vector<std::string> incorrect_modules;                     // A list of mischaracterized modules, e.g., derivative modules included in the steady state module list
    
    // A string for describing problems with the inputs
    std::string error_string;
    
    // Make sure the inputs meet some basic requirements
    basic_input_checks();
    
    // Initialize a module factory, which will be used to get module I/O information
    //  and to create modules
    // Note: the inputs to the module factory have not been fully initialized yet. We can
    //  get the module input/output variables right now, but any attempt to create a module
    //  will fail
    module_wrapper_factory module_factory;
    
    // Collect variable names from the modules
    // Along the way, check for any duplicated variable or module
    //  names
    get_variables_from_modules(
        module_factory,
        duplicate_output_variables,
        duplicate_module_names
    );
    
    // Report the variable usage to the user, i.e., report the list of inputs and outputs
    if(verbose) report_variable_usage();
    
    // Collect information about any errors that may have occurred while checking the parameters
    process_errors(duplicate_module_names, std::string("Some modules were duplicated in the input list"), error_string, verbose, print_msg);
    process_errors(duplicate_output_variables, std::string("Some steady state module output parameters were already included by previous steady state modules"), error_string, verbose, print_msg);
    
    // If any errors have occurred, notify the user and throw an error to halt the operation,
    //  since the user needs to address these problems before continuing
    report_errors(error_string, verbose);
    
    // Now that we have a complete list of variables, we can use the module factory to create the modules
    create_modules(module_factory, incorrect_modules, &variables, &module_output_map);
    
    // Collect information about any errors that may have occurred while creating the modules
    process_errors(incorrect_modules, std::string("Some modules were mischaracterized in the input lists"), error_string, verbose, print_msg);
    
    // If any errors have occurred, notify the user and throw an error to halt the operation,
    //  since the user needs to address these problems before continuing
    report_errors(error_string, verbose);
}

void Standalone_SS::basic_input_checks() {
    // Check to make sure at least one module was supplied
    if(verbose) print_msg("Checking to make sure at least one module was specified... ");
    if(steady_state_module_names.size() == 0) {
        throw std::logic_error(std::string("No input modules were found! A standalone_ss requires at least one module.\n"));
    }
    if(verbose) print_msg("done!\n\n");
}

void Standalone_SS::get_variables_from_modules(
    module_wrapper_factory& module_factory,
    std::vector<std::string>& duplicate_output_variables,
    std::vector<std::string>& duplicate_module_names)
{
    // Collect variable names from the steady state modules
    get_variables_from_ss_modules(
        steady_state_module_names,
        module_factory,
        unique_module_inputs,
        unique_module_outputs,
        duplicate_output_variables,
        duplicate_module_names
    );
    
    // Check for any steady state module outputs that were already defined as inputs
    for(std::string p : unique_module_outputs) {
        if(unique_module_inputs.find(p) != unique_module_inputs.end()) duplicate_output_variables.push_back(std::string("Variable '") + p + std::string("'"));
    }
    
    // Build the main lists of variables
    for(std::string p : unique_module_inputs) variables[p] = 0.0;
    for(std::string p : unique_module_outputs) variables[p] = 0.0;
    module_output_map = variables;
}

void Standalone_SS::report_variable_usage() {
    // Let the user know which inputs are required
    print_msg("The following variables are required as inputs to at least one module:\n");
    for(std::string p : unique_module_inputs) print_msg("  %s\n", p.c_str());
    print_msg("\n");
    
    // Let the user know which variables are output
    print_msg("The following veriables are produced as outputs:\n");
    for(std::string p : unique_module_outputs) print_msg("  %s\n", p.c_str());
    print_msg("\n");
}

void Standalone_SS::create_modules(module_wrapper_factory& module_factory,
        std::vector<std::string>& incorrect_modules,
        std::unordered_map<std::string, double>* quantities,
        std::unordered_map<std::string, double>* module_output_map )
{
    // Make a vector to store the names of modules that are incompatible with adaptive step size integrators
    std::vector<std::string> adaptive_step_size_incompat;
    
    // Create the steady state modules, checking to make sure they are properly categorized and whether they are
    //  compatible with adaptive step size integrators
    if(verbose) print_msg("Creating the modules from the list and making sure the list only includes steady state modules... ");
    create_modules_from_names(
        steady_state_module_names,
        false,
        steady_state_modules,
        module_factory,
        quantities,
        module_output_map,
        incorrect_modules,
        adaptive_step_size_incompat,
        verbose,
        print_msg
    );
    if(verbose) print_msg("\n...done!\n\n");
    
    // Let the user know about adaptive step size compatibility
    if(adaptive_step_size_incompat.size() != 0) {
        adaptive_compatible = false;
        if(verbose) {
            print_msg("The following sub-modules are incompatible with adaptive step size integrators, so this standalone_ss is incompatible with adaptive step size integrators:\n");
            for(std::string s : adaptive_step_size_incompat) print_msg("%s\n", s.c_str());
            print_msg("\n");
        }
    }
    else {
        adaptive_compatible = true;
        if(verbose) print_msg("All sub-modules are compatible with adaptive step size integrators, so this standlone_ss is compatible with adaptive step size integrators.\n\n");
    }
}

void Standalone_SS::get_pointer_pairs(
    std::unordered_map<std::string, const double*> const& input_var_ptrs,
    std::unordered_map<std::string, double*> const& output_var_ptrs)
{
    // Lists for describing problems with the inputs
    std::vector<std::string> extra_input_names;                     // A list of supplied input parameter names that don't match the input parameter list
    std::vector<std::string> undefined_inputs;                      // A list of required input parameters that were not in the input parameter list
    std::vector<std::string> bad_output_names;                      // A list of supplied output parameter names that don't match the output parameter list
    
    // A string for describing problems with the inputs
    std::string error_string;                                       // A message to send to the user about any issues that were discovered during the system setup
    
    // Get pointers to the output variables in the main maps
    for(std::string p : unique_module_outputs) {
        std::pair<double*, double*> temp(&variables.at(p), &module_output_map.at(p));
        module_output_ptrs.push_back(temp);
    }
    
    // Create the input pointer map, which contains pairs of pointers
    // In each pair, the first points to an entry in the main variables map,
    //  while the second points to an input variable outside the object
    // If the input_var_ptrs map has the wrong size, throw a length_error.
    // This will allow R_get_standalone_ss_info to disregard the problem.
    if(input_var_ptrs.size() >= unique_module_inputs.size()) {
        // Go through the supplied input_var_ptrs map, checking for extraneous parameters and storing required pointers
        for(auto x : input_var_ptrs) {
            if(unique_module_inputs.find(x.first) == unique_module_inputs.end()) extra_input_names.push_back(x.first);  // This input parameter isn't actually required
            else {
                std::pair<double*, const double*> temp(&variables.at(x.first), x.second);
                input_ptrs.push_back(temp);
            }
        }
        // Check to see if any required inputs are missing
        for(std::string p : unique_module_inputs) {
            if(input_var_ptrs.find(p) == input_var_ptrs.end()) undefined_inputs.push_back(p);
        }
    }
    else throw std::length_error(std::string("Thrown by standalone_ss: the supplied input pointer map does not have enough parameters.\n"));
    
    // Let the user know about any extra input parameters
    // It may be helpful to inform the user about them, but they aren't
    //  serious enough to prevent the standalone_ss from operating
    if(extra_input_names.size() != 0 && verbose) {
        print_msg("The following input parameters were supplied but are not required by the modules:\n");
        for(std::string s : extra_input_names) print_msg("%s\n", s.c_str());
        print_msg("\n");
    }
    
    // Create the output pointer map, which contains pairs of pointers
    // In each pair, the first points to an entry in the module_output map,
    //  while the second points to an output variable outside the class
    if(output_var_ptrs.size() > 0) {
        for(auto x : output_var_ptrs) {
            if(unique_module_outputs.find(x.first) == unique_module_inputs.end()) bad_output_names.push_back(x.first);
            else {
                std::pair<double*, double*> temp(&module_output_map.at(x.first), x.second);
                output_ptrs.push_back(temp);
            }
        }
    }
    else error_string += "The supplied output pointer map is empty.\n";
    
    // Collect information about any errors that may have occurred while creating the pointer pairs
    process_errors(undefined_inputs, std::string("Some of the required input parameters were not provided by the input pointer map"), error_string, verbose, print_msg);
    process_errors(bad_output_names, std::string("Some of the supplied output parameters were not produced by the modules"), error_string, verbose, print_msg);
    
    // If any errors have occurred, notify the user and throw an error to halt the operation,
    //  since the user needs to address these problems before continuing
    report_errors(error_string, verbose);
}

void Standalone_SS::run() const {    
    // First get the input parameter values
    for(auto x : input_ptrs) *(x.first) = *(x.second);
    
    // Then clear the module output map
    for(auto x : module_output_ptrs) *x.second = 0.0;
    
    // Now go through the modules
    for(auto it = steady_state_modules.begin(); it != steady_state_modules.end(); ++it) {
        (*it)->run();                                               // Run the module
        for(auto x : module_output_ptrs) *(x.first) = *(x.second);  // Store its output in the main parameter map
    }
    
    // Finally, export the output parameter values
    for(auto x : output_ptrs) *(x.second) = *(x.first);
}
