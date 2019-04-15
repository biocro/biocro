#ifndef m_bucket_soil_drainage
#define m_bucket_soil_drainage
#include "../modules.h"

class bucket_soil_drainage : public IModule {
    public:
        bucket_soil_drainage()
            : IModule("bucket_soil_drainage",
                    std::vector<std::string> {"precipitation_rate", "soil_evaporation_rate", "soil_water_content", "soil_field_capacity", "soil_wilting_point",
                    "soil_saturation_capacity", "soil_depth", "soil_sand_content", "soil_saturated_conductivity", "soil_air_entry",
                    "soil_b_coefficient", "canopy_transpiration_rate"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            constexpr double g = 9.8;  // m / s^2. Acceleration due to gravity.
            constexpr double density_of_water_at_20_celcius = 998.2;  // kg m^-3.

            double field_capacity = s.at("soil_field_capacity");  // m^3 / m^3.
            double wilting_point = s.at("soil_wilting_point");  // m^3 / m^3.
            double water_content = s.at("soil_water_content");  // m^3 / m^3.
            double saturation_capacity = s.at("soil_saturation_capacity");  // m^3 / m^3.
            double soil_depth = s.at("soil_depth"); // meters;
            double precipitation_rate = s.at("precipitation_rate") / 1e3;  // m / s.

            /* soil_matric_potential is calculated as per "Dynamic Simulation of Water Deficit Effects upon Maize Yield" R. F. Grant Agricultural Systems. 33(1990) 13-39. */
            double soil_matric_potential = -exp(log(0.033) + log(field_capacity / water_content) / log(field_capacity / wilting_point) * log(1.5 / 0.033)) * 1e3;  // This last term converts from MPa to kPa.
            double hydraulic_conductivity = s.at("soil_saturated_conductivity") * pow(s.at("soil_air_entry") / soil_matric_potential, 2 + 3 / s.at("soil_b_coefficient"));  // kg s / m^3.

            double drainage = - hydraulic_conductivity * g / density_of_water_at_20_celcius;  // m / s.

            constexpr double runoff_rate = 1 / 3600;  // Runoff 1 m^3 / hr.
            double runoff = std::min(0.0, water_content - saturation_capacity) * runoff_rate * soil_depth; // m / s.

            double transpiration_rate = s.at("canopy_transpiration_rate") / density_of_water_at_20_celcius * 1000 / 10000 / 3600;  // m / s.
            double evaporation_rate = s.at("soil_evaporation_rate") / density_of_water_at_20_celcius;  // m / s.

            state_map result { {"soil_water_content", (precipitation_rate - transpiration_rate - evaporation_rate - runoff - drainage) / soil_depth * 3600} };  // m^3 / m^3 / hr;
            return result;
        }
};

#endif
