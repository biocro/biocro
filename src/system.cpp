#include <algorithm>
#include "system.h"
#include "validate_system.h"

System::System(
    state_map const& init_state,
    state_map const& invariant_params,
    state_vector_map const& varying_params,
    std::vector<std::string> const& ss_module_names,
    std::vector<std::string> const& deriv_module_names) : initial_state(init_state),
                                                          invariant_parameters(invariant_params),
                                                          varying_parameters(varying_params),
                                                          steady_state_module_names(ss_module_names),
                                                          derivative_module_names(deriv_module_names)
{
    startup_message = std::string("");

    // Make sure the inputs can form a valid system
    bool valid = validate_system_inputs(startup_message, init_state, invariant_params, varying_params, ss_module_names, deriv_module_names);
    if (!valid) {
        throw std::logic_error("Thrown by System::System: the supplied inputs cannot form a valid system.\n\n" + startup_message);
    }

    // Make the central list of quantities and the module output map
    quantities = define_quantity_map(
        std::vector<state_map>{init_state, invariant_params, at(varying_params, 0)},
        std::vector<string_vector>{ss_module_names});
    module_output_map = quantities;

    // Instantiate the modules
    steady_state_modules = get_module_vector(std::vector<string_vector>{ss_module_names}, &quantities, &module_output_map);
    derivative_modules = get_module_vector(std::vector<string_vector>{deriv_module_names}, &quantities, &module_output_map);

    // Check for adaptive compatibility
    adaptive_compatible = true;
    for (std::unique_ptr<Module>& m : steady_state_modules) {
        if (!m->is_adaptive_compatible()) {
            adaptive_compatible = false;
        }
    }
    for (std::unique_ptr<Module>& m : derivative_modules) {
        if (!m->is_adaptive_compatible()) {
            adaptive_compatible = false;
        }
    }

    // Get lists of subsets of quantity names
    std::vector<std::string> steady_state_output_names;
    for (auto const& m : ss_module_names) {
        auto w = module_wrapper_factory::create(m);
        auto names = w->get_outputs();
        steady_state_output_names.insert(steady_state_output_names.begin(), names.begin(), names.end());
    }

    auto istate_names = keys(init_state);
    auto ip_names = keys(invariant_params);
    auto vp_names = keys(varying_params);

    // Get a pointer to the timestep
    timestep_ptr = &quantities.at("timestep");

    // Get pairs of pointers to important subsets of the variables
    // These pairs allow us to efficiently retrieve the output of each
    //  module and store it in the main variable map when running the system,
    //  to update the varying parameters at new time points, etc

    // Get pointers to the state variables in the parameter and module output maps
    for (auto const& x : init_state) {
        std::pair<double*, double*> temp(&quantities.at(x.first), &module_output_map.at(x.first));
        state_ptrs.push_back(temp);
    }

    // Get pointers to the steady state parameters in the parameter and module output maps
    for (std::string const& p : steady_state_output_names) {
        std::pair<double*, double*> temp(&quantities.at(p), &module_output_map.at(p));
        steady_state_ptrs.push_back(temp);
    }

    // Get pointers to the varying parameters in the parameter and varying parameter maps
    for (auto const& x : varying_parameters) {
        std::pair<double*, std::vector<double>*> temp(&quantities.at(x.first), &varying_parameters.at(x.first));
        varying_ptrs.push_back(temp);
    }

    // Fill in the initial values and test the modules
    startup_message += std::string("\nTrying to run all the modules... ");

    try {
        test_steady_state_modules();
    } catch (const std::exception& e) {
        startup_message += e.what();
    }

    // Test the derivative modules
    std::vector<double> temp_vec(state_ptrs.size());
    try {
        test_derivative_modules(temp_vec);
    } catch (const std::exception& e) {
        startup_message += e.what();
    }

    startup_message += std::string("done!");

    // Get the number of time points
    auto vp = varying_params.begin();
    ntimes = (vp->second).size();

    // Create a vector of the names of variables that change throughout a simulation
    for (auto const& names : std::vector<std::vector<std::string>>{istate_names, vp_names, steady_state_output_names}) {
        output_param_vector.insert(output_param_vector.begin(), names.begin(), names.end());
    }

    // Create a vector of pointers to the variables that change throughout a simulation
    output_ptr_vector.resize(output_param_vector.size());
    for (size_t i = 0; i < output_param_vector.size(); i++) output_ptr_vector[i] = &quantities.at(output_param_vector[i]);

    // Reset the derivative evaluation counter
    ncalls = 0;
}

void System::reset()
{
    // Put all parameters back to their original values
    int t = 0;
    update_varying_params(t);
    for (auto const& x : initial_state) quantities[x.first] = x.second;
    run_steady_state_modules();
}

// For integer time
void System::update_varying_params(int time_indx)
{
    for (auto const& x : varying_ptrs) *(x.first) = (*(x.second))[time_indx];
}

// For size_t time
void System::update_varying_params(size_t time_indx)
{
    for (auto x : varying_ptrs) *(x.first) = (*(x.second))[time_indx];
}

// For double time
void System::update_varying_params(double time_indx)
{
    // Find the two closest integers
    int t1 = (int)(time_indx + 0.5);
    int t2 = (t1 > time_indx) ? (t1 - 1) : (t1 + 1);
    // Make a linear interpolation
    for (auto const& x : varying_ptrs) {
        *(x.first) = (*(x.second))[t1] + (time_indx - t1) * ((*(x.second))[t2] - (*(x.second))[t1]) / (t2 - t1);
    }
}

void System::run_steady_state_modules()
{
    for (auto const& x : steady_state_ptrs) {
        *x.second = 0.0;  // Clear the module output map
    }
    for (auto it = steady_state_modules.begin(); it != steady_state_modules.end(); ++it) {
        (*it)->run();  // Run the module
        for (auto const& x : steady_state_ptrs) {
            *x.first = *x.second;  // Store its output in the main parameter map
        }
    }
}

void System::test_steady_state_modules()
{
    // Identical to run_steady_state_modules except for a try-catch block
    for (auto const& x : steady_state_ptrs) *x.second = 0.0;
    for (auto it = steady_state_modules.begin(); it != steady_state_modules.end(); ++it) {
        try {
            (*it)->run();
        } catch (const std::exception& e) {
            throw std::logic_error(std::string("Steady state module '") + (*it)->get_name() + std::string("' generated an exception while calculating steady state parameters: ") + e.what() + std::string("\n"));
        }
        for (auto const& x : steady_state_ptrs) *x.first = *x.second;
    }
}
