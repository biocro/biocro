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
 *  @brief Computes total shortwave radiation absorbed by a leaf.
 *
 *  The total shortwave radiation absorbed by a leaf is the sum of the absorbed
 *  energy in the photosynthetically active radiation (PAR) and near-infrared
 *  (NIR) bands: `total_absorbed = par_absorbed + nir_absorbed`.
 *
 *  In each band, the absorbed radiation can be calculated from the incident
 *  radiation using the thin_layer_absorption() function with the appropriate
 *  values of leaf reflectance and transmission for each band.
 *
 *  The incident PAR energy can be calculated from the incident
 *  photosynthetically active photon flux density (PPFD) using the average
 *  energy per photon in the PAR band.
 *
 *  @param [in] incident_nir Energy flux in the NIR band incident on a leaf
 *              expressed in micromol / m^2 / s
 *
 *  @param [in] incident_ppfd Photosynthetically active photon flux density
 *              (PPFD) incident on a leaf expressed in micromol / m^2 / s
 *
 *  @param [in] par_energy_content The average energy per photon in the PAR band
 *              expressed in J /  micromol
 *
 *  @param [in] leaf_reflectance_par The fractional amount of PAR band radiation
 *              reflected by the leaf
 *
 *  @param [in] leaf_transmittance_par The fractional amount of PAR band
 *              radiation transmitted through the leaf
 *
 *  @param [in] leaf_reflectance_nir The fractional amount of NIR band radiation
 *              reflected by the leaf
 *
 *  @param [in] leaf_transmittance_nir The fractional amount of NIR band
 *              radiation transmitted through the leaf
 *
 *  @return The total shortwave radiation absorbed by the leaf expressed in
 *          J / m^2 / s
 */
double absorbed_shortwave(
    double incident_nir,            // J / m^2 / s
    double incident_ppfd,           // micromol / m^2 / s
    double par_energy_content,      // J / micromol
    double leaf_reflectance_par,    // dimensionless
    double leaf_transmittance_par,  // dimensionless
    double leaf_reflectance_nir,    // dimensionless
    double leaf_transmittance_nir   // dimensionless
)
{
    double const absorbed_par = thin_layer_absorption(
        leaf_reflectance_par,
        leaf_transmittance_par,
        incident_ppfd * par_energy_content);  // J / m^2 / s

    double const absorbed_nir = thin_layer_absorption(
        leaf_reflectance_nir,
        leaf_transmittance_nir,
        incident_nir);  // J / m^2 / s

    return absorbed_par + absorbed_nir;  // J / m^2 / s
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
 *  @param [in] alpha The (dimensionless) leaf absorptivity.
 *
 *  @param [in] ell The cumulative leaf area index, which is dimensionless from
 *              m^2 leaf / m^2 ground.
 *
 *  @return The amount radiation incident on shaded leaves at depth `ell` in the
 *          canopy, expressed in the same units as `Q_ob`.
 */
double shaded_radiation(
    double Q_ob,       // Light units such as `micromol / m^2 / s` or `J / m^2 / s`
    double Q_od,       // same units as `Q_ob`
    double k_direct,   // dimensionless
    double k_diffuse,  // dimensionless
    double alpha,      // dimensionless
    double ell         // dimensionless from m^2 leaf / m^2 ground
)
{
    return total_radiation(Q_od, k_diffuse, alpha, ell) +
           downscattered_radiation(Q_ob, k_direct, alpha, ell);  // same units as `Q_ob`
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
 *  @param [in] chil Ratio of average projected areas of canopy elements on
 *              horizontal surfaces; for a spherical leaf distribution,
 *              `chil = 0`; for a vertical leaf distribution, `chil = 1`; for a
 *              horizontal leaf distribution, `chil` approaches infinity
 *              (dimensionless from m^2 / m^2)
 *
 *  @param [in] cosine_zenith_angle Cosine of the solar zenith angle
 *              (dimensionless)
 *
 *  @param [in] heightf Leaf area density, i.e., LAI per height of canopy (m^-1
 *              from m^2 leaf / m^2 ground / m height)
 *
 *  @param [in] k_diffuse Extinction coefficient for diffuse light
 *              (dimensionless)
 *
 *  @param [in] lai Leaf area index (LAI) of the entire canopy, which represents
 *              the leaf area per unit of ground area (dimensionless from m^2
 *              leaf / m^2 ground)
 *
 *  @param [in] leaf_reflectance_nir The fractional amount of NIR band radiation
 *              reflected by the leaf
 *
 *  @param [in] leaf_reflectance_par The fractional amount of PAR band radiation
 *              reflected by the leaf
 *
 *  @param [in] leaf_transmittance_nir The fractional amount of NIR band
 *              radiation transmitted through the leaf
 *
 *  @param [in] leaf_transmittance_par The fractional amount of PAR band
 *              radiation transmitted through the leaf
 *
 *  @param [in] par_energy_content The average energy per photon in the PAR band
 *              expressed in J /  micromol
 *
 *  @param [in] par_energy_fraction The fraction of total shortwave energy in
 *              the PAR band
 *
 *  @param [in] nlayers Integer number of layers in the canopy
 *
 *  @return An n-layered light profile representing quantities within
 *          the canopy, including several photon flux densities and
 *          the relative fractions of shaded and sunlit leaves
 */
Light_profile sunML(
    double ambient_ppfd_beam,       // micromol / (m^2 beam) / s
    double ambient_ppfd_diffuse,    // micromol / m^2 / s
    double chil,                    // dimensionless from m^2 / m^2
    double cosine_zenith_angle,     // dimensionless
    double heightf,                 // m^-1 from m^2 leaf / m^2 ground / m height
    double k_diffuse,               // dimensionless
    double lai,                     // dimensionless from m^2 / m^2
    double leaf_reflectance_nir,    // dimensionless
    double leaf_reflectance_par,    // dimensionless
    double leaf_transmittance_nir,  // dimensionless
    double leaf_transmittance_par,  // dimensionless
    double par_energy_content,      // J / micromol
    double par_energy_fraction,     // dimensionless
    int nlayers                     // dimensionless
)
{
    if (nlayers < 1 || nlayers > MAXLAY) {
        throw std::out_of_range("nlayers must be at least 1 but no more than MAXLAY.");
    }

    if (cosine_zenith_angle > 1 || cosine_zenith_angle < -1) {
        throw std::out_of_range("cosine_zenith_angle must be between -1 and 1.");
    }

    if (k_diffuse > 1 || k_diffuse < 0) {
        throw std::out_of_range("k_diffuse must be between 0 and 1.");
    }

    if (chil < 0) {
        throw std::out_of_range("chil must be non-negative.");
    }

    if (heightf <= 0) {
        throw std::out_of_range("heightf must greater than zero.");
    }

    // Calculate absorptivity from leaf reflectance and transmission
    double const absorptivity_nir = 1.0 - leaf_reflectance_nir - leaf_transmittance_nir; // dimensionless
    double const absorptivity_par = 1.0 - leaf_reflectance_par - leaf_transmittance_par; // dimensionless

    if (absorptivity_par > 1 || absorptivity_par < 0) {
        throw std::out_of_range("absorptivity_par must be between 0 and 1.");
    }

    if (absorptivity_nir > 1 || absorptivity_nir < 0) {
        throw std::out_of_range("absorptivity_nir must be between 0 and 1.");
    }

    // Calculate the leaf shape factor for an ellipsoidal leaf angle
    // distribution using the equation from page 251 of Campbell & Norman
    // (1998). We will use this value as `k_direct`, the canopy extinction
    // coefficient for direct photosynthetically active radiation throughout the
    // canopy. This quantity represents the ratio of horizontal area to total
    // area for leaves in the canopy and is therefore dimensionless from
    // (m^2 ground) / (m^2 leaf).
    double zenith_angle = acos(cosine_zenith_angle);  // radians
    double k0 = sqrt(pow(chil, 2) + pow(tan(zenith_angle), 2));
    double k1 = chil + 1.744 * pow((chil + 1.182), -0.733);
    double k_direct = k0 / k1;  // dimensionless

    double lai_per_layer = lai / nlayers;

    // Calculate the fraction of direct radiation that passes through the canopy
    // using Equation 15.1. Note that this is equivalent to the fraction of
    // ground area below the canopy that is exposed to direct sunlight. Note
    // that if the sun is at or below the horizon, no part of the soil is
    // sunlit; this corresponds to the case where cosine_zenith_angle is close
    // to or below zero.
    double canopy_direct_transmission_fraction =
        cosine_zenith_angle <= 1E-10 ? 0.0 : exp(-k_direct * lai);  // dimensionless

    // Calculate the ambient direct PPFD through a surface parallel to the ground
    const double ambient_ppfd_beam_ground = ambient_ppfd_beam * cosine_zenith_angle;  // micromol / (m^2 ground) / s

    // Calculate the ambient direct PPFD through a unit area of leaf surface
    double ambient_ppfd_beam_leaf = ambient_ppfd_beam_ground * k_direct;  // micromol / (m^2 leaf) / s

    // Calculate related NIR energy fluxes
    const double ambient_nir_beam = nir_from_ppfd(
        ambient_ppfd_beam, par_energy_content, par_energy_fraction);  // J / (m^2 beam) / s

    const double ambient_nir_beam_ground = nir_from_ppfd(
        ambient_ppfd_beam_ground, par_energy_content, par_energy_fraction);  // J / (m^2 ground) / s

    const double ambient_nir_diffuse = nir_from_ppfd(
        ambient_ppfd_diffuse, par_energy_content, par_energy_fraction);  // J / (m^2 ground) / s

    double ambient_nir_beam_leaf = nir_from_ppfd(
        ambient_ppfd_beam_leaf, par_energy_content, par_energy_fraction);  // J / (m^2 leaf) / s

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
            k_direct, k_diffuse,
            absorptivity_par, cumulative_lai);  // micromol / m^2 / s

        // Calculate the NIR incident on shaded leaves
        double shaded_nir = shaded_radiation(
            ambient_nir_beam_ground, ambient_nir_diffuse,
            k_direct, k_diffuse,
            absorptivity_nir, cumulative_lai);  // J / m^2 / s

        // Calculate the fraction of sunlit and shaded leaves in this canopy
        // layer using Equation 15.22.
        double sunlit_fraction = exp(-k_direct * cumulative_lai);  // dimensionless
        double shaded_fraction = 1 - sunlit_fraction;              // dimensionless

        // For values of cosine_zenith_angle close to or less than 0, in place
        // of the calculations above, we want to use the limits of the above
        // expressions as cosine_zenith_angle approaches 0 from the right:
        if (cosine_zenith_angle <= 1E-10) {
            ambient_ppfd_beam_leaf = ambient_ppfd_beam / k1;
            ambient_nir_beam_leaf = ambient_nir_beam / k1;
            shaded_ppfd = ambient_ppfd_diffuse * exp(-k_diffuse * cumulative_lai);
            shaded_nir = ambient_nir_diffuse * exp(-k_diffuse * cumulative_lai);
            sunlit_fraction = 0;
            shaded_fraction = 1;
        }

        // Store values of incident PPFD
        light_profile.height[i] = (lai - cumulative_lai) / heightf;                    // m
        light_profile.shaded_fraction[i] = shaded_fraction;                            // dimensionless from m^2 / m^2
        light_profile.shaded_incident_ppfd[i] = shaded_ppfd;                           // micromol / (m^2 leaf) / s
        light_profile.shaded_incident_nir[i] = shaded_nir;                             // J / (m^2 leaf) / s
        light_profile.sunlit_fraction[i] = sunlit_fraction;                            // dimensionless from m^2 / m^2
        light_profile.sunlit_incident_ppfd[i] = ambient_ppfd_beam_leaf + shaded_ppfd;  // micromol / (m^2 leaf) / s
        light_profile.sunlit_incident_nir[i] = ambient_nir_beam_leaf + shaded_nir;     // J / (m^2 leaf) / s

        // Store values of absorbed PPFD
        light_profile.sunlit_absorbed_ppfd[i] =
            thin_layer_absorption(
                leaf_reflectance_par,
                leaf_transmittance_par,
                ambient_ppfd_beam_leaf + shaded_ppfd);  // micromol / m^2 / s

        light_profile.shaded_absorbed_ppfd[i] =
            thin_layer_absorption(
                leaf_reflectance_par,
                leaf_transmittance_par,
                shaded_ppfd);  // micromol / m^2 / s

        // Store values of absorbed solar energy (including PAR and NIR)
        light_profile.sunlit_absorbed_shortwave[i] =
            absorbed_shortwave(
                ambient_nir_beam_leaf + shaded_nir,
                ambient_ppfd_beam_leaf + shaded_ppfd,
                par_energy_content,
                leaf_reflectance_par,
                leaf_transmittance_par,
                leaf_reflectance_nir,
                leaf_transmittance_nir);  // J / (m^2 leaf) / s

        light_profile.shaded_absorbed_shortwave[i] =
            absorbed_shortwave(
                shaded_nir,
                shaded_ppfd,
                par_energy_content,
                leaf_reflectance_par,
                leaf_transmittance_par,
                leaf_reflectance_nir,
                leaf_transmittance_nir);  // J / (m^2 leaf) / s
    }
    return light_profile;
}
