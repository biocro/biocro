#ifndef m_stomata_water_stress_linear
#define m_stomata_water_stress_linear
#include "../modules.h"

class stomata_water_stress_linear : public IModule {
    public:
        stomata_water_stress_linear()
            : IModule("stomata_water_stress_linear",
                    std::vector<std::string> {"soil_water_content", "soil_field_capacity", "soil_wilting_point"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            double slope = 1 / (s.at("soil_field_capacity") - s.at("soil_wilting_point"));
            double intercept = 1 - s.at("soil_field_capacity") * slope;
            state_map result { {"StomataWS", std::min(std::max(slope * s.at("soil_water_content") + intercept, 1e-10), 1.0)} };
            return result;
        };
};

#endif
