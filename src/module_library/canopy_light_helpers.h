#ifndef CANOPY_LIGHT_HELPERS_H
#define CANOPY_LIGHT_HELPERS_H
#include "atmosphere_light_scattering.h"

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
 * @brief Pre-computes the radiation environment for a plant canopy.
 *
 * Constructed once from canopy structural and optical parameters; extinction
 * coefficients and ambient flux conversions are computed in the constructor.
 * Call `get_light_profile(cumulative_lai)` to evaluate the model at any depth.
 *
 * @par Construction
 * Supply beam and diffuse PPFD directly via the primary constructor, or use
 * `from_solar` to derive them from a total solar flux and an
 * `atmosphere_light_scattering` result.
 *
 * @par Invariants
 * All input parameters and derived absorptances are validated on construction;
 * invalid values throw `std::out_of_range`.  Once constructed, no member can
 * be modified — all data is stored in private `const`-logically-protected
 * fields.
 */
struct canopy_light {
    /**
     * @brief Structural and optical canopy parameters required for construction.
     *
     * Validated by `canopy_light` on construction; `std::out_of_range` is
     * thrown if any value is outside its physically meaningful range.
     */
    struct parameters {
        double chil;                    //!< Leaf angle distribution parameter (dimensionless from m^2 / m^2)
        double cosine_zenith_angle;     //!< Cosine of the solar zenith angle (dimensionless, [-1, 1])
        double heightf;                 //!< Leaf area density, LAI per canopy height (m^-1, > 0)
        double k_diffuse;               //!< Extinction coefficient for diffuse radiation (dimensionless, [0, 1])
        double lai;                     //!< Leaf area index of the whole canopy (dimensionless from m^2 / m^2)
        double leaf_reflectance_nir;    //!< Leaf NIR reflectance (dimensionless)
        double leaf_reflectance_par;    //!< Leaf PAR reflectance (dimensionless)
        double leaf_transmittance_nir;  //!< Leaf NIR transmittance (dimensionless)
        double leaf_transmittance_par;  //!< Leaf PAR transmittance (dimensionless)
        double par_energy_content;      //!< Average energy per PAR photon (J / micromol)
        double par_energy_fraction;     //!< Fraction of total shortwave energy in the PAR band (dimensionless)
    };

    /**
     * @brief Evaluates the canopy radiation model at a given cumulative LAI depth.
     *
     * @param cumulative_lai Cumulative leaf area index from the top of the canopy
     *        (dimensionless, m^2 leaf / m^2 ground).  Typically in `[0, lai]`.
     * @return A `light_profile` with incident and absorbed fluxes for sunlit and
     *         shaded leaves, their area fractions, and the layer height.
     */
    light_profile get_light_profile(double cumulative_lai) const;

    /**
     * @brief Constructs from beam and diffuse PPFD at the top of the canopy.
     *
     * @param ppfd_beam   Beam photon flux density perpendicular to the sun's rays
     *                    (micromol / (m^2 beam) / s).
     * @param ppfd_diffuse Diffuse photon flux density through any horizontal plane
     *                    (micromol / m^2 / s).
     * @param p           Canopy structural and optical parameters.
     * @throws std::out_of_range if any parameter or derived absorptance is invalid.
     */
    canopy_light(double ppfd_beam, double ppfd_diffuse, parameters p);

    /**
     * @brief Constructs from total solar flux and atmospheric scattering.
     *
     * Derives beam PPFD as `solar * a.direct_fraction` and diffuse PPFD as
     * `solar * a.diffuse_fraction`, then delegates to the primary constructor.
     *
     * @param solar Total solar photon flux density (micromol / m^2 / s).
     * @param a     Pre-computed atmospheric scattering result.
     * @param p     Canopy structural and optical parameters.
     * @throws std::out_of_range if any parameter or derived absorptance is invalid.
     */
    static canopy_light from_solar(double solar, atmosphere_light_scattering const& a, parameters const& p);

    /// Returns the fraction of ground area exposed to direct sunlight (dimensionless).
    double direct_transmission_fraction() const;

   private:
    /// Derived quantities pre-computed from inputs during construction.
    /// Stored privately to prevent modification after invariants are checked.
    struct derived_t {
        double absorptance_nir;                      // dimensionless
        double absorptance_par;                      // dimensionless
        double k1;                                   // dimensionless — leaf angle shape factor denominator
        double k_direct;                             // dimensionless — extinction coefficient for direct radiation
        double canopy_direct_transmission_fraction;  // dimensionless
        double ppfd_beam_ground;                     // micromol / (m^2 ground) / s
        double ppfd_beam_leaf;                       // micromol / (m^2 leaf) / s
        double nir_beam;                             // J / (m^2 beam) / s
        double nir_beam_ground;                      // J / (m^2 ground) / s
        double nir_diffuse;                          // J / (m^2 ground) / s
        double nir_beam_leaf;                        // J / (m^2 leaf) / s
    };

    double ppfd_beam;     // micromol / (m^2 beam) / s — beam PPFD perpendicular to sun
    double ppfd_diffuse;  // micromol / m^2 / s        — diffuse PPFD through any plane
    parameters p;
    derived_t d;

    /// Validates input parameters and computes all derived quantities.
    /// @throws std::out_of_range on invalid inputs or derived absorptances.
    static derived_t compute(double ppfd_beam, double ppfd_diffuse, parameters const& p);

    /// Throws std::out_of_range if any input parameter is outside its valid range.
    static void validate_params(parameters const& p);

    /// Throws std::out_of_range if any derived absorptance is outside [0, 1].
    static void validate_derived(derived_t const& der);

    /// Private constructor — stores pre-validated inputs and pre-computed derived values.
    canopy_light(double ppfd_beam, double ppfd_diffuse, parameters par, derived_t der);
};

#endif
