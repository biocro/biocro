#ifndef m_soil_type_selector
#define m_soil_type_selector
#include "../modules.h"

class soil_type_selector : public IModule {
    public:
        soil_type_selector()
            : IModule("soil_type_selector",
                    std::vector<std::string> {"soil_type_identifier"},
                       std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const
        {
            soilText_str soil_properties = get_soil_properties(s.at("soil_type_indicator"));
            state_map result {
                { "soil_silt_content",  soil_properties.silt },
                { "soil_clay_content",  soil_properties.clay },
                { "soil_sand_content",  soil_properties.sand },
                { "soil_air_entry",  soil_properties.air_entry },
                { "soil_b_coefficient",  soil_properties.b },
                { "soil_saturated_conductivity",  soil_properties.Ks },
                { "soil_saturation_capacity",  soil_properties.satur },
                { "soil_field_capacity",  soil_properties.fieldc },
                { "soil_wilting_point",  soil_properties.wiltp },
                { "soil_bulk_density",  soil_properties.bulk_density }
            };
            return result;
        }
};

#endif
