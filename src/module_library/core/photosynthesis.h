#ifndef CANOPY_PHOTO_CORE_H
#define CANOPY_PHOTO_CORE_H
#include <cmath>
#include "canopy_light_distribution.h"    // CanopyLight, LightProfile
#include "atmosphere_light_scattering.h"  // AtmosphereLightScattering
#include "../../framework/constants.h"

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
 * - `LeafAssim` — aggregates all per-leaf photosynthesis outputs (net
 *   assimilation, stomatal conductance, transpiration, gross assimilation,
 *   leaf respiration, photorespiration).  It satisfies the vector-space
 *   interface required by the `quadrature::` library — `operator+=` and
 *   scalar `operator*` are defined — so a canopy-integrated value is
 *   obtained by passing a `CanopyIntegrand` directly to a quadrature
 *   function with `T = LeafAssim`.
 *
 * - `CanopyIntegrand<LeafPhoto>` — a functor templated on a leaf
 *   photosynthesis callable.  Given a cumulative LAI depth it queries a
 *   `CanopyLight` object for the local radiation environment, calls
 *   `LeafPhoto` separately for the sunlit and shaded leaf classes, and
 *   returns their LAI-fraction-weighted sum as a `LeafAssim`.  This is the
 *   integrand passed to `quadrature::gauss_legendre<2>`.
 *
 * **Typical call chain** in a canopy photosynthesis function:
 * 1. Construct `PhotoCore::AtmosphereLightScattering` to split total solar
 *    radiation into direct and diffuse components.
 * 2. Construct `PhotoCore::CanopyLight` from those components plus canopy
 *    structural parameters (LAI, leaf angle, optical properties, etc.).
 * 3. Define a `leaf_photo` lambda wrapping a single-leaf photosynthesis
 *    model (e.g. `c3photoC`) with its energy balance convergence loop.
 * 4. Construct `PhotoCore::CanopyIntegrand(leaf_photo, canopy_light, ...)`.
 * 5. Call `quadrature::gauss_legendre<2, LeafAssim>(integrand, 0, LAI, n)`
 *    to obtain the canopy-integrated `LeafAssim`.
 */

namespace PhotoCore
{

// forward declarations
double leaf_nitrogen_profile(double cumulative_lai, double LeafN, double kpLN);
double wind_speed_profile(double cumulative_lai, double wind_speed);

/**
 * @brief A simple structure for holding the output of leaf photosynthesis
 * calculations; which will be summed into canopy photosynthesis rates. This type must have vector space operations: vector addition and scalar multiplication.
 */
struct LeafAssim {
    double assim = 0;                           //!< Net CO2 assimilation rate (micromol / m^2 / s)
    double stomatal_vapor_conductance = 0;      //!< Stomatal conductance to water vapor (mol / m^2 / s)
    double penman = 0;                          //!< P-M transpiration rate (mmol / m^2 / s)
    double priestly = 0;                        //!< Priestly transpiration rate (mmol / m^2 / s)
    double carboxylation = 0;                   //!< Gross CO2 assimilation rate (micromol / m^2 / s)
    double leaf_respiration = 0;                //!< Rate of non-photorespiratory CO2 release in the light (micromol / m^2 / s)
    double photorespiration = 0;                //!< Rate of photorespiration (micromol / m^2 / s)
    double transpiration = 0;                   //!< Transpiration rate (Mg / ha / hr)
    double whole_plant_growth_respiration = 0;  //!< Whole-plant growth respiration rate (micromol / m^2 / s)

    LeafAssim() = default;

    LeafAssim& operator+=(const LeafAssim& rhs)
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

    LeafAssim& operator*=(double scalar)
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

inline LeafAssim operator+(const LeafAssim& lhs, const LeafAssim& rhs)
{
    LeafAssim out = lhs;
    out += rhs;
    return out;
}

inline LeafAssim operator*(const LeafAssim& lhs, double scalar)
{
    LeafAssim out = lhs;
    out *= scalar;
    return out;
}

inline LeafAssim operator*(double scalar, const LeafAssim& rhs)
{
    LeafAssim out = rhs;
    out *= scalar;
    return out;
}

template <typename LeafPhoto, bool UseAbsorbed = true>
struct CanopyIntegrand {
    CanopyIntegrand(
        LeafPhoto photo_func,
        CanopyLight light_model,
        double kpLN,
        double leafN,      // micromol / m^2 / s
        double wind_speed  // m / s
        ) : leaf_photosynthesis{photo_func},
            canopy_light_model{light_model},
            kpLN{kpLN},
            leafN{leafN},
            wind_speed{wind_speed}
    {
    }

    LeafAssim operator()(double cumulative_lai)
    {
        // Calculations that are the same for sunlit and shaded leaves
        double layer_leafN = leaf_nitrogen_profile(cumulative_lai, leafN, kpLN);
        double layer_wind_speed = wind_speed_profile(cumulative_lai, wind_speed);
        LightProfile light_profile = canopy_light_model.get_light_profile(cumulative_lai);

        // currently c4 model uses incident ppfd rather than absorbed ppfd but c3 model uses absorbed
        auto select_ppfd = [](LightProfile::LightType const& lt) -> double {
            if constexpr (UseAbsorbed)
                return lt.absorbed_ppfd;
            else
                return lt.incident_ppfd;
        };

        // Calculations for sunlit leaves.
        double i_dir = select_ppfd(light_profile.sunlit);        // micromol / m^2 / s
        double j_dir = light_profile.sunlit.absorbed_shortwave;  // J / m^2 / s
        LeafAssim leaf_assim = leaf_photosynthesis(i_dir, j_dir, layer_wind_speed, layer_leafN) * light_profile.sunlit.fraction;

        // Calculations for shaded leaves.
        double i_diff = select_ppfd(light_profile.shaded);
        double j_diff = light_profile.shaded.absorbed_shortwave;  // J / m^2 / s
        leaf_assim += leaf_photosynthesis(i_diff, j_diff, layer_wind_speed, layer_leafN) * light_profile.shaded.fraction;
        return leaf_assim;
    }

   private:
    LeafPhoto leaf_photosynthesis;
    CanopyLight canopy_light_model;
    double kpLN;
    double leafN;
    double wind_speed;
    bool use_absorbed;
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

}  // namespace PhotoCore
#endif
