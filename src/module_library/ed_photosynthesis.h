#ifndef ED_LEAF_PHOTOSYNTHESIS_H
#define ED_LEAF_PHOTOSYNTHESIS_H

#include "../modules.h"
#include "../validate_system.h"         // for find_strictly_required_inputs
#include "../simultaneous_equations.h"  // for get_unknown_quantities

namespace ed_leaf_photosynthesis_stuff
{
// Define a list of "typical" module names
string_vector typical_module_names =
{
        "ed_apply_stomatal_water_stress_via_conductance",
        "ed_gas_concentrations",
        "ed_ball_berry",
        "ed_collatz_c4_assimilation",
        "ed_long_wave_energy_loss",
        "water_vapor_properties_from_air_temperature",
        "ed_penman_monteith_leaf_temperature"
};

// Return all inputs required by the typical modules, excluding any unknowns
string_vector get_typical_inputs()
{
    string_set all_inputs_required_by_modules = find_strictly_required_inputs(std::vector<string_vector>{typical_module_names});
    string_vector unknown_quantities_from_modules = get_unknown_quantities(std::vector<string_vector>{typical_module_names});
    return string_vector_difference(string_set_to_string_vector(all_inputs_required_by_modules), unknown_quantities_from_modules);
}

// Return all outputs produced by the typical modules, including any unknowns
string_vector get_typical_outputs()
{
    return string_set_to_string_vector(find_unique_module_outputs(std::vector<string_vector>{typical_module_names}));
}

}  // namespace ed_leaf_photosynthesis_stuff

/**
 * @class ed_leaf_photosynthesis
 * 
 * @brief A general module representing photosynthesis calculations.
 * Note that this module cannot be created by the module_wrapper_factory
 * since its constructor has a different signature than a typical module.
 * Instead, it must have concrete derived classes. This class has been
 * made abstract to indicate this.
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
class ed_leaf_photosynthesis : public SteadyModule
{
   public:
    ed_leaf_photosynthesis(
        std::string module_name,
        std::string stomatal_water_stress_application_module,
        std::string gas_concentration_module,
        std::string stomatal_conductance_module,
        std::string assimilation_module,
        std::string long_wave_energy_loss_module,
        std::string water_vapor_properties_module,
        std::string leaf_temperature_module,
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters) :  // Define basic module properties by passing its name to its parent class
                                                                      SteadyModule(module_name)
    // Get pointers to input parameters
    // Get pointers to output parameters
    {
    }
    virtual ~ed_leaf_photosynthesis() = 0;

   private:
    // Pointers to input parameters
    // Pointers to output parameters
    // Main operation
    void do_operation() const override;
};

// A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header
inline ed_leaf_photosynthesis::~ed_leaf_photosynthesis() {}

void ed_leaf_photosynthesis::do_operation() const
{
    // Update the output parameter list
}

/**
 * @class ed_c4_leaf_photosynthesis
 * 
 * @brief Determines stomatal conductance, net assimilation, and leaf temperature.
 * Stomatal water stress is included by modifying stomatal conductance,
 * the Ball-Berry model is used to calculate stomatal conductance,
 * the Collatz model for C4 photosynthesis is used to calculate assimilation,
 * and the Penman-Monteith model is used to calculate leaf temperature.
 * The Newton-Raphson method is used to find self-consistent values for the
 * unknown quantities.
 */
class ed_c4_leaf_photosynthesis : public ed_leaf_photosynthesis
{
   public:
    ed_c4_leaf_photosynthesis(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters) : ed_leaf_photosynthesis("ed_c4_leaf_photosynthesis",
                                                                                             "ed_apply_stomatal_water_stress_via_conductance",
                                                                                             "ed_gas_concentrations",
                                                                                             "ed_ball_berry",
                                                                                             "ed_collatz_c4_assimilation",
                                                                                             "ed_long_wave_energy_loss",
                                                                                             "water_vapor_properties_from_air_temperature",
                                                                                             "ed_penman_monteith_leaf_temperature",
                                                                                             input_parameters,
                                                                                             output_parameters) {}
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();
};

// Get inputs from the typical modules
std::vector<std::string> ed_c4_leaf_photosynthesis::get_inputs()
{
    return ed_leaf_photosynthesis_stuff::get_typical_inputs();
}

// Get outputs from the typical modules
std::vector<std::string> ed_c4_leaf_photosynthesis::get_outputs()
{
    return ed_leaf_photosynthesis_stuff::get_typical_outputs();
}

#endif