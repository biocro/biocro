#ifndef COLLATZ_LEAF
#define COLLATZ_LEAF
#include "../modules.h"
#include "penman_monteith_leaf_temperature.hpp"

class collatz_leaf : public IModule {
    public:
        collatz_leaf()
            : IModule("collatz_leaf",
                    std::vector<std::string> {"solar", "vmax", "alpha", "kparm", "theta", "beta", "Rd", "upperT", "lowerT", "k_Q10", "water_stress_approach", "Catm", "rh", "b0", "b1", "temp", "water_vapor_pressure", "layer_wind_speed", "leafwidth", "leaf_reflectance", "leaf_transmittance", "stefan_boltzman"},  // The first vector is state the module requires.
                    std::vector<std::string> {})  // The second vector is state the module creates or modifies.
            {}
    private:
        state_map do_operation (state_map const& s) const;

        IModule const * const leaf_temperature_module = new penman_monteith_leaf_temperature();

        ~collatz_leaf() {
            delete leaf_temperature_module;
        }
};

#endif

