#include "sunML.h"

/**
 *  @brief Computes absorbed light from incident light for a thin layer of
 *  material.
 *
 *  Suppose light of intensity `I_0` (representing a flux density of photons or
 *  energy, expressed in units of photons per area per time, or energy per area
 *  per time) is incident on a thin layer of a material that reflects, absorbs,
 *  and transmits light. If `R` and `T` represent the fractions of light
 *  reflected by and transmitted through the layer, then we can calculate the
 *  light absorbed by the layer (`I_abs`) as follows:
 *
 *  `I_abs = I_0 * (1 - R - T)`     [Equation (1)]
 *
 *  In this equation, the factor `(1 - R - T)` represents the fraction of light
 *  absorbed by the layer. In BioCro, this equation is often used to calculate
 *  the light absorbed by a leaf or a thin layer of leaf material.
 *
 *  @param [in] R The fractional amount of light reflected by a thin layer of
 *              the material in the appropriate wavelength band.
 *
 *  @param [in] T The fractional amount of light transmitted by a thin layer of
 *              the material in the appropriate wavelength band.
 *
 *  @param [in] I_0 The amount of light incident on the material, perhaps
 *              restricted to a particular wavelength band; for quantum fluxes,
 *              the units will typically be micromol / m^2 / s; for energy
 *              fluxes, the units will typically be J / m^2 / s.
 *
 *  @return The amount of radiation absorbed by the material expressed in the
 *              same units as `I_0`.
 */
double thin_layer_absorption(
    double R,   // dimensionless
    double T,   // dimensionless
    double I_0  // Light units such as `micromol / m^2 / s` or `J / m^2 / s`
)
{
    return I_0 * (1 - R - T);  // same units as `I_0`
}

/**
 *  @brief Computes absorbed light from incident light for a thick layer of
 *  material.
 *
 *  Suppose light of intensity `I_0` (representing a flux density of photons or
 *  energy, expressed in units of photons per area per time, or energy per area
 *  per time) is incident on an infinitely thick layer of a material that
 *  reflects, absorbs, and transmits light. If `R` and `T` represent the
 *  fractions of light reflected by and transmitted through a thin layer of the
 *  material, then we can calculate the light absorbed by an infinitely thick
 *  layer of the material (`I_abs`) as follows:
 *
 *  `I_abs = I_0 * (1 - R - T) / (1 - T)`     [Equation (1)]
 *
 *  In this equation, the factor `(1 - R - T) / (1 - T)` represents the fraction
 *  of light absorbed by the thick layer. See the "Light Absorption by a Thick
 *  Layer" vignette for more information about this equation.
 *
 *  @param [in] R The fractional amount of light reflected by a thin layer of
 *              the material in the appropriate wavelength band; note that this
 *              reflectance is not necessary the same as would be measured from
 *              a thin layer in isolation.
 *
 *  @param [in] T The fractional amount of light transmitted by a thin layer of
 *              the material in the appropriate wavelength band; note that this
 *              transmittance is not necessary the same as would be measured
 *              from a thin layer in isolation.
 *
 *  @param [in] I_0 The amount of light incident on the material, perhaps
 *              restricted to a particular wavelength band; for quantum fluxes,
 *              the units will typically be micromol / m^2 / s; for energy
 *              fluxes, the units will typically be J / m^2 / s.
 *
 *  @return The amount of radiation absorbed by the material expressed in the
 *              same units as `I_0`.
 */
double thick_layer_absorption(
    double R,   // dimensionless
    double T,   // dimensionless
    double I_0  // Light units such as `micromol / m^2 / s` or `J / m^2 / s`
)
{
    return I_0 * (1 - R - T) / (1 - T);  // same units as `I_0`
}

/**
 *  @brief Computes energy flux in the near-infrared band (in J / m^2 / s) from
 *  the photosynthetically active photon flux density (PPFD; in
 *  micromol / m^2 / s).
 *
 *  We can calculate the energy in the photosynthetically active radiation (PAR)
 *  band from the PPFD using the average energy per photon in this band. Then,
 *  we can calculate the energy in the NIR band using the ratio of
 *  `(PAR energy) / (total energy)`, which we call `par_energy_fraction`. If all
 *  shortwave energy is split between the PAR and NIR bands, then
 *  `par_energy_fraction = PAR / (PAR + NIR)`, so
 *  `NIR / PAR = (1 - par_energy_fraction) / par_energy_fraction`.
 */
double nir_from_ppfd(
    double ppfd,                // micromol / m^2 / s
    double par_energy_content,  // J / micromol
    double par_energy_fraction  // dimensionless
)
{
    return ppfd * par_energy_content *
           (1 - par_energy_fraction) / par_energy_fraction;  // J / m^2 / s
}

/**
 *  @brief Computes total absorbed shortwave radiation from the
 *  photosynthetically active photon flux density (PPFD) incident on a leaf.
 *
 *  The total absorbed shortwave radiation is determined using the following
 *  steps:
 *  - determine the incident near-infrared radiation (NIR) from the incident
 *    PPFD using the PAR energy content and PAR energy fraction
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

    double incident_nir = nir_from_ppfd(
        incident_ppfd, par_energy_content, par_energy_fraction);  // J / m^2 /s

    double incident_shortwave = incident_par + incident_nir;  // J / m^2 / s

    return thin_layer_absorption(
        leaf_reflectance,
        leaf_transmittance,
        incident_shortwave);  // J / m^2 / s
}

/**
 *  @brief Computes total radiation (direct and downscattered) using
 *  Equation 15.15 from Campbell & Norman (1998).
 *
 *  The total radiation `Q_t` is given by
 *
 *  `Q_t = tau * Q_o`                       [Equation (1)]
 *
 *  where `Q_o` is the radiation just above the canopy and `tau` is a
 *  transmission factor whose value decreases deeper into the canopy. `tau`
 *  is given by Equation 15.6:
 *
 *  `tau = exp(-sqrt(alpha) * k * ell)`       [Equation (2)]
 *
 *  where `alpha` is the leaf absorptivity, `k` is the canopy extinction
 *  coefficient, and `ell` is the cumulative leaf area index. Combining
 *  Equations (1) and (2), we have
 *
 *  `Q_t = Q_o * exp(-sqrt(alpha) * k * ell)`  [Equation (3)]
 *
 *  which is implemented in the code below.
 *
 *  Note that `Q_o` and `Q_t` are expressed on a ground area basis; in other
 *  words, they are fluxes of photons or energy passing through a horizontal
 *  surface.
 *
 *  Also note that this equation can be used for direct or diffuse radiation,
 *  provided appropriate values of `Q_o`, `alpha` and `k` are used.
 *
 *  @param [in] Q_o The radiation just above the canopy expressed on a ground
 *              area basis using either quantum flux density
 *              (micromol / m^2 / s) or energy density (J / m^2 / s).
 *
 *  @param [in] k The (dimensionless) canopy extinction coefficient.
 *
 *  @param [in] alpha The (dimensionless) leaf absorptivity.
 *
 *  @param [in] ell The cumulative leaf area index, which is dimensionless from
 *              m^2 leaf / m^2 ground.
 *
 *  @return The total radiation (direct and downscattered) at depth `ell` in the
 *          canopy, expressed in the same units as `Q_o`.
 */
double total_radiation(
    double Q_o,    // Light units such as `micromol / m^2 / s` or `J / m^2 / s`
    double k,      // dimensionless
    double alpha,  // dimensionless
    double ell     // dimensionless from m^2 leaf / m^2 ground
)
{
    return Q_o * exp(-k * sqrt(alpha) * ell);  // same units as `Q_ob`
}

/**
 *  @brief Computes downscattered radiation using Equation 15.20 from Campbell &
 *  Norman (1998).
 *
 *  The downscattered radiation `Q_sc` is given by
 *
 *  `Q_sc = Q_bt - Q_b`             [Equation (1)]
 *
 *  where `Q_bt` is the total beam radiation (direct and downscattered) and
 *  `Q_b` is the direct beam radiation. `Q_bt` can be calculated using the
 *  total_radiation() function, while `Q_b` is given by Equation 15.16:
 *
 *  `Q_b = tau_b * Q_ob`            [Equation (2)]
 *
 *  where `Q_ob` is the beam radiation just above the canopy and `tau_b` is a
 *  transmission factor whose value decreases deeper into the
 *  canopy. `tau_b` is given by an un-numbered equation in the text which can be
 *  found just after Equation 15.17:
 *
 *  `tau_b = exp(-k_direct * ell)`  [Equation (3)]
 *
 *  where `k_direct` is the canopy extinction coefficient for direct radiation,
 *  and `ell` is the cumulative leaf area index.
 *
 *  Combining Equations (1-3) above, we arrive at
 *
 *  `Q_sc = Q_bt - Q_ob * exp(-sqrt(alpha) * k_direct * ell))`
 *
 *  which is implemented in the code below.
 *
 *  Note that `Q_ob` and `Q_bt` are expressed on a ground area basis; in other
 *  words, they are fluxes of photons or energy passing through a horizontal
 *  surface. However, `Q_sc` is diffuse radiation; it is not directional, and
 *  its value represents the flux through _any_ surface.
 *
 *  @param [in] Q_ob The beam radiation just above the canopy expressed on a
 *              ground area basis using either quantum flux density
 *              (micromol / m^2 / s) or energy density (J / m^2 / s).
 *
 *  @param [in] k_direct The (dimensionless) canopy extinction coefficient for
 *              direct radiation.
 *
 *  @param [in] alpha_direct The (dimensionless) leaf absorptivity for direct
 *              radiation.
 *
 *  @param [in] ell The cumulative leaf area index, which is dimensionless from
 *              m^2 leaf / m^2 ground.
 *
 *  @return The amount of downscattered radiation at depth `ell` in the canopy,
 *          expressed in the same units as `Q_ob`.
 */
double downscattered_radiation(
    double Q_ob,          // Light units such as `micromol / m^2 / s` or `J / m^2 / s`
    double k_direct,      // dimensionless
    double alpha_direct,  // dimensionless
    double ell            // dimensionless from m^2 leaf / m^2 ground
)
{
    return total_radiation(Q_ob, k_direct, alpha_direct, ell) -
           Q_ob * exp(-k_direct * ell);  // same units as `Q_ob`
}

/**
 *  @brief Computes the radiation incident on shaded leaves using Equation 15.19
 *  from Campbell & Norman (1998).
 *
 *  The radiation incident on shaded leaves (`Q_sh`) is given by
 *
 *  `Q_sh = Q_d + Q_sc`  [Equation (1)]
 *
 *  where `Q_d` is diffuse flux from the sky and `Q_sc` is downscattered
 *  radiation that is produced as the direct beam is scattered by leaves. `Q_sc`
 *  can be calculated using the downscattered_radiation() function, while `Q_d`
 *  is given by Equation 15.17:
 *
 *  `Q_d = tau_dt * Q_od`  [Equation (2)]
 *
 *  where `Q_od` is the diffuse radiation just above the canopy and `tau_dt` is
 *  a transmission factor whose value decreases deeper into the canopy. `tau_dt`
 *  is given by Equation 15.6, so `Q_d` can be calculated using the
 *  total_radiation() function.
 *
 *  Note that `Q_ob` and `Q_od` are expressed on a ground area basis; in other
 *  words, they are fluxes of photons or energy passing through a horizontal
 *  surface. However, `Q_sh`, `Q_d`, and `Q_sc` are diffuse radiation; they are
 *  not directional, and their values represent the flux through _any_ surface.
 *
 *  @param [in] Q_ob The direct radiation just above the canopy expressed on a
 *              ground area basis using either quantum flux density
 *              (micromol / m^2 / s) or energy density (J / m^2 / s).
 *
 *  @param [in] Q_od The diffuse radiation just above the canopy expressed on a
 *              ground area basis using either quantum flux density
 *              (micromol / m^2 / s) or energy density (J / m^2 / s).
 *
 *  @param [in] k_direct The (dimensionless) canopy extinction coefficient for
 *              direct radiation.
 *
 *  @param [in] k_diffuse The (dimensionless) canopy extinction coefficient for
 *              diffuse radiation.
 *
 *  @param [in] alpha_direct The (dimensionless) leaf absorptivity for direct
 *              radiation.
 *
 *  @param [in] alpha_diffuse The (dimensionless) leaf absorptivity for diffuse
 *              radiation.
 *
 *  @param [in] ell The cumulative leaf area index, which is dimensionless from
 *              m^2 leaf / m^2 ground.
 *
 *  @return The amount radiation incident on shaded leaves at depth `ell` in the
 *          canopy, expressed in the same units as `Q_ob`.
 */
double shaded_radiation(
    double Q_ob,           // Light units such as `micromol / m^2 / s` or `J / m^2 / s`
    double Q_od,           // same units as `Q_ob`
    double k_direct,       // dimensionless
    double k_diffuse,      // dimensionless
    double alpha_direct,   // dimensionless
    double alpha_diffuse,  // dimensionless
    double ell             // dimensionless from m^2 leaf / m^2 ground
)
{
    return total_radiation(Q_od, k_diffuse, alpha_diffuse, ell) +
           downscattered_radiation(Q_ob, k_direct, alpha_direct, ell);  // same units as `Q_ob`
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

    // Calculate the fraction of direct radiation that passes through the canopy
    // using Equation 15.1. Note that this is equivalent to the fraction of
    // ground area below the canopy that is exposed to direct sunlight. Note
    // that if the sun is at or below the horizon, no part of the soil is
    // sunlit; this corresponds to the case where cosine_zenith_angle is close
    // to or below zero.
    double canopy_direct_transmission_fraction =
        cosine_zenith_angle <= 1E-10 ? 0.0 : exp(-k * lai);  // dimensionless

    // Calculate the ambient direct PPFD through a surface parallel to the ground
    const double ambient_ppfd_beam_ground = ambient_ppfd_beam * cosine_zenith_angle;  // micromol / (m^2 ground) / s

    // Calculate the ambient direct PPFD through a unit area of leaf surface
    double ambient_ppfd_beam_leaf = ambient_ppfd_beam_ground * k;  // micromol / (m^2 leaf) / s

    // Start to fill in the light profile values
    Light_profile light_profile;
    light_profile.canopy_direct_transmission_fraction = canopy_direct_transmission_fraction;

    // Fill in the layer-dependent light profile values
    for (int i = 0; i < nlayers; ++i) {
        // Get the cumulative LAI for this layer, which represents the total
        // leaf area above this layer
        const double cumulative_lai = lai_per_layer * (i + 0.5);

        // Calculate the PPFD incident on shaded leaves
        double shaded_ppfd = shaded_radiation(
            ambient_ppfd_beam_ground, ambient_ppfd_diffuse,
            k, kd,
            absorptivity, 1.0,
            cumulative_lai);  // micromol / m^2 / s

        // Calculate the amount of PPFD scattered out of the direct beam. This
        // is a diffuse flux density representing the flux through any surface.
        const double scattered_ppfd = downscattered_radiation(
            ambient_ppfd_beam_ground, k, absorptivity, cumulative_lai);  // micromol / m^2 / s

        // Calculate the fraction of sunlit and shaded leaves in this canopy
        // layer using Equation 15.22.
        double sunlit_fraction = exp(-k * cumulative_lai);  // dimensionless
        double shaded_fraction = 1 - sunlit_fraction;       // dimensionless

        // For values of cosine_zenith_angle close to or less than 0, in place
        // of the calculations above, we want to use the limits of the above
        // expressions as cosine_zenith_angle approaches 0 from the right:
        if (cosine_zenith_angle <= 1E-10) {
            ambient_ppfd_beam_leaf = ambient_ppfd_beam / k1;
            shaded_ppfd = ambient_ppfd_diffuse * exp(-kd * cumulative_lai);
            sunlit_fraction = 0;
            shaded_fraction = 1;
        }

        // Store values of incident PPFD
        light_profile.sunlit_incident_ppfd[i] = ambient_ppfd_beam_leaf + shaded_ppfd;  // micromol / (m^2 leaf) / s
        light_profile.incident_ppfd_scattered[i] = scattered_ppfd;                     // micromol / m^2 / s
        light_profile.shaded_incident_ppfd[i] = shaded_ppfd;                           // micromol / (m^2 leaf) / s
        light_profile.sunlit_fraction[i] = sunlit_fraction;                            // dimensionless from m^2 / m^2
        light_profile.shaded_fraction[i] = shaded_fraction;                            // dimensionless from m^2 / m^2
        light_profile.height[i] = (lai - cumulative_lai) / heightf;                    // m

        // Store values of absorbed PPFD
        light_profile.sunlit_absorbed_ppfd[i] =
            thin_layer_absorption(
                leaf_reflectance,
                leaf_transmittance,
                ambient_ppfd_beam_leaf + shaded_ppfd);  // micromol / m^2 / s

        light_profile.shaded_absorbed_ppfd[i] =
            thin_layer_absorption(
                leaf_reflectance,
                leaf_transmittance,
                shaded_ppfd);  // micromol / m^2 / s

        // Store values of absorbed solar energy (including PAR and NIR)
        light_profile.sunlit_absorbed_shortwave[i] =
            absorbed_shortwave_from_incident_ppfd(
                ambient_ppfd_beam_leaf + shaded_ppfd,
                par_energy_content,
                par_energy_fraction,
                leaf_reflectance,
                leaf_transmittance);  // J / (m^2 leaf) / s

        light_profile.shaded_absorbed_shortwave[i] =
            absorbed_shortwave_from_incident_ppfd(
                shaded_ppfd,
                par_energy_content,
                par_energy_fraction,
                leaf_reflectance,
                leaf_transmittance);  // J / (m^2 leaf) / s
    }
    return light_profile;
}
