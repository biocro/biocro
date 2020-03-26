#include <Rinternals.h>  // for Rprintf
#include <vector>
#include <string>
#include "R_helper_functions.h"
#include "validate_system.h"
#include "standalone_ss.h"

extern "C" {

SEXP R_get_standalone_ss_info(SEXP module_name_input)
{
    try {
        // module_name_input should be a string vector with one or more elements
        std::vector<std::string> module_name_vector = make_vector(module_name_input);

        std::string message = std::string("\nFinding all quantities required as inputs or produced as outputs by the modules:\n");

        // Get the required inputs
        string_set module_inputs = find_strictly_required_inputs(std::vector<string_vector>{module_name_vector});
        process_criterion<string_set>(
            message,
            [=]() -> string_set { return module_inputs; },
            [](string_set string_list) -> std::string { return create_message(
                                                            std::string("No quantities were required by the set of modules"),
                                                            std::string("The following quantities were required by the set of modules:"),
                                                            std::string(""),
                                                            string_list); });

        // Get the outputs
        string_set module_outputs = find_unique_module_outputs(std::vector<string_vector>{module_name_vector});
        process_criterion<string_set>(
            message,
            [=]() -> string_set { return module_outputs; },
            [](string_set string_list) -> std::string { return create_message(
                                                            std::string("No quantities were produced by any of the modules"),
                                                            std::string("The following quantities were each produced by at least one module:"),
                                                            std::string(""),
                                                            string_list); });

        // Make a bogus input/output pointer vectors
        double bogus_value;

        std::unordered_map<std::string, const double*> bogus_input_ptrs;
        for (std::string const& name : module_inputs) {
            bogus_input_ptrs[name] = &bogus_value;
        }

        std::unordered_map<std::string, double*> bogus_output_ptrs;
        for (std::string const& name : module_outputs) {
            bogus_output_ptrs[name] = &bogus_value;
        }

        // Use the bogus pointers to check the validity of the set of modules
        message += std::string("\nChecking the ability to create a Standalone_SS from these modules, assuming all inputs and outputs are properly supplied:\n");
        bool valid = validate_standalone_ss_inputs(message, module_name_vector, bogus_input_ptrs, bogus_output_ptrs);
        if (valid) {
            message += std::string("\nConclusion: The set of modules is valid and can be used to specify a Standalone_SS object.\n\n");
        } else {
            message += std::string("\n");
            Rprintf(message.c_str());
            throw std::logic_error(std::string("The set of modules is not valid and cannot be used to specify a Standalone_SS object."));
        }

        // Print the message
        Rprintf(message.c_str());

        // Return a map containing the module's input parameters
        return list_from_map(state_map_from_names(module_inputs));

    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_get_standalone_ss_info: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_get_standalone_ss_info.");
    }
}

SEXP R_test_standalone_ss(SEXP module_name_input, SEXP input_parameters, SEXP verbose)
{
    try {
        // module_name_input should be a string vector with one or more elements
        std::vector<std::string> module_name_vector = make_vector(module_name_input);

        // input_parameters should be a state map
        state_map i_parameters = map_from_list(input_parameters);

        // Get the verbosity
        bool loquacious = LOGICAL(VECTOR_ELT(verbose, 0))[0];

        // Get all the outputs from the modules in the list, initializing
        //  their values to -1000000.0, which should make it obvious if any are not
        //  updated by the standalone_ss object
        state_map o_parameters;
        for (size_t i = 0; i < module_name_vector.size(); i++) {
            auto w = module_wrapper_factory::create(module_name_vector[i]);
            std::vector<std::string> outputs = w->get_outputs();
            for (std::string p : outputs) o_parameters[p] = -1000000.0;
        }

        // Make input and output pointer maps and set their values
        std::unordered_map<std::string, const double*> input_param_ptrs;
        std::unordered_map<std::string, double*> output_param_ptrs;
        for (auto x : i_parameters) input_param_ptrs[x.first] = &i_parameters[x.first];
        for (auto x : o_parameters) {
            output_param_ptrs[x.first] = new double;
            *output_param_ptrs[x.first] = x.second;
        }

        // Make a standalone_ss object
        Standalone_SS module_combo(module_name_vector, input_param_ptrs, output_param_ptrs);

        if (loquacious) {
            std::string message = module_combo.generate_startup_report() + std::string("\n");
            Rprintf(message.c_str());
        }

        // Run the standalone_ss
        module_combo.run();

        // Update the outputs
        for (auto x : output_param_ptrs) o_parameters[x.first] = *(x.second);

        // Return a map containing the module's parameter list
        return list_from_map(o_parameters);

    } catch (std::exception const& e) {
        Rf_error((std::string("Caught exception in R_test_standalone_ss: ") + e.what()).c_str());
    } catch (...) {
        Rf_error("Caught unhandled exception in R_test_standalone_ss.");
    }
}

}  // extern "C"