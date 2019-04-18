#include "one_layer_soil_profile.hpp"
#include "BioCro.h"

state_map one_layer_soil_profile::do_operation(state_map const &s) const
{
    double soilEvap = s.at("soil_evaporation_rate") * 3600 * 1e-3 * 10000;  // Mg / ha / hr. 3600 s / hr * 1e-3 Mg / kg * 10000 m^2 / ha.
    double TotEvap = soilEvap + s.at("canopy_transpiration_rate");

    struct ws_str WaterS = watstr(s.at("precip"), TotEvap, s.at("soil_water_content"), s.at("soil_depth"), s.at("soil_field_capacity"),
            s.at("soil_wilting_point"), s.at("phi1"), s.at("phi2"), s.at("soil_saturation_capacity"), s.at("soil_sand_content"),
            s.at("soil_saturated_conductivity"), s.at("soil_air_entry"), s.at("soil_b_coefficient"));

    state_map derivs { { "soil_water_content", WaterS.awc - s.at("soil_water_content") } };
    return derivs;
}
