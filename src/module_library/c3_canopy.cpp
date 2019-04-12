#include "c3_canopy.hpp"

state_map c3_canopy::do_operation(state_map const &s) const
{

    struct Can_Str can_result = c3CanAC(s.at("lai"), s.at("doy"), s.at("hour"), s.at("solar"), s.at("temp"),
            s.at("rh"), s.at("windspeed"), s.at("lat"), (int)s.at("nlayers"), s.at("vmax"),
            s.at("jmax"), s.at("Rd"), s.at("Catm"), s.at("O2"), s.at("b0"),
            s.at("b1"), s.at("theta"), s.at("kd"), s.at("heightf"), s.at("LeafN"),
            s.at("kpLN"), s.at("lnb0"), s.at("lnb1"), (int)s.at("lnfun"), s.at("chil"),
            s.at("StomataWS"), s.at("growth_respiration_fraction"), (int)s.at("water_stress_approach"), s.at("electrons_per_carboxylation"), s.at("electrons_per_oxygenation"));

    state_map new_state {
        { "canopy_assimilation_rate", can_result.Assim },  // Mg / ha / hr.
        { "canopy_transpiration_rate", can_result.Trans },  // Mg / ha / hr.
        { "GrossAssim", can_result.GrossAssim }
    };

    return new_state;
}
