#ifndef ED_C4PHOTO_H
#define ED_C4PHOTO_H

#include "../modules.h"
#include "../state_map.h"
#include "c4photo.h"

/**
 * @class ed_c4photo
 *
 * @brief Just a module wrapper for the `c4photo()` function.
 * Currently only intended for use by Ed.
 */
class ed_c4photo : public direct_module
{
   public:
    ed_c4photo(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          direct_module("ed_c4photo"),

          // Get pointers to input quantities
          collatz_PAR_flux_ip(get_ip(input_quantities, "collatz_PAR_flux")),
          temperature_leaf_ip(get_ip(input_quantities, "temperature_leaf")),
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

          // Get pointers to output quantities
          mole_fraction_co2_intercellular_op(get_op(output_quantities, "mole_fraction_co2_intercellular")),
          conductance_stomatal_h2o_op(get_op(output_quantities, "conductance_stomatal_h2o")),
          assimilation_gross_op(get_op(output_quantities, "assimilation_gross")),
          assimilation_net_op(get_op(output_quantities, "assimilation_net"))

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ed_c4photo"; }

   private:
    // Pointers to input quantities
    const double* collatz_PAR_flux_ip;
    const double* temperature_leaf_ip;
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

    // Pointers to output quantities
    double* mole_fraction_co2_intercellular_op;
    double* conductance_stomatal_h2o_op;
    double* assimilation_gross_op;
    double* assimilation_net_op;

    // Main operation
    void do_operation() const override;
};

string_vector ed_c4photo::get_inputs()
{
    return {
        "collatz_PAR_flux",                   // mol / m^2 / s
        "temperature_leaf",                   // deg. C
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
        "collatz_rubisco_temperature_lower"   // deg. C
    };
}

string_vector ed_c4photo::get_outputs()
{
    return {
        "mole_fraction_co2_intercellular",  // dimensionless from mol / mol
        "conductance_stomatal_h2o",         // mol / m^2 / s
        "assimilation_gross",               // mol / m^2 / s
        "assimilation_net"                  // mol / m^2 / s
    };
}

void ed_c4photo::do_operation() const
{
    // Get the inputs to c4photo and convert units as necessary
    double Qp = *collatz_PAR_flux_ip * 1e6;                  // micromole / m^2 / s
    double Tl = *temperature_leaf_ip;                        // deg. C
    double RH = *rh_ip;                                      // dimensionless
    double vmax = *collatz_vmax_ip * 1e6;                    // micromole / m^2 / s
    double alpha = *collatz_alpha_ip;                        // dimensionless
    double kparm = *collatz_k_ip;                            // mol / m^2 / s
    double theta = *collatz_theta_ip;                        // dimensionless
    double beta = *collatz_beta_ip;                          // dimensionless
    double Rd = *collatz_rd_ip * 1e6;                        // micromole / m^2 / s
    double bb0 = *ball_berry_intercept_ip;                   // mol / m^2 / s
    double bb1 = *ball_berry_slope_ip;                       // dimensionless
    double Gs_min = *conductance_stomatal_h2o_min_ip * 1e3;  // mmol / m^2 / s
    double StomaWS = *StomataWS_ip;                          // dimensionless
    double Ca = *mole_fraction_co2_atmosphere_ip * 1e6;      // micromole / mol
    double atmospheric_pressure = *atmospheric_pressure_ip;  // Pa
    double upperT = *collatz_rubisco_temperature_upper_ip;   // deg. C
    double lowerT = *collatz_rubisco_temperature_lower_ip;   // deg. C
    int water_stress_approach = 1;

    // Call c4photoC
    struct c4_str c4_results = c4photoC(
        Qp,
        Tl,
        RH,
        vmax,
        alpha,
        kparm,
        theta,
        beta,
        Rd,
        bb0,
        bb1,
        Gs_min,
        StomaWS,
        Ca,
        atmospheric_pressure,
        water_stress_approach,
        upperT,
        lowerT);

    // Convert and return the results
    update(mole_fraction_co2_intercellular_op, c4_results.Ci * 1e-6);  // mol / m^2 / s
    update(conductance_stomatal_h2o_op, c4_results.Gs * 1e-3);         // mol / m^2 / s
    update(assimilation_gross_op, c4_results.GrossAssim * 1e-6);       // mol / m^2 / s
    update(assimilation_net_op, c4_results.Assim * 1e-6);              // mol / m^2 / s
}

#endif
