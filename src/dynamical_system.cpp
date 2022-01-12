#include "dynamical_system.h"
#include "validate_dynamical_system.h"
#include "utils/module_dependency_utilities.h"  // for get_evaluation_order

dynamical_system::dynamical_system(
    state_map const& init_values,
    state_map const& params,
    state_vector_map const& drivers,
    mc_vector const& dir_mcs,
    mc_vector const& differential_mcs)
    : initial_values{init_values},
      parameters{params},
      drivers{drivers},
      direct_mcs{},  // put modules in suitable order before filling
      differential_mcs{differential_mcs}
{
    startup_message = string("");

    // Make sure the inputs can form a valid system
    bool valid = validate_dynamical_system_inputs(
        startup_message,
        init_values,
        params,
        drivers,
        dir_mcs,
        differential_mcs);

    if (!valid) {
        throw std::logic_error(
            string("Thrown by dynamical_system::dynamical_system: the ") +
            string("supplied inputs cannot form a valid dynamical system\n\n") +
            startup_message);
    }

    try {
        direct_mcs = get_evaluation_order(dir_mcs);
    } catch (boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::not_a_dag>> const& e) {
        throw std::logic_error(
            string("Cyclic dependencies should be caught in the validation ") +
            string("routine.  We shouldn't ever get here."));
    }

    // Make the central list of quantities
    all_quantities = define_quantity_map(
        vector<state_map>{init_values, params, at(drivers, 0)},
        direct_mcs);

    // Make a map to store the output of differential modules (i.e., derivatives
    // of the differential quantities), which should correspond to the
    // quantities in the "initial values" input.
    differential_quantity_derivatives = init_values;

    // Instantiate the modules. Differential modules should not modify the main
    // quantity map since their output represents derivatives of quantity values
    // rather than actual quantity values, but direct modules should
    // directly modify the main output map.
    direct_modules = get_module_vector(
        direct_mcs,
        all_quantities,
        &all_quantities);

    differential_modules = get_module_vector(
        differential_mcs,
        all_quantities,
        &differential_quantity_derivatives);

    // Make lists of subsets of the quantities that comprise the state:
    // - the direct quantities, i.e., the quantities whose instantaneous values
    //   are calculated by direct modules
    // - the differential quantities, i.e., the quantities whose derivatives are
    //   calculated by differential modules
    // - the drivers
    string_vector direct_quantity_names =
        string_set_to_string_vector(
            find_unique_module_outputs(direct_mcs));

    string_vector differential_quantity_names = keys(init_values);
    string_vector driver_quantity_names = keys(drivers);

    // Get vectors of "pointer pairs," i.e., a std::pair of pointers that point
    // to the same quantity in different `state_map` objects. These pairs allow
    // us to update the central quantity map when the differential modules are
    // run or when the driver values are updated, without needing to search
    // through the map keys.
    differential_quantity_ptr_pairs = get_pointer_pairs(
        differential_quantity_names,
        all_quantities,
        differential_quantity_derivatives);

    driver_quantity_ptr_pairs = get_pointer_pairs(
        driver_quantity_names,
        all_quantities,
        drivers);

    // Get a pointer to the timestep
    if (params.find("timestep") == params.end()) {
        throw std::runtime_error(
            string("The quantity 'timestep' was not defined in the ") +
            string("parameters state_map."));
    }
    timestep_ptr = &(all_quantities.at("timestep"));
}

/**
 *  @brief Resets all internally stored quantities back to their original values
 */
void dynamical_system::reset()
{
    update_drivers(size_t(0));  // t = 0
    for (auto const& x : initial_values) all_quantities[x.first] = x.second;
    run_module_list(direct_modules);
}

/**
 *  @brief Updates values of the drivers in the internally stored quantity map
 *         to match their values in the internally stored drivers table at time
 *         `time_index`; values of the drivers at non-integer values of
 *         `time_index` are determined using linear interpolation.
 */
void dynamical_system::update_drivers(double time_indx)
{
    // Find two closest surrounding integers:
    int t1 = std::floor(time_indx);
    int t2 = t1 + 1;  // note t2 - t1 = 1
    for (const auto& x : driver_quantity_ptr_pairs) {
        // Use linear interpolation to find value at time_indx:
        auto value_at_t1 = (*(x.second))[t1];
        auto value_at_t2 = (*(x.second))[t2];
        auto value_at_time_indx =
            value_at_t1 + (time_indx - t1) * (value_at_t2 - value_at_t1);

        *(x.first) = value_at_time_indx;
    }
}

/**
 *  @brief Returns pointers that can be used to access quantity values from the
 *  dynamical_system's central map of quantities
 */
vector<const double*> dynamical_system::get_quantity_access_ptrs(string_vector quantity_names) const
{
    vector<const double*> access_ptrs;
    for (const string& name : quantity_names) {
        access_ptrs.push_back(&all_quantities.at(name));
    }
    return access_ptrs;
}

/**
 *  @brief Returns a vector of the names of all quantities that may change
 *         throughout a simulation
 *
 *  The quantities that change are:
 *
 *  - quantities whose derivatives are calculated by differential modules, i.e.,
 *    the differential quantities
 *
 *  - quantities whose instantaneous values are calculated by direct modules,
 *    i.e., the direct quantities
 *
 *  - the drivers
 *
 *  @note Even though each variable in the initial values should correspond to a
 *  derivative calculated by some differential module, it is possible and
 *  permissible to have some quantity \f$v\f$ in the initial values that doesn't
 *  correspond to any such derivative. In this case, a differential equation
 *  \f$dv/dt = 0\f$ is assumed. So such variables in fact _do not_ change during
 *  the course of the simulation.
 */
string_vector dynamical_system::get_output_quantity_names() const
{
    return get_defined_quantity_names(
        vector<state_map>{initial_values, at(drivers, 0)},
        direct_mcs);
}
