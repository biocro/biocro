#include "FvCB.hpp"

state_map FvCB::do_operation(state_map const &s) const
{
    double Ci = s.at("Ci");
    double Gstar = s.at("Gstar");
    double rubisco_limited = s.at("Vcmax") * (Ci - Gstar) / (Ci + s.at("Kc") * (1.0 + s.at("O2") / s.at("Ko")));

    double rubp_limited = s.at("J") * (Ci - Gstar) / (s.at("electrons_per_carboxylation") * Ci + 2.0 * s.at("electrons_per_oxygenation") * Gstar);
    rubp_limited = std::max(rubp_limited, 0.0);  // This rate can't be negative.

    double tpu_limited = 3.0 * s.at("maximum_tpu_rate") / (1.0 - Gstar / Ci);

    double carboxylation_rate = std::min(rubisco_limited, std::min(rubp_limited, tpu_limited));  // The overall carboxylation rate is the rate of the slowest process.
    double net_assimilation_rate = carboxylation_rate - s.at("Rd");

    Ci = s.at("Ca") - net_assimilation_rate * 1.6 * s.at("atmospheric_pressure") / s.at("leaf_stomatal_conductance");

    return {
        { "carboxylation_rate", carboxylation_rate },
        { "net_assimilation_rate", net_assimilation_rate },
        { "Ci", Ci }
    };
}
