#ifndef PENMAN_MONTEITH_TRANSPIRATION_H
#define PENMAN_MONTEITH_TRANSPIRATION_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class penman_monteith_transpiration : public direct_module
{
   public:
    penman_monteith_transpiration(
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

          // Get pointers to output quantities
          leaf_transpiration_rate_op{get_op(output_quantities, "leaf_transpiration_rate")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "penman_monteith_transpiration"; }

   private:
    // References to input quantities
    double const& slope_water_vapor;
    double const& psychrometric_parameter;
    double const& latent_heat_vaporization_of_water;
    double const& leaf_boundary_layer_conductance;
    double const& leaf_stomatal_conductance;
    double const& leaf_net_irradiance;
    double const& vapor_density_deficit;

    // Pointers to output quantities
    double* leaf_transpiration_rate_op;
    // Main operation
    void do_operation() const;
};

string_vector penman_monteith_transpiration::get_inputs()
{
    return {
        "slope_water_vapor",                  // kg / m^3 / K
        "psychrometric_parameter",            // kg / m^3 / K
        "latent_heat_vaporization_of_water",  // J / kg
        "leaf_boundary_layer_conductance",    // m / s
        "leaf_stomatal_conductance",          // mmol / m^2 / s
        "leaf_net_irradiance",                // J / m^2 / s (leaf area basis)
        "vapor_density_deficit"               // kg / m^3
    };
}

string_vector penman_monteith_transpiration::get_outputs()
{
    return {
        "leaf_transpiration_rate"  // kg / m^2 / s (leaf area basis)
    };
}

void penman_monteith_transpiration::do_operation() const
{
    // From Thornley and Johnson 1990. pg 408. equation 14.4k.

    // TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the
    // model state. (1 * R * temperature) / pressure
    double volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol

    double gc = leaf_boundary_layer_conductance * 1e-3 * volume_of_one_mole_of_air;  // m / s

    double et_num =
        slope_water_vapor * leaf_net_irradiance +
        latent_heat_vaporization_of_water * psychrometric_parameter *
            leaf_boundary_layer_conductance * vapor_density_deficit;

    double et_denom =
        latent_heat_vaporization_of_water *
        (slope_water_vapor + psychrometric_parameter * (1 + leaf_boundary_layer_conductance / gc));

    double evapotranspiration = et_num / et_denom;  // kg / m^2 / s (leaf area basis)

    // Update the output quantity list
    update(leaf_transpiration_rate_op, evapotranspiration);  // kg / m^2 / s (leaf area basis)
}

}  // namespace standardBML
#endif
