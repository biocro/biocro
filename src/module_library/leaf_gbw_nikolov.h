#ifndef LEAF_GBW_NIKOLOV_H
#define LEAF_GBW_NIKOLOV_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "water_and_air_properties.h"  // for saturation_vapor_pressure
#include "AuxBioCro.h"                 // for leaf_boundary_layer_conductance_nikolov

namespace standardBML
{
/**
 * @class leaf_gbw_nikolov
 *
 * @brief Calculates the boundary layer conductance using the
 * `leaf_boundary_layer_conductance_nikolov()` function.
 *
 * The `leaf_boundary_layer_conductance_nikolov()` function returns a
 * conductance with units of m / s. However, in BioCro, we prefer to use
 * molecular conductances, so we convert using an approximate conversion factor
 * that is only exact when both the leaf and air temperatures are 20 degrees C.
 * Errors due to this conversion are expected to be smaller than the errors
 * associated with calculating a boundary layer conductance from environmental
 * parameters.
 *
 * ### BioCro module implementation
 *
 * In BioCro, we use the following names for this model's inputs and outputs:
 * - ``'height'`` for the canopy height above the ground
 * - ``'windspeed'`` for the wind speed
 * - ``'gbw'`` for the leaf boundary layer conductance to water
 */
class leaf_gbw_nikolov : public direct_module
{
   public:
    leaf_gbw_nikolov(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          windspeed{get_input(input_quantities, "windspeed")},
          leafwidth{get_input(input_quantities, "leafwidth")},
          air_temperature{get_input(input_quantities, "temp")},
          leaf_temperature{get_input(input_quantities, "leaf_temperature")},
          Gs{get_input(input_quantities, "Gs")},
          rh{get_input(input_quantities, "rh")},
          minimum_gbw{get_input(input_quantities, "minimum_gbw")},

          // Get pointers to output quantities
          gbw_op{get_op(output_quantities, "gbw")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "leaf_gbw_nikolov"; }

   private:
    // References to input quantities
    double const& windspeed;
    double const& leafwidth;
    double const& air_temperature;
    double const& leaf_temperature;
    double const& Gs;
    double const& rh;
    double const& minimum_gbw;

    // Pointers to output quantities
    double* gbw_op;

    // Main operation
    void do_operation() const;
};

string_vector leaf_gbw_nikolov::get_inputs()
{
    return {
        "windspeed",         // m / s
        "leafwidth",         // m
        "temp",              // degrees C
        "leaf_temperature",  // degrees C
        "Gs",                // mol / m^2 / s
        "rh",                // dimensionless
        "minimum_gbw"        // mol / m^2 / s
    };
}

string_vector leaf_gbw_nikolov::get_outputs()
{
    return {
        "gbw"  // mol / m^2 / s
    };
}

void leaf_gbw_nikolov::do_operation() const
{
    // This is for about 20 degrees C at 100000 Pa
    constexpr double volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol

    // Get the temperature difference between the leaf and the air
    const double delta_t = leaf_temperature - air_temperature;  // degrees C

    // Determine the partial pressure of water vapor from the air temperature
    // and relative humidity
    const double SWVP = saturation_vapor_pressure(air_temperature);  // Pa
    const double water_vapor_pressure = rh * SWVP;                   // Pa

    // Calculate the boundary layer conductance. Here we need to convert
    // stomatal conductance from mol / m^2 / s to m / s.
    const double gbw = leaf_boundary_layer_conductance_nikolov(
        windspeed,
        leafwidth,
        air_temperature,
        delta_t,
        Gs * volume_of_one_mole_of_air,
        water_vapor_pressure,
        minimum_gbw * volume_of_one_mole_of_air);  // m / s

    // Update the output quantity list
    update(gbw_op, gbw / volume_of_one_mole_of_air);  // mol / m^2 / s
}

}  // namespace standardBML
#endif
