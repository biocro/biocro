#ifndef m_thermal_time_and_frost_senescence
#define m_thermal_time_and_frost_senescence
#include "../modules.h"

class thermal_time_and_frost_senescence : public IModule {
    public:
        thermal_time_and_frost_senescence()
            : ISenescence_module("thermal_time_and_frost_senescence",
                    std::vector<std::string> {"TTc", "leafdeathrate", "lat", "doy", "Tfrostlow", "Tfrosthigh", "seneLeaf", "seneStem", "seneRoot", "seneRhizome",
                    "kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
                    "leaf_senescence_index", "stem_senescence_index", "root_senescence_index", "rhizome_senescence_index"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation(state_vector_map const &s_history, state_vector_map const &d_history, state_map const &parameters) const;
};

#endif
