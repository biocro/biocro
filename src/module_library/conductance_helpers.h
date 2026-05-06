#ifndef CONDUCTANCE_HELPERS_H
#define CONDUCTANCE_HELPERS_H

#include <initializer_list>            // for std::initializer_list
#include "water_and_air_properties.h"  // for molar_volume

/**
 *  @brief Convert a conductance value from a "molar" basis (in units of
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
 *  2. The "molar" basis: In this convention, gas concentrations are expressed
 *     as a fraction of the total number of molecules (typically mol / mol),
 *     often referred to as "mole fractions", and gas fluxes are expressed as a
 *     number of molecules per area per time (typically mol / m^2 / s). Thus,
 *     conductance must be expressed as a number of molecules per area per time
 *     (typically mol / m^2 / s).
 *
 *  Converting between these conventions is not always straightforward. In the
 *  simple case where temperature and pressure are constant across the gas path,
 *  it can be shown that G_mass = G_molar * molar_volume, where molar_volume
 *  is the molar volume of an ideal gas at the given temperature and pressure.
 *
 *  We often use this conversion in BioCro, even when temperature is not
 *  constant across the gas path, because errors due to this approximation are
 *  expected to be small.
 *
 *  See `g_to_molar` for the opposite conversion.
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
 *  m / s) to a "molar" basis (in units of mol / m^2 / s).
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
inline double g_to_molar(
    double const pressure,     // Pa
    double const conductance,  // mol / m^2 / s
    double const temperature   // degrees C
)
{
    double const mv_tl = molar_volume(temperature, pressure);  // m^3 / mol
    return conductance / mv_tl;                                // mol / m^2 / s
}

double sequential_conductance(std::initializer_list<double> conductances);

#endif
