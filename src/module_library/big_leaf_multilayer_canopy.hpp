#ifndef BIG_LEAF_MULTILAYER_CANOPY
#define BIG_LEAF_MULTILAYER_CANOPY
#include "../modules.h"
#include "water_vapor_properties_from_air_temperature.hpp"
#include "penman_monteith_transpiration.hpp"
#include "convergence_iteration.hpp"

class big_leaf_multilayer_canopy : public IModule {
    public:
        big_leaf_multilayer_canopy()
            : IModule("big_leaf_multilayer_canopy",
                    std::vector<std::string> {
                        "lat", "doy", "hour", "chil", "nlayers",
                        "rh", "lai", "vmax1", "alpha1", "windspeed",
                        "solar", "kd",
                        // Required from collatz_leaf.
                        "k_Q10", "StomataWS",
                    },
                    std::vector<std::string> {})
            {}
    private:
        state_map do_operation (state_map const& s) const;

        IModule const * const water_vapor_module = new water_vapor_properties_from_air_temperature();
        IModule const * const leaf_assim_module = new collatz_leaf();
        IModule const * const leaf_evapo_module = new penman_monteith_transpiration();

        ~big_leaf_multilayer_canopy() {
            delete leaf_evapo_module;
            delete leaf_assim_module;
            delete water_vapor_module;
        }
};



class canac_with_collatz : public IModule {
    public:
        canac_with_collatz()
            : IModule("canac_with_collatz",
                    std::vector<std::string> {"lat", "doy", "hour", "chil", "nlayers", "rh", "lai", "vmax1", "alpha1", "windspeed", "vmax1", "alpha1", "solar"},
                    std::vector<std::string> {})
            {}
    private:
        state_map do_operation (state_map const& s) const;
};

#endif

