#ifndef m_utilization_growth_and_senescence_module
#define m_utilization_growth_and_senescence_module
#include "../modules.h"

class utilization_growth_and_senescence_module: public IModule {
    public:
        utilization_growth_and_senescence_module()
            : IModule("utilization_growth_and_senescence_module",
                    std::vector<std::string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                    "Leaf", "Stem", "Root", "Rhizome", "Grain",
                    "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                    "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                    "resistance_leaf_to_stem", "resistance_stem_to_grain", "resistance_stem_to_root", "resistance_stem_to_rhizome",
                    "rate_constant_leaf", "rate_constant_stem", "rate_constant_root", "rate_constant_rhizome",
                    "remobilization_fraction", "grain_TTc"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
        virtual state_map do_operation(state_map const &s) const;
};

#endif
