#include "system.h"
#include "validate_system.h"
#include "utils/module_dependency_utilities.h"  // for get_evaluation_order

System::System(
    state_map const& init_values,
    state_map const& params,
    state_vector_map const& drivers,
    string_vector const& dir_module_names,
    string_vector const& differential_module_names)
    : initial_values{init_values},
      parameters{params},
      drivers{drivers},
      direct_module_names{},  // put modules in suitable order before filling
      differential_module_names{differential_module_names}
{
    startup_message = std::string("");

    // Make sure the inputs can form a valid system
    bool valid = validate_system_inputs(startup_message, init_values, params, drivers, dir_module_names, differential_module_names);

    if (!valid) {
        throw std::logic_error("Thrown by System::System: the supplied inputs cannot form a valid system.\n\n" + startup_message);
    }

    try {
        direct_module_names = get_evaluation_order(dir_module_names);
    } catch (boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::not_a_dag>> e) {
        throw std::logic_error("Cyclic dependencies should be caught in the validation routine.  We shouldn't ever get here.");
    }

    // Make the central list of quantities
    quantities = define_quantity_map(
        std::vector<state_map>{init_values, params, at(drivers, 0)},
        std::vector<string_vector>{direct_module_names});

    // Make a map to store the output of differential modules, which can only
    // include quantities in the initial values
    differential_module_outputs = init_values;

    // Instantiate the modules. Differential modules should not modify the main
    // quantity map since their output represents derivatives of quantity values
    // rather than actual quantity values, but direct modules should
    // directly modify the main output map.
    direct_modules = get_module_vector(std::vector<string_vector>{direct_module_names}, quantities, &quantities);
    differential_modules = get_module_vector(std::vector<string_vector>{differential_module_names}, quantities, &differential_module_outputs);

    // Make lists of subsets of quantity names
    string_vector direct_output_names =
        string_set_to_string_vector(
            find_unique_module_outputs({direct_module_names}));
    string_vector istate_names = keys(init_values);
    string_vector driver_names = keys(drivers);

    // Get vectors of pointers to important subsets of the quantities
    // These pointers allow us to efficiently reset portions of the
    //  module output map before running the modules
    state_ptrs = get_pointers(istate_names, differential_module_outputs);

    // Get pairs of pointers to important subsets of the quantities
    // These pairs allow us to efficiently retrieve the output of each
    // module and store it in the main quantity map when running the
    // system, to update the drivers at new time points,
    // etc.
    state_ptr_pairs = get_pointer_pairs(istate_names, quantities, differential_module_outputs);
    driver_ptr_pairs = get_pointer_pairs(driver_names, quantities, drivers);

    // Get a pointer to the timestep
    if (params.find("timestep") == params.end()) {
        throw std::runtime_error("The quantity 'timestep' was not defined in the parameters state_map.");
    }
    timestep_ptr = &(quantities.at("timestep"));
}

/**
 * @brief Resets all quantities back to their original values
 */
void System::reset()
{
    update_drivers(size_t(0));  // t = 0
    for (auto const& x : initial_values) quantities[x.first] = x.second;
    run_module_list(direct_modules);
}

/**
 * @brief Gets values from the drivers at the input time (double)
 */
void System::update_drivers(double time_indx)
{
    // Find two closest surrounding integers:
    int t1 = std::floor(time_indx);
    int t2 = t1 + 1;  // note t2 - t1 = 1
    for (const auto& x : driver_ptr_pairs) {
        // Use linear interpolation to find value at time_indx:
        auto value_at_t1 = (*(x.second))[t1];
        auto value_at_t2 = (*(x.second))[t2];
        auto value_at_time_indx = value_at_t1 +
                                  (time_indx - t1) * (value_at_t2 - value_at_t1);

        *(x.first) = value_at_time_indx;
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
 * all quantities in the initial values; (2) the drivers; (3) the
 * _direct_ variables, that is, the variables that are outputs of
 * direct modules.
 *
 * @note Even though each variable in the initial values should
 * correspond to a derivative calculated by some differential module, it
 * is possible and permissible to have some variable \f$v\f$ in the
 * initial values that doesn't correspond to any such derivative.  In
 * that case, a differential equation \f$dv/dt = 0\f$ is assumed.  So
 * such variables in fact _do not_ change during the course of the
 * simulation.
 */
string_vector System::get_output_param_names() const
{
    return get_defined_quantity_names(
        std::vector<state_map>{initial_values, at(drivers, 0)},
        std::vector<string_vector>{direct_module_names});
}
