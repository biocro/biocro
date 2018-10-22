#ifndef BIG_LEAF_MULTILAYER_CANOPY
#define BIG_LEAF_MULTILAYER_CANOPY
#include "modules.h"

class big_leaf_multilayer_canopy : public IModule {
    public:
        big_leaf_multilayer_canopy()
            : IModule("big_leaf_multilayer_canopy",
                    std::vector<std::string> {"lat", "doy", "hour", "chil", "nlayers", "rh", "lai", "vmax1", "alpha1", "windspeed", "vmax1", "alpha1", "solar"},
                    std::vector<std::string> {})
            {}
    private:
        state_map do_operation (state_map const& s) const;
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

