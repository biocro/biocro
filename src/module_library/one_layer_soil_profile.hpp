#ifndef ONE_LAYER_SOIL_PROFILE
#define ONE_LAYER_SOIL_PROFILE
#include "../modules.h"

class one_layer_soil_profile : public ISoil_evaporation_module {
    public:
        one_layer_soil_profile()
            : ISoil_evaporation_module("one_layer_soil_profile",
                    std::vector<std::string> {"lai", "temp", "solar", "soil_water_content",
                    "soil_field_capacity", "soil_wilting_point", "windspeed", "rh", "rsec",
                    "canopy_transpiration_rate", "precip", "soil_depth", "phi1", "phi2",
                    "StomataWS", "LeafWS", "soil_evaporation_rate"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_map const &s) const;
};

#endif
