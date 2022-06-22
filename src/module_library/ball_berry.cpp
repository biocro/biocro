#include <cmath>
#include <stdexcept>
#include "ball_berry.hpp"

/* Ball Berry stomatal conductance function */
double ball_berry(double assimilation,  // mol / m^2 / s
                  double atmospheric_co2_concentration,  // mol / mol
                  double atmospheric_relative_humidity,  // Pa / Pa
                  double beta0,  // mol / m^2 / s
                  double beta1)  // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
{
    const double gbw = 1.2;  // mol / m^2 / s.  Boundary-layer conductance. Collatz et al. (1992) Aust. J. Plant Physiol. pg. 526. The units in the manuscript, micromole / m^2 / s, are wrong . They are actually mol / m^2 / s.
    double gswmol;  // mol / m^2 / s. stomatal conductance to water vapor.

    if (assimilation > 0) {
        const double Cs = atmospheric_co2_concentration - (1.4 / gbw) * assimilation;  // mol / mol.
        if (Cs < 0.0) {
            throw std::range_error("Thrown in ball_berry: Cs is less than 0.");
        }

        double acs = assimilation / Cs;
        if (acs < 1e-6) {
            acs = 1e-6;
        }

        /* Calculate leaf surface relative humidity, hs, from the quadratic equation:
         * a * hs^2 + b * hs + c = 0
         *
         * This can be derived as follows:
         * At steady-state
         *  E = gs * (hs - hi)  -- Equation 1
         *  and
         *  E = gb * (ha - hs)  -- Equation 2
         *
         * Substitute gs in equation 1 using the Ball-Berry model:
         *  gs = b1 * A * hs / cs + b0
         *
         *  Where A is assimilation rate, hs is relative humidity at the surface of the leaf, and cs is the CO2 mole fraction at the surface of the leaf.
         *
         * Assume hi = 1 based on saturation of water vapor in the interal airspace of a leaf.
         * Use the equality of equations 1 and 2 to solve for hs, and it's a quadratic with the coefficients given in the code.
         */
        const double a = beta1 * acs;  // Equivalent to a = beta1 * assimilation / cs
        const double b = beta0 + gbw - beta1 * acs;  // Equivalent to b = beta0 + gbw - beta1 * assimilation / cs
        const double c = -atmospheric_relative_humidity * gbw - beta0;

        const double root_term = b * b - 4 * a * c;
        const double hs = (-b + sqrt(root_term)) / (2 * a);

        gswmol = beta1 * hs * assimilation / Cs + beta0; // Ball-Berry equation (Collatz 1991, equation 1).
    } else {
        /* Set stomatal conductance to the minimum value, beta0 */
        gswmol = beta0;
        // hs = (beta0 + atmospheric_relative_humidity * gbw) / (beta0 + gbw)
    }

    if (gswmol <= 0) {
        gswmol = 1e-2;
    }

    return gswmol * 1000; // mmol / m^2 / s
}
