#include "priestley_transpiration.hpp"

state_map priestley_transpiration::do_operation(state_map const &s) const
{
    double const slope_water_vapor = s.at("slope_water_vapor");
    double const psychr_parameter = s.at("psychrometric_parameter");
    double const LHV = s.at("latent_heat_vaporization_of_water");
    double const evapotranspiration = 1.26 * slope_water_vapor * s.at("PhiN") / (LHV * (slope_water_vapor + psychr_parameter));

    state_map new_state { { "transpiration_rate", evapotranspiration } };  // kg / m^2 / s. Leaf area basis.
    return new_state;
}
