#ifndef ED_C4PHOTO_H
#define ED_C4PHOTO_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "c4photo.h"

namespace standardBML
{
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
        : direct_module{},

          // Get pointers to input quantities
          collatz_PAR_flux{get_input(input_quantities, "collatz_PAR_flux")},
          temperature_leaf{get_input(input_quantities, "temperature_leaf")},
          rh{get_input(input_quantities, "rh")},
          collatz_vmax{get_input(input_quantities, "collatz_vmax")},
          collatz_alpha{get_input(input_quantities, "collatz_alpha")},
          collatz_k{get_input(input_quantities, "collatz_k")},
          collatz_theta{get_input(input_quantities, "collatz_theta")},
          collatz_beta{get_input(input_quantities, "collatz_beta")},
          collatz_rd{get_input(input_quantities, "collatz_rd")},
          ball_berry_intercept{get_input(input_quantities, "ball_berry_intercept")},
          ball_berry_slope{get_input(input_quantities, "ball_berry_slope")},
          conductance_stomatal_h2o_min{get_input(input_quantities, "conductance_stomatal_h2o_min")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          mole_fraction_co2_atmosphere{get_input(input_quantities, "mole_fraction_co2_atmosphere")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          collatz_rubisco_temperature_upper{get_input(input_quantities, "collatz_rubisco_temperature_upper")},
          collatz_rubisco_temperature_lower{get_input(input_quantities, "collatz_rubisco_temperature_lower")},
          conductance_boundary_h2o{get_input(input_quantities, "conductance_boundary_h2o")},

          // Get pointers to output quantities
          mole_fraction_co2_intercellular_op{get_op(output_quantities, "mole_fraction_co2_intercellular")},
          conductance_stomatal_h2o_op{get_op(output_quantities, "conductance_stomatal_h2o")},
          assimilation_net_op{get_op(output_quantities, "assimilation_net")}

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ed_c4photo"; }

   private:
    // References to input quantities
    double const& collatz_PAR_flux;
    double const& temperature_leaf;
    double const& rh;
    double const& collatz_vmax;
    double const& collatz_alpha;
    double const& collatz_k;
    double const& collatz_theta;
    double const& collatz_beta;
    double const& collatz_rd;
    double const& ball_berry_intercept;
    double const& ball_berry_slope;
    double const& conductance_stomatal_h2o_min;
    double const& StomataWS;
    double const& mole_fraction_co2_atmosphere;
    double const& atmospheric_pressure;
    double const& collatz_rubisco_temperature_upper;
    double const& collatz_rubisco_temperature_lower;
    double const& conductance_boundary_h2o;

    // Pointers to output quantities
    double* mole_fraction_co2_intercellular_op;
    double* conductance_stomatal_h2o_op;
    double* assimilation_net_op;

    // Main operation
    void do_operation() const override;
};

string_vector ed_c4photo::get_inputs()
{
    return {
        "collatz_PAR_flux",                   // mol / m^2 / s
        "temperature_leaf",                   // degrees C
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
        "collatz_rubisco_temperature_upper",  // degrees C
        "collatz_rubisco_temperature_lower",  // degrees C
        "conductance_boundary_h2o"            // mol / m^2 / s
    };
}

string_vector ed_c4photo::get_outputs()
{
    return {
        "mole_fraction_co2_intercellular",  // dimensionless from mol / mol
        "conductance_stomatal_h2o",         // mol / m^2 / s
        "assimilation_net"                  // mol / m^2 / s
    };
}

void ed_c4photo::do_operation() const
{
    // Specify inputs and convert units as necessary
    double Qp = collatz_PAR_flux * 1e6;                  // micromol / m^2 / s
    double vmax = collatz_vmax * 1e6;                    // micromol / m^2 / s
    double Rd = collatz_rd * 1e6;                        // micromol / m^2 / s
    double Gs_min = conductance_stomatal_h2o_min * 1e3;  // mmol / m^2 / s
    double Ca = mole_fraction_co2_atmosphere * 1e6;      // micromol / mol
    int water_stress_approach = 1;

    // Call c4photoC
    photosynthesis_outputs c4_results = c4photoC(
        Qp,
        temperature_leaf,
        rh,
        vmax,
        collatz_alpha,
        collatz_k,
        collatz_theta,
        collatz_beta,
        Rd,
        ball_berry_intercept,
        ball_berry_slope,
        Gs_min,
        StomataWS,
        Ca,
        atmospheric_pressure,
        water_stress_approach,
        collatz_rubisco_temperature_upper,
        collatz_rubisco_temperature_lower,
        conductance_boundary_h2o);

    // Convert and return the results
    update(mole_fraction_co2_intercellular_op, c4_results.Ci * 1e-6);  // mol / m^2 / s
    update(conductance_stomatal_h2o_op, c4_results.Gs * 1e-3);         // mol / m^2 / s
    update(assimilation_net_op, c4_results.Assim * 1e-6);              // mol / m^2 / s
}

}  // namespace standardBML
#endif
