#ifndef CANOPY_GBW_THORNLEY_H
#define CANOPY_GBW_THORNLEY_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "boundary_layer_conductance.h"  // for canopy_boundary_layer_conductance_thornley

namespace standardBML
{
/**
 * @class canopy_gbw_thornley
 *
 * @brief Calculates the boundary layer conductance using the
 * `canopy_boundary_layer_conductance_thornley()` function.
 */
class canopy_gbw_thornley : public direct_module
{
   public:
    canopy_gbw_thornley(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          canopy_height{get_input(input_quantities, "canopy_height")},
          windspeed{get_input(input_quantities, "windspeed")},
          min_gbw_canopy{get_input(input_quantities, "min_gbw_canopy")},
          windspeed_height{get_input(input_quantities, "windspeed_height")},

          // Get pointers to output quantities
          gbw_canopy_op{get_op(output_quantities, "gbw_canopy")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "canopy_gbw_thornley"; }

   private:
    // References to input quantities
    double const& canopy_height;
    double const& windspeed;
    double const& min_gbw_canopy;
    double const& windspeed_height;

    // Pointers to output quantities
    double* gbw_canopy_op;

    // Main operation
    void do_operation() const;
};

string_vector canopy_gbw_thornley::get_inputs()
{
    return {
        "canopy_height",    // m
        "windspeed",        // m / s
        "min_gbw_canopy",   // m / s
        "windspeed_height"  // m
    };
}

string_vector canopy_gbw_thornley::get_outputs()
{
    return {
        "gbw_canopy"  // mol / m^2 / s
    };
}

void canopy_gbw_thornley::do_operation() const
{
    // Calculate the boundary layer conductance
    const double gbw_canopy = canopy_boundary_layer_conductance_thornley(
        canopy_height,
        windspeed,
        min_gbw_canopy,
        windspeed_height);  // m / s

    // Update the output quantity list
    update(gbw_canopy_op, gbw_canopy);  // m / s
}

}  // namespace standardBML
#endif
