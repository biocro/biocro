#ifndef CONDUCTANCE_HELPERS_H
#define CONDUCTANCE_HELPERS_H

#include "water_and_air_properties.h"  // for molar_volume

/**
 *  @brief Convert a conductance value from a "molecular" basis (in units of
 *  mol / m^2 / s) to a "mass" basis (in units of m / s).
 *
 *  There are two different conventions for specifying gas concentrations and
 *  fluxes, which leads to two different units for conductances. As a reminder,
 *  for a one dimensional gas flow at steady state, the relationship between
 *  these variables is F = G * deltaC, where F is a flux, G is a conductance,
 *  and deltaC is the difference in gas concentration at the two ends of the
 *  gas path.
 *
 *  1. The "mass" basis: In this convention, gas concentrations are expressed as
 *     mass per volume (typically kg / m^3) and gas fluxes are expressed as mass
 *     per area per time (typically kg / m^2 / s). Thus, conductance must be
 *     expressed as length per time (typically m / s).
 *
 *  2. The "molecular" basis: In this convention, gas concentrations are
 *     expressed as a fraction of total molecules (typically mol / mol) and gas
 *     fluxes are expressed as molecules per area per time (typically
 *     mol / m^2 / s). Thus, conductance must be expressed as molecules per
 *     area per time (typically mol / m^2 / s).
 *
 *  Converting between these conventions is not always straightforward. In the
 *  simple case where temperature and pressure are constant across the gas path,
 *  it can be shown that G_mass = G_molecular * molar_volume, where molar_volume
 *  is the molar volume of an ideal gas at the given temperature and pressure.
 *
 *  We often use this conversion in BioCro, even when temperature is not
 *  constant across the gas path, because errors due to this approximation are
 *  expected to be small.
 *
 *  See `g_to_molecular` for the opposite conversion.
 *
 *  @param [in] pressure Pressure along the gas path in Pa.
 *
 *  @param [in] conductance Conductance along the gas path in mol / m^2 / s.
 *
 *  @param [in] temperature Temperature along the gas path in degrees C.
 *
 *  @return Conductance along the gas path in m / s.
 */
inline double g_to_mass(
    double const pressure,     // Pa
    double const conductance,  // mol / m^2 / s
    double const temperature   // degrees C
)
{
    double const mv_tl = molar_volume(temperature, pressure);  // m^3 / mol
    return conductance * mv_tl;                                // m / s
}

/**
 *  @brief Convert a conductance value from a "mass" basis (in units of
 *  m / s) to a "molecular" basis (in units of mol / m^2 / s).
 *
 *  See `g_to_mass` for more information.
 *
 *  @param [in] pressure Pressure along the gas path in Pa.
 *
 *  @param [in] conductance Conductance along the gas path in m / s.
 *
 *  @param [in] temperature Temperature along the gas path in degrees C.
 *
 *  @return Conductance along the gas path in mol / m^2 / s.
 */
inline double g_to_molecular(
    double const pressure,     // Pa
    double const conductance,  // mol / m^2 / s
    double const temperature   // degrees C
)
{
    double const mv_tl = molar_volume(temperature, pressure);  // m^3 / mol
    return conductance / mv_tl;                                // mol / m^2 / s
}

/**
 *  @brief Calculates the total conductance across two sequential gas paths.
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
 *  @param [in] conductance_1 Conductance along one step in any conductance
 *              units.
 *
 *  @param [in] conductance_2 Conductance along the other step in the same units
 *              as conductance_1.
 *
 *  @return Total conductance along both steps in the same units as
 *          conductance_1.
 */
inline double sequential_conductance(
    double const conductance_1,  // any conductance units
    double const conductance_2   // same units as conductance_1
)
{
    return 1.0 / (1.0 / conductance_1 + 1.0 / conductance_2);  // same units as conductance_1
}

#endif
