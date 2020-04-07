#ifndef ED_LEAF_PHOTOSYNTHESIS_H
#define ED_LEAF_PHOTOSYNTHESIS_H

#include "../validate_system.h"         // for find_strictly_required_inputs and all_are_in_list
#include "../simultaneous_equations.h"  // for get_unknown_quantities
#include "../state_map.h"               // for keys
#include "../modules.h"
#include "../module_library/module_wrapper_factory.h"
#include "../se_solver_library/se_solver_factory.h"

namespace ed_leaf_photosynthesis
{
/** Define a list of reference module names */
const string_vector reference_module_names = {
    "ed_apply_stomatal_water_stress_via_conductance",
    "ed_gas_concentrations",
    "ed_ball_berry",
    "ed_collatz_c4_assimilation",
    "ed_long_wave_energy_loss",
    "water_vapor_properties_from_air_temperature",
    "ed_penman_monteith_leaf_temperature"};

/**
 * @brief A function that finds all inputs required by the reference modules, excluding any unknowns.
 * Here it is important to sort the results so the order is correct when using
 * simultaneous_equations::update_known_quantities.
 */
string_vector get_reference_inputs()
{
    string_set all_reference_inputs = find_strictly_required_inputs(std::vector<string_vector>{reference_module_names});

    string_vector unknown_quantities = get_unknown_quantities(std::vector<string_vector>{reference_module_names});

    string_vector result = string_vector_difference(
        string_set_to_string_vector(all_reference_inputs),
        unknown_quantities);

    std::sort(result.begin(), result.end());

    return result;
}

/**
 * @brief A function that finds all outputs produced by the reference modules.
 * Here it is important to sort the results so the order is correct when using
 * simultaneous_equations::update_known_quantities. This is accomplished automatically
 * because the `find_unique_module_outputs` returns a string_set (which is sorted).
 */
string_vector get_reference_outputs()
{
    return string_set_to_string_vector(find_unique_module_outputs(std::vector<string_vector>{reference_module_names}));
}

/**
 * @brief Check the the test module to make sure it has the same inputs and outputs as the reference module.
 * This function throws an error if there is a problem.
 */
void check_module_io_equivalence(std::string reference_module, std::string module_to_test)
{
    // Make a lambda to simplify the checks. check_type should be "inputs" or "outputs".
    auto check_equiv =
        [&reference_module, &module_to_test](string_vector const& ref_sv, string_vector const& test_sv, std::string check_type) -> void {
        if (ref_sv.size() != test_sv.size() || !all_are_in_list(ref_sv, test_sv)) {
            throw std::logic_error(
                std::string("The '") +
                module_to_test +
                std::string("' module does not have the same ") +
                check_type +
                std::string(" as the reference model ('") +
                reference_module +
                std::string("'"));
        }
    };

    // Get the module wrappers
    auto w_ref = module_wrapper_factory::create(reference_module);
    auto w_test = module_wrapper_factory::create(module_to_test);

    // Perform the tests
    check_equiv(w_ref->get_inputs(), w_test->get_inputs(), "inputs");
    check_equiv(w_ref->get_outputs(), w_test->get_outputs(), "outputs");
}

/**
 * @brief Checks to make sure the input modules are valid.
 * The modules are valid if and only if each module has the same inputs and outputs as the
 * corresponding reference module. This function throws an error if there is a problem.
 */
bool validate_modules(string_vector input_module_names)
{
    if (input_module_names.size() != reference_module_names.size()) {
        throw std::logic_error("Thrown by ed_leaf_photosynthesis::validate_modules: wrong number of input modules.");
    }

    for (size_t i = 0; i < input_module_names.size(); ++i) {
        try {
            check_module_io_equivalence(reference_module_names[i], input_module_names[i]);
        } catch (std::exception const& e) {
            throw std::logic_error(std::string("Thrown by ed_leaf_photosynthesis::validate_modules: ") + e.what());
        }
    }

    return true;
}

/**
 * @class module_base
 * 
 * @brief A general module representing photosynthesis calculations.
 * Note that this module cannot be created by the module_wrapper_factory
 * since its constructor has a different signature than a typical module.
 * Instead, it must have concrete derived classes. This class has been
 * made abstract to indicate this. Note also that this module does not
 * have a do_operation method defined.
 * 
 * @param[in] stomatal_water_stress_application_module
 *            the name of a module with the same input and output quantities
 *            as `ed_apply_stomatal_water_stress_via_conductance`
 * 
 * @param[in] gas_concentration_module
 *            the name of a module with the same input and output quantities
 *            as `ed_gas_concentrations`
 * 
 * @param[in] stomatal_conductance_module
 *            the name of a module with the same input and output quantities
 *            as `ed_ball_berry`
 * 
 * @param[in] assimilation_module
 *            the name of a module with the same input and output quantities
 *            as `ed_collatz_c4_assimilation`
 * 
 * @param[in] long_wave_energy_loss_module
 *            the name of a module with the same input and output quantities
 *            as `ed_long_wave_energy_loss`
 * 
 * @param[in] water_vapor_properties_module
 *            the name of a module with the same input and output quantities
 *            as `water_vapor_properties_from_air_temperature`
 * 
 * @param[in] leaf_temperature_module
 *            the name of a module with the same input and output quantities
 *            as `ed_penman_monteith_leaf_temperature`
 */
class module_base : public SteadyModule
{
   public:
    module_base(std::string module_name,
                string_vector input_module_names,
                const state_map* input_parameters,
                state_map* output_parameters)
        : SteadyModule(module_name),
          validated(validate_modules(input_module_names)),
          input_ptrs(get_ip(input_parameters, get_reference_inputs())),
          output_ptrs(get_op(output_parameters, get_reference_outputs()))
    {
    }
    virtual ~module_base() = 0;

   private:
    bool validated;

   protected:
    // Pointers to input parameters
    std::vector<const double*> input_ptrs;
    // Pointers to output parameters
    std::vector<double*> output_ptrs;
};

/** A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header */
inline module_base::~module_base() {}

}  // namespace ed_leaf_photosynthesis

#endif
