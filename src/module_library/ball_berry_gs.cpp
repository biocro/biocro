#include <cmath>
#include <stdexcept>
#include "ball_berry_gs.h"
#include "../framework/constants.h"       // for dr_boundary
#include "../framework/quadratic_root.h"  // for quadratic_root_plus

using calculation_constants::eps_zero;
using physical_constants::dr_boundary;

/* Ball Berry stomatal conductance function */
double ball_berry_gs(
    double assimilation,                   // mol / m^2 / s
    double atmospheric_co2_concentration,  // mol / mol
    double atmospheric_relative_humidity,  // Pa / Pa
    double bb_offset,                      // mol / m^2 / s
    double bb_slope,                       // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
    double gbw                             // mol / m^2 / s
)
{
    double gswmol;  // mol / m^2 / s. stomatal conductance to water vapor.

    if (assimilation > 0) {
        const double Cs = atmospheric_co2_concentration -
                          (dr_boundary / gbw) * assimilation;  // mol / mol.

        if (Cs < 0.0) {
            throw std::range_error("Thrown in ball_berry_gs: Cs is less than 0.");
        }

        const double acs = assimilation / Cs;

        /* Calculate leaf surface relative humidity, hs, from the quadratic
         * equation:
         *  a * hs^2 + b * hs + c = 0
         *
         * This can be derived as follows:
         * At steady-state
         *  E = gs * (hs - hi)  -- Equation 1
         *  and
         *  E = gb * (ha - hs)  -- Equation 2
         *
         * Substitute gs in equation 1 using the Ball-Berry model:
         *  gs = b1 * A * hs / cs + b0,
         *
         * where A is assimilation rate, hs is relative humidity at the surface
         *  of the leaf, and cs is the CO2 mole fraction at the surface of the
         *  leaf.
         *
         * Assume hi = 1 based on saturation of water vapor in the internal
         * airspace of a leaf. Use the equality of equations 1 and 2 to solve
         * for hs, and it's a quadratic with the coefficients given in the code.
         */
        const double a = bb_slope * acs;       // Equivalent to a = bb_slope * assimilation / cs
        const double b = bb_offset + gbw - a;  // Equivalent to b = bb_offset + gbw - bb_slope * assimilation / cs
        const double c = -atmospheric_relative_humidity * gbw - bb_offset;

        const double hs = quadratic_root_plus(a, b, c);

        if (hs < 0) {
            throw std::range_error("Thrown in ball_berry_gs: hs is less than 0.");
        }

        gswmol = bb_slope * hs * acs + bb_offset;  // Ball-Berry equation (Collatz 1991, equation 1).
    } else {
        /* Set stomatal conductance to the minimum value, bb_offset */
        gswmol = bb_offset;
        // hs = (bb_offset + atmospheric_relative_humidity * gbw) / (bb_offset + gbw)
    }

    if (gswmol <= 0) {
        gswmol = 1e-2;
    }

    return gswmol * 1000;  // mmol / m^2 / s
}
