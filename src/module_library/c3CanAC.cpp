#include "../framework/constants.h"            // for molar_mass_of_water, molar_mass_of_glucose
#include "../math/quadrature/quad.h"           // for quadrature::gauss_legendre_2
#include "../math/roots/onedim/fixed_point.h"  // for fixed_point
#include "c3photo.h"                           // for c3photoC
#include "core/photosynthesis.h"               // for PhotoCore::LeafAssim, CanopyIntegrand
#include "leaf_energy_balance.h"               // for leaf_energy_balance
#include "core/atmosphere_light_scattering.h"  // for PhotoCore::AtmosphereLightScattering
#include "respiration.h"                       // for growth_resp
#include "c3CanAC.h"

canopy_photosynthesis_outputs c3CanAC(
    c3_temperature_response_parameters const tr_param,
    double const absorbed_longwave,            // J / m^2 / s
    double const ambient_temperature,          // degrees C
    double const atmospheric_pressure,         // Pa
    double const atmospheric_scattering,       // dimensionless
    double const atmospheric_transmittance,    // dimensionless
    double const b0,                           // mol / m^2 / s
    double const b1,                           // dimensionless
    double const beta_PSII,                    // dimensionless (fraction of absorbed light that reaches photosystem II)
    double const Catm,                         // ppm
    double const chil,                         // dimensionless
    double const cosine_zenith_angle,          // dimensionless
    double const electrons_per_carboxylation,  // self-explanatory units
    double const electrons_per_oxygenation,    // self-explanatory units
    double const gbw_canopy,                   // m / s
    double const gm_at_25,                     // mol / m^2 / s / Pa
    double const growth_respiration_fraction,  // dimensionless
    double const Gs_min,                       // mol / m^2 / s
    double const Gstar_at_25,                  // micromol / mol
    double const heightf,                      // m^(-1)
    double const Jmax_at_25,                   // micromol / m^2 / s
    double const k_diffuse,                    // dimensionless
    double const Kc_at_25,                     // micromol / mol
    double const Ko_at_25,                     // mmol / mol
    double const kpLN,
    double const LAI,                     // dimensionless
    double const leaf_reflectance_nir,    // dimensionless
    double const leaf_reflectance_par,    // dimensionless
    double const leaf_transmittance_nir,  // dimensionless
    double const leaf_transmittance_par,  // dimensionless
    double const leaf_width,              // m
    double const leafN,
    double const lnb0,  // micromol / m^2 / s
    double const lnb1,
    double const o2,                   // mmol / mol
    double const par_energy_content,   // J / micromol
    double const par_energy_fraction,  // dimensionless
    double const RH,                   // Pa / Pa
    double const RL_at_25,             // micromol / m^2 / s
    double const solarR,               // micromol / m^2 / s
    double const StomataWS,            // dimensionless
    double const Tp_at_25,             // micromol / m^2 / s
    double Vcmax_at_25,                // micromol / m^2 / s
    double const WindSpeed,            // m / s
    double const WindSpeedHeight,      // m
    int const lnfun,                   // dimensionless switch
    int const nlayers                  // dimensionless
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

    PhotoCore::CanopyLight canopy_light_model(
        q_dir,
        q_diff,  // micromol / m^2 / s
        chil,
        cosine_zenith_angle,
        heightf,
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
    auto leaf_photo = [&](double iabs, double j_shortwave, double layer_wind_speed, double layer_leafN) -> PhotoCore::LeafAssim {
        double const effective_Vcmax = (lnfun != 0) ? layer_leafN * lnb1 + lnb0 : Vcmax_at_25;
        double constexpr gbw_guess = 1.2;  // mol / m^2 / s

        // Initial guess: evaluate photosynthesis at ambient leaf temperature
        double gsw_estimate =
            c3photoC(
                tr_param, iabs, ambient_temperature, ambient_temperature,
                RH, gm_at_25, Gstar_at_25, Kc_at_25, Ko_at_25, effective_Vcmax, Jmax_at_25,
                Tp_at_25, RL_at_25, b0, b1, Gs_min, Catm, atmospheric_pressure,
                o2, StomataWS, electrons_per_carboxylation,
                electrons_per_oxygenation, beta_PSII, gbw_guess)
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
                leaf_width,
                RH,
                current_gs,
                layer_wind_speed);

                double leaf_temperature_dir =
                ambient_temperature + et.Deltat;  // degrees C

            photo = c3photoC(
                tr_param, iabs, leaf_temperature_dir, ambient_temperature,
                RH, gm_at_25, Gstar_at_25, Kc_at_25, Ko_at_25, effective_Vcmax, Jmax_at_25,
                Tp_at_25, RL_at_25, b0, b1, Gs_min, Catm, atmospheric_pressure,
                o2, StomataWS, electrons_per_carboxylation, electrons_per_oxygenation,
                beta_PSII, et.gbw_molar);

            return photo.Gs;
        };

        result_t result = solver.solve(gs_func, gsw_estimate);

        if (!is_successful(result.flag)) {
            throw std::runtime_error(
                "c3Canopy solver reports failed convergence. Termination flag:\n    " +
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

    PhotoCore::CanopyIntegrand integrand(
        leaf_photo,
        canopy_light_model,
        kpLN,
        leafN,     // micromol / m^2 / s
        WindSpeed  // m / s

    );

    // use `quadrature::midpoint_rule` for previous behavior
    PhotoCore::LeafAssim const canopy =
        quadrature::gauss_legendre<2, PhotoCore::LeafAssim>(integrand, 0.0, LAI, nlayers);

    // Calculate the rate of whole-plant growth respiration
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
