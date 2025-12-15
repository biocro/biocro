#ifndef CONDUCTANCE_LIMITED_ASSIM_H
#define CONDUCTANCE_LIMITED_ASSIM_H

#include "../framework/constants.h"  // for dr_boundary, dr_stomata
#include "conductance_helpers.h"     // for sequential_conductance

/**
 *  @brief Computes the conductance-limited net CO2 assimilation rate.
 *
 *  The flow of CO2 from the atmosphere to the intercellular air spaces is
 *  modeled using a simple one-dimensional flux equation:
 *
 *  \f[ A_n = g_{tc} \cdot \left( C_a - C_i \right), \qquad \text{(1)} \f]
 *
 *  where \f$ A_n \f$ is the net CO2 assimilation rate, \f$ g_{tc} \f$ is the
 *  total conductance to CO2 diffusion across the boundary layer and through the
 *  stomata, \f$ C_a \f$ is the atmospheric CO2 concentration, and \f$ C_i \f$
 *  is the CO2 concentration in the intercellular air spaces. Here, \f$ C_i \f$
 *  cannot be negative, since it is not possible to have a negative number of
 *  CO2 molecules in a gas mixture; this places a restriction on the maximum
 *  value that \f$ A_n \f$ can take, since very large values would require
 *  \f$ C_i < 0 \f$ in Equation `(1)`.
 *
 *  To derive this restriction, we first solve Equation `(1)` for \f$ Ci \f$:
 *
 *  \f[ C_i = C_a - A_n / g_{tc}. \qquad \text{(2)} \f]
 *
 *  Now, if \f$ C_i = 0 \f$ when \f$ A_n = A_n^{max} \f$, where
 *  \f$ A_n^{max} \f$ is the maximum assimilation rate limited by CO2
 *  conductance, we can use Equation `(2)` to solve for \f$ A_n^{max} \f$:
 *
 *  \f[ A_n^{max} = C_a \cdot g_{tc}. \qquad \text{(3)} \f]
 *
 *  Finally, we note that the total conductance to CO2 diffusion can be
 *  calculated from the boundary layer conductance to H2O diffusion
 *  (\f$ g_{bw} \f$) and the stomatal conductance to H2O diffusion
 *  (\f$ g_{sw} \f$) according to
 *
 *  \f[
 *      \frac{1}{g_{tc}} = \frac{DR_b}{g_{bw}} + \frac{DR_s}{g_{sw}},
 *          \qquad \text{(4)}
 *  \f]
 *
 *  where \f$ DR_b \f$ is the ratio of diffusivities of H20 and CO2 in the
 *  boundary layer and \f$ DR_s \f$ is the same ratio in the stomata. Combining
 *  Equations `(3)` and `(4)`, we have:
 *
 *  \f[
 *      A_n^{max} = \frac{C_a}{\frac{DR_b}{g_{bw}} + \frac{DR_s}{g_{sw}}}.
 *          \qquad \text{(5)}
 *  \f]
 *
 *  This function implements Equation `(5)`.
 *
 *  @param [in] Ca The atmospheric CO2 concentration expressed as a mole
 *              fraction with units of micromol / mol.
 *
 *  @param [in] gbw The conductance to H2O diffusion in the boundary layer with
 *              units of mol / m^2 / s. For an isolated leaf, this should be the
 *              leaf boundary layer conductance; for a leaf within a canopy,
 *              this should be the total conductance including the leaf and
 *              canopy boundary layer conductances.
 *
 *  @param [in] gsw The conductance to H2O diffusion in the stomata with units
 *              of mol / m^2 / s.
 *
 *  @return The conductance-limited net CO2 assimilation rate in units of
 *              micromol / m^2 / s.
 */
inline double conductance_limited_assim(
    double Ca,   // micromol / mol
    double gbw,  // mol / m^2 / s
    double gsw   // mol / m^2 / s
)
{
    using physical_constants::dr_boundary;
    using physical_constants::dr_stomata;

    return Ca * sequential_conductance(gbw / dr_boundary, gsw / dr_stomata);  // micromol / m^2 / s
}

#endif
