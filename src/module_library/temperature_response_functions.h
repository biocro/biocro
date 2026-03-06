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
 *  where `k` is the rate constant, `A` is a scaling factor, `E_a` is the
 *  activation energy, `R` is the ideal gas constant, and `T` is the absolute
 *  temperature.
 *
 *  For practical reasons, it is convenient to calculate a normalized reaction
 *  rate `k / k_0`, where `k_0` is the rate at a reference temperature `T_ref`.
 *  Using Equation (1), we can see that `k_0` is given by
 *
 *  > `k_0 = A * e^(-E_a / R / T_ref)` (2)
 *
 *  and hence the normalized reaction rate is
 *
 *  > `k / k_0 = e^(-E_a / R / T) / e^(-E_a / R / T_ref)` (3)
 *
 *  This can also be expressed as
 *
 *  > `k / k_0 = e^(c - E_a / R / T)` (4)
 *
 *  where `c = E_a / R / T_ref`.
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
    double const activation_energy,        // J / mol
    double const reference_temperature_k,  // Kelvin
    double const temperature_k             // Kelvin
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
    double const temperature,  // degrees C
    double const Tref          // degrees C
)
{
    constexpr double Q10 = 2.0;
    return pow(Q10, (temperature - Tref) / 10.0);
}

/**
 *  @brief Calculates the Eyring equation normalized to its value at a
 *  reference temperature.
 *
 *  The Eyring equation (Eyring 1935) gives the dependence of the rate constant
 *  of a chemical reaction on the absolute temperature:
 *
 *  > `k = kappa * k_b * T / h * e^(S / R - H / R / T)` (1)
 *
 *  where `k` is the rate constant, `kappa` is the transmission coefficient,
 *  `S` is the entropy of activation, `H` is the heat of activation, `T` is the
 *  absolute temperature, `R` is the ideal gas constant, `k_b` is Boltzmann's
 *  constant, and `h` is Planck's constant.
 *
 *  For practical reasons, it is convenient to calculate a normalized reaction
 *  rate `k / k_0`, where `k_0` is the rate at a reference temperature `T_ref`.
 *  Using Equation (1), we can see that `k_0` is given by
 *
 *  > `k_0 = kappa * k_b * T_ref / h * e^(S / R - H / R / T_ref)` (2)
 *
 *  and hence the normalized reaction rate is
 *
 *  > `k / k_0 = T / T_ref * e^(S / R - H / R / T) / e^(S / R - H / R / T_ref)` (3)
 *
 *  Note that the exponential terms simplify to `e^(H / R / T_ref - H / R / T)`,
 *  which can be recognized as the normalized Arrhenius equation (see
 *  `arrhenius_exponential()` for more details). Thus, this equation can be
 *  simplified to
 *
 *  > `k / k_0 = T / T_ref * Arr` (4)
 *
 *  where `Arr` is the value of the normalized Arrhenius equation with
 *  `E_a = H`. An interesting aspect of Equation (4) is that `S` no longer
 *  appears, indicating that this parameter does not influence the normalized
 *  Eyring response.
 *
 *  References:
 *  - [Eyring, Henry. Chemical Reviews 17, 65–77 (1935)]
 *    (https://doi.org/10.1021/cr60056a006)
 *
 *
 *  @param [in] H Heat of activation of the reaction (J / mol)
 *
 *  @param [in] reference_temperature_k Reference temperature (K)
 *
 *  @param [in] temperature_k Temperature at which the reaction is occurring (K)
 *
 *  @return The Arrhenius equation normalized to its value at the reference
 *          temperature (dimensionless)
 */
inline double eyring_response(
    double const H,                        // J / mol
    double const reference_temperature_k,  // K
    double const temperature_k             // K
)
{
    return temperature_k / reference_temperature_k *
        arrhenius_exponential(H, reference_temperature_k, temperature_k);
}

/**
 *  @brief Calculate the peaked Arrhenius equation normalized to its value at a
 *  reference temperature.
 *
 *  The peaked Arrhenius equation gives the dependence of the value of a
 *  parameter on the absolute temperature:
 *
 *  > `p = kappa * exp(-H_a / R / T) / [1 + exp(S / R - H_d / R / T)]` (1)
 *
 *  Where `p` is the parameter value, `kappa` is a scaling factor, `H_a` is the
 *  enthalpy of activation, `S` is the entropy, `H_d` is the enthalpy of
 *  deactivation, `T` is the absolute temperature, and `R` is the ideal gas
 *  constant.
 *
 *  For practical reasons, it is convenient to calculate a normalized parameter
 *  value `p / p_0`, where `p_0` is the parameter value at a reference
 *  temperature `T_ref`. Using Equation (1), we can see that `p_0` is given by
 *
 *  > `p_0 = kappa * exp(-H_a / R / T_ref) / [1 + exp(S / R - H_d / R / T_ref)]` (2)
 *
 *  and hence the normalized parameter value is
 *
 *  > `p / p_0 = exp(-H_a / R / T) / exp(-H_a / R / T_ref) *`
 *  > `          [1 + exp(S / R - H_d / R / T_ref)] /`
 *  > `          [1 + exp(S / R - H_d / R / T)]` (3)
 *
 *  The factor `exp(-H_a / R / T) / exp(-H_a / R / T_ref)` can be recognized as
 *  the normalized Arrhenius equation (see `arrhenius_exponential()` for more
 *  details). Thus, this equation can be simplified further to
 *
 *  > `p / p_0 = Arr * c / [1 + exp(S / R - H_d / R / T)]` (4)
 *
 *  where `Arr` is the value of the normalized Arrhenius equation with
 *  `E_a = H_a`, and `c` is a normalization factor defined by
 *  `c = 1 + exp(S / R - H_d / R / T_ref)`.
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
 *  @param [in] Ha Enthalpy of activation (J / mol)
 *
 *  @param [in] Hd Enthalpy of deactivation (J / mol)
 *
 *  @param [in] reference_temperature_k Reference temperature (K)
 *
 *  @param [in] S Entropy (J / K / mol)
 *
 *  @param [in] temperature_k Temperature at which the reaction is occurring (K)
 *
 *  @return The peaked Arrhenius equation normalized to its value at the
 *          reference temperature (dimensionless)
 */
inline double peaked_arrhenius_response(
    double const Ha,                       // J / mol
    double const Hd,                       // J / mol
    double const reference_temperature_k,  // Kelvin
    double const S,                        // J / K / mol
    double const temperature_k             // K
)
{
    using physical_constants::ideal_gas_constant;  // J / k / mol

    double const c = 1 + exp(S / ideal_gas_constant - Hd / ideal_gas_constant / reference_temperature_k);

    return arrhenius_exponential(Ha, reference_temperature_k, temperature_k) *
           c / (1 + exp(S / ideal_gas_constant - Hd / ideal_gas_constant / temperature_k));
}

/**
 *  @brief Calculate the Johnson-Eyring-Williams equation normalized to its
 *  value at a reference temperature.
 *
 *  The Johnson-Eyring-Williams equation was originally defined in Johnson,
 *  Eyring, and Williams (1942). It gives the dependence of the rate
 *  constant of a chemical reaction on the absolute temperature:
 *
 *  > `k = c'' * T * exp(-H_a / R / T) / [1 + exp(S / R - H_d / R / T)]` (1)
 *
 *  Where `k` is the rate constant, `c''` is a scaling factor, `H_a` is the
 *  enthalpy of activation, `S` is the entropy, `H_d` is the enthalpy of
 *  deactivation, `T` is the absolute temperature, and `R` is the ideal gas
 *  constant.
 *
 *  For practical reasons, it is convenient to calculate a normalized reaction
 *  rate `k / k_0`, where `k_0` is the rate at a reference temperature `T_ref`.
 *  Using Equation (1), we can see that `k_0` is given by
 *
 *  > `k_0 = c'' * T_ref * exp(-H_a / R / T_ref) / [1 + exp(S / R - H_d / R / T)]` (2)
 *
 *  and hence the normalized value is
 *
 *  > `k / k_0 = T / T_ref *`
 *  > `          exp(-H_a / R / T) / exp(-H_a / R / T_ref) *`
 *  > `          [1 + exp(S / R - H_d / R / T_ref)] /`
 *  > `          [1 + exp(S / R - H_d / R / T)]` (3)
 *
 *  The terms following `T / T_ref` can be recognized as the normalized peaked
 *  Arrhenius equation (see `peaked_arrhenius_response()` for more details.)
 *  Thus, this equation can be simplified further to
 *
 *  > `k / k_0 = T / T_ref * PA` (4)
 *
 *  where `PA` is the value of the normalized peaked Arrhenius equation.
 *
 *  Note that a similar equation is also derived in Sharpe and DeMichele (1977),
 *  where the main difference is the inclusion of an additional entropy term.
 *  As this new `S` approaches negative infinity, the equation from Sharpe and
 *  DeMichele approaches the Johnson-Eyring-Williams equation.
 *
 *  For an in-depth discussion of these equations and their role in plant
 *  modeling, see Murphy & Stinziano (2021) and Yin (2021).
 *
 *  References:
 *  - [Johnson, F. H., Eyring, H. & Williams, R. W. Journal of Cellular and
 *    Comparative Physiology 20, 247–268 (1942)](https://doi.org/10.1002/jcp.1030200302)
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
 *  @param [in] Ha Enthalpy of activation (J / mol)
 *
 *  @param [in] Hd Enthalpy of deactivation (J / mol)
 *
 *  @param [in] reference_temperature_k Reference temperature (K)
 *
 *  @param [in] S Entropy (J / K / mol)
 *
 *  @param [in] temperature_k Temperature at which the reaction is occurring (K)
 *
 *  @return The Johnson-Eyring-Williams equation normalized to its value at the
 *          reference temperature (dimensionless)
 */
inline double johnson_eyring_williams_response(
    double const Ha,                       // J / mol
    double const Hd,                       // J / mol
    double const reference_temperature_k,  // K
    double const S,                        // J / K / mol
    double const temperature_k             // K
)
{
    return temperature_k / reference_temperature_k *
           peaked_arrhenius_response(Ha, Hd, reference_temperature_k, S, temperature_k);
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
