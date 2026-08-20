#include "../framework/constants.h"       // for molar_mass_of_water, molar_mass_of_glucose
#include "../math/quadrature/quad.h"      // for quadrature::gauss_legendre_2
#include "atmosphere_light_scattering.h"  // for atmosphere_light_scattering
#include "c3photo.h"                      // for c3photoC, solve_c3_gs
#include "leaf_energy_balance.h"          // for leaf_energy_balance
#include "photosynthesis.h"               // for leaf_assim, CanopyIntegrand
#include "respiration.h"                  // for growth_resp
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
    atmosphere_light_scattering const light_model(
        cosine_zenith_angle,
        atmospheric_pressure,
        atmospheric_transmittance,
        atmospheric_scattering);

    canopy_light::parameters params =
        {chil,
         cosine_zenith_angle,
         heightf,
         k_diffuse,
         LAI,
         leaf_reflectance_nir,
         leaf_reflectance_par,
         leaf_transmittance_nir,
         leaf_transmittance_par,
         par_energy_content,
         par_energy_fraction};

    canopy_light light_dist = canopy_light::from_solar(solarR, light_model, params);

    // Leaf-level photosynthesis function for use with canopy_integrand.
    // Solves the coupled stomatal conductance / energy balance system for a
    // single leaf class (sunlit or shaded) and returns a LeafAssim summary.
    auto leaf_photo = [&](double iabs, double j_shortwave, double layer_wind_speed, double layer_leafN) -> leaf_assim {
        double const effective_Vcmax = (lnfun != 0) ? layer_leafN * lnb1 + lnb0 : Vcmax_at_25;

        // Solve for gs
        root_finding::result_t const result = solve_c3_gs(
            tr_param,
            absorbed_longwave,            // J / (m^2 leaf) / s
            iabs,                         // micromol / (m^2 leaf) / s
            j_shortwave,                  // J / (m^2 leaf) / s
            ambient_temperature,          // degrees C
            atmospheric_pressure,         // Pa
            b0,                           // mol / m^2 / s
            b1,                           // dimensionless
            beta_PSII,                    // dimensionless
            Catm,                         // micromol / mol
            electrons_per_carboxylation,  // self-explanatory units
            electrons_per_oxygenation,    // self-explanatory units
            gbw_canopy,                   // mol / m^2 / s
            gm_at_25,                     // mol / m^2 / s / Pa
            Gs_min,                       // mol / m^2 / s
            Gstar_at_25,                  // micromol / mol
            Jmax_at_25,                   // micromol / m^2 / s
            Kc_at_25,                     // micromol / mol
            Ko_at_25,                     // mmol / mol
            leaf_width,                   // m
            o2,                           // mmol / mol
            RH,                           // dimensionless
            RL_at_25,                     // micromol / m^2 / s
            StomataWS,                    // dimensionless
            Tp_at_25,                     // micromol / m^2 / s
            effective_Vcmax,              // micromol / m^2 / s
            layer_wind_speed              // m / s
        );

        // Get final values
        double const Gs = result.root;  // mol / m^2 / s

        energy_balance_outputs const et = leaf_energy_balance(
            absorbed_longwave,
            j_shortwave,
            atmospheric_pressure,
            ambient_temperature,
            gbw_canopy,
            leaf_width,
            RH,
            Gs,
            layer_wind_speed);

        double const Tleaf = ambient_temperature + et.Deltat;  // degrees C

        photosynthesis_outputs const photo = c3photoC(
            tr_param,
            iabs,
            Tleaf,
            ambient_temperature,
            RH,
            gm_at_25,
            Gstar_at_25,
            Kc_at_25,
            Ko_at_25,
            effective_Vcmax,
            Jmax_at_25,
            Tp_at_25,
            RL_at_25,
            b0,
            b1,
            Gs_min,
            Catm,
            atmospheric_pressure,
            o2,
            StomataWS,
            electrons_per_carboxylation,
            electrons_per_oxygenation,
            beta_PSII,
            et.gbw_molar);

        // mmol / m^2 / s -> Mg / ha / hr: (3600 s/hr)(1e-3 mol/mmol)(1e-3 Mg/kg)(1e4 m^2/ha)
        double constexpr cf2 = physical_constants::molar_mass_of_water * 36;

        return leaf_assim{
            /* .assim = */ photo.Assim,
            /* .stomatal_vapor_conductance = */ photo.Gs,
            /* .penman = */ et.EPenman,
            /* .priestly = */ et.EPriestly,
            /* .carboxylation = */ photo.GrossAssim,
            /* .leaf_respiration = */ photo.RL,
            /* .photorespiration = */ photo.Rp,
            /* .transpiration = */ et.TransR * cf2};
    };

    canopy_integrand integrand(
        leaf_photo,
        light_dist,
        kpLN,
        leafN,     // micromol / m^2 / s
        WindSpeed  // m / s

    );

    // use `quadrature::midpoint_rule` for previous behavior
    leaf_assim const canopy =
        quadrature::gauss_legendre<2, leaf_assim>(integrand, 0.0, LAI, nlayers);

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
