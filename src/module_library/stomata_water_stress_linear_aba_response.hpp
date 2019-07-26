#include <modules.h>

class stomata_water_stress_linear_and_aba_response : public IModule {
    public:
        stomata_water_stress_linear()
            : IModule("stomata_water_stress_linear",
                    std::vector<std::string> {"soil_water_content", "soil_field_capacity", "soil_wilting_point", "soil_aba_concentration", "aba_influence_coefficient", "max_b1"},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation(state_map const &s) const
        {
            double const slope = 1 / (s.at("soil_field_capacity") - s.at("soil_wilting_point"));
            double const intercept = 1 - s.at("soil_field_capacity") * slope;

            double const aba_effect = std::exp(s.at("soil_aba_concentration") / s.at("aba_influence_coefficient"));  // dimensionless. A value in the interval (0, 1] that will reduce the slope of the Ball-Berry model.

            state_map result {
                {"StomataWS", std::min(std::max(slope * s.at("soil_water_content") + intercept, 1e-10), 1.0)},
                {"b1", s.at("max_b1") * aba_effect},
            };
            return result;
        };
};


