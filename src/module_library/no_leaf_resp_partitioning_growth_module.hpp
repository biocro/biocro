#ifndef m_no_leaf_resp_partitioning_growth_module
#define m_no_leaf_resp_partitioning_growth_module
#include "../modules.h"

class no_leaf_resp_partitioning_growth_module : public IModule {
    public:
        no_leaf_resp_partitioning_growth_module()
            : IModule("no_leaf_resp_partitioning_growth_module",
                    std::vector<std::string> {"TTc", "LeafWS", "temp", "canopy_assimilation_rate",
                    "Leaf", "Stem", "Root", "Rhizome", "Grain",
                    "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                    "mrc1", "mrc2"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

#endif
