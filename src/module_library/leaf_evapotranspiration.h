#ifndef LEAF_EVAPOTRANSPIRATION_H
#define LEAF_EVAPOTRANSPIRATION_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "leaf_energy_balance.h"

namespace standardBML
{
/**
 *  @class leaf_evapotranspiration
 *
 *  @brief Uses `leaf_energy_balance()` to determine transpiration rate and leaf
 *  temperature.
 */
class leaf_evapotranspiration : public direct_module
{
   public:
    leaf_evapotranspiration(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          absorbed_shortwave{get_input(input_quantities, "absorbed_shortwave")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          canopy_height{get_input(input_quantities, "canopy_height")},
          Gs{get_input(input_quantities, "Gs")},
          leafwidth{get_input(input_quantities, "leafwidth")},
          min_gbw_canopy{get_input(input_quantities, "min_gbw_canopy")},
          rh{get_input(input_quantities, "rh")},
          temp{get_input(input_quantities, "temp")},
          windspeed{get_input(input_quantities, "windspeed")},
          wind_speed_height{get_input(input_quantities, "wind_speed_height")},

          // Get pointers to output quantities
          EPenman_op{get_op(output_quantities, "EPenman")},
          EPriestly_op{get_op(output_quantities, "EPriestly")},
          E_loss_op{get_op(output_quantities, "E_loss")},
          gbw_canopy_op{get_op(output_quantities, "gbw_canopy")},
          gbw_leaf_op{get_op(output_quantities, "gbw_leaf")},
          gbw_op{get_op(output_quantities, "gbw")},
          gsw_op{get_op(output_quantities, "gsw")},
          H_op{get_op(output_quantities, "H")},
          leaf_temperature_op{get_op(output_quantities, "leaf_temperature")},
          PhiN_op{get_op(output_quantities, "PhiN")},
          storage_op{get_op(output_quantities, "storage")},
          TransR_op{get_op(output_quantities, "TransR")},
          iterations_op{get_op(output_quantities, "iterations")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "leaf_evapotranspiration"; }

   private:
    // Pointers to input quantities
    double const& absorbed_shortwave;
    double const& atmospheric_pressure;
    double const& canopy_height;
    double const& Gs;
    double const& leafwidth;
    double const& min_gbw_canopy;
    double const& rh;
    double const& temp;
    double const& windspeed;
    double const& wind_speed_height;

    // Pointers to output quantities
    double* EPenman_op;
    double* EPriestly_op;
    double* E_loss_op;
    double* gbw_canopy_op;
    double* gbw_leaf_op;
    double* gbw_op;
    double* gsw_op;
    double* H_op;
    double* leaf_temperature_op;
    double* PhiN_op;
    double* storage_op;
    double* TransR_op;
    double* iterations_op;

    // Main operation
    void do_operation() const;
};

string_vector leaf_evapotranspiration::get_inputs()
{
    return {
        "absorbed_shortwave",    // J / m^2 / s
        "atmospheric_pressure",  // Pa
        "canopy_height",         // m
        "Gs",                    // mol / m^2 / s
        "leafwidth",             // m
        "min_gbw_canopy",        // m / s
        "rh",                    // dimensionless from Pa / Pa
        "temp",                  // degrees C
        "windspeed",             // m / s
        "wind_speed_height"      // m
    };
}

string_vector leaf_evapotranspiration::get_outputs()
{
    return {
        "EPenman",           // mmol / m^2 / s
        "EPriestly",         // mmol / m^2 / s
        "E_loss",            // J / m^2 / s
        "gbw",               // m / s
        "gbw_canopy",        // m / s
        "gbw_leaf",          // m / s
        "gsw",               // m / s
        "H",                 // J / m^2 / s
        "leaf_temperature",  // degrees C
        "PhiN",              // J / m^2 / s
        "storage",           // J / m^2 / s
        "TransR",            // mmol / m^2 / s
        "iterations"         // not a physical quantity
    };
}

void leaf_evapotranspiration::do_operation() const
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

    energy_balance_outputs result = leaf_energy_balance(
        absorbed_longwave,
        absorbed_shortwave,
        atmospheric_pressure,
        temp,
        gbw_canopy,
        leafwidth,
        rh,
        Gs,
        windspeed);

    update(EPenman_op, result.EPenman);
    update(EPriestly_op, result.EPriestly);
    update(E_loss_op, result.E_loss);
    update(gbw_canopy_op, result.gbw_canopy);
    update(gbw_leaf_op, result.gbw_leaf);
    update(gbw_op, result.gbw);
    update(gsw_op, result.gsw);
    update(H_op, result.H);
    update(iterations_op, result.iterations);
    update(leaf_temperature_op, temp + result.Deltat);
    update(PhiN_op, result.PhiN);
    update(storage_op, result.storage);
    update(TransR_op, result.TransR);
}

}  // namespace standardBML
#endif
