#ifndef m_thermal_time_accumulator
#define m_thermal_time_accumulator
#include "../modules.h"

class thermal_time_accumulator : public IModule {
    public:
        thermal_time_accumulator()
            : IModule("thermal_time_accumulator",
                    std::vector<std::string> {"TTc", "temp", "tbase"},
                       std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const
        {
            double temp_diff = s.at("temp") - s.at("tbase");
            state_map result { {"TTc", (temp_diff > 0) ? temp_diff / 24 : 0 } };
            return result;
        }
};

#endif
