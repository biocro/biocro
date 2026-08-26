#include <algorithm>                  // for std::min, std::max
#include "../framework/constants.h"   // for molar_mass_of_water
#include "../math/quadrature/quad.h"  // for quadrature::gauss_legendre_2
#include "c4photo.h"                  // for c4photoC, solve_c4_gs
#include "leaf_energy_balance.h"      // for leaf_energy_balance
#include "photosynthesis.h"           // for leaf_assim, CanopyIntegrand
#include "respiration.h"              // for growth_resp
#include "CanAC.h"

canopy_photosynthesis_outputs CanAC(
    nitroParms const& nitroP,
    double const absorbed_longwave,  // J / m^2 / s
    double const Alpha,
    double const ambient_temperature,          // degrees C
    double const atmospheric_pressure,         // Pa
    double const b0,                           // mol / m^2 / s
    double const b1,                           // dimensionless
    double const beta,                         // dimensionless
    double const Catm,                         // ppm
    double const chil,                         // dimensionless
    double const cosine_zenith_angle,          // dimensionless
    double const gbw_canopy,                   // m / s
    double const growth_respiration_fraction,  // dimensionless
    double const Gs_min,                       // mol / m^2 / s
    double const k_diffuse,                    // dimensionless
    double const Kparm,
    double const kpLN,
    double const LAI,                     // dimensionless from m^2 / m^2
    double const leaf_reflectance_nir,    // dimensionless
    double const leaf_reflectance_par,    // dimensionless
    double const leaf_transmittance_nir,  // dimensionless
    double const leaf_transmittance_par,  // dimensionless
    double const leafN,
    double const leafwidth,              // m
    double const lowerT,                 // degrees C
    double const nir_incident_diffuse,   // J / m^2 / s
    double const nir_incident_direct,    // J / m^2 / s
    double const par_energy_content,     // J / micromol
    double const par_energy_fraction,    // dimensionless
    double const ppfd_incident_diffuse,  // micromol / m^2 / s
    double const ppfd_incident_direct,   // micromol / m^2 / s
    double const RH,                     // dimensionless from Pa / Pa
    double const RL_at_25,               // micromol / m^2 / s
    double const solarR,                 // micromol / m^2 / s
    double const StomataWS,              // dimensionless
    double const theta,                  // dimensionless
    double const upperT,                 // degrees C
    double const Vcmax_at_25,            // micromol / m^2 / s
    double const WindSpeed,              // m / s
    int const lnfun,                     // dimensionless switch
    int const nlayers                    // dimensionless
)
{
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

    canopy_light light_dist = {
        nir_incident_direct,
        nir_incident_diffuse,
        ppfd_incident_direct,
        ppfd_incident_diffuse,
        params};

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
