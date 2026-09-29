#ifndef CANOPY_LIGHT_HELPERS_H
#define CANOPY_LIGHT_HELPERS_H

/**
 * @file
 * @brief Models the distribution of sunlight through a plant canopy.
 *
 * The model follows Campbell & Norman, _An Introduction to Environmental
 * Biophysics_, 2nd edition (1998), Chapter 15.  Leaves are partitioned into
 * two classes: **sunlit** leaves intercept direct beam radiation in addition
 * to diffuse and scattered radiation; **shaded** leaves receive only diffuse
 * and scattered radiation.
 *
 * **Key types:**
 *
 * - `canopy_light` — the main model object.  Constructed once from canopy
 *   structural and optical parameters (LAI, solar zenith angle, leaf
 *   reflectance, transmittance, etc.); extinction coefficients and ambient
 *   flux conversions are pre-computed in the constructor.  Call
 *   `get_light_profile(cumulative_lai)` to evaluate the model at any depth.
 *
 * - `light_profile` — returned by `get_light_profile`; holds incident PPFD,
 *   incident NIR, absorbed shortwave energy, and leaf-area fraction for each
 *   of the sunlit and shaded leaf classes at a given cumulative LAI depth.
 *
 * **Users of this file:**
 * - `photosynthesis.h` — `canopy_integrand` calls `get_light_profile` at each
 *   quadrature node to supply radiation inputs to the leaf photosynthesis
 *   function.
 * - `multilayer_canopy_properties` — samples `get_light_profile` at `nlayers`
 *   discrete midpoints and writes per-layer values as BioCro module outputs.
 */

/**
 * @brief Radiation quantities for sunlit and shaded leaves at a single canopy depth.
 *
 * Returned by `canopy_light::get_light_profile`.
 */
struct light_profile {
    /**
     * @brief Radiation fluxes and leaf-area fraction for one leaf class.
     */
    struct light_type {
        double fraction;            //!< Fraction of total leaf area in this class (dimensionless)
        double absorbed_ppfd;       //!< Absorbed photon flux density (micromol / (m^2 leaf) / s)
        double absorbed_shortwave;  //!< Absorbed shortwave energy flux (J / (m^2 leaf) / s)
        double incident_nir;        //!< Incident near-infrared energy flux (J / (m^2 leaf) / s)
        double incident_ppfd;       //!< Incident photon flux density (micromol / (m^2 leaf) / s)
    };

    double height;      //!< Height above the ground of this canopy layer (m)
    light_type shaded;  //!< Radiation quantities for shaded leaves
    light_type sunlit;  //!< Radiation quantities for sunlit leaves
};

double thick_layer_absorption(
    double leaf_reflectance,    // dimensionless
    double leaf_transmittance,  // dimensionless
    double incident_light       // micromol / m^2 / s or J / m^2 / s
);

double thin_layer_absorption(
    double leaf_reflectance,    // dimensionless
    double leaf_transmittance,  // dimensionless
    double incident_light       // micromol / m^2 / s or J / m^2 / s
);

double nir_from_ppfd(
    double ppfd,                // micromol / m^2 / s
    double par_energy_content,  // J / micromol
    double par_energy_fraction  // dimensionless
);

double absorbed_shortwave(
    double incident_nir,            // J / m^2 / s
    double incident_ppfd,           // micromol / m^2 / s
    double par_energy_content,      // J / micromol
    double leaf_reflectance_par,    // dimensionless
    double leaf_transmittance_par,  // dimensionless
    double leaf_reflectance_nir,    // dimensionless
    double leaf_transmittance_nir   // dimensionless
);

double total_radiation(
    double Q_o,    // micromol / m^2 / s or J / m^2 / s
    double k,      // dimensionless
    double alpha,  // dimensionless
    double ell     // dimensionless from m^2 leaf / m^2 ground
);

double downscattered_radiation(
    double Q_ob,   // micromol / m^2 / s or J / m^2 / s
    double k,      // dimensionless
    double alpha,  // dimensionless
    double ell     // dimensionless from m^2 leaf / m^2 ground
);

double shaded_radiation(
    double Q_ob,       // micromol / m^2 / s or J / m^2 / s
    double Q_od,       // same units as Q_ob
    double k_direct,   // dimensionless
    double k_diffuse,  // dimensionless
    double alpha,      // dimensionless
    double ell         // dimensionless from m^2 leaf / m^2 ground
);

/**
 *  @brief A "data transfer object" for holding the return values of the
 *  `canopy_light()` function.
 *
 */
struct canopy_light_outputs {
    double absorptance_nir;                      //!< Leaf absorptance for NIR wavelengths (dimensionless)
    double absorptance_par;                      //!< Leaf absorptance for PAR wavelengths (dimensionless)
    double k1;                                   //!< Leaf angle shape factor denominator (dimensionless)
    double k_direct;                             //!< Extinction coefficient for direct radiation in the canopy (dimensionless)
    double canopy_direct_transmission_fraction;  //!< Fraction of direct light transmitted through the canopy (dimensionless)
    double ppfd_beam_ground;                     //!< PPFD in the direct beam on a ground area basis (micromol / (m^2 ground) / s)
    double ppfd_beam_leaf;                       //!< PPFD in the direct beam on a leaf area basis (micromol / (m^2 leaf) / s)
    double nir_beam_ground;                      //!< NIR energy flux density on a ground area basis (J / (m^2 ground) / s)
    double nir_beam_leaf;                        //!< NIR energy flux density on a leaf area basis (J / (m^2 leaf) / s)
};

canopy_light_outputs canopy_light(
    double const chil,                    // Leaf angle distribution parameter (dimensionless from m^2 / m^2)
    double const cosine_zenith_angle,     // Cosine of the solar zenith angle (dimensionless, [-1, 1])
    double const lai,                     // Leaf area index of the whole canopy (dimensionless from m^2 / m^2)
    double const leaf_reflectance_nir,    // Leaf NIR reflectance (dimensionless)
    double const leaf_reflectance_par,    // Leaf PAR reflectance (dimensionless)
    double const leaf_transmittance_nir,  // Leaf NIR transmittance (dimensionless)
    double const leaf_transmittance_par,  // Leaf PAR transmittance (dimensionless)
    double const nir_beam,                // Direct light energy flux density in the NIR range (J / m^2 / s)
    double const ppfd_beam                // Direct photosynthetically active photon flux density (micromol / m^2 / s)
);

light_profile get_light_profile(
    canopy_light_outputs const canopy_info,
    double const cosine_zenith_angle,     //
    double const cumulative_lai,          //
    double const heightf,                 //
    double const k_diffuse,               //
    double const lai,                     //
    double const leaf_reflectance_nir,    //
    double const leaf_reflectance_par,    //
    double const leaf_transmittance_nir,  //
    double const leaf_transmittance_par,  //
    double const nir_diffuse,             //
    double const par_energy_content,      //
    double const ppfd_diffuse             //
);

#endif
