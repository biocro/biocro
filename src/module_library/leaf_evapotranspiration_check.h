#ifndef LEAF_EVAPOTRANSPIRATION_CHECK_H
#define LEAF_EVAPOTRANSPIRATION_CHECK_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "water_and_air_properties.h"  // for TempToCp, dry_air_density, etc
#include "leaf_energy_balance.h"

namespace standardBML
{
/**
 *  @class leaf_evapotranspiration_check
 *
 *  @brief Uses `check_leaf_temp()` to calculate a temperature difference.
 *
 *  This module is not intended for use in crop models. Rather, it enables
 *  investigations into the leaf energy balance equations. To solve them, a
 *  value of `leaf_temperature` must be found such that `leaf_temp_check` is
 *  zero. See `leaf_energy_balance()` for more information.
 */
class leaf_evapotranspiration_check : public direct_module
{
   public:
    leaf_evapotranspiration_check(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          absorbed_shortwave{get_input(input_quantities, "absorbed_shortwave")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          canopy_height{get_input(input_quantities, "canopy_height")},
          Gs{get_input(input_quantities, "Gs")},
          leaf_temperature{get_input(input_quantities, "leaf_temperature")},
          leafwidth{get_input(input_quantities, "leafwidth")},
          min_gbw_canopy{get_input(input_quantities, "min_gbw_canopy")},
          rh{get_input(input_quantities, "rh")},
          temp{get_input(input_quantities, "temp")},
          wind_speed_height{get_input(input_quantities, "wind_speed_height")},
          windspeed{get_input(input_quantities, "windspeed")},

          // Get pointers to output quantities
          leaf_temp_check_op{get_op(output_quantities, "leaf_temp_check")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "leaf_evapotranspiration_check"; }

   private:
    // Pointers to input quantities
    double const& absorbed_shortwave;
    double const& atmospheric_pressure;
    double const& canopy_height;
    double const& Gs;
    double const& leaf_temperature;
    double const& leafwidth;
    double const& min_gbw_canopy;
    double const& rh;
    double const& temp;
    double const& wind_speed_height;
    double const& windspeed;

    // Pointers to output quantities
    double* leaf_temp_check_op;

    // Main operation
    void do_operation() const;
};

string_vector leaf_evapotranspiration_check::get_inputs()
{
    return {
        "absorbed_shortwave",    // J / m^2 / s
        "atmospheric_pressure",  // Pa
        "canopy_height",         // m
        "Gs",                    // mol / m^2 / s
        "leaf_temperature",      // degrees C
        "leafwidth",             // m
        "min_gbw_canopy",        // m / s
        "rh",                    // dimensionless from Pa / Pa
        "temp",                  // degrees C
        "wind_speed_height",     // m
        "windspeed"              // m / s
    };
}

string_vector leaf_evapotranspiration_check::get_outputs()
{
    return {
        "leaf_temp_check"  // degrees C
    };
}

void leaf_evapotranspiration_check::do_operation() const
{
    // Get absorbed longwave radiation
    double const absorbed_longwave =
        1.0 * physical_constants::stefan_boltzmann *
        pow(conversion_constants::celsius_to_kelvin + temp, 4);  // J / m^2 / s

    // Get canopy boundary layer conductance to water vapor
    double const gbw_canopy = canopy_boundary_layer_conductance_thornley(
        canopy_height,
        windspeed,
        min_gbw_canopy,
        wind_speed_height);  // m / s

    // Set some constants
    double constexpr epsilon_s = 1.0;  // dimensionless

    // Get water vapor and air properties based on the air temperature
    double const c_p = TempToCp(temp);                                        // J / kg / K
    double const lambda = water_latent_heat_of_vaporization_henderson(temp);  // J / kg
    double const p_w_sat_air = saturation_vapor_pressure(temp);               // Pa
    double const rho_ta = dry_air_density(temp, atmospheric_pressure);        // kg / m^3
    double const s = TempToSFS(temp);                                         // kg / m^3 / K

    // Get the pyschrometric parameter
    double const gamma = rho_ta * c_p / lambda;  // kg / m^3 / K

    // Get vapor density in the ambient air.
    double const p_w_air = p_w_sat_air * rh;  // Pa

    double const rho_w_air =
        vapor_density_from_pressure(rho_ta, atmospheric_pressure, p_w_air);  // kg / m^3

    // Get vapor density deficit
    double const rho_w_sat =
        vapor_density_from_pressure(rho_ta, atmospheric_pressure, p_w_sat_air);  // kg / m^3

    double const Delta_rho = rho_w_sat - rho_w_air;  // kg / m^3

    // Get total absorbed light energy (longwave and shortwave)
    double const J_a = absorbed_shortwave + absorbed_longwave;  // J / m^2 / s

    // Get the temperature difference
    double const leaf_temp_check = check_leaf_temp(
        atmospheric_pressure,
        temp,
        Delta_rho,
        epsilon_s,
        gamma,
        gbw_canopy,
        J_a,
        lambda,
        leaf_temperature,
        leafwidth,
        s,
        Gs,
        windspeed);

    update(leaf_temp_check_op, leaf_temp_check);
}

}  // namespace standardBML
#endif
