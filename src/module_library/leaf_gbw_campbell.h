#ifndef LEAF_GBW_CAMPBELL_H
#define LEAF_GBW_CAMPBELL_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "water_and_air_properties.h"    // for saturation_vapor_pressure, molar_volume
#include "boundary_layer_conductance.h"  // for leaf_boundary_layer_conductance_campbell

namespace standardBML
{
/**
 * @class leaf_gbw_campbell
 *
 * @brief Calculates the boundary layer conductance using the
 * `leaf_boundary_layer_conductance_campbell()` function.
 */
class leaf_gbw_campbell : public direct_module
{
   public:
    leaf_gbw_campbell(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          windspeed{get_input(input_quantities, "windspeed")},
          leafwidth{get_input(input_quantities, "leafwidth")},
          air_temperature{get_input(input_quantities, "temp")},
          leaf_temperature{get_input(input_quantities, "leaf_temperature")},
          air_pressure{get_input(input_quantities, "air_pressure")},

          // Get pointers to output quantities
          gbw_leaf_op{get_op(output_quantities, "gbw_leaf")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "leaf_gbw_campbell"; }

   private:
    // References to input quantities
    double const& windspeed;
    double const& leafwidth;
    double const& air_temperature;
    double const& leaf_temperature;
    double const& air_pressure;

    // Pointers to output quantities
    double* gbw_leaf_op;

    // Main operation
    void do_operation() const;
};

string_vector leaf_gbw_campbell::get_inputs()
{
    return {
        "windspeed",         // m / s
        "leafwidth",         // m
        "temp",              // degrees C
        "leaf_temperature",  // degrees C
        "air_pressure"       // Pa
    };
}

string_vector leaf_gbw_campbell::get_outputs()
{
    return {
        "gbw_leaf"  // m / s
    };
}

void leaf_gbw_campbell::do_operation() const
{
    // Calculate the boundary layer conductance. Here we need to convert
    // stomatal conductance from mol / m^2 / s to m / s.
    const double gbw_leaf = leaf_boundary_layer_conductance_campbell(
        air_temperature,
        leaf_temperature - air_temperature,
        leafwidth,
        windspeed,
        air_pressure);  // m / s

    // Update the output quantity list
    update(gbw_leaf_op, gbw_leaf);  // m / s
}

}  // namespace standardBML
#endif
