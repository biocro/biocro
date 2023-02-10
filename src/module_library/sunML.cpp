#include "sunML.h"

/**
 *  @brief Computes absorbed light from incident light for isolated leaves.
 *
 *  In general, the light absorbed by a leaf (`Iabs`) is related to the light
 *  incident on the leaf (`Iinc`) by the leaf absorptance (`abs`), which
 *  represents the fraction of light absorbed by the leaf. In other words,
 *  `Iabs = abs * Iinc`.
 *
 *  For a leaf in isolation, any light that is reflected or transmitted is not
 *  absorbed, so `abs = 1 - leaf_reflectance - leaf_transmittance`. This is the
 *  typical definition of leaf absorptance.
 *
 *  @param [in] leaf_reflectance The fractional amount of light reflected by
 *              the leaves (weighted across the appropriate wavelength band)
 *
 *  @param [in] leaf_transmittance The fractional amount of light transmitted by
 *              the leaves (weighted across the appropriate wavelength band)
 *
 *  @param [in] incident_light The amount of light incident on the leaves; for
 *              quantum fluxes, the units will typically be micromol / m^2 / s;
 *              for energy fluxes, the units will typically be J / m^2 / s.
 *
 *  @return The amount of radiation absorbed by the leaves expressed in the same
 *              units as `incident_light`.
 */
double absorbed_from_incident_leaf(
    double leaf_reflectance,    // dimensionless
    double leaf_transmittance,  // dimensionless
    double incident_light       // Light units such as `micromol / m^2 / s` or
                                //   `J / m^2 / s`
)
{
    // Return value has same units as `incident_light`
    return incident_light * (1 - leaf_reflectance - leaf_transmittance);
}

/**
 *  @brief Computes absorbed light from incident light for leaves in a canopy.
 *
 *  In general, the light absorbed by a leaf (`Iabs`) is related to the light
 *  incident on the leaf (`Iinc`) by the leaf absorptance (`abs`), which
 *  represents the fraction of light absorbed by the leaf. In other words,
 *  `Iabs = abs * Iinc`.
 *
 *  For a leaf in isolation, any light that is reflected or transmitted is not
 *  absorbed, so `abs = 1 - leaf_reflectance - leaf_transmittance`. This is the
 *  typical definition of leaf absorptance.
 *
 *  The situation is more complicated for a leaf within a plant canopy because
 *  any reflected or transmitted light can be subseqently reflected back to the
 *  leaf by other leaves in the canopy. In this case, it can be shown (see Notes
 *  below) that the fraction of absorbed light is given by
 *  `abs = (1 - leaf_reflectance - leaf_transmittance) / (1 - leaf_transmittance)`.
 *  Provided that the the leaf transmittance is not zero, this "canopy leaf
 *  absorptance" will be larger than the absorptance of a leaf in isolation.
 *
 *  Notes from EBL: So far, I have not actually found an explanation for this
 *  equation. It can be found as Eq 8.7c on page 204 of Thornley and Johnson
 *  (2000) Plant and Crop Modelling: A Mathematical Approach to Plant and Crop
 *  Physiology. However, I don't think this book explains where it comes from.
 *  Also note that when leaf transmittance is zero, the "canopy leaf
 *  absorptance" is identical to the absorptance of a leaf in isolation. From
 *  this it seems that the equation considers that only transmitted light will
 *  be reflected back to the leaf; thus, it may actually only be appropriate for
 *  upper or sunlit leaves, where any reflected light can be safely assumed to
 *  be "lost."
 *
 *  @param [in] leaf_reflectance The fractional amount of light reflected by
 *              the leaves (weighted across the appropriate wavelength band)
 *
 *  @param [in] leaf_transmittance The fractional amount of light transmitted by
 *              the leaves (weighted across the appropriate wavelength band)
 *
 *  @param [in] incident_light The amount of light incident on the leaves; for
 *              quantum fluxes, the units will typically be micromol / m^2 / s;
 *              for energy fluxes, the units will typically be J / m^2 / s.
 *
 *  @return The amount of radiation absorbed by the leaves expressed in the same
 *              units as `incident_light`.
 */
double absorbed_from_incident_in_canopy(
    double leaf_reflectance,    // dimensionless
    double leaf_transmittance,  // dimensionless
    double incident_light       // Light units such as `micromol / m^2 / s` or
                                //   `J / m^2 / s`
)
{
    return incident_light *
           (1 - leaf_reflectance - leaf_transmittance) /
           (1 - leaf_transmittance);  // same units as `incident_light`
}

/**
 *  @brief Computes total absorbed shortwave radiation from the
 *  photosynthetically active photon flux density (PPFD) incident on a leaf.
 *
 *  The total absorbed shortwave radiation is determined using the following
 *  steps:
 *  - determine the incident photosynthetically active radiation (PAR) using the
 *    energy content of PAR
 *  - determine the incident near-infrared radiation (NIR) from the incident PAR
 *    using the PAR energy fraction
 *  - determine the total incident radiation by adding the incident PAR and NIR
 *  - determine the total absorbed radiation using the leaf's reflection and
 *    transmission coefficients
 *
 *  @param [in] incident_ppfd Photosynthetically active photon flux density
 *              (PPFD) incident on a leaf expressed in micromol / m^2 / s
 *
 *  @param [in] par_energy_content The energy content of PPFD expressed in J /
 *              micromol
 *
 *  @param [in] par_energy_fraction The fraction of total shortwave energy
 *              contained in the PAR band expressed as a real number between 0
 *              and 1
 *
 *  @param [in] leaf_reflectance The fractional amount of shortwave radiation
 *              reflected by the leaf (weighted across all shortwave radiation)
 *
 *  @param [in] leaf_transmittance The fractional amount of shortwave radiation
 *              transmitted through the leaf (weighted across all shortwave
 *              radiation)
 *
 *  @return The total shortwave radiation absorbed by the leaf expressed in
 *          J / m^2 / s
 */
double absorbed_shortwave_from_incident_ppfd(
    double incident_ppfd,        // micromol / m^2 / s
    double par_energy_content,   // J / micromol
    double par_energy_fraction,  // dimensionless
    double leaf_reflectance,     // dimensionless
    double leaf_transmittance    // dimensionless
)
{
    double incident_par = incident_ppfd * par_energy_content;  // J / m^2 / s

    double incident_nir = incident_par *
                          (1 - par_energy_fraction) /
                          par_energy_fraction;  // J / m^2 /s

    double incident_shortwave = incident_par + incident_nir;  // J / m^2 / s

    return absorbed_from_incident_in_canopy(
        leaf_reflectance,
        leaf_transmittance,
        incident_shortwave);  // J / m^2 / s
}

/**
 *  @brief Computes an n-layered light profile from the direct light, diffuse
 *  light, leaf area index, solar zenith angle, and other parameters.
 *
 *  @param [in] ambient_ppfd_beam Photosynthetically active photon flux density
 *              (PPFD) for beam light passing through a surface perpendicular
 *              to the beam direction at the top of the canopy; this represents
 *              direct sunlight for a plant in a field
 *              (micromol / (m^2 beam) / s)
 *
 *  @param [in] ambient_ppfd_diffuse Photosynthetically active photon flux
 *              density (PPFD) for diffuse light at the top of the canopy; this
 *              represents diffuse light scattered out of the solar beam by the
 *              Earth's atmosphere for a plant in a field; as a diffuse flux
 *              density, this represents the flux through any surface
 *              (micromol / m^2 / s)
 *
 *  @param [in] lai Leaf area index (LAI) of the entire canopy, which represents
 *              the leaf area per unit of ground area (dimensionless from m^2
 *              leaf / m^2 ground)
 *
 *  @param [in] nlayers Integer number of layers in the canopy
 *
 *  @param [in] cosine_zenith_angle Cosine of the solar zenith angle
 *              (dimensionless)
 *
 *  @param [in] kd Extinction coefficient for diffuse light (dimensionless)
 *
 *  @param [in] chil Ratio of average projected areas of canopy elements on
 *              horizontal surfaces; for a spherical leaf distribution,
 *              `chil = 0`; for a vertical leaf distribution, `chil = 1`; for a
 *              horizontal leaf distribution, `chil` approaches infinity
 *              (dimensionless from m^2 / m^2)
 *
 *  @param [in] absorptivity The leaf absorptivity on a quantum basis
 *              (dimensionless from mol / mol)
 *
 *  @param [in] heightf Leaf area density, i.e., LAI per height of canopy (m^-1
 *              from m^2 leaf / m^2 ground / m height)
 *
 *  @return An n-layered light profile representing quantities within
 *          the canopy, including several photon flux densities and
 *          the relative fractions of shaded and sunlit leaves
 */
Light_profile sunML(
    double ambient_ppfd_beam,     // micromol / (m^2 beam) / s
    double ambient_ppfd_diffuse,  // micromol / m^2 / s
    double lai,                   // dimensionless from m^2 / m^2
    int nlayers,                  // dimensionless
    double cosine_zenith_angle,   // dimensionless
    double kd,                    // dimensionless
    double chil,                  // dimensionless from m^2 / m^2
    double absorptivity,          // dimensionless from mol / mol
    double heightf,               // m^-1 from m^2 leaf / m^2 ground / m height
    double par_energy_content,    // J / micromol
    double par_energy_fraction,   // dimensionless
    double leaf_transmittance,    // dimensionless
    double leaf_reflectance       // dimensionless
)
{
    if (nlayers < 1 || nlayers > MAXLAY) {
        throw std::out_of_range("nlayers must be at least 1 but no more than MAXLAY.");
    }
    if (cosine_zenith_angle > 1 || cosine_zenith_angle < -1) {
        throw std::out_of_range("cosine_zenith_angle must be between -1 and 1.");
    }
    if (kd > 1 || kd < 0) {
        throw std::out_of_range("kd must be between 0 and 1.");
    }
    if (chil < 0) {
        throw std::out_of_range("chil must be non-negative.");
    }
    if (absorptivity > 1 || absorptivity < 0) {
        throw std::out_of_range("absorptivity must be between 0 and 1.");
    }
    if (heightf <= 0) {
        throw std::out_of_range("heightf must greater than zero.");
    }

    // Calculate the leaf shape factor for an ellipsoidal leaf angle
    // distribution using the equation from page 251 of Campbell & Norman
    // (1998). We will use this value as `k`, the canopy extinction coefficient
    // for photosynthetically active radiation throughout the canopy. This
    // quantity represents the ratio of horizontal area to total area for leaves
    // in the canopy and is therefore dimensionless from
    // (m^2 ground) / (m^2 leaf).
    double zenith_angle = acos(cosine_zenith_angle);  // radians
    double k0 = sqrt(pow(chil, 2) + pow(tan(zenith_angle), 2));
    double k1 = chil + 1.744 * pow((chil + 1.182), -0.733);
    double k = k0 / k1;  // dimensionless

    double lai_per_layer = lai / nlayers;

    // Calculate the ambient direct PPFD through a surface parallel to the ground
    const double ambient_ppfd_beam_ground = ambient_ppfd_beam * cosine_zenith_angle;  // micromol / (m^2 ground) / s

    // Calculate the ambient direct PPFD through a unit area of leaf surface
    double ambient_ppfd_beam_leaf = ambient_ppfd_beam_ground * k;  // micromol / (m^2 leaf) / s

    Light_profile light_profile;
    for (int i = 0; i < nlayers; ++i) {
        // Get the cumulative LAI for this layer, which represents the total
        // leaf area above this layer
        const double cumulative_lai = lai_per_layer * (i + 0.5);

        // Calculate the amount of PPFD scattered out of the direct beam using
        // Equations 15.6 and 15.1 from Campbell & Norman (1998), following
        // example 15.2. This is a diffuse flux density representing the flux
        // through any surface.
        const double scattered_ppfd =
            ambient_ppfd_beam_ground * (exp(-k * sqrt(absorptivity) * cumulative_lai) -
                                        exp(-k * cumulative_lai));  // micromol / m^2 / s

        // Calculate the total flux of diffuse photosynthetically active light
        // in this layer by combining the scattered PPFD with the ambient
        // diffuse PPFD. Here we use Equation 15.6 with `alpha` = 1 and
        // `kbe(phi)` = kd.
        double diffuse_ppfd =
            ambient_ppfd_diffuse * exp(-kd * cumulative_lai) + scattered_ppfd;  // micromol / m^2 / s

        // Calculate the fraction of sunlit and shaded leaves in this canopy
        // layer using Equation 15.21.
        const double Ls = (1 - exp(-k * lai_per_layer)) * exp(-k * cumulative_lai) / k;  // dimensionless
        double sunlit_fraction = Ls / lai_per_layer;                                     // dimensionless
        double shaded_fraction = 1 - sunlit_fraction;                                    // dimensionless

        // Calculate an "average" incident PPFD for the sunlit and shaded leaves
        // that doesn't seem to be based on a formula from Campbell & Norman
        // (1998). It's interpreted as a flux density through a unit of leaf
        // area, but that may not be correct.
        double average_ppfd =
            (sunlit_fraction * (ambient_ppfd_beam_leaf + diffuse_ppfd) + shaded_fraction * diffuse_ppfd) *
            (1 - exp(-k * lai_per_layer)) / k;  // micromol / (m^2 leaf) / s

        // For values of cosine_zenith_angle close to or less than 0, in place
        // of the calculations above, we want to use the limits of the above
        // expressions as cosine_zenith_angle approaches 0 from the right:
        if (cosine_zenith_angle <= 1E-10) {
            ambient_ppfd_beam_leaf = ambient_ppfd_beam / k1;
            diffuse_ppfd = ambient_ppfd_diffuse * exp(-kd * cumulative_lai);
            sunlit_fraction = 0;
            shaded_fraction = 1;
            average_ppfd = 0;
        }

        // Store values of incident PPFD
        light_profile.sunlit_incident_ppfd[i] = ambient_ppfd_beam_leaf + diffuse_ppfd;  // micromol / (m^2 leaf) / s
        light_profile.incident_ppfd_scattered[i] = scattered_ppfd;                      // micromol / m^2 / s
        light_profile.shaded_incident_ppfd[i] = diffuse_ppfd;                           // micromol / (m^2 leaf) / s
        light_profile.average_incident_ppfd[i] = average_ppfd;                          // micromol / (m^2 leaf) / s
        light_profile.sunlit_fraction[i] = sunlit_fraction;                             // dimensionless from m^2 / m^2
        light_profile.shaded_fraction[i] = shaded_fraction;                             // dimensionless from m^2 / m^2
        light_profile.height[i] = (lai - cumulative_lai) / heightf;                     // m

        // Store values of absorbed PPFD
        light_profile.sunlit_absorbed_ppfd[i] =
            absorbed_from_incident_leaf(
                leaf_reflectance,
                leaf_transmittance,
                ambient_ppfd_beam_leaf + diffuse_ppfd);  // micromol / m^2 / s

        light_profile.shaded_absorbed_ppfd[i] =
            absorbed_from_incident_leaf(
                leaf_reflectance,
                leaf_transmittance,
                diffuse_ppfd);  // micromol / m^2 / s

        // Store values of absorbed solar energy (including PAR and NIR)
        light_profile.sunlit_absorbed_shortwave[i] =
            absorbed_shortwave_from_incident_ppfd(
                ambient_ppfd_beam_leaf + diffuse_ppfd,
                par_energy_content,
                par_energy_fraction,
                leaf_reflectance,
                leaf_transmittance);  // J / (m^2 leaf) / s

        light_profile.shaded_absorbed_shortwave[i] =
            absorbed_shortwave_from_incident_ppfd(
                diffuse_ppfd,
                par_energy_content,
                par_energy_fraction,
                leaf_reflectance,
                leaf_transmittance);  // J / (m^2 leaf) / s

        light_profile.average_absorbed_shortwave[i] =
            absorbed_shortwave_from_incident_ppfd(
                average_ppfd,
                par_energy_content,
                par_energy_fraction,
                leaf_reflectance,
                leaf_transmittance);  // J / (m^2 leaf) / s
    }
    return light_profile;
}
