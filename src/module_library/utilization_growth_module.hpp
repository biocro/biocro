#ifndef m_utilization_growth_module
#define m_utilization_growth_module
#include "../modules.h"

class utilization_growth_module : public IModule {
    public:
        utilization_growth_module()
            : IModule("utilization_growth_module",
                    std::vector<std::string> {"canopy_assimilation_rate",
                    "Leaf", "Stem", "Root", "Rhizome", "Grain",
                    "rate_constant_leaf", "rate_constant_stem", "rate_constant_root", "rate_constant_rhizome", "rate_constant_grain",
                    "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                    "resistance_leaf_to_stem", "resistance_stem_to_grain", "resistance_stem_to_root", "resistance_stem_to_rhizome",
                    "substrate_pool_leaf", "substrate_pool_stem", "substrate_pool_root", "substrate_pool_rhizome", "substrate_pool_grain"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
        virtual state_map do_operation(state_map const &s) const;
};

#endif
