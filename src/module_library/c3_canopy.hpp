#ifndef C3_CANOPY
#define C3_CANOPY
#include "../modules.h"

class c3_canopy : public ICanopy_photosynthesis_module {
    public:
        c3_canopy()
            : ICanopy_photosynthesis_module("c3_canopy",
                    std::vector<std::string> {"lai", "doy", "hour", "solar", "temp",
                    "rh", "windspeed", "lat", "nlayers", "vmax",
                    "jmax", "Rd", "Catm", "O2", "b0",
                    "b1", "theta", "kd", "heightf", "LeafN",
                    "kpLN", "lnb0", "lnb1", "lnfun", "chil", "StomataWS",
                    "water_stress_approach", "electrons_per_carboxylation", "electrons_per_oxygenation"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation (state_map const &s) const;
};

#endif
