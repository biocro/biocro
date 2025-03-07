#ifndef TEMPERATURE_RESPONSE_FUNCTIONS_H
#define TEMPERATURE_RESPONSE_FUNCTIONS_H

#include <cmath>                     // for exp, pow
#include "../framework/constants.h"  // for ideal_gas_constant

/**
 *  @brief Calculates the exponential term of the Arrhenius equation.
 *
 *  The Arrhenius equation gives the dependence of the rate constant of a
 *  chemical reaction on the absolute temperature and is often written as
 *
 *  > `k = A * e^(-E_a / R / T)` (1)
 *
 *  where `k` is the rate constant, `E_a` is the activation energy, `R` is the
 *  ideal gas constant, and `T` is the absolute temperature. As `T` approaches
 *  infinity, `E_a / R / T` approaches zero and so `k` approaches `A`. Thus,
 *  `A` represents the rate constant in the limit of infinite temperature. From
 *  a practical standpoint, `A` is not a particularly useful parameter and for
 *  this reason it is often written as
 *
 *  > `A = k_0 * e^c` (2)
 *
 *  where `k_0` is the rate constant measured at a reference temperature (e.g.
 *  25 degrees C) and `c` is a dimensionless parameter chosen so that `k = k_0`
 *  at the reference temperature.
 *
 *  In order to make this function reusable, it only calculates the exponential
 *  factor, which is always dimensionless. I.e., it calculates
 *  `e^(c - E_a / R / T)`.
 *
 *  @param [in] c Dimensionless parameter related to the reference temperature
 *                at which the rate constant was measured
 *
 *  @param [in] activation_energy Activation energy of the reaction in J / mol
 *
 *  @param [in] temperature_k Absolute temperature in Kelvin
 *
 *  @return Return The Arrhenius exponential `e^(c - E_a / R / T)`
 */
inline double arrhenius_exponential(
    double c,                  // dimensionless
    double activation_energy,  // J / mol
    double temperature_k       // Kelvin
)
{
    using physical_constants::ideal_gas_constant;  // J / k / mol
    return exp(c - activation_energy / (ideal_gas_constant * temperature_k));
}

/**
 *  @brief A typical Q10-based temperature response. The two temperatures below
 *  must be supplied in the same units, but the units can be either K or degrees
 *  C.
 *
 *  @param [in] temperature Temperature (K or degrees C)
 *
 *  @param [in] Tref Reference temperature (K or degrees C)
 *
 *  @return A scaling factor
 *
 *  ### Sources
 *  https://doi.org/10.1016/j.fcr.2010.07.007
 */
inline double Q10_temperature_response(
    double temperature,  // degrees C
    double Tref          // degrees C
)
{
    constexpr double Q10 = 2.0;
    return pow(Q10, (temperature - Tref) / 10.0);
}

/**
 *  @brief A temperature response function originally defined in Johnson,
 *         Eyring, and Williams (1942), but used more recently to describe the
 *         temperature response of triose phosphate utilization limitations in
 *         Harley et al. (1992), Sharkey et al. (2007), and Yang et al. (2016).
 *
 *  References:
 *  - [Johnson, F. H., Eyring, H. & Williams, R. W. Journal of Cellular and
 *    Comparative Physiology 20, 247–268 (1942)](https://doi.org/10.1002/jcp.1030200302)
 *
 *  - [Harley, P. C., Thomas, R. B., Reynolds, J. F. & Strain, B. R. Plant, Cell
 *    & Environment 15, 271–282 (1992)](https://doi.org/10.1111/j.1365-3040.1992.tb00974.x)
 *
 *  - [Sharkey, T. D., Bernacchi, C. J., Farquhar, G. D. & Singsaas, E. L.
 *    Plant, Cell & Environment 30, 1035–1040 (2007)](10.1111/j.1365-3040.2007.01710.x)
 *
 *  - [Yang, J. T., Preiser, A. L., Li, Z., Weise, S. E. & Sharkey, T. D. Planta
 *    243, 687–698 (2016)](10.1007/s00425-015-2436-8)
 *
 *  @param [in] c Dimensionless scaling parameter
 *
 *  @param [in] Ha Enthalpy of activation (J / mol)
 *
 *  @param [in] Hd Enthalpy of deactivation (J / mol)
 *
 *  @param [in] S Entropy (J / K / mol)
 *
 *  @param [in] temperature_k Absolute temperature (K)
 *
 *  @return The value of a reaction rate (or some other parameter) at the
 *          specified temperature; the units of the return value will depend on
 *          the particular situation where this function is being used.
 */
inline double johnson_eyring_williams_response(
    double c,             // dimensionless
    double Ha,            // J / mol
    double Hd,            // J / mol
    double S,             // J / K / mol
    double temperature_k  // K
)
{
    using physical_constants::ideal_gas_constant;  // J / k / mol

    double const top =
        temperature_k * arrhenius_exponential(c, Ha, temperature_k);

    double const bot =
        1.0 + arrhenius_exponential(S / ideal_gas_constant, Hd, temperature_k);

    return top / bot;
}

/**
 *  @brief A simple second-order polynomial equation describing the temperature
 *         response of a reaction rate or other parameter.
 *
 *  @param [in] c0 The zeroth-order coefficient, in the same units as the output
 *                 value
 *
 *  @param [in] c1 The first-order coefficient, in units of
 *                 (output units) / (degrees C)
 *
 *  @param [in] c2 The second-order coefficient, in units of
 *                 (output units) / (degrees C)^2
 *
 *  @param [in] temperature_c Temperature (degrees C)
 *
 */
inline double polynomial_response(
    double c0,            // output units
    double c1,            // output units * (degrees C)^(-1)
    double c2,            // output units * (degrees C)^(-2)
    double temperature_c  // degrees C
)
{
    return c0 + c1 * temperature_c + c2 * pow(temperature_c, 2);
}

#endif
