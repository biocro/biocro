#ifndef PENMAN_MONTEITH_LEAF_TEMPERATURE_H
#define PENMAN_MONTEITH_LEAF_TEMPERATURE_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class penman_monteith_leaf_temperature : public direct_module
{
   public:
    penman_monteith_leaf_temperature(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          slope_water_vapor{get_input(input_quantities, "slope_water_vapor")},
          psychrometric_parameter{get_input(input_quantities, "psychrometric_parameter")},
          latent_heat_vaporization_of_water{get_input(input_quantities, "latent_heat_vaporization_of_water")},
          leaf_boundary_layer_conductance{get_input(input_quantities, "leaf_boundary_layer_conductance")},
          leaf_stomatal_conductance{get_input(input_quantities, "leaf_stomatal_conductance")},
          leaf_net_irradiance{get_input(input_quantities, "leaf_net_irradiance")},
          vapor_density_deficit{get_input(input_quantities, "vapor_density_deficit")},
          temp{get_input(input_quantities, "temp")},

          // Get pointers to output quantities
          leaf_temperature_op{get_op(output_quantities, "leaf_temperature")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "penman_monteith_leaf_temperature"; }

   private:
    // References to input quantities
    double const& slope_water_vapor;
    double const& psychrometric_parameter;
    double const& latent_heat_vaporization_of_water;
    double const& leaf_boundary_layer_conductance;
    double const& leaf_stomatal_conductance;
    double const& leaf_net_irradiance;
    double const& vapor_density_deficit;
    double const& temp;

    // Pointers to output quantities
    double* leaf_temperature_op;

    // Main operation
    void do_operation() const;
};

}  // namespace standardBML
#endif
