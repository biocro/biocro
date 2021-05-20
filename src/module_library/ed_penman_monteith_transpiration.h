#ifndef ED_PENMAN_MONTEITH_TRANSPIRATION_H
#define ED_PENMAN_MONTEITH_TRANSPIRATION_H

#include <cmath>           // For pow
#include "../constants.h"  // for ideal_gas_constant and celsius_to_kelvin
#include "../modules.h"
#include "../state_map.h"

/**
 * @class ed_penman_monteith_transpiration
 *
 * @brief Uses the Penman-Monteith equation to determine leaf transpiration (see eq. 14.4k from Thornley (1990), p. 408).
 * Currently only intended for use by Ed.
 *
 * See the "ed_nikolov_conductance_forced" module for a discussion about converting molar conductances (with units
 * of mol / m^2 / s) to energy conductances (with units of m / s). In short, we replace all conductances g in the
 * Thornley formula with g * volume_per_mol_of_an_ideal_gas.
 */
class ed_penman_monteith_transpiration : public SteadyModule
{
   public:
    ed_penman_monteith_transpiration(
        state_map const* input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_penman_monteith_transpiration"),

          // Get pointers to input quantities
          long_wave_energy_loss_leaf(get_input(input_quantities, "long_wave_energy_loss_leaf")),
          solar_energy_absorbed_leaf(get_input(input_quantities, "solar_energy_absorbed_leaf")),
          slope_water_vapor(get_input(input_quantities, "slope_water_vapor")),
          psychrometric_parameter(get_input(input_quantities, "psychrometric_parameter")),
          latent_heat_vaporization_of_water(get_input(input_quantities, "latent_heat_vaporization_of_water")),
          vapor_density_deficit(get_input(input_quantities, "vapor_density_deficit")),
          conductance_boundary_h2o(get_input(input_quantities, "conductance_boundary_h2o")),
          conductance_stomatal_h2o(get_input(input_quantities, "conductance_stomatal_h2o")),
          temperature_air(get_input(input_quantities, "temp")),
          atmospheric_pressure(get_input(input_quantities, "atmospheric_pressure")),

          // Get pointers to output quantities
          transpiration_rate_op(get_op(output_quantities, "transpiration_rate"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // References to input quantities
    double const& long_wave_energy_loss_leaf;
    double const& solar_energy_absorbed_leaf;
    double const& slope_water_vapor;
    double const& psychrometric_parameter;
    double const& latent_heat_vaporization_of_water;
    double const& vapor_density_deficit;
    double const& conductance_boundary_h2o;
    double const& conductance_stomatal_h2o;
    double const& temperature_air;
    double const& atmospheric_pressure;

    // Pointers to output quantities
    double* transpiration_rate_op;

    // Main operation
    void do_operation() const override;
};

string_vector ed_penman_monteith_transpiration::get_inputs()
{
    return {
        "long_wave_energy_loss_leaf",         // W / m^2
        "solar_energy_absorbed_leaf",         // W / m^2
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

string_vector ed_penman_monteith_transpiration::get_outputs()
{
    return {
        "transpiration_rate"  // mol / m^2 / s
    };
}

void ed_penman_monteith_transpiration::do_operation() const
{
    // Determine the total energy available to the leaf
    const double total_available_energy = solar_energy_absorbed_leaf - long_wave_energy_loss_leaf;  // W / m^2

    // Convert temperatures to Kelvin
    const double Tak = temperature_air + conversion_constants::celsius_to_kelvin;  // Kelvin

    // Calculate the volume of one mole of a gas at air temperature and pressure
    // using the ideal gas law
    const double volume_per_mol = physical_constants::ideal_gas_constant * Tak / atmospheric_pressure;  // m^3 / mol

    // Convert conductances to m / s
    const double gc = conductance_stomatal_h2o * volume_per_mol;  // m / s
    const double ga = conductance_boundary_h2o * volume_per_mol;  // m / s

    // Calculate the transpiration rate
    // Some notes about units:
    //   1 W = 1 J / s
    //   1 J = 1 N * m
    //   1 N = 1 kg * m / s^2
    //   So 1 J = 1 kg * m^2 / s^2
    //   And 1 W = 1 kg * m^2 / s^3
    // Units for slope_water_vapor * total_available energy:
    //    kg / m^3 / degree K * W / m^2
    //  = kg / m^3 / degree K * kg * m^2 / s^3 / m^2
    //  = kg^2 / m^3 / s^3 / degree K
    // Units for latent_heat_vaporization_of_water * psychrometric_parameter * ga * vapor_density_deficit:
    //    J / kg * kg / m^3 / degree K * m / s * kg / m^3
    //  = kg * m^2 / s^2 / kg * kg / m^3 / degree K * m / s * kg / m^3
    //  = kg^2 / m^3 / s^3 / degree K
    // Units for latent_heat_vaporization_of_water * slope_water_vapor:
    //    J / kg * kg / m^3 / degree K
    //  = kg * m^2 / s^2 / kg * kg / m^3 / degree K
    //  = kg / m / s^2 / degree K
    // Units for psychrometric_parameter are same as slope_water_vapor
    // So units for E are kg / m^2 / s
    const double E_numerator = slope_water_vapor * total_available_energy +
                               latent_heat_vaporization_of_water * psychrometric_parameter * ga * vapor_density_deficit;
    const double E_denomenator = latent_heat_vaporization_of_water * (slope_water_vapor + psychrometric_parameter * (1 + ga / gc));
    const double E = E_numerator / E_denomenator;  // kg / m^2 / s

    // Convert E from kg / m^2 / s to mol / m^2 / s using:
    //  1e3 g / kg
    //  18 g / mol for water
    const double transpiration_rate = E * 1.0e3 / 18.0;

    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"ga cannot be zero",               fabs(ga) < calculation_constants::eps_zero},            // divide by zero
        {"gc cannot be zero",               fabs(gc) < calculation_constants::eps_zero},            // divide by zero
        {"E_denomenator cannot be zero",    fabs(E_denomenator) < calculation_constants::eps_zero}  // divide by zero
    };

    check_error_conditions(errors_to_check, get_name());

    update(transpiration_rate_op, transpiration_rate);
}

#endif
