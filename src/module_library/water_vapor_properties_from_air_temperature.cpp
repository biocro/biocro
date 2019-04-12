#include "water_vapor_properties_from_air_temperature.hpp"

state_map water_vapor_properties_from_air_temperature::do_operation(state_map const &s) const
{
    double constexpr specific_heat_of_water = 1010;                // J / kg / K
    double constexpr molar_mass_of_water = 18.01528e-3;  // kg / mol
    double constexpr R = 8.314472;                       // joule / kelvin / mole.

    double const air_temperature = s.at("temp");  // degrees C.
    double const density_of_dry_air = TempToDdryA(air_temperature);  // kg / m^3. 
    double const latent_heat_vaporization_of_water = TempToLHV(air_temperature);  // J / kg
    double const saturation_water_vapor_pressure = saturation_vapor_pressure(air_temperature);  // Pa
    double const saturation_water_vapor_content = saturation_water_vapor_pressure / R / (air_temperature + 273.15) * molar_mass_of_water;  // kg / m^3. Convert from vapor pressure to vapor density using the ideal gas law. This is approximately right for temperatures what won't kill plants.
    double const vapor_density_deficit = saturation_water_vapor_content * (1 - s.at("rh"));

    state_map new_state {
        { "latent_heat_vaporization_of_water", latent_heat_vaporization_of_water },  // J / kg
        { "slope_water_vapor", TempToSFS(air_temperature) },  // kg / m^3 / K. It is also kg / m^3 / degrees C since it's a change in temperature.
        { "saturation_water_vapor_pressure", saturation_water_vapor_pressure },  // Pa
        { "water_vapor_pressure", saturation_water_vapor_pressure * s.at("rh") },  // Pa
        { "vapor_density_deficit", vapor_density_deficit },  // kg / m^3
        { "psychrometric_parameter", density_of_dry_air * specific_heat_of_water / latent_heat_vaporization_of_water},  // kg / m^3 / K
    };
    return new_state;
}
