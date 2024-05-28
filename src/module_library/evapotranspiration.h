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
          absorbed_shortwave_energy{get_input(input_quantities, "absorbed_shortwave_energy")},
          air_pressure{get_input(input_quantities, "air_pressure")},
          air_temperature{get_input(input_quantities, "air_temperature")},
          canopy_height{get_input(input_quantities, "canopy_height")},
          leaf_width{get_input(input_quantities, "leaf_width")},
          min_gbw_canopy{get_input(input_quantities, "min_gbw_canopy")},
          min_gbw_leaf{get_input(input_quantities, "min_gbw_leaf")},
          relative_humidity{get_input(input_quantities, "relative_humidity")},
          Gs{get_input(input_quantities, "Gs")},
          windspeed{get_input(input_quantities, "windspeed")},
          wind_speed_height{get_input(input_quantities, "wind_speed_height")},

          // Get pointers to output quantities
          Deltat_op{get_op(output_quantities, "Deltat")},
          E_loss_op{get_op(output_quantities, "E_loss")},
          gbw_op{get_op(output_quantities, "gbw")},
          gbw_canopy_op{get_op(output_quantities, "gbw_canopy")},
          gbw_leaf_op{get_op(output_quantities, "gbw_leaf")},
          gsw_op{get_op(output_quantities, "gsw")},
          H_op{get_op(output_quantities, "H")},
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
    double const& absorbed_shortwave_energy;
    double const& air_pressure;
    double const& air_temperature;
    double const& canopy_height;
    double const& leaf_width;
    double const& min_gbw_canopy;
    double const& min_gbw_leaf;
    double const& relative_humidity;
    double const& Gs;
    double const& windspeed;
    double const& wind_speed_height;

    // Pointers to output quantities
    double* Deltat_op;
    double* E_loss_op;
    double* gbw_op;
    double* gbw_canopy_op;
    double* gbw_leaf_op;
    double* gsw_op;
    double* H_op;
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
        "absorbed_shortwave_energy",  // J / m^2 / s
        "air_pressure",               // Pa
        "air_temperature",            // degrees C
        "canopy_height",              // m
        "leaf_width",                 // m
        "min_gbw_canopy",             // m / s
        "min_gbw_leaf",               // m / s
        "relative_humidity",          // dimensionless from Pa / Pa
        "Gs",                         // mol / m^2 / s
        "windspeed",                  // m / s
        "wind_speed_height"           // m
    };
}

string_vector leaf_evapotranspiration::get_outputs()
{
    return {
        "Deltat",      // degrees C
        "E_loss",      // J / m^2 / s
        "gbw",         // m / s
        "gbw_canopy",  // m / s
        "gbw_leaf",    // m / s
        "gsw",         // m / s
        "H",           // J / m^2 / s
        "PhiN",        // J / m^2 / s
        "storage",     // J / m^2 / s
        "TransR",      // mmol / m^2 / s
        "iterations"   // not a physical quantity
    };
}

void leaf_evapotranspiration::do_operation() const
{
    double const absorbed_longwave_energy =
        1.0 * physical_constants::stefan_boltzmann *
        pow(conversion_constants::celsius_to_kelvin + air_temperature, 4);  // J / m^2 / s

    energy_balance_outputs result = leaf_energy_balance(
        absorbed_longwave_energy,
        absorbed_shortwave_energy,
        air_pressure,
        air_temperature,
        canopy_height,
        leaf_width,
        min_gbw_canopy,
        min_gbw_leaf,
        relative_humidity,
        Gs,
        windspeed,
        wind_speed_height);

    update(Deltat_op, result.Deltat);
    update(E_loss_op, result.E_loss);
    update(gbw_op, result.gbw);
    update(gbw_canopy_op, result.gbw_canopy);
    update(gbw_leaf_op, result.gbw_leaf);
    update(gsw_op, result.gsw);
    update(H_op, result.H);
    update(PhiN_op, result.PhiN);
    update(storage_op, result.storage);
    update(TransR_op, result.TransR);
    update(iterations_op, result.iterations);
}

}  // namespace standardBML
#endif
