#ifndef m_leaf_water_stress_exponential
#define m_leaf_water_stress_exponential
#include "../modules.h"

class leaf_water_stress_exponential : public IModule {
    public:
        leaf_water_stress_exponential()
            : IModule("leaf_water_stress_exponential",
                    std::vector<std::string> {"soil_water_content", "soil_field_capacity", "phi2"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            state_map result { {"LeafWS",  std::min(std::max(pow(s.at("soil_water_content") / s.at("soil_field_capacity"), s.at("phi2")), 1e-10), 1.0)} };
            return result;
        };
};

#endif
