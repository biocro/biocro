#ifndef PENMAN_MONTEITH_LEAF_TEMPERATURE
#define PENMAN_MONTEITH_LEAF_TEMPERATURE
#include "../modules.h"

class penman_monteith_leaf_temperature : public IModule {
    public:
        penman_monteith_leaf_temperature()
            : IModule("penman_monteith_leaf_temperature",
                    std::vector<std::string> {},
                    std::vector<std::string> {})
        {}
    private:
        state_map do_operation (state_map const &s) const;
};

#endif
