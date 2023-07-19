#include "penman_monteith_leaf_temperature.h"

using standardBML::penman_monteith_leaf_temperature;

string_vector penman_monteith_leaf_temperature::get_inputs()
{
    return {
        "slope_water_vapor",                  // kg / m^3 / K
        "psychrometric_parameter",            // kg / m^3 / K
        "latent_heat_vaporization_of_water",  // J / kg
        "leaf_boundary_layer_conductance",    // m / s
        "leaf_stomatal_conductance",          // mmol / m^2 / s
        "leaf_net_irradiance",                // W / m^2. Leaf area basis
        "vapor_density_deficit",              // kg / m^3
        "temp"                                // degrees C
    };
}

string_vector penman_monteith_leaf_temperature::get_outputs()
{
    return {
        "leaf_temperature"  // degrees C
    };
}

void penman_monteith_leaf_temperature::do_operation() const
{
    // From Thornley and Johnson 1990. pg 418. equation 14.11e

    // TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the
    // model state. (1 * R * temperature) / pressure
    double constexpr volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol

    double const gc = leaf_stomatal_conductance * 1e-3 * volume_of_one_mole_of_air;  // m / s

    double const delta_t_num =
        leaf_net_irradiance * (1 / leaf_boundary_layer_conductance + 1 / gc) -
        latent_heat_vaporization_of_water * vapor_density_deficit;

    double const delta_t_denom =
        latent_heat_vaporization_of_water *
        (slope_water_vapor + psychrometric_parameter * (1 + leaf_boundary_layer_conductance / gc));

    double const delta_t = delta_t_num / delta_t_denom;  // K. It is also degrees C because it's a change in temperature.

    double leaf_temperature = temp + delta_t;

    // Update the output quantity list
    update(leaf_temperature_op, leaf_temperature);
}
