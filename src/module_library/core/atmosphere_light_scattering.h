#ifndef ATMO_LIGHT_SCATTERING_H
#define ATMO_LIGHT_SCATTERING_H

/**
 * @file
 * @brief Models the partitioning of incoming solar radiation into direct and
 * diffuse components by the atmosphere.
 *
 * `AtmosphereLightScattering` is a value struct whose constructor computes
 * the direct and diffuse fractions of solar radiation at the Earth's surface
 * given the solar zenith angle, local atmospheric pressure, and two empirical
 * scattering parameters.  The model is based on Chapter 11 of Campbell &
 * Norman, _An Introduction to Environmental Biophysics_, 2nd edition (1998).
 *
 * In the canopy photosynthesis pipeline this is typically the first step:
 * `direct_fraction` and `diffuse_fraction` are multiplied by a measured
 * total solar flux (`solarR`) to obtain the beam and diffuse PPFD values
 * used to construct a `PhotoCore::CanopyLight` object.
 */
namespace PhotoCore {
struct AtmosphereLightScattering {
    double direct_transmittance;   //!< Atmospheric transmittance to direct radiation (dimensionless)
    double diffuse_transmittance;  //!< Atmospheric transmittance to diffuse radiation (dimensionless)
    double direct_fraction;        //!< Fraction of direct irradiance at the Earth's surface (dimensionless)
    double diffuse_fraction;       //!< Fraction of diffuse irradiance at the Earth's surface (dimensionless)
    
    
    AtmosphereLightScattering(
        double cosine_zenith_angle,
        double atmospheric_pressure,
        double atmospheric_transmittance,
        double atmospheric_scattering);
};

}
#endif
