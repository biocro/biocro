#ifndef ED_ABC_BLC_H
#define ED_ABC_BLC_H

#include "../modules.h"
#include "../state_map.h"
#include "AuxBioCro.h"  // for leaf_boundary_layer_conductance

/**
 * @class ed_abc_blc
 *
 * @brief This module is a wrapper for the `leaf_boundary_layer_conductance` (BLC)
 * function in `AuxBioCro` (ABC). Currently only intended for use by Ed.
 */
class ed_abc_blc : public SteadyModule
{
   public:
    ed_abc_blc(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_abc_blc"),

          // Get pointers to input parameters
          windspeed(get_input(input_parameters, "windspeed")),
          leafwidth(get_input(input_parameters, "leafwidth")),
          temperature_air(get_input(input_parameters, "temp")),
          temperature_leaf(get_input(input_parameters, "temperature_leaf")),
          conductance_stomatal_h2o(get_input(input_parameters, "conductance_stomatal_h2o")),
          mole_fraction_h2o_atmosphere(get_input(input_parameters, "mole_fraction_h2o_atmosphere")),
          atmospheric_pressure(get_input(input_parameters, "atmospheric_pressure")),

          // Get pointers to output parameters
          conductance_boundary_h2o_op(get_op(output_parameters, "conductance_boundary_h2o"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // References to input parameters
    double const& windspeed;
    double const& leafwidth;
    double const& temperature_air;
    double const& temperature_leaf;
    double const& conductance_stomatal_h2o;
    double const& mole_fraction_h2o_atmosphere;
    double const& atmospheric_pressure;

    // Pointers to output parameters
    double* conductance_boundary_h2o_op;

    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_abc_blc::get_inputs()
{
    return {
        "windspeed",                     // m / s
        "leafwidth",                     // m
        "temp",                          // degrees C
        "temperature_leaf",              // degrees C
        "conductance_stomatal_h2o",      //  mol / m^2 / s
        "mole_fraction_h2o_atmosphere",  // dimensionless from mol / mol
        "atmospheric_pressure"           // Pa
    };
}

std::vector<std::string> ed_abc_blc::get_outputs()
{
    return {
        "conductance_boundary_h2o"  // mol / m^2 / s
    };
}

void ed_abc_blc::do_operation() const
{
    double constexpr volume_of_one_mole_of_air = 24.39e-3;                                    // m^3 / mol
    double const delta_t = temperature_leaf - temperature_air;                                // degrees C
    double const stomcond = conductance_stomatal_h2o * volume_of_one_mole_of_air;             // m / s
    double const water_vapor_pressure = mole_fraction_h2o_atmosphere * atmospheric_pressure;  // Pa

    double const blc = leaf_boundary_layer_conductance(
        windspeed,
        leafwidth,
        temperature_air,
        delta_t,
        stomcond,
        water_vapor_pressure);  // m / s

    double const blc_mol = blc / volume_of_one_mole_of_air;  // mol / m^2 / s

    // Update the output parameter list
    update(conductance_boundary_h2o_op, blc_mol);
}

#endif
