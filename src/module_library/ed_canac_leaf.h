#ifndef ED_CANAC_LEAF_H
#define ED_CANAC_LEAF_H

#include "../modules.h"
#include "c4photo.h"

/**
 * @class ed_canac_leaf
 * 
 * @brief Uses the Stefan-Boltzmann law to calculate long-wave energy losses
 * from the leaf to the air. Currently only intended for use by Ed.
 */
class ed_canac_leaf : public SteadyModule
{
   public:
    ed_canac_leaf(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_canac_leaf"),
          // Get pointers to input parameters
          collatz_PAR_flux_ip(get_ip(input_parameters, "collatz_PAR_flux")),
          temperature_air_ip(get_ip(input_parameters, "temp")),
          rh_ip(get_ip(input_parameters, "rh")),
          collatz_vmax_ip(get_ip(input_parameters, "collatz_vmax")),
          collatz_alpha_ip(get_ip(input_parameters, "collatz_alpha")),
          collatz_k_ip(get_ip(input_parameters, "collatz_k")),
          collatz_theta_ip(get_ip(input_parameters, "collatz_theta")),
          collatz_beta_ip(get_ip(input_parameters, "collatz_beta")),
          collatz_rd_ip(get_ip(input_parameters, "collatz_rd")),
          ball_berry_intercept_ip(get_ip(input_parameters, "ball_berry_intercept")),
          ball_berry_slope_ip(get_ip(input_parameters, "ball_berry_slope")),
          StomataWS_ip(get_ip(input_parameters, "StomataWS")),
          mole_fraction_co2_atmosphere_ip(get_ip(input_parameters, "mole_fraction_co2_atmosphere")),
          collatz_rubisco_temperature_upper_ip(get_ip(input_parameters, "collatz_rubisco_temperature_upper")),
          collatz_rubisco_temperature_lower_ip(get_ip(input_parameters, "collatz_rubisco_temperature_lower")),
          windspeed_ip(get_ip(input_parameters, "windspeed")),
          leafwidth_ip(get_ip(input_parameters, "leafwidth")),
          solar_energy_absorbed_leaf_ip(get_ip(input_parameters, "solar_energy_absorbed_leaf")),
          // Get pointers to output parameters
          mole_fraction_co2_intercellular_op(get_op(output_parameters, "mole_fraction_co2_intercellular")),
          conductance_stomatal_h2o_op(get_op(output_parameters, "conductance_stomatal_h2o")),
          assimilation_gross_op(get_op(output_parameters, "assimilation_gross")),
          assimilation_net_op(get_op(output_parameters, "assimilation_net")),
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
    const double* collatz_PAR_flux_ip;
    const double* temperature_air_ip;
    const double* rh_ip;
    const double* collatz_vmax_ip;
    const double* collatz_alpha_ip;
    const double* collatz_k_ip;
    const double* collatz_theta_ip;
    const double* collatz_beta_ip;
    const double* collatz_rd_ip;
    const double* ball_berry_intercept_ip;
    const double* ball_berry_slope_ip;
    const double* StomataWS_ip;
    const double* mole_fraction_co2_atmosphere_ip;
    const double* collatz_rubisco_temperature_upper_ip;
    const double* collatz_rubisco_temperature_lower_ip;
    const double* windspeed_ip;
    const double* leafwidth_ip;
    const double* solar_energy_absorbed_leaf_ip;
    // Pointers to output parameters
    double* mole_fraction_co2_intercellular_op;
    double* conductance_stomatal_h2o_op;
    double* assimilation_gross_op;
    double* assimilation_net_op;
    double* evapotranspiration_penman_monteith_op;
    double* evapotranspiration_penman_op;
    double* evapotranspiration_priestly_op;
    double* temperature_leaf_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_canac_leaf::get_inputs()
{
    return {
        "collatz_PAR_flux",                   // mol / m^2 / s
        "temp",                               // deg. C
        "rh",                                 // dimensionless from Pa / Pa
        "collatz_vmax",                       // mol / m^2 / s
        "collatz_alpha",                      // dimensionless
        "collatz_k",                          // mol / m^2 / s
        "collatz_theta",                      // dimensionless
        "collatz_beta",                       // dimensionless
        "collatz_rd",                         // mol / m^2 / s
        "ball_berry_intercept",               // mol / m^2 / s
        "ball_berry_slope",                   // dimensionless
        "StomataWS",                          // dimensionless
        "mole_fraction_co2_atmosphere",       // dimensionless from mol / mol
        "collatz_rubisco_temperature_upper",  // deg. C
        "collatz_rubisco_temperature_lower",  // deg. C
        "windspeed",                          // m / s
        "leafwidth",                          // m
        "solar_energy_absorbed_leaf"          // J / m^2 / s
    };
}

std::vector<std::string> ed_canac_leaf::get_outputs()
{
    return {
        "mole_fraction_co2_intercellular",     // dimensionless from mol / mol
        "conductance_stomatal_h2o",            // mol / m^2 / s
        "assimilation_gross",                  // mol / m^2 / s
        "assimilation_net",                    // mol / m^2 / s
        "evapotranspiration_penman_monteith",  // mol / m^2 / s
        "evapotranspiration_penman",           // mol / m^2 / s
        "evapotranspiration_priestly",         // mol / m^2 / s
        "temperature_leaf"                     // mol / m^2 / s
    };
}

void ed_canac_leaf::do_operation() const
{
    // Get the inputs and convert units as necessary
    const double Qp = *collatz_PAR_flux_ip * 1e6;                 // micromole / m^2 / s
    const double temperature_air = *temperature_air_ip;           // deg. C
    const double RH = *rh_ip;                                     // dimensionless
    const double vmax = *collatz_vmax_ip * 1e6;                   // micromole / m^2 / s
    const double alpha = *collatz_alpha_ip;                       // dimensionless
    const double kparm = *collatz_k_ip;                           // mol / m^2 / s
    const double theta = *collatz_theta_ip;                       // dimensionless
    const double beta = *collatz_beta_ip;                         // dimensionless
    const double Rd = *collatz_rd_ip * 1e6;                       // micromole / m^2 / s
    const double bb0 = *ball_berry_intercept_ip;                  // mol / m^2 / s
    const double bb1 = *ball_berry_slope_ip;                      // dimensionless
    const double StomaWS = *StomataWS_ip;                         // dimensionless
    const double Ca = *mole_fraction_co2_atmosphere_ip * 1e6;     // micromole / mol
    const double upperT = *collatz_rubisco_temperature_upper_ip;  // deg. C
    const double lowerT = *collatz_rubisco_temperature_lower_ip;  // deg. C
    const int water_stress_approach = 1;                          // Apply water stress via stomatal conductance
    const double Rad = 0.0;                                       // micromoles / m^2 / s (only used for evapotranspiration)
    const double WindSpeed = *windspeed_ip;                       // m / s
    const double LeafAreaIndex = 0.0;                             // dimensionless from m^2 / m^2 (not actually used by EvapoTrans2)
    const double CanopyHeight = 0.0;                              // meters (not actually used by EvapoTrans2)
    const double leaf_width = *leafwidth_ip;                      // meter
    const int eteq = 0;                                           // Report Penman-Monteith transpiration

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

    // Run c4photoC once assuming the leaf is at air temperature to get an initial guess for gs
    const struct c4_str first_c4photoC_output = c4photoC(Qp, temperature_air, RH,
                                                         vmax, alpha, kparm, theta, beta, Rd,
                                                         bb0, bb1, StomaWS, Ca, water_stress_approach,
                                                         upperT, lowerT);

    const double stomatal_conductance = first_c4photoC_output.Gs;  // mmol / m^2 / s

    // Run EvapoTrans2 to get the leaf temperature
    const struct ET_Str evapotrans_output = EvapoTrans2(Rad, Iave, temperature_air, RH, WindSpeed,
                                                        LeafAreaIndex, CanopyHeight, stomatal_conductance, leaf_width, eteq);

    const double leaf_temperature = temperature_air + evapotrans_output.Deltat;

    // Run c4photoC a second time to get final values for assimilation and conductance
    const struct c4_str second_c4photoC_output = c4photoC(Qp, leaf_temperature, RH,
                                                          vmax, alpha, kparm, theta, beta, Rd,
                                                          bb0, bb1, StomaWS, Ca, water_stress_approach,
                                                          upperT, lowerT);

    // Convert and return the results
    update(mole_fraction_co2_intercellular_op, second_c4photoC_output.Ci * 1e-6);    // mol / m^2 / s
    update(conductance_stomatal_h2o_op, second_c4photoC_output.Gs * 1e-3);           // mol / m^2 / s
    update(assimilation_gross_op, second_c4photoC_output.GrossAssim * 1e-6);         // mol / m^2 / s
    update(assimilation_net_op, second_c4photoC_output.Assim * 1e-6);                // mol / m^2 / s
    update(evapotranspiration_penman_monteith_op, evapotrans_output.TransR * 1e-3);  // mol / m^2 / s
    update(evapotranspiration_penman_op, evapotrans_output.EPenman * 1e-3);          // mol / m^2 / s
    update(evapotranspiration_priestly_op, evapotrans_output.EPriestly * 1e-3);      // mol / m^2 / s
    update(temperature_leaf_op, leaf_temperature);                                   // deg. C
}

#endif
