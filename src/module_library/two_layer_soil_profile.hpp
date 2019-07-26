#ifndef TWO_LAYER_SOIL_PROFILE
#define TWO_LAYER_SOIL_PROFILE
#include "../modules.h"

class two_layer_soil_profile : public IModule {
    public:
        two_layer_soil_profile()
            : IModule("two_layer_soil_profile",
                    std::vector<std::string> {"precip", "canopy_transpiration_rate", "cws1", "cws2", "soil_depth1",
                    "soil_depth2", "soil_depth3", "soil_field_capacity", "soil_wilting_point", "soil_saturation_capacity",
                    "soil_air_entry", "soil_saturated_conductivity", "soil_b_coefficient", "soil_sand_content", "phi1",
                    "phi2", "soil_type_indicator", "wsFun", "Root", "lai",
                    "temp", "solar", "windspeed", "rh", "hydrDist",
                    "rfl", "rsec", "rsdf", "StomataWS", "LeafWS"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_map const &s) const;
};

#endif
