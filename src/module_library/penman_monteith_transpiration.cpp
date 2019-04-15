#include "penman_monteith_transpiration.hpp"

state_map penman_monteith_transpiration::do_operation(state_map const &s) const
{
    //check_state s(ss);
    // From Thornley and Johnson 1990. pg 408. equation 14.4k.
    double const slope_water_vapor = s.at("slope_water_vapor");  // kg / m^3 / K
    double const psychr_parameter = s.at("psychrometric_parameter");  // kg / m^3 / K
    double const LHV = s.at("latent_heat_vaporization_of_water");  // J / kg
    double const ga = s.at("leaf_boundary_layer_conductance");  // m / s

    double constexpr volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol. TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the model state. (1 * R * temperature) / pressure
    double const gc = s.at("leaf_stomatal_conductance") * 1e-3 * volume_of_one_mole_of_air;  // m / s

    double const PhiN = s.at("leaf_net_irradiance");  // W / m^2. Leaf area basis.

    //Rprintf("SlopeFS %f, PhiN %f, LHV %f, PsycParam %f, ga %f, vapor_density_deficit %f, conductance_in... %f\n", slope_water_vapor, PhiN, LHV, psychr_parameter, ga, s.at("vapor_density_deficit"), gc);
    double const evapotranspiration = (slope_water_vapor * PhiN + LHV * psychr_parameter * ga * s.at("vapor_density_deficit"))
        /
        (LHV * (slope_water_vapor + psychr_parameter * (1 + ga / gc)));

    //Rprintf("trans rate %f\n", evapotranspiration * 1e3 * 1e3 / 18);
    state_map new_state { { "leaf_transpiration_rate", evapotranspiration } };  // kg / m^2 / s. Leaf area basis.
    return new_state;
}
