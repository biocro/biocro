#include <numeric>  // for std::accumulate
#include "conductance_helpers.h"

/**
 *  @brief Calculates the total conductance across sequential gas paths.
 *
 *  If gas flows from location A to location B and then to location C, there
 *  are two sequential steps: A to B (step 1), and B to C (step 2). Using one
 *  dimensional gas flow equations, we have:
 *
 *  F_1 = G_1 * (C_B - C_A)
 *  F_2 = G_2 * (C_C - C_B)
 *
 *  where F_1/F_2 are fluxes across steps 1 and 2, G_1/G_2 are conductances
 *  across steps 1 and 2, and C_A/C_B/C_C are concentrations at A, B, and C.
 *
 *  At steady state, F_1 = F_2 = F, and the flux across the entire path is
 *  given by
 *
 *  F = G_T * (C_C - C_A)
 *
 *  where G_T is the total conductance across the path. Solving for G_T, we can
 *  find that 1 / G_T = 1 / G_1 + 1 / G_2.
 *
 *  The reciprocal of a conductance is a resistance, so this can be re-expressed
 *  as R_T = R_1 + R_2, where R_1 = 1 / G_1 and R_2 = 1 / G_2 are the
 *  resistances across steps 1 and 2, and R_T = 1 / G_T is the total resistance.
 *
 *  In this form, it is straightforward to see that the equation can be extended
 *  to any number of sequential paths, where the total resistance is the sum of
 *  the individual resistances: R_T = R_1 + R_2 + ... + R_N. Then the total
 *  conductance is the reciprocal of the total resistance.
 *
 *  @param [in] conductances A list of conductances for each sequential gas
 *              path. Any conductance units are acceptable, but each conductance
 *              must have the same units (e.g. mol / m^2 / s) and should
 *              represent the conductance for the same gas species (e.g. CO2).
 *
 *  @return Total conductance along both steps in the same units as the input
 *          conductances.
 */
double sequential_conductance(
    std::initializer_list<double> conductances  // any conductance units
)
{
    // Lambda for calculating a reciprocal
    auto recip = [](double k) { return 1.0 / k; };

    // Lambda for accumulating reciprocals
    auto accum_recip = [&recip](double acc, double g) { return acc + recip(g); };

    // Total resistance along the path
    double const resistance = std::accumulate(
        conductances.begin(), conductances.end(),
        0.0,
        accum_recip);  // reciprocal of original conductance units

    // Total conductance along the path
    return recip(resistance);  // original conductance units
}
