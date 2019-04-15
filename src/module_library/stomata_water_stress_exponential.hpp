#ifndef m_stomata_water_stress_exponential
#define m_stomata_water_stress_exponential
#include "../modules.h"

class stomata_water_stress_exponential : public IModule {
    public:
        stomata_water_stress_exponential()
            : IModule("stomata_water_stress_exponential",
                    std::vector<std::string> {"soil_water_content", "soil_field_capacity", "soil_wilting_point"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            double wilting_point = s.at("soil_wilting_point");
            double field_capacity = s.at("soil_field_capacity");

            double slope = (1 - wilting_point) / (field_capacity - wilting_point);
            double intercept = 1 - field_capacity * slope;
            double theta = slope * s.at("soil_water_content") + intercept;
            state_map result { {"StomataWS", std::min(std::max((1 - exp(-2.5 * (theta - wilting_point)/(1 - wilting_point))) / (1 - exp(-2.5)), 1e-10), 1.0)} };
            return result;
        };
};

#endif
