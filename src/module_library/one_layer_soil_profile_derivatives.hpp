#ifndef m_one_layer_soil_profile_derivatives
#define m_one_layer_soil_profile_derivatives
#include "../modules.h"

class one_layer_soil_profile_derivatives : public IModule {
    public:
        one_layer_soil_profile_derivatives()
            : IModule("one_layer_soil_profile_derivatives",
                    std::vector<std::string> {"precipitation_rate", "soil_saturation_capacity", "soil_field_capacity", "soil_wilting_point", "soil_water_content",
                    "soil_depth", "soil_sand_content", "evapotranspiration", "soil_saturated_conductivity", "soil_air_entry",
                    "soil_b_coefficient", "acceleration_from_gravity"},
                    std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const
        {
            const double soil_water_content = s.at("soil_water_content"); // m^3 m^-3.
            const double soil_depth = s.at("soil_depth");

            double soil_water_potential = -exp(log(0.033) + ((log(s.at("soil_field_capacity")) - log(soil_water_content)) / (log(s.at("soil_field_capacity")) - log(s.at("soil_wilting_point"))) * (log(1.5) - log(0.033)))) * 1e3; // kPa.
            double hydraulic_conductivity = s.at("soil_saturated_conductivity") * pow(s.at("soil_air_entry") / soil_water_potential, 2 + 3 / s.at("soil_b_coefficient")); // Kg s m^-3.
            double J_w = -hydraulic_conductivity * (-soil_water_potential / (soil_depth * 0.5)) - s.at("acceleration_from_gravity") * hydraulic_conductivity; // kg m^-2 s^-1. Campbell, pg 129. I multiply soil depth by 0.5 to calculate the average depth.

            constexpr double density_of_water_at_20_celcius = 998.2; // kg m^-3

            double drainage = J_w / density_of_water_at_20_celcius;  // m s^-1.
            double precipitation_m_s = s.at("precipitation_rate") * 1e-3; // m s^-2.

            constexpr double second_per_hour = 3600;
            double runoff = (soil_water_content - s.at("soil_saturation_capacity")) * soil_depth / second_per_hour;  // m s^-1. The previous model drained everything in a single time-step. By default the time step was one hour, so use a rate that drains everything in one hour.
            double n_leach = runoff / 18 * (0.2 + 0.7 * s.at("soil_sand_content")) / second_per_hour;  // Base the rate on an hour for the same reason as was used with `runoff`.

            double evapotranspiration_volume = s.at("evapotranspiration") / density_of_water_at_20_celcius / 1e4 / second_per_hour;  // m^3 m^-2 s^-1


            state_map derivatives;
            derivatives["soil_water_content"] = (drainage + precipitation_m_s - runoff - evapotranspiration_volume) / soil_depth;
            derivatives["soil_n_content"] = -n_leach;
            return derivatives;
        }
};

#endif
