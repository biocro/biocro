#ifndef m_utilization_growth_flowering
#define m_utilization_growth_flowering
#include "../modules.h"

class utilization_growth_flowering: public IModule {
    public:
        utilization_growth_flowering()
            : IModule("utilization_growth_flowering",
                    std::vector<std::string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                    "Leaf", "Stem", "Root", "Rhizome", "Grain",
                    "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                    "KmLeaf", "KmStem", "KmRoot", "KmRhizome", "KmGrain",
                    "resistance_leaf_to_stem", "resistance_stem_to_grain", "resistance_stem_to_root", "resistance_stem_to_rhizome",
                    "GI_on", "GI_off", "FKF1_on", "FKF1_off", "CDF_on", "CDF_off", "FT_on", "FT_off", "FT_inhibition", "FKF1_timing", "CDF_timing", "dawn"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

#endif
