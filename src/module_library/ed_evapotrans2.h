#ifndef ED_EVAPOTRANS2_H
#define ED_EVAPOTRANS2_H

#include "../modules.h"
#include "../state_map.h"
#include "c4photo.h"
#include "BioCro.h"  // for absorbed_shortwave_from_incident_ppfd

/**
 * @class ed_evapotrans2
 *
 * @brief Just a module wrapper for the EvapoTrans2 function.
 * Currently only intended for use by Ed.
 */
class ed_evapotrans2 : public SteadyModule
{
   public:
    ed_evapotrans2(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_evapotrans2"),
          // Get pointers to input quantities
          temperature_air_ip(get_ip(input_quantities, "temp")),
          rh_ip(get_ip(input_quantities, "rh")),
          windspeed_ip(get_ip(input_quantities, "windspeed")),
          conductance_stomatal_h2o_ip(get_ip(input_quantities, "conductance_stomatal_h2o")),
          leafwidth_ip(get_ip(input_quantities, "leafwidth")),
          specific_heat_of_air_ip(get_ip(input_quantities, "specific_heat_of_air")),
          solar_energy_absorbed_leaf_ip(get_ip(input_quantities, "solar_energy_absorbed_leaf")),
          // Get pointers to output quantities
          evapotranspiration_penman_monteith_op(get_op(output_quantities, "evapotranspiration_penman_monteith")),
          evapotranspiration_penman_op(get_op(output_quantities, "evapotranspiration_penman")),
          evapotranspiration_priestly_op(get_op(output_quantities, "evapotranspiration_priestly")),
          temperature_leaf_op(get_op(output_quantities, "temperature_leaf"))

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Pointers to input quantities
    const double* temperature_air_ip;
    const double* rh_ip;
    const double* windspeed_ip;
    const double* conductance_stomatal_h2o_ip;
    const double* leafwidth_ip;
    const double* specific_heat_of_air_ip;
    const double* solar_energy_absorbed_leaf_ip;
    // Pointers to output quantities
    double* evapotranspiration_penman_monteith_op;
    double* evapotranspiration_penman_op;
    double* evapotranspiration_priestly_op;
    double* temperature_leaf_op;
    // Main operation
    void do_operation() const override;
};

string_vector ed_evapotrans2::get_inputs()
{
    return {
        "temp",                       // deg. C
        "rh",                         // unitless from Pa / Pa
        "windspeed",                  // m / s
        "conductance_stomatal_h2o",   // mol / m^2 / s
        "leafwidth",                  // m
        "specific_heat_of_air",       // J / kg / K
        "solar_energy_absorbed_leaf"  // J / m^2 / s
    };
}

string_vector ed_evapotrans2::get_outputs()
{
    return {
        "evapotranspiration_penman_monteith",  // mol / m^2 / s
        "evapotranspiration_penman",           // mol / m^2 / s
        "evapotranspiration_priestly",         // mol / m^2 / s
        "temperature_leaf"                     // mol / m^2 / s
    };
}

void ed_evapotrans2::do_operation() const
{
    // Collect inputs to EvapoTrans2 and convert units as necessary
    const double absorbed_shortwave_radiation_et = 0.0;                             // micromoles / m^2 / s (only used for evapotranspiration)
    const double airTemp = *temperature_air_ip;                                     // degrees C
    const double RH = *rh_ip;                                                       // dimensionless from Pa / Pa
    const double WindSpeed = *windspeed_ip;                                         // m / s
    const double LeafAreaIndex = 0.0;                                               // dimensionless from m^2 / m^2 (not actually used by EvapoTrans2)
    const double CanopyHeight = 0.0;                                                // meters (not actually used by EvapoTrans2)
    const double stomatal_conductance = *conductance_stomatal_h2o_ip * 1e3;         // mmol / m^2 / s
    const double leaf_width = *leafwidth_ip;                                        // meter
    const double specific_heat_of_air = *specific_heat_of_air_ip;                   // J / kg / K
    const int eteq = 0;                                                             // Report Penman-Monteith transpiration
    const double absorbed_shortwave_radiation_lt = *solar_energy_absorbed_leaf_ip;  // J / m^2 / s

    // Call EvapoTrans2
    struct ET_Str et_results = EvapoTrans2(
        absorbed_shortwave_radiation_et,
        absorbed_shortwave_radiation_lt,
        airTemp,
        RH,
        WindSpeed,
        LeafAreaIndex,
        CanopyHeight,
        stomatal_conductance,
        leaf_width,
        specific_heat_of_air,
        eteq);

    // Convert and return the results
    update(evapotranspiration_penman_monteith_op, et_results.TransR * 1e-3);  // mol / m^2 / s
    update(evapotranspiration_penman_op, et_results.EPenman * 1e-3);          // mol / m^2 / s
    update(evapotranspiration_priestly_op, et_results.EPriestly * 1e-3);      // mol / m^2 / s
    update(temperature_leaf_op, airTemp + et_results.Deltat);                 // deg. C
}

#endif
