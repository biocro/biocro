#ifndef WATER_VAPOR_PROPERTIES_FROM_AIR_TEMPERATURE
#define WATER_VAPOR_PROPERTIES_FROM_AIR_TEMPERATURE
#include "../modules.h"

class water_vapor_properties_from_air_temperature : public IModule {
    public:
        water_vapor_properties_from_air_temperature()
            : IModule("water_vapor_properties_from_air_temperature",
                    std::vector<std::string> {"temp", "rh"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation (state_map const &s) const;
};

#endif
