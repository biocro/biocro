#ifndef COLLATZ_LEAF
#define COLLATZ_LEAF
#include "modules.h"

class collatz_leaf : public IModule {
    public:
        collatz_leaf()
            : IModule("collatz_leaf",
                    std::vector<std::string> {},  // The first vector is state the module creates or modifies.
                    std::vector<std::string> {"solar", "leaf_temperature", "vmax", "alpha", "kparm", "theta", "beta", "Rd", "upperT", "lowerT", "k_Q10", "intercellular_co2_molar_fraction", "water_stress_approach", "Catm", "RH", "b0", "b1"})  // The second vector is state the module requires.
            {}
    private:
        state_map do_operation (state_map const& s) const;
};

#endif

