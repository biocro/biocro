#include <algorithm>                      // for std::min, std::max
#include "../framework/constants.h"       // for molar_mass_of_water
#include "../math/quadrature/quad.h"      // for quadrature::gauss_legendre_2
#include "atmosphere_light_scattering.h"  // for atmosphere_light_scattering
#include "c4photo.h"                      // for c4photoC, solve_c4_gs
#include "leaf_energy_balance.h"          // for leaf_energy_balance
#include "photosynthesis.h"               // for leaf_assim, CanopyIntegrand
#include "respiration.h"                  // for growth_resp
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
    atmosphere_light_scattering const light_model(
        cosine_zenith_angle,
        atmospheric_pressure,
        atmospheric_transmittance,
        atmospheric_scattering);

    double heightf = 1.0;
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
    auto leaf_photo = [&](double i_ppfd, double j_shortwave, double layer_wind_speed, double layer_leafN) -> leaf_assim {
        double eff_Vcmax = Vcmax_at_25;
        double eff_Alpha = Alpha;
        double eff_RL = RL_at_25;
        if (lnfun != 0) {
            eff_Vcmax = std::max(0.0, std::min(Vcmax_at_25, nitroP.Vmaxb1 * layer_leafN + nitroP.Vmaxb0));
            eff_Alpha = nitroP.alphab1 * layer_leafN + nitroP.alphab0;
            eff_RL = nitroP.Rdb1 * layer_leafN + nitroP.Rdb0;
        }

        // Solve for gs
        root_finding::result_t const result = solve_c4_gs(
            absorbed_longwave,     // J / (m^2 leaf) / s
            j_shortwave,           // J / (m^2 leaf) / s
            eff_Alpha,             // mol / mol
            ambient_temperature,   // degrees C
            atmospheric_pressure,  // Pa
            b0,                    // mol / m^2 / s
            b1,                    // dimensionless
            beta,                  // dimensionless
            Catm,                  // micromol / mol
            gbw_canopy,            // m / s
            Gs_min,                // mol / m^2 / s
            i_ppfd,                // micromol / m^2 / s
            Kparm,                 // mol / m^2 / s
            leafwidth,             // m
            lowerT,                // degrees C
            RH,                    // dimensionless
            eff_RL,                // micromol / m^2 / s
            StomataWS,             // dimensionless
            theta,                 // dimensionless
            upperT,                // degrees C
            eff_Vcmax,             // micromol / m^2 / s
            layer_wind_speed       // m / s
        );

        // Get final values
        double const Gs = result.root;  // mol / m^2 / s

        energy_balance_outputs const et = leaf_energy_balance(
            absorbed_longwave,
            j_shortwave,
            atmospheric_pressure,
            ambient_temperature,
            gbw_canopy,
            leafwidth,
            RH,
            Gs,
            layer_wind_speed);

        double const Tleaf = ambient_temperature + et.Deltat;  // degrees C

        photosynthesis_outputs const photo = c4photoC(
            i_ppfd,
            Tleaf,
            ambient_temperature,
            RH,
            eff_Vcmax,
            eff_Alpha,
            Kparm,
            theta,
            beta,
            eff_RL,
            b0,
            b1,
            Gs_min,
            StomataWS,
            Catm,
            atmospheric_pressure,
            upperT,
            lowerT,
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

    canopy_integrand<decltype(leaf_photo), false> canopy_integrand(leaf_photo, light_dist, kpLN, leafN, WindSpeed);
    // use `quadrature::midpoint_rule` for previous behavior
    leaf_assim const canopy =
        quadrature::gauss_legendre<2, leaf_assim>(canopy_integrand, 0.0, LAI, nlayers);

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
