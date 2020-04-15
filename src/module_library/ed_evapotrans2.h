#ifndef ED_EVAPOTRANS2_H
#define ED_EVAPOTRANS2_H

#include "../modules.h"
#include "c4photo.h"

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
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_evapotrans2"),
          // Get pointers to input parameters
          temperature_air_ip(get_ip(input_parameters, "temp")),
          rh_ip(get_ip(input_parameters, "rh")),
          windspeed_ip(get_ip(input_parameters, "windspeed")),
          conductance_stomatal_h2o_ip(get_ip(input_parameters, "conductance_stomatal_h2o")),
          leafwidth_ip(get_ip(input_parameters, "leafwidth")),
          solar_energy_absorbed_leaf_ip(get_ip(input_parameters, "solar_energy_absorbed_leaf")),
          // Get pointers to output parameters
          evapotranspiration_penman_monteith_op(get_op(output_parameters, "evapotranspiration_penman_monteith")),
          evapotranspiration_penman_op(get_op(output_parameters, "evapotranspiration_penman")),
          evapotranspiration_priestly_op(get_op(output_parameters, "evapotranspiration_priestly")),
          temperature_leaf_op(get_op(output_parameters, "temperature_leaf"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* temperature_air_ip;
    const double* rh_ip;
    const double* windspeed_ip;
    const double* conductance_stomatal_h2o_ip;
    const double* leafwidth_ip;
    const double* solar_energy_absorbed_leaf_ip;
    // Pointers to output parameters
    double* evapotranspiration_penman_monteith_op;
    double* evapotranspiration_penman_op;
    double* evapotranspiration_priestly_op;
    double* temperature_leaf_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_evapotrans2::get_inputs()
{
    return {
        "temp",                       // deg. C
        "rh",                         // unitless from Pa / Pa
        "windspeed",                  // m / s
        "conductance_stomatal_h2o",   // mol / m^2 / s
        "leafwidth",                  // m
        "solar_energy_absorbed_leaf"  // J / m^2 / s
    };
}

std::vector<std::string> ed_evapotrans2::get_outputs()
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
    const double Rad = 0.0;                                                  // micromoles / m^2 / s (only used for evapotranspiration)
    const double airTemp = *temperature_air_ip;                              // degrees C
    const double RH = *rh_ip;                                                // dimensionless from Pa / Pa
    const double WindSpeed = *windspeed_ip;                                  // m / s
    const double LeafAreaIndex = 0.0;                                        // dimensionless from m^2 / m^2 (not actually used by EvapoTrans2)
    const double CanopyHeight = 0.0;                                         // meters (not actually used by EvapoTrans2)
    const double stomatal_conductance = *conductance_stomatal_h2o_ip * 1e3;  // mmol / m^2 / s
    const double leaf_width = *leafwidth_ip;                                 // meter
    const int eteq = 0;                                                      // Report Penman-Monteith transpiration

    // For ed_penman_monteith_leaf_temperature, the light input is called `solar_energy_absorbed_leaf`
    // and is in units of W / m^2 / s. This quantity is equivalent to `Ja2` in Evapotrans2. `Iave` is
    // related to `Ja2` by:
    //  Ja2 = total_average_irradiance * (1 - LeafReflectance - tau) / (1 - tau)
    //      = Iave * joules_per_micromole_PAR / fraction_of_irradiance_in_PAR * (1 - LeafReflectance - tau) / (1 - tau)
    // where:
    //  joules_per_micromole_PAR = 0.235
    //  fraction_of_irradiance_in_PAR = 0.5
    //  LeafReflectance = 0.2
    //  tau = 0.2
    // So overall, Ja2 = Iave * 0.235 / 0.5 * (1 - 0.2 - 0.2) / (1 - 0.2)
    //                 = Iave * 0.3525
    // So we can find Iave according to Iave = Ja2 / 0.3525
    const double Iave = *solar_energy_absorbed_leaf_ip / 0.3525;  // micromoles / m^2 / s

    // Call EvapoTrans2
    struct ET_Str et_results = EvapoTrans2(
        Rad,
        Iave,
        airTemp,
        RH,
        WindSpeed,
        LeafAreaIndex,
        CanopyHeight,
        stomatal_conductance,
        leaf_width,
        eteq);

    // Convert and return the results
    update(evapotranspiration_penman_monteith_op, et_results.TransR * 1e-3);  // mol / m^2 / s
    update(evapotranspiration_penman_op, et_results.EPenman * 1e-3);          // mol / m^2 / s
    update(evapotranspiration_priestly_op, et_results.EPriestly * 1e-3);      // mol / m^2 / s
    update(temperature_leaf_op, airTemp + et_results.Deltat);                 // deg. C
}

#endif
