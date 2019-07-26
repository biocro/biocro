#ifndef m_soil_evaporation
#define m_soil_evaporation
#include "../modules.h"
#include "BioCro.h"

class soil_evaporation : public IModule {
    public:
        soil_evaporation()
            : IModule("soil_evaporation",
                    std::vector<std::string> {"lai", "temp", "solar", "soil_field_capacity", "soil_wilting_point", "soil_water_content",
                    "soil_sand_content", "soil_saturated_conductivity", "soil_air_entry", "soil_reflectance", "soil_transmission",
                    "soil_clod_size", "rsec"},
                    std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const
        {
            double soilEvap = SoilEvapo(s.at("lai"), 0.68, s.at("temp"), s.at("solar"), s.at("soil_water_content"),
                        s.at("soil_field_capacity"), s.at("soil_wilting_point"), s.at("windspeed"), s.at("rh"), s.at("rsec"),
                        s.at("soil_clod_size"), s.at("soil_reflectance"), s.at("soil_transmission"),
                        s.at("specific_heat"), s.at("stefan_boltzman"));  // kg / m^2 / s.
            state_map result { {"soil_evaporation_rate", soilEvap } };
            return result;
        }
};

#endif
