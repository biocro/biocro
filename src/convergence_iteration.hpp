#ifndef COLLATZ_LEAF
#define COLLATZ_LEAF
#include "modules.h"

class collatz_leaf : public IModule {
    public:
        collatz_leaf()
            : IModule("collatz_leaf",
                    std::vector<std::string> {"solar", "vmax", "alpha", "kparm", "theta", "beta", "Rd", "upperT", "lowerT", "k_Q10", "water_stress_approach", "Catm", "rh", "b0", "b1", "temp", "water_vapor_pressure", "layer_wind_speed", "leafwidth"},  // The first vector is state the module requires.
                    std::vector<std::string> {})  // The second vector is state the module creates or modifies.
            {}
    private:
        state_map do_operation (state_map const& s) const;
};

#endif

