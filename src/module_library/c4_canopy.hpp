#ifndef C4_CANOPY
#define C4_CANOPY
#include "../modules.h"

class c4_canopy : public IModule {
    public:
        c4_canopy()
            : IModule("c4_canopy",
                    std::vector<std::string> {"lai", "doy", "hour", "solar", "temp",
                    "rh", "windspeed", "lat", "nlayers", "vmax1",
                    "alpha1", "kparm", "beta", "Rd", "Catm",
                    "b0", "b1", "theta", "kd", "chil",
                    "heightf", "LeafN", "kpLN", "lnb0", "lnb1",
                    "lnfun", "upperT", "lowerT", "leafwidth",
                    "et_equation", "StomataWS", "water_stress_approach",
                    "nileafn", "nkln", "kln", "nvmaxb0", "nvmaxb1",
                    "nalphab1", "nalphab0", "nRdb1", "nRdb0", "nkpLN",
                    "nlnb0", "nlnb1"},
                    std::vector<std::string> {})
        {}
    private:
        virtual state_map do_operation (state_map const &s) const;
};

#endif
