#include "system.h"
#include "validate_system.h"
#include "utils/module_dependency_utilities.h" // for get_evaluation_order

System::System(
    state_map const& init_state,
    state_map const& invariant_params,
    state_vector_map const& varying_params,
    string_vector const& ss_module_names,
    string_vector const& deriv_module_names)
    : initial_state{init_state},
      invariant_parameters{invariant_params},
      varying_parameters{varying_params},
      steady_state_module_names{}, // put modules in suitable order before filling
      derivative_module_names{deriv_module_names}
{
    startup_message = std::string("");

    // Make sure the inputs can form a valid system
    bool valid = validate_system_inputs(startup_message, init_state, invariant_params, varying_params, ss_module_names, deriv_module_names);

    if (!valid) {
        throw std::logic_error("Thrown by System::System: the supplied inputs cannot form a valid system.\n\n" + startup_message);
    }

    try {
        steady_state_module_names = get_evaluation_order(ss_module_names);
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::not_a_dag>> e) {
        throw std::logic_error("Cyclic dependencies should be caught in the validation routine.  We shouldn't ever get here.");
    }

    // Make the central list of quantities and the module output map
    quantities = define_quantity_map(
        std::vector<state_map>{init_state, invariant_params, at(varying_params, 0)},
        std::vector<string_vector>{steady_state_module_names});
    module_output_map = quantities;

    // Instantiate the modules
    steady_state_modules = get_module_vector(std::vector<string_vector>{steady_state_module_names}, &quantities, &module_output_map);
    derivative_modules = get_module_vector(std::vector<string_vector>{deriv_module_names}, &quantities, &module_output_map);

    // Make lists of subsets of quantity names
    string_vector steady_state_output_names =
        string_set_to_string_vector(
            find_unique_module_outputs({steady_state_module_names})
        );
    string_vector istate_names = keys(init_state);
    string_vector vp_names = keys(varying_params);

    // Get vectors of pointers to important subsets of the quantities
    // These pointers allow us to efficiently reset portions of the
    //  module output map before running the modules
    state_ptrs = get_pointers(istate_names, module_output_map);
    steady_state_ptrs = get_pointers(steady_state_output_names, module_output_map);

    // Get pairs of pointers to important subsets of the quantities
    // These pairs allow us to efficiently retrieve the output of each
    // module and store it in the main quantity map when running the
    // system, to update the varying parameters at new time points,
    // etc.
    state_ptr_pairs = get_pointer_pairs(istate_names, quantities, module_output_map);
    steady_state_ptr_pairs = get_pointer_pairs(steady_state_output_names, quantities, module_output_map);
    varying_ptr_pairs = get_pointer_pairs(vp_names, quantities, varying_parameters);

    // Get a pointer to the timestep
    timestep_ptr = &(quantities.at("timestep"));
}

/**
 * @brief Resets all quantities back to their original values
 */
void System::reset()
{
    update_varying_params(size_t(0));  // t = 0
    for (auto const& x : initial_state) quantities[x.first] = x.second;
    run_steady_state_modules();
}

/**
 * @brief Gets values from the varying parameters at the input time (double)
 */
void System::update_varying_params(double time_indx)
{
    // Find two closest surrounding integers:
    int t1 = std::floor(time_indx);
    int t2 = t1 + 1; // note t2 - t1 = 1
    for (const auto& x : varying_ptr_pairs) {
        // Use linear interpolation to find value at time_indx:
        auto value_at_t1 = (*(x.second))[t1];
        auto value_at_t2 = (*(x.second))[t2];
        auto value_at_time_indx = value_at_t1 +
            (time_indx - t1) * (value_at_t2 - value_at_t1);

        *(x.first) = value_at_time_indx;
    }
}

/**
 * @brief Runs all the steady state modules
 */
void System::run_steady_state_modules()
{
    // Clear the module output map
    for (double* const& x : steady_state_ptrs) {
        *x = 0.0;
    }

    // Run each module and store its output in the main quantity map
    for (std::unique_ptr<Module> const& m : steady_state_modules) {
        m->run();
        for (auto const& x : steady_state_ptr_pairs) {
            *x.first = *x.second;
        }
    }
}

/**
 * @brief Returns pointers that can be used to access quantity values from the system's central
 * map of quantities
 */
std::vector<const double*> System::get_quantity_access_ptrs(string_vector quantity_names) const
{
    std::vector<const double*> access_ptrs;
    for (const std::string& name : quantity_names) {
        access_ptrs.push_back(&quantities.at(name));
    }
    return access_ptrs;
}

/**
 * @brief Returns a vector of the names of all quantities that change
 *        throughout a simulation.
 *
 * The quantities that change are (1) the quantites that are
 * calculated using differential equations; these should coincide with
 * all quantities in the initial state; (2) the varying known
 * parameters; (3) the _secondary_ variables, that is, the variables
 * that are outputs of steady-state modules.
 *
 * @note Even though each variable in the initial state should
 * correspond to a derivative calculated by some derivative module, it
 * is possible and permissible to have some variable \f$v\f$ in the
 * initial state that doesn't correspond to any such derivative.  In
 * that case, a differential equation \f$dv/dt = 0\f$ is assumed.  So
 * such variables in fact _do not_ change during the course of the
 * simulation.
 */
string_vector System::get_output_param_names() const
{
    return get_defined_quantity_names(
        std::vector<state_map>{initial_state, at(varying_parameters, 0)},
        std::vector<string_vector>{steady_state_module_names});
}
