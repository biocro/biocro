#ifndef m_stomata_water_stress_sigmoid
#define m_stomata_water_stress_sigmoid
#include "../modules.h"

class stomata_water_stress_sigmoid : public IModule {
    public:
        stomata_water_stress_sigmoid()
            : IModule("stomata_water_stress_sigmoid",
                    std::vector<std::string> {"soil_water_content", "soil_field_capacity", "soil_wilting_point", "phi1"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            const double phi10 = (s.at("soil_field_capacity") + s.at("soil_wilting_point")) / 2;
            state_map result { {"StomataWS", std::min(std::max(1 / (1 + exp((phi10 - s.at("soil_water_content")) / s.at("phi1"))), 1e-10), 1.0)} };
            return result;
        };
};

#endif
