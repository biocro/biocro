#ifndef thermal_time_senescense
#define thermal_time_senescense
#include "../modules.h"

class thermal_time_senescence : public IModule {
    public:
        thermal_time_senescence()
            : IModule("thermal_time_senescence",
                    std::vector<std::string> {"TTc", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                    "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                    "leaf_senescence_index", "stem_senescence_index", "root_senescence_index", "rhizome_senescence_index",
                    "mrc1", "mrc2"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

#endif
