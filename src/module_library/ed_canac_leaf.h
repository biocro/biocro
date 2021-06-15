#ifndef ED_CANAC_LEAF_H
#define ED_CANAC_LEAF_H

#include "../modules.h"
#include "../state_map.h"
#include "c4photo.h"
#include "BioCro.h"  // for absorbed_shortwave_from_incident_ppfd

/**
 * @class ed_canac_leaf
 *
 * @brief Currently only intended for use by Ed.
 */
class ed_canac_leaf : public SteadyModule
{
   public:
    ed_canac_leaf(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_canac_leaf"),

          // Get pointers to input quantities
          collatz_PAR_flux_ip(get_ip(input_quantities, "collatz_PAR_flux")),
          temperature_air_ip(get_ip(input_quantities, "temp")),
          rh_ip(get_ip(input_quantities, "rh")),
          collatz_vmax_ip(get_ip(input_quantities, "collatz_vmax")),
          collatz_alpha_ip(get_ip(input_quantities, "collatz_alpha")),
          collatz_k_ip(get_ip(input_quantities, "collatz_k")),
          collatz_theta_ip(get_ip(input_quantities, "collatz_theta")),
          collatz_beta_ip(get_ip(input_quantities, "collatz_beta")),
          collatz_rd_ip(get_ip(input_quantities, "collatz_rd")),
          ball_berry_intercept_ip(get_ip(input_quantities, "ball_berry_intercept")),
          ball_berry_slope_ip(get_ip(input_quantities, "ball_berry_slope")),
          conductance_stomatal_h2o_min_ip(get_ip(input_quantities, "conductance_stomatal_h2o_min")),
          StomataWS_ip(get_ip(input_quantities, "StomataWS")),
          mole_fraction_co2_atmosphere_ip(get_ip(input_quantities, "mole_fraction_co2_atmosphere")),
          atmospheric_pressure_ip(get_ip(input_quantities, "atmospheric_pressure")),
          collatz_rubisco_temperature_upper_ip(get_ip(input_quantities, "collatz_rubisco_temperature_upper")),
          collatz_rubisco_temperature_lower_ip(get_ip(input_quantities, "collatz_rubisco_temperature_lower")),
          windspeed_ip(get_ip(input_quantities, "windspeed")),
          leafwidth_ip(get_ip(input_quantities, "leafwidth")),
          specific_heat_of_air_ip(get_ip(input_quantities, "specific_heat_of_air")),
          solar_energy_absorbed_leaf_ip(get_ip(input_quantities, "solar_energy_absorbed_leaf")),
          par_energy_content_ip(get_ip(input_quantities, "par_energy_content")),
          par_energy_fraction_ip(get_ip(input_quantities, "par_energy_fraction")),
          leaf_transmittance_ip(get_ip(input_quantities, "leaf_transmittance")),
          leaf_reflectance_ip(get_ip(input_quantities, "leaf_reflectance")),

          // Get pointers to output quantities
          mole_fraction_co2_intercellular_op(get_op(output_quantities, "mole_fraction_co2_intercellular")),
          conductance_stomatal_h2o_op(get_op(output_quantities, "conductance_stomatal_h2o")),
          assimilation_gross_op(get_op(output_quantities, "assimilation_gross")),
          assimilation_net_op(get_op(output_quantities, "assimilation_net")),
          evapotranspiration_penman_monteith_op(get_op(output_quantities, "evapotranspiration_penman_monteith")),
          evapotranspiration_penman_op(get_op(output_quantities, "evapotranspiration_penman")),
          evapotranspiration_priestly_op(get_op(output_quantities, "evapotranspiration_priestly")),
          temperature_leaf_op(get_op(output_quantities, "temperature_leaf")),
          conductance_boundary_h2o_op(get_op(output_quantities, "conductance_boundary_h2o"))

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Pointers to input quantities
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
    const double* conductance_stomatal_h2o_min_ip;
    const double* StomataWS_ip;
    const double* mole_fraction_co2_atmosphere_ip;
    const double* atmospheric_pressure_ip;
    const double* collatz_rubisco_temperature_upper_ip;
    const double* collatz_rubisco_temperature_lower_ip;
    const double* windspeed_ip;
    const double* leafwidth_ip;
    const double* specific_heat_of_air_ip;
    const double* solar_energy_absorbed_leaf_ip;
    const double* par_energy_content_ip;
    const double* par_energy_fraction_ip;
    const double* leaf_transmittance_ip;
    const double* leaf_reflectance_ip;

    // Pointers to output quantities
    double* mole_fraction_co2_intercellular_op;
    double* conductance_stomatal_h2o_op;
    double* assimilation_gross_op;
    double* assimilation_net_op;
    double* evapotranspiration_penman_monteith_op;
    double* evapotranspiration_penman_op;
    double* evapotranspiration_priestly_op;
    double* temperature_leaf_op;
    double* conductance_boundary_h2o_op;

    // Main operation
    void do_operation() const override;
};

string_vector ed_canac_leaf::get_inputs()
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
        "conductance_stomatal_h2o_min",       // mol / m^2 / s
        "StomataWS",                          // dimensionless
        "mole_fraction_co2_atmosphere",       // dimensionless from mol / mol
        "atmospheric_pressure",               // Pa
        "collatz_rubisco_temperature_upper",  // deg. C
        "collatz_rubisco_temperature_lower",  // deg. C
        "windspeed",                          // m / s
        "leafwidth",                          // m
        "specific_heat_of_air",               // J / kg / K
        "solar_energy_absorbed_leaf",         // J / m^2 / s
        "par_energy_content",                 // J / micromol
        "par_energy_fraction",                // dimensionless
        "leaf_transmittance",                 // dimensionless
        "leaf_reflectance"                    // dimensionless
    };
}

string_vector ed_canac_leaf::get_outputs()
{
    return {
        "mole_fraction_co2_intercellular",     // dimensionless from mol / mol
        "conductance_stomatal_h2o",            // mol / m^2 / s
        "assimilation_gross",                  // mol / m^2 / s
        "assimilation_net",                    // mol / m^2 / s
        "evapotranspiration_penman_monteith",  // mol / m^2 / s
        "evapotranspiration_penman",           // mol / m^2 / s
        "evapotranspiration_priestly",         // mol / m^2 / s
        "temperature_leaf",                    // mol / m^2 / s
        "conductance_boundary_h2o"             // mol / m^2 / s
    };
}

void ed_canac_leaf::do_operation() const
{
    // Get the inputs and convert units as necessary
    const double Qp = *collatz_PAR_flux_ip * 1e6;                                   // micromole / m^2 / s
    const double temperature_air = *temperature_air_ip;                             // deg. C
    const double RH = *rh_ip;                                                       // dimensionless
    const double vmax = *collatz_vmax_ip * 1e6;                                     // micromole / m^2 / s
    const double alpha = *collatz_alpha_ip;                                         // dimensionless
    const double kparm = *collatz_k_ip;                                             // mol / m^2 / s
    const double theta = *collatz_theta_ip;                                         // dimensionless
    const double beta = *collatz_beta_ip;                                           // dimensionless
    const double Rd = *collatz_rd_ip * 1e6;                                         // micromole / m^2 / s
    const double bb0 = *ball_berry_intercept_ip;                                    // mol / m^2 / s
    const double bb1 = *ball_berry_slope_ip;                                        // dimensionless
    const double Gs_min = *conductance_stomatal_h2o_min_ip * 1e3;                   // mmol / m^2 / s
    const double StomaWS = *StomataWS_ip;                                           // dimensionless
    const double Ca = *mole_fraction_co2_atmosphere_ip * 1e6;                       // micromole / mol
    const double atmospheric_pressure = *atmospheric_pressure_ip;                   // Pa
    const double upperT = *collatz_rubisco_temperature_upper_ip;                    // deg. C
    const double lowerT = *collatz_rubisco_temperature_lower_ip;                    // deg. C
    const int water_stress_approach = 1;                                            // Apply water stress via stomatal conductance
    const double WindSpeed = *windspeed_ip;                                         // m / s
    const double LeafAreaIndex = 0.0;                                               // dimensionless from m^2 / m^2 (not actually used by EvapoTrans2)
    const double CanopyHeight = 0.0;                                                // meters (not actually used by EvapoTrans2)
    const double leaf_width = *leafwidth_ip;                                        // meter
    const double specific_heat_of_air = *specific_heat_of_air_ip;                   // J / kg / K
    const int eteq = 0;                                                             // Report Penman-Monteith transpiration
    const double absorbed_shortwave_radiation_lt = *solar_energy_absorbed_leaf_ip;  // J / m^2 / s
    const double par_energy_content = *par_energy_content_ip;                       // J / micromol
    const double par_energy_fraction = *par_energy_fraction_ip;                     // dimensionless
    const double leaf_transmittance = *leaf_transmittance_ip;                       // dimensionless
    const double leaf_reflectance = *leaf_reflectance_ip;                           // dimensionless

    double const absorbed_shortwave_radiation_et =
        absorbed_shortwave_from_incident_ppfd(
            Qp, par_energy_content, par_energy_fraction, leaf_reflectance,
            leaf_transmittance);  // J / m^2 / s

    // Run c4photoC once assuming the leaf is at air temperature to get an initial guess for gs
    const struct c4_str first_c4photoC_output =
        c4photoC(
            Qp, temperature_air, RH, vmax, alpha, kparm, theta, beta, Rd, bb0,
            bb1, Gs_min, StomaWS, Ca, atmospheric_pressure,
            water_stress_approach, upperT, lowerT);

    const double stomatal_conductance = first_c4photoC_output.Gs;  // mmol / m^2 / s

    // Run EvapoTrans2 to get the leaf temperature
    const struct ET_Str evapotrans_output =
        EvapoTrans2(
            absorbed_shortwave_radiation_et, absorbed_shortwave_radiation_lt,
            temperature_air, RH, WindSpeed, LeafAreaIndex, CanopyHeight,
            stomatal_conductance, leaf_width, specific_heat_of_air, eteq);

    const double leaf_temperature = temperature_air + evapotrans_output.Deltat;

    // Run c4photoC a second time to get final values for assimilation and conductance
    const struct c4_str second_c4photoC_output =
        c4photoC(
            Qp, leaf_temperature, RH, vmax, alpha, kparm, theta, beta, Rd, bb0,
            bb1, Gs_min, StomaWS, Ca, atmospheric_pressure,
            water_stress_approach, upperT, lowerT);

    // Convert and return the results
    update(mole_fraction_co2_intercellular_op, second_c4photoC_output.Ci * 1e-6);       // mol / m^2 / s
    update(conductance_stomatal_h2o_op, second_c4photoC_output.Gs * 1e-3);              // mol / m^2 / s
    update(assimilation_gross_op, second_c4photoC_output.GrossAssim * 1e-6);            // mol / m^2 / s
    update(assimilation_net_op, second_c4photoC_output.Assim * 1e-6);                   // mol / m^2 / s
    update(evapotranspiration_penman_monteith_op, evapotrans_output.TransR * 1e-3);     // mol / m^2 / s
    update(evapotranspiration_penman_op, evapotrans_output.EPenman * 1e-3);             // mol / m^2 / s
    update(evapotranspiration_priestly_op, evapotrans_output.EPriestly * 1e-3);         // mol / m^2 / s
    update(temperature_leaf_op, leaf_temperature);                                      // deg. C
    update(conductance_boundary_h2o_op, evapotrans_output.boundary_layer_conductance);  // mol / m^2 / s
}

#endif
