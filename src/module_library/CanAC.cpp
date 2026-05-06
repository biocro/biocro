#include <algorithm>                           // for std::min, std::max
#include "../framework/constants.h"            // for molar_mass_of_water
#include "../math/quadrature/quad.h"           // for quadrature::gauss_legendre_2
#include "../math/roots/onedim/fixed_point.h"  // for fixed_point
#include "c4photo.h"                           // for c4photoC
#include "core/atmosphere_light_scattering.h"  // for PhotoCore::AtmosphereLightScattering
#include "core/photosynthesis.h"               // for PhotoCore::LeafAssim, CanopyIntegrand
#include "leaf_energy_balance.h"               // for leaf_energy_balance
#include "respiration.h"                       // for growth_resp
#include "CanAC.h"

canopy_photosynthesis_outputs CanAC(
    const nitroParms& nitroP,
    double absorbed_longwave,  // J / m^2 / s
    double Alpha,
    double ambient_temperature,          // degrees C
    double atmospheric_pressure,         // Pa
    double atmospheric_scattering,       // dimensionless
    double atmospheric_transmittance,    // dimensionless
    double b0,                           // mol / m^2 / s
    double b1,                           // dimensionless
    double beta,                         // dimensionless
    double Catm,                         // ppm
    double chil,                         // dimensionless
    double cosine_zenith_angle,          // dimensionless
    double gbw_canopy,                   // m / s
    double growth_respiration_fraction,  // dimensionless
    double Gs_min,                       // mol / m^2 / s
    double k_diffuse,                    // dimensionless
    double Kparm,
    double kpLN,
    double LAI,                     // dimensionless from m^2 / m^2
    double leaf_reflectance_nir,    // dimensionless
    double leaf_reflectance_par,    // dimensionless
    double leaf_transmittance_nir,  // dimensionless
    double leaf_transmittance_par,  // dimensionless
    double leafN,
    double leafwidth,            // m
    double lowerT,               // degrees C
    double par_energy_content,   // J / micromol
    double par_energy_fraction,  // dimensionless
    double RH,                   // dimensionless from Pa / Pa
    double RL_at_25,             // micromol / m^2 / s
    double solarR,               // micromol / m^2 / s
    double StomataWS,            // dimensionless
    double theta,                // dimensionless
    double upperT,               // degrees C
    double Vcmax_at_25,          // micromol / m^2 / s
    double WindSpeed,            // m / s
    int lnfun,                   // dimensionless switch
    int nlayers                  // dimensionless
)
{
    PhotoCore::AtmosphereLightScattering const light_model(
        cosine_zenith_angle,
        atmospheric_pressure,
        atmospheric_transmittance,
        atmospheric_scattering);

    // q_dir: flux through a plane perpendicular to the rays of the sun
    // q_diff: flux through any surface
    double const q_dir = light_model.direct_fraction * solarR;    // micromol / m^2 / s
    double const q_diff = light_model.diffuse_fraction * solarR;  // micromol / m^2 / s

    // heightf = 1 since canopy height is not used anywhere in this function.
    PhotoCore::CanopyLight const canopy_light(
        q_dir,
        q_diff,
        chil,
        cosine_zenith_angle,
        1.0,
        k_diffuse,
        LAI,
        leaf_reflectance_nir,
        leaf_reflectance_par,
        leaf_transmittance_nir,
        leaf_transmittance_par,
        par_energy_content,
        par_energy_fraction);


    using namespace root_finding;

    // Set convergence criteria
    root_finding::fixed_point solver(50, 1e-3, 1e-3);

    // Leaf-level photosynthesis function for use with PhotoCore::CanopyIntegrand.
    // Solves the coupled stomatal conductance / energy balance system for a
    // single leaf class (sunlit or shaded) and returns a LeafAssim summary.
    auto leaf_photo = [&](double i_ppfd, double j_shortwave, double layer_wind_speed, double layer_leafN) -> PhotoCore::LeafAssim {
        double eff_Vcmax = Vcmax_at_25;
        double eff_Alpha = Alpha;
        double eff_RL = RL_at_25;
        if (lnfun != 0) {
            eff_Vcmax = std::max(0.0, std::min(Vcmax_at_25, nitroP.Vmaxb1 * layer_leafN + nitroP.Vmaxb0));
            eff_Alpha = nitroP.alphab1 * layer_leafN + nitroP.alphab0;
            eff_RL = nitroP.Rdb1 * layer_leafN + nitroP.Rdb0;
        }

        double constexpr gbw_guess = 1.2;  // mol / m^2 / s
        // Initial guess: evaluate photosynthesis at ambient leaf temperature
        double gsw_estimate =
            c4photoC(
                i_ppfd, ambient_temperature, ambient_temperature,
                RH, eff_Vcmax, eff_Alpha, Kparm,
                theta, beta, eff_RL, b0, b1, Gs_min, StomataWS, Catm,
                atmospheric_pressure, upperT, lowerT,
                gbw_guess)
                .Gs;  // mol / m^2 / s

        energy_balance_outputs et;
        photosynthesis_outputs photo;

        auto gs_func = [&](double current_gs) {
            et = leaf_energy_balance(
                absorbed_longwave,
                j_shortwave,
                atmospheric_pressure,
                ambient_temperature,
                gbw_canopy,
                leafwidth,
                RH,
                current_gs,
                layer_wind_speed);

            double leaf_temperature_dir =
                ambient_temperature + et.Deltat;  // degrees C

            photo =
                c4photoC(
                    i_ppfd, leaf_temperature_dir, ambient_temperature,
                    RH, Vcmax_at_25, Alpha, Kparm,
                    theta, beta, RL_at_25, b0, b1, Gs_min, StomataWS, Catm,
                    atmospheric_pressure, upperT, lowerT,
                    et.gbw_molar);

            return photo.Gs;
        };


        result_t result = solver.solve(gs_func, gsw_estimate);

        if (!is_successful(result.flag)) {
            throw std::runtime_error(
                "CanAC solver reports failed convergence with termination flag:\n    " +
                flag_message(result.flag));
        }

        // mmol / m^2 / s -> Mg / ha / hr: (3600 s/hr)(1e-3 mol/mmol)(1e-3 Mg/kg)(1e4 m^2/ha)
        double constexpr cf2 = physical_constants::molar_mass_of_water * 36;

        return PhotoCore::LeafAssim{
            /* .assim = */ photo.Assim,
            /* .stomatal_vapor_conductance = */ photo.Gs,
            /* .penman = */ et.EPenman,
            /* .priestly = */ et.EPriestly,
            /* .carboxylation = */ photo.GrossAssim,
            /* .leaf_respiration = */ photo.RL,
            /* .photorespiration = */ photo.Rp,
            /* .transpiration = */ et.TransR * cf2};
    };

    PhotoCore::CanopyIntegrand<decltype(leaf_photo), false> canopy_integrand(leaf_photo, canopy_light, kpLN, leafN, WindSpeed);
    // use `quadrature::midpoint_rule` for previous behavior
    PhotoCore::LeafAssim const canopy =
        quadrature::gauss_legendre<2, PhotoCore::LeafAssim>(canopy_integrand, 0.0, LAI, nlayers);

    double const whole_plant_gr =
        growth_resp(canopy.assim, growth_respiration_fraction);  // micromol / m^2 / s

    return canopy_photosynthesis_outputs{
        /* .Assim = */ canopy.assim - whole_plant_gr,                   // micromol / m^2 / s
        /* .canopy_conductance = */ canopy.stomatal_vapor_conductance,  // mol / m^2 / s
        /* .canopy_transpiration_penman = */ canopy.penman,             // mmol / m^2 / s
        /* .canopy_transpiration_priestly =*/canopy.priestly,           // mmol / m^2 / s
        /* .GrossAssim = */ canopy.carboxylation,                       // micromol / m^2 / s
        /* .RL = */ canopy.leaf_respiration,                            // micromol / m^2 / s
        /* .Rp = */ canopy.photorespiration,                            // micromol / m^2 / s
        /* .Trans = */ canopy.transpiration,                            // Mg / ha / hr
        /* .whole_plant_gr = */ whole_plant_gr                          // micromol / m^2 / s
    };
}
