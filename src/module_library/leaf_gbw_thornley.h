#ifndef LEAF_GBW_THORNLEY_H
#define LEAF_GBW_THORNLEY_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "AuxBioCro.h"  // for leaf_boundary_layer_conductance_thornley

namespace standardBML
{
/**
 * @class leaf_gbw_thornley
 *
 * @brief Calculates the boundary layer conductance using the
 * `leaf_boundary_layer_conductance_thornley()` function.
 *
 * The `leaf_boundary_layer_conductance_thornley()` function returns a
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
class leaf_gbw_thornley : public direct_module
{
   public:
    leaf_gbw_thornley(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          height{get_input(input_quantities, "height")},
          windspeed{get_input(input_quantities, "windspeed")},
          minimum_gbw{get_input(input_quantities, "minimum_gbw")},
          windspeed_height{get_input(input_quantities, "windspeed_height")},

          // Get pointers to output quantities
          gbw_op{get_op(output_quantities, "gbw")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "leaf_gbw_thornley"; }

   private:
    // References to input quantities
    double const& height;
    double const& windspeed;
    double const& minimum_gbw;
    double const& windspeed_height;

    // Pointers to output quantities
    double* gbw_op;

    // Main operation
    void do_operation() const;
};

string_vector leaf_gbw_thornley::get_inputs()
{
    return {
        "height",           // m
        "windspeed",        // m / s
        "minimum_gbw",      // mol / m^2 / s
        "windspeed_height"  // m
    };
}

string_vector leaf_gbw_thornley::get_outputs()
{
    return {
        "gbw"  // mol / m^2 / s
    };
}

void leaf_gbw_thornley::do_operation() const
{
    // This is for about 20 degrees C at 100000 Pa
    constexpr double volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol

    // Calculate the boundary layer conductance
    const double gbw = leaf_boundary_layer_conductance_thornley(
        height,
        windspeed,
        minimum_gbw * volume_of_one_mole_of_air,
        windspeed_height);  // m / s

    // Update the output quantity list
    update(gbw_op, gbw / volume_of_one_mole_of_air);  // mol / m^2 / s
}

}  // namespace standardBML
#endif
