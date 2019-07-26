//TODO: Delete all of the template comments.
#ifndef ABA_DECAY
#define ABA_DECAY
#include "modules.h"

class aba_decay : public IModule {
    public:
        aba_decay()
            : IModule("aba_decay",
                    // TODO: Add unlisted parameters to the table in parameters.h, including a description and units.
                    std::vector<std::string> {"soil_aba_concentration", "aba_decay_constant"})  // The first vector is state the module requires.
                    std::vector<std::string> {"soil_aba_concentration"},  // The second vector is state the module creates or modifies.
            {}
    private:
        state_map do_operation (state_map const& s) const
        {
            double const decay_rate = -s.at("aba_decay_constant") * s.at("soil_aba_concentration");

            state_map result {
                {"soil_ABA_concentration", decay_rate},
            };
            return result;
        };
};

#endif
