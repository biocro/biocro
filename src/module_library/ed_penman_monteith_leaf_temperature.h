#ifndef ED_PENMAN_MONTEITH_LEAF_TEMPERATURE_H
#define ED_PENMAN_MONTEITH_LEAF_TEMPERATURE_H

#include <cmath>           // For pow
#include "../constants.h"  // for ideal_gas_constant and celsius_to_kelvin
#include "../modules.h"

/**
 * @class ed_penman_monteith_leaf_temperature
 * 
 * @brief Uses the Penman-Monteith equation to determine leaf temperature (see eq. 14.11e from Thornley (1990), p. 418).
 * Currently only intended for use by Ed.
 * 
 * See the "ed_nikolov_conductance_forced" module for a discussion about converting molar conductances (with units
 * of mol / m^2 / s) to energy conductances (with units of m / s). In short, we replace all conductances g in the
 * Thornley formula with g * volume_per_mol_of_an_ideal_gas.
 */
class ed_penman_monteith_leaf_temperature : public SteadyModule
{
   public:
    ed_penman_monteith_leaf_temperature(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_penman_monteith_leaf_temperature"),
          // Get pointers to input parameters
          long_wave_energy_loss_leaf_ip(get_ip(input_parameters, "long_wave_energy_loss_leaf")),
          solar_energy_absorbed_leaf_ip(get_ip(input_parameters, "solar_energy_absorbed_leaf")),
          slope_water_vapor_ip(get_ip(input_parameters, "slope_water_vapor")),
          psychrometric_parameter_ip(get_ip(input_parameters, "psychrometric_parameter")),
          latent_heat_vaporization_of_water_ip(get_ip(input_parameters, "latent_heat_vaporization_of_water")),
          vapor_density_deficit_ip(get_ip(input_parameters, "vapor_density_deficit")),
          conductance_boundary_h2o_ip(get_ip(input_parameters, "conductance_boundary_h2o")),
          conductance_stomatal_h2o_ip(get_ip(input_parameters, "conductance_stomatal_h2o")),
          temperature_air_ip(get_ip(input_parameters, "temp")),
          atmospheric_pressure_ip(get_ip(input_parameters, "atmospheric_pressure")),
          // Get pointers to output parameters
          temperature_leaf_op(get_op(output_parameters, "temperature_leaf"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* long_wave_energy_loss_leaf_ip;
    const double* solar_energy_absorbed_leaf_ip;
    const double* slope_water_vapor_ip;
    const double* psychrometric_parameter_ip;
    const double* latent_heat_vaporization_of_water_ip;
    const double* vapor_density_deficit_ip;
    const double* conductance_boundary_h2o_ip;
    const double* conductance_stomatal_h2o_ip;
    const double* temperature_air_ip;
    const double* atmospheric_pressure_ip;
    // Pointers to output parameters
    double* temperature_leaf_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_penman_monteith_leaf_temperature::get_inputs()
{
    return {
        "long_wave_energy_loss_leaf",         // W / m^2
        "solar_energy_absorbed_leaf",         // W / m^2 [should include losses to soil / lower canopy layers]
        "slope_water_vapor",                  // kg / m^3 / deg. K
        "psychrometric_parameter",            // kg / m^3 / deg. K
        "latent_heat_vaporization_of_water",  // J / kg
        "vapor_density_deficit",              // kg / m^3
        "conductance_boundary_h2o",           // mol / m^2 / s
        "conductance_stomatal_h2o",           // mol / m^2 / s
        "temp",                               // deg. C
        "atmospheric_pressure"                // Pa
    };
}

std::vector<std::string> ed_penman_monteith_leaf_temperature::get_outputs()
{
    return {
        "temperature_leaf"  // deg. C
    };
}

void ed_penman_monteith_leaf_temperature::do_operation() const
{
    // Determine the total energy available to the leaf
    const double total_available_energy = *solar_energy_absorbed_leaf_ip - *long_wave_energy_loss_leaf_ip;  // W / m^2

    // Convert temperatures to Kelvin
    const double Tak = *temperature_air_ip + physical_constants::celsius_to_kelvin;  // Kelvin

    // Calculate the volume of one mole of a gas at air temperature and pressure
    // using the ideal gas law
    const double volume_per_mol = physical_constants::ideal_gas_constant * Tak / *atmospheric_pressure_ip;  // m^3 / mol

    // Convert conductances to m / s
    const double gc = *conductance_stomatal_h2o_ip * volume_per_mol;  // m / s
    const double ga = *conductance_boundary_h2o_ip * volume_per_mol;  // m / s

    const double delta_t_numerator = total_available_energy * (1 / ga + 1 / gc) - *latent_heat_vaporization_of_water_ip * *vapor_density_deficit_ip;
    const double delta_t_denomenator = *latent_heat_vaporization_of_water_ip * (*slope_water_vapor_ip + *psychrometric_parameter_ip * (1 + ga / gc));
    const double delta_t = delta_t_numerator / delta_t_denomenator;

    const double temperature_leaf = *temperature_air_ip + delta_t;

    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"ga cannot be zero",                   ga == 0},                   // divide by zero
        {"gc cannot be zero",                   gc == 0},                   // divide by zero
        {"delta_t_denomenator cannot be zero",  delta_t_denomenator == 0}   // divide by zero
    };

    check_error_conditions(errors_to_check, get_name());

    update(temperature_leaf_op, temperature_leaf);
}

#endif
