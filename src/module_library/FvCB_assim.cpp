#include <algorithm>  // for std::min, std::max
#include <limits>     // for std::numeric_limits
#include "FvCB_assim.h"

double inf = std::numeric_limits<double>::infinity();

FvCB_str FvCB_assim(
    double Ci,                           // micromol / mol
    double Gstar,                        // micromol / mol
    double J,                            // micromol / m^2 / s
    double Kc,                           // micromol / mol
    double Ko,                           // mmol / mol
    double Oi,                           // mmol / mol
    double Rd,                           // micromol / m^2 / s
    double TPU,                          // micromol / m^2 / s
    double Vcmax,                        // micromol / m^2 / s
    double alpha_TPU,                    // dimensionless
    double electrons_per_carboxylation,  // self-explanatory units
    double electrons_per_oxygenation     // self-explanatory units
)
{
    // Initialize
    FvCB_str result;

    // Calculate the net CO2 assimilation rate using the method described in
    // "Avoiding Pitfalls When Using the FvCB Model"
    if (Ci == 0.0) {
        // RuBP-saturated net assimilation rate when Ci is 0
        double Ac0 =
            -Gstar * Vcmax / (Kc * (1 + Oi / Ko)) - Rd;  // micromol / m^2 / s

        // RuBP-regeneration-limited net assimilation when C is 0
        double Aj0 =
            -J / (2.0 * electrons_per_oxygenation) - Rd;  // micromol / m^2 / s

        // Store results
        result.An = std::max(Ac0, Aj0);  // micromol / m^2 / s
        result.Ac = Ac0;                 // micromol / m^2 / s
        result.Aj = Aj0;                 // micromol / m^2 / s
        result.Ap = inf;                 // micromol / m^2 / s
        result.Vc = 0.0;                 // micromol / m^2 / s
        result.Wc = 0.0;                 // micromol / m^2 / s
        result.Wj = 0.0;                 // micromol / m^2 / s
        result.Wp = inf;                 // micromol / m^2 / s

    } else {
        // RuBP-saturated carboxylation rate
        double Wc = Vcmax * Ci /
                    (Ci + Kc * (1.0 + Oi / Ko));  // micromol / m^2 / s

        // RuBP-regeneration-limited carboxylation rate (micromol / m^2 / s)
        double Wj = J * Ci /
                    (electrons_per_carboxylation * Ci +
                     2.0 * electrons_per_oxygenation * Gstar);

        // Triose-phosphate-utilization-limited carboxylation rate. There is an
        // asymptote at Ci = Gstar * (1 + 3 * alpha_TPU), and TPU cannot limit
        // the carboxylation rate for values of Ci below this asymptote. A
        // simple way to handle this is to make Wp infinite for
        // Ci <= Gstar * (1 + 3 * alpha_TPU), so that it is never limiting in
        // this case.
        double Wp =
            Ci > Gstar * (1.0 + 3.0 * alpha_TPU)
                ? 3.0 * TPU * Ci / (Ci - Gstar * (1.0 + 3.0 * alpha_TPU))
                : inf;  // micromol / m^2 / s

        // Assimilated carbon per carboxylation
        double a_per_c = (1.0 - Gstar / Ci);  // dimensionless

        // Limiting carboxylation rate
        double Vc = std::min(Wc, std::min(Wj, Wp));  // micromol / m^2 / s

        // Store results
        result.An = a_per_c * Vc - Rd;  // micromol / m^2 / s
        result.Ac = a_per_c * Wc - Rd;  // micromol / m^2 / s
        result.Aj = a_per_c * Wj - Rd;  // micromol / m^2 / s
        result.Ap = a_per_c * Wp - Rd;  // micromol / m^2 / s
        result.Vc = Vc;                 // micromol / m^2 / s
        result.Wc = Wc;                 // micromol / m^2 / s
        result.Wj = Wj;                 // micromol / m^2 / s
        result.Wp = Wp;                 // micromol / m^2 / s
    }

    return result;
}
