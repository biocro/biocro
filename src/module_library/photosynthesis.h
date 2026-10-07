#ifndef PHOTOSYNTHESIS_H
#define PHOTOSYNTHESIS_H

#include <cmath>
#include "canopy_light_helpers.h"  // canopy_light, light_profile
#include "../framework/constants.h"

/**
 * @file
 * @brief Shared infrastructure for computing canopy-integrated photosynthesis.
 *
 * This file is **not** a BioCro module; it defines types and utilities used
 * internally by the canopy photosynthesis functions `c3CanAC` (C3 crops) and
 * `CanAC` (C4 crops).
 *
 * **Key types:**
 *
 * - `leaf_assim` — aggregates all per-leaf photosynthesis outputs (net
 *   assimilation, stomatal conductance, transpiration, gross assimilation,
 *   leaf respiration, photorespiration).  It satisfies the vector-space
 *   interface required by the `quadrature::` library — `operator+=` and
 *   scalar `operator*` are defined — so a canopy-integrated value is
 *   obtained by passing a `canopy_integrand` directly to a quadrature
 *   function with `T = leaf_assim`.
 *
 * - `canopy_integrand<leaf_photo>` — a functor templated on a leaf
 *   photosynthesis callable.  Given a cumulative LAI depth it queries a
 *   `canopy_light` object for the local radiation environment, calls
 *   `leaf_photo` separately for the sunlit and shaded leaf classes, and
 *   returns their LAI-fraction-weighted sum as a `leaf_assim`.  This is the
 *   integrand passed to `quadrature::gauss_legendre<2>`.
 *
 * **Typical call chain** in a canopy photosynthesis function:
 * 1. Call `canopy_light` to calculate `canopy_info`, a struct containing key
      values needed to construct a canopy integrand.
 * 2. Define a `leaf_photo` lambda wrapping a single-leaf photosynthesis
 *    model (e.g. `c3photoC`) with its energy balance convergence loop.
 * 3. Construct `canopy_integrand(leaf_photo, canopy_info, ...)`.
 * 4. Call `quadrature::gauss_legendre<2, leaf_assim>(integrand, 0, LAI, n)`
 *    to obtain the canopy-integrated `leaf_assim`.
 */

// forward declarations
double leaf_nitrogen_profile(double cumulative_lai, double LeafN, double kpLN);
double wind_speed_profile(double cumulative_lai, double wind_speed);

/**
 * @brief Aggregates per-leaf photosynthesis outputs for canopy integration.
 *
 * Satisfies the vector-space interface required by `quadrature::`: defines
 * `operator+=` and scalar `operator*` so canopy totals are formed by
 * weighted summation inside the quadrature loop.
 */
struct leaf_assim {
    double assim = 0;                       //!< Net CO2 assimilation rate (micromol / m^2 / s)
    double stomatal_vapor_conductance = 0;  //!< Stomatal conductance to water vapor (mol / m^2 / s)
    double penman = 0;                      //!< P-M transpiration rate (mmol / m^2 / s)
    double priestly = 0;                    //!< Priestly transpiration rate (mmol / m^2 / s)
    double carboxylation = 0;               //!< Gross CO2 assimilation rate (micromol / m^2 / s)
    double leaf_respiration = 0;            //!< Rate of non-photorespiratory CO2 release in the light (micromol / m^2 / s)
    double photorespiration = 0;            //!< Rate of photorespiration (micromol / m^2 / s)
    double transpiration = 0;               //!< Transpiration rate (Mg / ha / hr)

    leaf_assim() = default;

    leaf_assim& operator+=(const leaf_assim& rhs)
    {
        assim += rhs.assim;
        stomatal_vapor_conductance += rhs.stomatal_vapor_conductance;
        penman += rhs.penman;
        priestly += rhs.priestly;
        carboxylation += rhs.carboxylation;
        leaf_respiration += rhs.leaf_respiration;
        photorespiration += rhs.photorespiration;
        transpiration += rhs.transpiration;
        return *this;
    }

    leaf_assim& operator*=(double scalar)
    {
        assim *= scalar;
        stomatal_vapor_conductance *= scalar;
        penman *= scalar;
        priestly *= scalar;
        carboxylation *= scalar;
        leaf_respiration *= scalar;
        photorespiration *= scalar;
        transpiration *= scalar;
        return *this;
    }
};

inline leaf_assim operator+(const leaf_assim& lhs, const leaf_assim& rhs)
{
    leaf_assim out = lhs;
    out += rhs;
    return out;
}

inline leaf_assim operator*(const leaf_assim& lhs, double scalar)
{
    leaf_assim out = lhs;
    out *= scalar;
    return out;
}

inline leaf_assim operator*(double scalar, const leaf_assim& rhs)
{
    leaf_assim out = rhs;
    out *= scalar;
    return out;
}

template <typename leaf_photo, bool use_absorbed = true>
struct canopy_integrand {
    canopy_integrand(
        leaf_photo photo_func,
        canopy_light_outputs canopy_info,
        double cosine_zenith_angle,
        double heightf,
        double k_diffuse,
        double kpLN,
        double lai,
        double leaf_reflectance_nir,
        double leaf_reflectance_par,
        double leaf_transmittance_nir,
        double leaf_transmittance_par,
        double leafN,
        double nir_diffuse,
        double par_energy_content,
        double ppfd_diffuse,
        double wind_speed
        ) : leaf_photosynthesis{photo_func},
            canopy_info{canopy_info},
            cosine_zenith_angle{cosine_zenith_angle},
            heightf{heightf},
            k_diffuse{k_diffuse},
            kpLN{kpLN},
            lai{lai},
            leaf_reflectance_nir{leaf_reflectance_nir},
            leaf_reflectance_par{leaf_reflectance_par},
            leaf_transmittance_nir{leaf_transmittance_nir},
            leaf_transmittance_par{leaf_transmittance_par},
            leafN{leafN},
            nir_diffuse{nir_diffuse},
            par_energy_content{par_energy_content},
            ppfd_diffuse{ppfd_diffuse},
            wind_speed{wind_speed}
    {
    }

    leaf_assim operator()(double cumulative_lai)
    {
        double layer_leafN = leaf_nitrogen_profile(cumulative_lai, leafN, kpLN);
        double layer_wind_speed = wind_speed_profile(cumulative_lai, wind_speed);

        light_profile lp = get_light_profile(
            canopy_info,
            cosine_zenith_angle,
            cumulative_lai,
            heightf,
            k_diffuse,
            lai,
            leaf_reflectance_nir,
            leaf_reflectance_par,
            leaf_transmittance_nir,
            leaf_transmittance_par,
            nir_diffuse,
            par_energy_content,
            ppfd_diffuse);

        // c4 model uses incident ppfd; c3 model uses absorbed ppfd
        auto select_ppfd = [](light_profile::light_type const& lt) -> double {
            if constexpr (use_absorbed)
                return lt.absorbed_ppfd;
            else
                return lt.incident_ppfd;
        };

        // Sunlit leaves
        double i_dir = select_ppfd(lp.sunlit);        // micromol / m^2 / s
        double j_dir = lp.sunlit.absorbed_shortwave;  // J / m^2 / s
        leaf_assim la = leaf_photosynthesis(i_dir, j_dir, layer_wind_speed, layer_leafN) * lp.sunlit.fraction;

        // Shaded leaves
        double i_diff = select_ppfd(lp.shaded);
        double j_diff = lp.shaded.absorbed_shortwave;  // J / m^2 / s
        la += leaf_photosynthesis(i_diff, j_diff, layer_wind_speed, layer_leafN) * lp.shaded.fraction;
        return la;
    }

   private:
    leaf_photo leaf_photosynthesis;
    canopy_light_outputs canopy_info;
    double cosine_zenith_angle;
    double heightf;
    double k_diffuse;
    double kpLN;
    double lai;
    double leaf_reflectance_nir;
    double leaf_reflectance_par;
    double leaf_transmittance_nir;
    double leaf_transmittance_par;
    double leafN;
    double nir_diffuse;
    double par_energy_content;
    double ppfd_diffuse;
    double wind_speed;
};

inline double leaf_nitrogen_profile(double cumulative_lai, double LeafN, double kpLN)
{
    return LeafN * std::exp(-kpLN * cumulative_lai);
}

inline double wind_speed_profile(double cumulative_lai, double wind_speed)
{
    constexpr double k = 0.7;
    return wind_speed * std::exp(-k * cumulative_lai);
}

#endif
