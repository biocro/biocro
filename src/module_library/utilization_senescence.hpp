#ifndef m_utilization_senescence
#define m_utilization_senescence
#include "../modules.h"

class utilization_senescence: public IModule {
    public:
        utilization_senescence()
            : IModule("utilization_senescence",
                    std::vector<std::string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                    "Leaf", "Stem", "Root", "Rhizome", "Grain",
                    "rate_constant_leaf_senescence", "rate_constant_stem_senescence", "rate_constant_root_senescence", "rate_constant_rhizome_senescence", "rate_constant_grain_senescence",
                    "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                    "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                    "resistance_leaf_to_stem", "resistance_stem_to_grain", "resistance_stem_to_root", "resistance_stem_to_rhizome",
                    "remobilization_fraction", "grain_TTc"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
        virtual state_map do_operation(state_map const &s) const;
};

#endif
