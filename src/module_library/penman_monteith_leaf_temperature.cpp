#include "penman_monteith_leaf_temperature.hpp"

state_map penman_monteith_leaf_temperature::do_operation(state_map const &s) const
{
    //check_state s(ss);
    //output_map(s);
    // From Thornley and Johnson 1990. pg 418. equation 14.11e.
    double const slope_water_vapor = s.at("slope_water_vapor");
    double const psychr_parameter = s.at("psychrometric_parameter");
    double const LHV = s.at("latent_heat_vaporization_of_water");
    double const ga = s.at("leaf_boundary_layer_conductance");  // m / s

    double constexpr volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol. TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the model state. (1 * R * temperature) / pressure
    double const gc = s.at("leaf_stomatal_conductance") * 1e-3 * volume_of_one_mole_of_air;  // m / s

    double const PhiN = s.at("leaf_net_irradiance");  // W / m^2. Leaf area basis.

    double const delta_t = (PhiN * (1 / ga + 1 / gc) - LHV * s.at("vapor_density_deficit"))
        /
        (LHV * (slope_water_vapor + psychr_parameter * (1 + ga / gc)));  // K. It is also degrees C because it's a change in temperature.

    state_map new_state { { "leaf_temperature", s.at("temp") + delta_t } };
    return new_state;
}
