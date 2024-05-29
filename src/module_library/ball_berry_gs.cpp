#include <cmath>
#include <stdexcept>
#include "ball_berry_gs.h"
#include "../framework/constants.h"       // for dr_boundary
#include "../framework/quadratic_root.h"  // for quadratic_root_plus
#include "water_and_air_properties.h"     // for saturation_vapor_pressure

using calculation_constants::eps_zero;
using physical_constants::dr_boundary;

/**
 *  @brief Calculates steady-state stomatal conductance to water vapor using the
 *  Ball-Berry model.
 *
 *  The Ball-Berry is a simple empirical model for the steady-state response of
 *  stomata to external conditions and was first described in Ball and Berry
 *  (1987). The main idea is that stomata open in response to brighter light or
 *  low CO2 availability, and close in response to low humidity (to limit water
 *  losses from transpiration). This idea can be expressed mathematically as
 *
 *  \f[
 *    g_{sw} = b_0 + b_1 \cdot \frac{ A_n \cdot h_s}{C_s} \quad
 *      \text{if} \; A_n \geq 0, \qquad \text{(1)}
 *  \f]
 *
 *  where \f$ g_{sw} \f$ is the stomatal conductance to water vapor diffusion,
 *  \f$ A_n \f$ is the net CO2 assimilation rate, \f$ b_0 \f$ and \f$ b_1 \f$
 *  are the Ball-Berry intercept and slope, \f$ h_s \f$ is the relative humidity
 *  at the leaf surface, and \f$ C_s \f$ is the CO2 concentration at the leaf
 *  surface. When \f$ A_n < 0 \f$, \f$ g_{sw} = b_0 \f$.
 *
 *  When using this model in the context of a crop growth simulation, it is
 *  necessary to determine \f$ h_s \f$ and \f$ C_s \f$ from the CO2
 *  concentration and relative humidity in the ambient air surrounding the crop.
 *  This can be accomplished using the following equations:
 *
 *  \f[
 *    C_s = C_a - \frac{A_n \cdot 1.37}{g_{bw}} \qquad \text{(2)}
 *  \f]
 *
 *  and
 *
 *  \f[
 *    h_s = \frac{-b + \sqrt{b^2 - 4 \cdot a \cdot c}}{2 \cdot a},
 *      \qquad \text{(3)}
 *  \f]
 *
 *  where
 *
 *  \f[ a = b_1 \cdot \frac{A_n}{C_s}, \f]
 *
 *  \f[ b = b_0 + g_{bw} - b_1 \cdot \frac{A_n}{C_s}, \f]
 *
 *  and
 *
 *  \f[
 *    c = - \left( g_{bw} \cdot h_a \cdot \frac{P_{w,sat}(T_a)}{P_{w,sat}(T_l)} +
 *      b_0 \right)
 *  \f].
 *
 *  See the "Using the Ball-Berry Model in Crop Growth Simulations" vignette for
 *  more information about this model and these equations.
 *
 *  References:
 *  - [Ball, Woodrow, and Berry. "A Model Predicting Stomatal Conductance and its Contribution
 *    to the Control of Photosynthesis under Different Environmental Conditions" (1987)]
 *    (https://doi.org/10.1007/978-94-017-0519-6_48)
 *
 *  @param [in] assimilation Net CO2 assimilation rate \f$ A_n \f$ in units of
 *              mol / m^2 / s.
 *
 *  @param [in] ambient_c Ambient CO2 concentration \f$ C_a \f$ in units of
 *              mol / mol.
 *
 *  @param [in] ambient_rh Ambient relative humidity \f$ h_a \f$ expressed as a
 *              fraction between 0 and 1 (dimensionless from Pa / Pa).
 *
 *  @param [in] bb_offset Ball-Berry offset \f$ b_0 \f$ in units of
 *              mol / m^2 / s.
 *
 *  @param [in] bb_slope Ball-Berry slope \f$ b_1 \f$ (dimensionless from
 *              [mol / m^2 / s] / [mol / m^2 / s]).
 *
 *  @param [in] gbw Boundary layer conductance to water vapor diffusion
 *              \f$ g_{bw} \f$ in units of mol / m^2 / s. For an isolated leaf,
 *              this should be the leaf boundary layer conductance; for a leaf
 *              within a canopy, this should be the total conductance including
 *              the leaf and canopy boundary layer conductances.
 *
 *  @param [in] leaf_temperature \f$ T_l \f$ in units of degrees C.
 *
 *  @param [in] ambient_air_temperature \f$ T_a \f$ in units of degrees C.
 *
 *  @return Stomatal conductance to water vapor diffusion \f$ g_{sw} \f$ in
 *          units of mol / m^2 / s
 */
stomata_outputs ball_berry_gs(
    double assimilation,            // mol / m^2 / s
    double ambient_c,               // mol / mol
    double ambient_rh,              // Pa / Pa
    double bb_offset,               // mol / m^2 / s
    double bb_slope,                // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
    double gbw,                     // mol / m^2 / s
    double leaf_temperature,        // degrees C
    double ambient_air_temperature  // degrees C
)
{
    // If An < 0, set b1 = 0 to ensure that gsw = b0 in Equation (1) as defined
    // above
    if (assimilation < 0) {
        bb_slope = 0.0;  // mol / m^2 / s
    }

    // Determine Cs using Equation (2) as defined above
    const double Cs = ambient_c -
                      (dr_boundary / gbw) * assimilation;  // mol / mol.

    if (Cs < 0.0) {
        throw std::range_error("Thrown in ball_berry_gs: Cs is less than 0.");
    }

    // Calculate some variables that will be used in later equations
    const double acs = assimilation / Cs;  // mol / m^2 / s

    const double swvp_ratio =
        saturation_vapor_pressure(ambient_air_temperature) /
        saturation_vapor_pressure(leaf_temperature);  // dimensionless

    // Calculate hs using Equation (3) as defined above
    const double a = bb_slope * acs;                              // mol / m^2 / s
    const double b = bb_offset + gbw - a;                         // mol / m^2 / s
    const double c = -ambient_rh * gbw * swvp_ratio - bb_offset;  // mol / m^2 / s

    // If hs is calculated to be larger than 1, this indicates dew formation. We
    // do not handle this in BioCro at the moment, so just limit hs to 1.
    const double hs = std::min(1.0, quadratic_root_plus(a, b, c));  // dimensionless

    if (hs < 0) {
        throw std::range_error("Thrown in ball_berry_gs: hs is less than 0.");
    }

    // Calculate stomatal conductance using Equation (1) above
    double const gswmol = a * hs + bb_offset;  // mol / m^2 / s

    return stomata_outputs{
        /* .cs = */ Cs * 1e6,  // micromol / mol
        /* .hs = */ hs,        // dimensionless
        /* .gsw = */ gswmol    // mol / m^2 / s
    };
}
