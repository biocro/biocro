#ifndef TEMPERATURE_RESPONSE_FUNCTIONS_H
#define TEMPERATURE_RESPONSE_FUNCTIONS_H

#include <cmath>                     // for exp, pow
#include "../framework/constants.h"  // for ideal_gas_constant

/**
 *  @brief Calculates the Arrhenius equation normalized to its value at a
 *  reference temperature.
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
 *  where `k_0` is the rate constant measured at a reference temperature `T_ref`
 *  (e.g. 25 degrees C) and `c` is a dimensionless parameter chosen so that
 *  `k = k_0` at the reference temperature. Using this definition, Equation (1)
 *  becomes
 *
 *  > `k = k_0 * e^(c - E_a / R / T)` (3)
 *
 *  The value of `c` can be found by solving
 *  `k_0 = k_0 * e^(c - E_a / R / T_ref)` for `c`, which yields
 *
 *  > `c = E_a / R / T_ref` (4)
 *
 *  Here we return `k / k_0 = e^(c - E_a / R / T)` with `c` defined as in
 *  Equation (4).
 *
 *  @param [in] activation_energy Activation energy of the reaction (J / mol)
 *
 *  @param [in] reference_temperature_k Reference temperature (K)
 *
 *  @param [in] temperature_k Temperature at which the reaction is occurring (K)
 *
 *  @return The Arrhenius equation normalized to its value at the reference
 *          temperature (dimensionless)
 */
inline double arrhenius_exponential(
    double activation_energy,        // J / mol
    double reference_temperature_k,  // Kelvin
    double temperature_k             // Kelvin
)
{
    using physical_constants::ideal_gas_constant;  // J / k / mol

    double const c = activation_energy /
                     (ideal_gas_constant * reference_temperature_k);  // dimensionless

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
 *  @brief Calculates a relative reaction rate using the Eyring equation
 *
 *  The Eyring equation (Eyring 1935) gives the dependence of the rate constant
 *  of a chemical reaction on the absolute temperature:
 *
 *  > `k = kappa * k_b * T / h * e^(S / R) * e^(-H / R / T)` (1)
 *
 *  where `k` is the rate constant, `kappa` is the transmission coefficient,
 *  `S` is the entropy of activation, `H` is the heat of activation, `T` is the
 *  absolute temperature, `R` is the ideal gas constant, `k_b` is Boltzmann's
 *  constant, and `h` is Planck's constant.
 *
 *  This function is often used to calculate relative temperature responses; in
 *  other words, `r(T) = k(T) / k(T_ref)`, where `T_ref` is a reference
 *  temperature, often chosen to be 25 degrees C. Using Equation (1), the
 *  relative rate is given by
 *
 *  > `r = [kappa * k_b / (h * k(T_ref))] * T * e^(S / R) * e^(-H / R / T)` (2)
 *
 *  For simplicity, here we define a constant scaling factor `gamma`, equal to
 *  `kappa * k_b / (h * k(T_ref))`. Thus, Equation 2 can be written as
 *
 *  > `r = gamma * T * e^(S / R - H / R / T)` (3)
 *
 *  The user should ensure that `gamma` is chosen such that `r` evaluates to 1
 *  at the reference temperature.
 *
 *  References:
 *  - [Eyring, Henry. Chemical Reviews 17, 65–77 (1935)]
 *    (https://doi.org/10.1021/cr60056a006)
 *
 *  @param [in] gamma Constant scaling factor (K^-1)
 *
 *  @param [in] H Heat of activation (J / mol)
 *
 *  @param [in] S Entropy of activation (J / K / mol)
 *
 *  @param [in] temperature_k Absolute temperature (K)
 *
 *  @return Relative reaction rate `r` (dimensionless)
 */
inline double eyring_response(
    double gamma,         // K^-1
    double H,             // J / mol
    double S,             // J / K / mol
    double temperature_k  // K
)
{
    using physical_constants::ideal_gas_constant;  // J / k / mol

    return gamma * temperature_k *
           exp(S / ideal_gas_constant - H / (ideal_gas_constant * temperature_k));
}

/**
 *  @brief A temperature response function originally defined in Hall (1979),
 *  often used to represent peaked temperature responses in plant models.
 *
 *  The peaked Arrhenius response gives the dependence of the value of a
 *  parameter on the absolute temperature:
 *
 *  > `p = kappa * exp(-H_a / R / T) / [1 + exp(S / R) * exp(-H_d / R / T)]` (1)
 *
 *  Where `p` is the parameter value, `kappa` is a scaling factor, `H_a` is the
 *  enthalpy of activation, `S` is the entropy, `H_d` is the enthalpy of
 *  deactivation, `T` is the absolute temperature, and `R` is the ideal gas
 *  constant.
 *
 *  By defining `c` such that `kappa = exp(c)`, this can be written as:
 *
 *  > `p = exp(c - H_a / R / T) / [1 + exp(S / R - H_d / R / T)]` (2)
 *
 *  This is the form used here. Typically `c` is chosen such that `p` is equal
 *  to 1 at a particular reference temperature, often 25 degrees C. Then `p`
 *  represents a relative parameter value.
 *
 *  This function was originally based on the Johnson-Eyring-Williams equation,
 *  but with the linear `T` factor "omitted because it has little influence on
 *  the function" (Hall 1979). Without this factor, the peaked Arrhenius
 *  equation simplifies to the Arrhenius equation when `H_d` is infinitely
 *  large.
 *
 *  The name of this function is somewhat arbitrary, since it also resembles the
 *  Eyring equation and the Johnson-Eyring-Williams equation. In the literature,
 *  it is also sometimes called the "modified Arrhenius response." Here we call
 *  it the "peaked Arrhenius response" because of its peaked shape and to
 *  highlight the fact that it simplifies to the Arrhenius equation for certain
 *  parameter values.
 *
 *  Note that Johnson, Eyring, and Williams (1942) is sometimes erroneously
 *  cited as the source of the peaked Arrhenius equation. (For example, see
 *  Harley et al. 1992). However, this equation does not appear in the 1942
 *  publication, and seems to have originated in Hall (1979). It is more widely
 *  used than the Johnson-Eyring-Williams equation, most famously in Farquhar
 *  et al. (1980) and Sharkey et al. (2007).
 *
 *  For an in-depth discussion of these equations and their role in plant
 *  modeling, see Murphy & Stinziano (2021) and Yin (2021).
 *
 *  References:
 *  - [Hall, A. E. Oecologia 43, 299–316 (1979)]
 *    (https://doi.org/10.1007/BF00344957)
 *
 *  - [Johnson, F. H., Eyring, H. & Williams, R. W. Journal of Cellular and
 *    Comparative Physiology 20, 247–268 (1942)](https://doi.org/10.1002/jcp.1030200302)
 *
 *  - [Harley, P. C., Thomas, R. B., Reynolds, J. F. & Strain, B. R. Plant, Cell
 *    & Environment 15, 271–282 (1992)](https://doi.org/10.1111/j.1365-3040.1992.tb00974.x)
 *
 *  - [Farquhar, G. D., von Caemmerer, S. & Berry, J. A. Planta 149, 78–90 (1980)]
 *    (https://doi.org/10.1007/BF00386231)
 *
 *  - [Sharkey, T. D., Bernacchi, C. J., Farquhar, G. D. & Singsaas, E. L.
 *    Plant, Cell & Environment 30, 1035–1040 (2007)](10.1111/j.1365-3040.2007.01710.x)
 *
 *  - [Murphy, B. K. & Stinziano, J. R. New Phytologist 231, 2371–2381 (2021)]
 *    (https://doi.org/10.1111/nph.16883)
 *
 *  - [Yin, X. New Phytologist 231, 2113–2116 (2021)]
 *    (https://doi.org/10.1111/nph.17341)
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
inline double peaked_arrhenius_response(
    double c,             // dimensionless
    double Ha,            // J / mol
    double Hd,            // J / mol
    double S,             // J / K / mol
    double temperature_k  // K
)
{
    using physical_constants::ideal_gas_constant;  // J / k / mol

    double const top = exp(c - Ha / (ideal_gas_constant * temperature_k));

    double const bot = 1.0 +
                       exp(S / ideal_gas_constant - Hd / (ideal_gas_constant * temperature_k));

    return top / bot;
}

/**
 *  @brief A temperature response function originally defined in Johnson,
 *  Eyring, and Williams (1942), but used more recently to describe the
 *  temperature response of triose phosphate utilization limitations in Yang et
 *  al. (2016).
 *
 *  The Johnson-Eyring-Williams response gives the dependence of the rate
 *  constant of a chemical reaction on the absolute temperature:
 *
 *  > `k = c'' * T * exp(-H_a / (R * T)) / [1 + exp(S / R) * exp(-H_d / (R * T))]` (1)
 *
 *  Where `k` is the rate constant, `c''` is a scaling factor, `H_a` is the
 *  enthalpy of activation, `S` is the entropy, `H_d` is the enthalpy of
 *  deactivation, `T` is the absolute temperature, and `R` is the ideal gas
 *  constant.
 *
 *  By defining `c` such that `c'' = exp(c)`, this can be written as:
 *
 *  > `k = T * exp(c - H_a / (R * T)) / [1 + exp(S / R) * exp(-H_d / (R * T))]` (2)
 *
 *  This is the form used here. Typically `c` is chosen such that `k` is equal
 *  to 1 at a particular reference temperature, often 25 degrees C. Then `k`
 *  represents a relative reaction rate.
 *
 *  Note that a similar equation is also derived in Sharpe and DeMichele (1977),
 *  where the main difference is the inclusion of an additional entropy term.
 *  As this new `S` approaches negative infinity, the equation from Sharpe and
 *  DeMichele approaches the Johnson-Eyring-Williams equation.
 *
 *  For an in-depth discussion of these equations and their role in plant
 *  modeling, see Murphy & Stinziano (2021) and Yin (2021).
 *
 *  Note that this equation is equivalent to a "peaked Arrhenius response" with
 *  an additional multiplicative factor of `T`. We use this to simplify the
 *  calculations in the code.
 *
 *  References:
 *  - [Johnson, F. H., Eyring, H. & Williams, R. W. Journal of Cellular and
 *    Comparative Physiology 20, 247–268 (1942)](https://doi.org/10.1002/jcp.1030200302)
 *
 *  - [Yang, J. T., Preiser, A. L., Li, Z., Weise, S. E. & Sharkey, T. D. Planta
 *    243, 687–698 (2016)](10.1007/s00425-015-2436-8)
 *
 *  - [Sharpe, P. J. H. & DeMichele, D. W. Journal of Theoretical Biology 64,
 *    649–670 (1977)](https://doi.org/10.1016/0022-5193(77)90265-X)
 *
 *  - [Murphy, B. K. & Stinziano, J. R. New Phytologist 231, 2371–2381 (2021)]
 *    (https://doi.org/10.1111/nph.16883)
 *
 *  - [Yin, X. New Phytologist 231, 2113–2116 (2021)]
 *    (https://doi.org/10.1111/nph.17341)
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
    return temperature_k * peaked_arrhenius_response(c, Ha, Hd, S, temperature_k);
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
