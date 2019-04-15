#ifndef BIOMASS_LEAF_N_LIMITATION
#define BIOMASS_LEAF_N_LIMITATION
#include "../modules.h"

class biomass_leaf_n_limitation : public IModule {
    public:
        biomass_leaf_n_limitation()
            : IModule(std::string("biomass_leaf_n_limitation"),
                    std::vector<std::string> {"LeafN_0", "Leaf", "Stem", "kln"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation (state_map const &s) const;

};

double biomass_leaf_nitrogen_limitation(state_map const &state);

#endif
