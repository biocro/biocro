#include <cmath>      // exp, tan, cos, acos, sqrt
#include <stdexcept>  // std::out_of_range
#include <utility>    // std::move
#include <string>
#include "canopy_light_helpers.h"

light_profile canopy_light::get_light_profile(double cumulative_lai) const
{
    light_profile profile;
    // For values of cosine_zenith_angle close to or less than 0, in place
    // of the calculations above, we want to use the limits of the above
    // expressions as cosine_zenith_angle approaches 0 from the right:
    if (p.cosine_zenith_angle <= 1e-10) {
        profile.shaded.incident_ppfd = ppfd_diffuse * std::exp(-p.k_diffuse * cumulative_lai);
        profile.shaded.incident_nir = d.nir_diffuse * std::exp(-p.k_diffuse * cumulative_lai);
        // Calculate the fraction of sunlit and shaded leaves in this canopy
        // layer using Equation 15.22.
        profile.sunlit.fraction = 0;
        profile.shaded.fraction = 1;
    } else {
        profile.shaded.incident_ppfd = shaded_radiation(
            d.ppfd_beam_ground, ppfd_diffuse,
            d.k_direct, p.k_diffuse,
            d.absorptance_par, cumulative_lai);
        profile.shaded.incident_nir = shaded_radiation(
            d.nir_beam_ground, d.nir_diffuse,
            d.k_direct, p.k_diffuse,
            d.absorptance_nir, cumulative_lai);
        // Calculate the fraction of sunlit and shaded leaves in this canopy
        // layer using Equation 15.22.
        profile.sunlit.fraction = std::exp(-d.k_direct * cumulative_lai);
        profile.shaded.fraction = 1 - profile.sunlit.fraction;
    }

    // Store values of incident PPFD
    profile.height = (p.lai - cumulative_lai) / p.heightf;                           // m
    profile.sunlit.incident_ppfd = d.ppfd_beam_leaf + profile.shaded.incident_ppfd;  // micromol / (m^2 leaf) / s
    profile.sunlit.incident_nir = d.nir_beam_leaf + profile.shaded.incident_nir;     // J / (m^2 leaf) / s

    // Store values of absorbed PPFD
    profile.sunlit.absorbed_ppfd =
        thin_layer_absorption(
            p.leaf_reflectance_par,
            p.leaf_transmittance_par,
            profile.sunlit.incident_ppfd);  // micromol / m^2 / s

    profile.shaded.absorbed_ppfd =
        thin_layer_absorption(
            p.leaf_reflectance_par,
            p.leaf_transmittance_par,
            profile.shaded.incident_ppfd);  // micromol / m^2 / s

    // Store values of absorbed solar energy (including PAR and NIR)
    profile.sunlit.absorbed_shortwave =
        absorbed_shortwave(
            profile.sunlit.incident_nir,
            profile.sunlit.incident_ppfd,
            p.par_energy_content,
            p.leaf_reflectance_par,
            p.leaf_transmittance_par,
            p.leaf_reflectance_nir,
            p.leaf_transmittance_nir);  // J / (m^2 leaf) / s

    profile.shaded.absorbed_shortwave =
        absorbed_shortwave(
            profile.shaded.incident_nir,
            profile.shaded.incident_ppfd,
            p.par_energy_content,
            p.leaf_reflectance_par,
            p.leaf_transmittance_par,
            p.leaf_reflectance_nir,
            p.leaf_transmittance_nir);  // J / (m^2 leaf) / s

    return profile;
}

canopy_light::canopy_light(
    double ppfd_beam,
    double ppfd_diffuse,
    parameters par,
    derived_t der) : ppfd_beam{ppfd_beam},
                     ppfd_diffuse{ppfd_diffuse},
                     p{std::move(par)},
                     d{std::move(der)}
{
}

canopy_light::canopy_light(
    double ppfd_beam,
    double ppfd_diffuse,
    parameters p) : canopy_light{ppfd_beam, ppfd_diffuse, p,
                                 canopy_light::compute(ppfd_beam, ppfd_diffuse, p)}
{
}

canopy_light canopy_light::from_solar(double solar, atmosphere_light_scattering const& a, canopy_light::parameters const& p)
{
    double beam = a.direct_fraction * solar;      // micromol / m^2 / s
    double diffuse = a.diffuse_fraction * solar;  // micromol / m^2 / s
    return canopy_light{beam, diffuse, p, canopy_light::compute(beam, diffuse, p)};
}

/**
 * @brief Validates inputs, then computes all derived canopy radiation quantities.
 *
 * Calls `validate_params` before any floating-point computation to ensure that
 * `std::acos` and `std::tan` receive valid arguments.  Calls `validate_derived`
 * after computing absorptances to check the leaf optical property constraints.
 */
canopy_light::derived_t canopy_light::compute(double ppfd_beam, double ppfd_diffuse, parameters const& p)
{
    validate_params(p);
    derived_t result;
    result.absorptance_nir = 1.0 - p.leaf_reflectance_nir - p.leaf_transmittance_nir;  // dimensionless
    result.absorptance_par = 1.0 - p.leaf_reflectance_par - p.leaf_transmittance_par;
    // Calculate the leaf shape factor for an ellipsoidal leaf angle
    // distribution using the equation from page 251 of Campbell & Norman
    // (1998). We will use this value as `k_direct`, the canopy extinction
    // coefficient for direct photosynthetically active radiation throughout the
    // canopy. This quantity represents the ratio of horizontal area to total
    // area for leaves in the canopy and is therefore dimensionless from
    // (m^2 ground) / (m^2 leaf).
    double zenith_angle = std::acos(p.cosine_zenith_angle);  // radians
    double k0 = std::sqrt(std::pow(p.chil, 2) + std::pow(std::tan(zenith_angle), 2));
    result.k1 = p.chil + 1.744 * std::pow((p.chil + 1.182), -0.733);
    result.k_direct = k0 / result.k1;  // dimensionless

    // Calculate the fraction of direct radiation that passes through the canopy
    // using Equation 15.1. Note that this is equivalent to the fraction of
    // ground area below the canopy that is exposed to direct sunlight. Note
    // that if the sun is at or below the horizon, no part of the soil is
    // sunlit; this corresponds to the case where cosine_zenith_angle is close
    // to or below zero.
    result.canopy_direct_transmission_fraction =
        p.cosine_zenith_angle <= 1e-10 ? 0.0 : exp(-result.k_direct * p.lai);  // dimensionless

    // Calculate the ambient direct PPFD through a surface parallel to the ground
    result.ppfd_beam_ground = ppfd_beam * p.cosine_zenith_angle;  // micromol / (m^2 ground) / s

    // Calculate related NIR energy fluxes
    result.nir_beam = nir_from_ppfd(
        ppfd_beam, p.par_energy_content, p.par_energy_fraction);  // J / (m^2 beam) / s

    result.nir_beam_ground = nir_from_ppfd(
        result.ppfd_beam_ground, p.par_energy_content, p.par_energy_fraction);  // J / (m^2 ground) / s

    result.nir_diffuse = nir_from_ppfd(
        ppfd_diffuse, p.par_energy_content, p.par_energy_fraction);  // J / (m^2 ground) / s
    // For values of cosine_zenith_angle close to or less than 0, in place
    // of the calculations above, we want to use the limits of the above

    // expressions as cosine_zenith_angle approaches 0 from the right:
    if (p.cosine_zenith_angle <= 1e-10) {
        result.ppfd_beam_leaf = ppfd_beam / result.k1;
        result.nir_beam_leaf = result.nir_beam / result.k1;
    } else {
        // Calculate the ambient direct PPFD through a unit area of leaf surface
        result.ppfd_beam_leaf = result.ppfd_beam_ground * result.k_direct;  // micromol / (m^2 leaf) / s
        result.nir_beam_leaf = nir_from_ppfd(
            result.ppfd_beam_leaf, p.par_energy_content, p.par_energy_fraction);  // J / (m^2 leaf) / s
    }

    validate_derived(result);
    return result;
}

double canopy_light::direct_transmission_fraction() const
{
    return d.canopy_direct_transmission_fraction;
}

/**
 * @brief Validates canopy input parameters.
 *
 * @throws std::out_of_range if `cosine_zenith_angle` is outside `[-1, 1]`,
 *         `k_diffuse` is outside `[0, 1]`, `chil` is negative, or `heightf`
 *         is not strictly positive.
 */
void canopy_light::validate_params(canopy_light::parameters const& p)
{
    std::string errors;
    if (p.cosine_zenith_angle > 1 || p.cosine_zenith_angle < -1)
        errors += "      cosine_zenith_angle = " + std::to_string(p.cosine_zenith_angle) + " is outside [-1, 1]\n";
    if (p.k_diffuse > 1 || p.k_diffuse < 0)
        errors += "      k_diffuse = " + std::to_string(p.k_diffuse) + " is outside [0, 1]\n";
    if (p.chil < 0)
        errors += "      chil = " + std::to_string(p.chil) + " must be non-negative\n";
    if (p.heightf <= 0)
        errors += "      heightf = " + std::to_string(p.heightf) + " must be greater than zero\n";

    if (!errors.empty()) {
        throw std::out_of_range("\n    canopy_light::parameters are invalid:\n" + errors);
    }
}

/**
 * @brief Validates derived leaf absorptances.
 *
 * @throws std::out_of_range if `absorptance_par` or `absorptance_nir` is
 *         outside `[0, 1]`, indicating that the leaf reflectance and
 *         transmittance parameters sum to more than 1 in either band.
 */
void canopy_light::validate_derived(canopy_light::derived_t const& d)
{
    std::string errors;
    if (d.absorptance_par > 1 || d.absorptance_par < 0) {
        errors +=
            "      absorptance_par = 1 - leaf_reflectance_par - leaf_transmittance_par = " +
            std::to_string(d.absorptance_par) +
            " is outside [0, 1]\n"
            "        (leaf_reflectance_par + leaf_transmittance_par must not exceed 1)\n";
    }

    if (d.absorptance_nir > 1 || d.absorptance_nir < 0) {
        errors +=
            "      absorptance_nir = 1 - leaf_reflectance_nir - leaf_transmittance_nir = " +
            std::to_string(d.absorptance_nir) +
            " is outside [0, 1]\n"
            "        (leaf_reflectance_nir + leaf_transmittance_nir must not exceed 1)\n";
    }

    if (!errors.empty()) {
        throw std::out_of_range("\n    canopy_light: derived leaf absorptance is invalid:\n" + errors);
    }
}
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
    return Q_o * std::exp(-k * std::sqrt(alpha) * ell);  // same units as `Q_ob`
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
           Q_ob * std::exp(-k_direct * ell);  // same units as `Q_ob`
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
