#ifndef PARAMETER_CALCULATOR
#define PARAMETER_CALCULATOR
#include "../modules.h"

class parameter_calculator : public IModule {
    public:
        parameter_calculator()
            : IModule("parameter_calculator",
                    std::vector<std::string> {"iSp", "Sp_thermal_time_decay", "Leaf", "LeafN_0",
                    "vmax_n_intercept", "vmax1", "alphab1", "alpha1"},
                      std::vector<std::string> {})
            {}
    private:
        state_map do_operation(state_map const &s) const;
};

#endif
