#include <algorithm>  // for std::min, std::max
#include <limits>     // for std::numeric_limits
#include "FvCB_assim.h"

double inf = std::numeric_limits<double>::infinity();

/**
 *  @brief Computes the net CO2 assimilation rate (and other values) using the
 *         Farquhar-von-Caemmerer-Berry model for C3 photosynthesis.
 *
 *  Here we use the model equations as described in Lochocki & McGrath (2023; in
 *  preparation). In this formulation, the net CO2 assimilation rate \f$ A_n \f$
 *  is given by
 *
 *  \f[
 *      A_n = \left( 1 - \Gamma^* / C \right) \cdot V_c - R_d, \qquad \text{(1)}
 *  \f]
 *
 *  where \f$ \Gamma^* \f$ is the CO2 compensation point in the absence of day
 *  respiration, \f$ C \f$ is the concentration of CO2 in the vicinity of
 *  Rubisco, \f$ V_c \f$ is the RuBP carboxylation rate, and \f$ R_d \f$ is the
 *  rate of day respiration. The RuBP carboxylation rate is taken to be the
 *  smallest of three potential carboxylation rates:
 *
 *  \f[ V_c = \text{min} \{ W_c, W_j, W_p \}. \qquad \text{(2)} \f]
 *
 *  Here, \f$ W_c \f$, \f$ W_j \f$, and \f$ W_p \f$ are the Rubisco-limited,
 *  RuBP-regeneration-limited, and triose-phosphate-utilization (TPU)-limited
 *  RuBP carboxylation rates, respectively. These rates are defined as follows:
 *
 *  \f[
 *      W_c = \frac{V_{c,max} \cdot C}{C + K_C \cdot
 *          \left( 1 + O / K_O \right)}, \qquad \text{(3)}
 *  \f]
 *
 *  \f[
 *      W_j = \frac{J \cdot C}{e_c \cdot C + 2 \cdot e_o \cdot \Gamma^*},
 *          \qquad \text{(4)}
 *  \f]
 *
 *  and
 *
 *  \f[
 *      W_p = \frac{3 \cdot C \cdot T_p}{C - \Gamma^* \cdot
 *          \left( 1 + 3 \cdot \alpha \right)}, \qquad \text{(5)}
 *  \f]
 *
 *  where \f$ V_{c,max} \f$ is the maximum Rubisco carboxylation rate,
 *  \f$ K_C \f$ and \f$ K_O \f$ are Michaelis-Menten constants for cabroxylation
 *  and oxygenation by Rubisco, \f$ O \f$ is the O2 concentration in the
 *  vicinity of Rubisco, \f$ J \f$ is the RuBP regeneration rate, \f$ e_c \f$ is
 *  the number of electrons per carboxylation, \f$ e_o \f$ is the number of
 *  electrons per oxygenation, \f$ T_p \f$ is the maximum rate of triose
 *  phosphate utilization, and \f$ \alpha \f$ is the fraction of glycolate
 *  carbon not returned to the chloroplast. Note that Equation `(5)` is only
 *  applicable when \f$ C > \Gamma^* \cdot ( 1 + 3 \cdot \alpha ) \f$; for
 *  smaller values of \f$ C \f$, \f$ W_p = \infty \f$.
 *
 *  Finally, it is also possible to use Equations `(1, 3-5)` to calculate the
 *  net CO2 assimilation rates that would occur when carboxylation is determined
 *  by either of the three potential rates:
 *
 *  \f[
 *      A_c = \left( 1 - \Gamma^* / C \right) \cdot W_c - R_d, \qquad \text{(6)}
 *  \f]
 *
 *  \f[
 *      A_j = \left( 1 - \Gamma^* / C \right) \cdot W_j - R_d, \qquad \text{(7)}
 *  \f]
 *
 *  and
 *
 *  \f[
 *      A_p = \left( 1 - \Gamma^* / C \right) \cdot W_p - R_d. \qquad \text{(8)}
 *  \f]
 *
 *  Note that the limits of the expressions for \f$ A_c \f$ and \f$ A_j \f$ in
 *  Equations `(6)` and `(7)` as \f$ C \rightarrow 0 \f$ are finite even when
 *  \f$ 1 - \Gamma^* / C \rightarrow -\infty \f$. So these net CO2 assimilation
 *  rates can be calculated even for \f$ C = 0 \f$. As discussed above, TPU
 *  cannot limit carboxylation or determine the net CO2 assimilation rate when
 *  \f$ C = 0 \f$.
 *
 *  @param [in] Ci The value of \f$ C \f$ in units of micromol / mol.
 *
 *  @param [in] Gstar The value of \f$ \Gamma^* \f$ in units of
 *              micromol / mol.
 *
 *  @param [in] J The value of \f$ J \f$ in units of micromol / m^2 / s.
 *
 *  @param [in] Kc The value of \f$ K_C \f$ in units of micromol / mol.
 *
 *  @param [in] Ko The value of \f$ K_O \f$ in units of mmol / mol.
 *
 *  @param [in] Oi The value of \f$ O \f$ in units of mmol / mol.
 *
 *  @param [in] Rd The value of \f$ R_d \f$ in units of micromol / m^2 / s.
 *
 *  @param [in] TPU The value of \f$ T_p \f$ in units of micromol / m^2 / s.
 *
 *  @param [in] Vcmax The value of \f$ V_{c,max} \f$ in units of
 *              micromol / m^2 / s.
 *
 *  @param [in] alpha_TPU The value of \f$ 0 \leq \alpha \leq 1 \f$
 *              (dimensionless).
 *
 *  @param [in] electrons_per_carboxylation The value of \f$ e_c \f$.
 *
 *  @param [in] electrons_per_oxygenation The value of \f$ e_o \f$.
 *
 *  @return A structure containing values of \f$ A_n \f$, \f$ A_c \f$,
 *          \f$ A_j \f$, \f$ A_p \f$, \f$ W_c \f$, \f$ W_j \f$, \f$ W_p \f$,
 *          in units of micromol / m^2 / s.
 */
FvCB_outputs FvCB_assim(
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
    FvCB_outputs result;

    // Calculate rates
    if (Ci == 0.0) {
        // RuBP-saturated net assimilation rate when Ci is 0
        double Ac0 =
            -Gstar * Vcmax / (Kc * (1 + Oi / Ko)) - Rd;  // micromol / m^2 / s

        // RuBP-regeneration-limited net assimilation when C is 0
        double Aj0 =
            -J / (2.0 * electrons_per_oxygenation) - Rd;  // micromol / m^2 / s

        // Store results; note that TPU cannot be limiting when
        // Ci < Gstar * (1 + 3 * alpha_TPU) and that An = max(Ac, Aj) when
        // Ci < Gstar.
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
