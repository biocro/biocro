#include <vector>
#include "../framework/constants.h"  // for molar_mass_of_water, molar_mass_of_glucose
#include "BioCro.h"                  // for WINDprof
#include "c3photo.h"                 // for c3photoC
#include "leaf_energy_balance.h"     // for leaf_energy_balance
#include "lightME.h"                 // for lightME
#include "respiration.h"             // for growth_resp
#include "sunML.h"                   // for sunML
#include "c3CanAC.h"
#include "canopy_photosynthesis_outputs.h" // for CanopyPhotosynthesis

CanopyPhotosynthesis c3CanAC(
    c3_temperature_response_parameters const tr_param,
    double absorbed_longwave,            // J / m^2 / s
    double ambient_temperature,          // degrees C
    double atmospheric_pressure,         // Pa
    double atmospheric_scattering,       // dimensionless
    double atmospheric_transmittance,    // dimensionless
    double b0,                           // mol / m^2 / s
    double b1,                           // dimensionless
    double beta_PSII,                    // dimensionless (fraction of absorbed light that reaches photosystem II)
    double Catm,                         // ppm
    double chil,                         // dimensionless
    double cosine_zenith_angle,          // dimensionless
    double electrons_per_carboxylation,  // self-explanatory units
    double electrons_per_oxygenation,    // self-explanatory units
    double gbw_canopy,                   // m / s
    double growth_respiration_fraction,  // dimensionless
    double Gs_min,                       // mol / m^2 / s
    double heightf,                      // m^(-1)
    double Jmax_at_25,                   // micromol / m^2 / s
    double k_diffuse,                    // dimensionless
    double kpLN,
    double LAI,                     // dimensionless
    double leaf_reflectance_nir,    // dimensionless
    double leaf_reflectance_par,    // dimensionless
    double leaf_transmittance_nir,  // dimensionless
    double leaf_transmittance_par,  // dimensionless
    double leaf_width,              // m
    double leafN,
    double lnb0,  // micromol / m^2 / s
    double lnb1,
    double o2,                   // mmol / mol
    double par_energy_content,   // J / micromol
    double par_energy_fraction,  // dimensionless
    double RH,                   // Pa / Pa
    double RL_at_25,             // micromol / m^2 / s
    double solarR,               // micromol / m^2 / s
    double StomataWS,            // dimensionless
    double Tp_at_25,             // micromol / m^2 / s
    double Vcmax_at_25,          // micromol / m^2 / s
    double WindSpeed,            // m / s
    double WindSpeedHeight,      // m
    int lnfun,                   // dimensionless switch
    int nlayers                  // dimensionless
)
{
    Light_model const light_model = lightME(
        cosine_zenith_angle,
        atmospheric_pressure,
        atmospheric_transmittance,
        atmospheric_scattering);

    // q_dir: flux through a plane perpendicular to the rays of the sun
    // q_diff: flux through any surface
    double const q_dir = light_model.direct_fraction * solarR;    // micromol / m^2 / s
    double const q_diff = light_model.diffuse_fraction * solarR;  // micromol / m^2 / s

    const CanopyLight canopy_light_model{
        q_dir,
        q_diff,
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
        par_energy_fraction};

    double const lai_per_layer = LAI / nlayers;  // dimensionless
    CanopyPhotosynthesis canopy(0);
    //double CanopyA{0.0};             // micromol / m^2 / s
    //double GCanopyA{0.0};            // micromol / m^2 / s
    //double canopy_rp{0.0};           // micromol / m^2 / s
    //double canopy_RL{0.0};           // micromol / m^2 / s
    //double CanopyT{0.0};             // mmol / m^2 / s
    //double CanopyPe{0.0};            // mmol / m^2 / s
    //double CanopyPr{0.0};            // mmol / m^2 / s
    //double canopy_conductance{0.0};  // mmol / m^2 / s
    
    double gbw_guess{1.2};  // mol / m^2 / s
    LightProfile light_profile;
    for (int i = 0; i < nlayers; ++i) {
        double cumulative_lai = (0.5 + i) * lai_per_layer;  // midpoint rule
        double cumulative_lai_at_top = i * lai_per_layer;   // windspeed is evaluated at top of layer, not midpoint

        // Calculations that are the same for sunlit and shaded leaves
        double leafN_lay = leaf_nitrogen_profile(cumulative_lai_at_top, leafN, kpLN);

        double layer_wind_speed = wind_speed_profile(cumulative_lai_at_top, WindSpeed);  // m/s
        light_profile = canopy_light_model.get_light_profile(cumulative_lai);
        // Calculations for sunlit leaves. First, estimate stomatal conductance
        // by assuming the leaf has the same temperature as the air. Then, use
        // energy balance to get a better temperature estimate using that value
        // of stomatal conductance. Get the final estimate of stomatal
        // conductance using the new value of the leaf temperature.
        double iabs_dir = light_profile.sunlit_absorbed_ppfd;    // micromol / m^2 / s
        double j_dir = light_profile.sunlit_absorbed_shortwave;  // J / m^2 / s
        double pLeafsun = light_profile.sunlit_fraction;         // dimensionless
        double Leafsun = lai_per_layer * pLeafsun;               // dimensionless

        double direct_gsw_estimate =
            c3photoC(
                tr_param, iabs_dir, ambient_temperature, ambient_temperature,
                RH, Vcmax_at_25, Jmax_at_25,
                Tp_at_25, RL_at_25, b0, b1, Gs_min, Catm, atmospheric_pressure,
                o2, StomataWS,
                electrons_per_carboxylation, electrons_per_oxygenation,
                beta_PSII, gbw_guess)
                .Gs;  // mol / m^2 / s

        energy_balance_outputs et_direct = leaf_energy_balance(
            absorbed_longwave,
            j_dir,
            atmospheric_pressure,
            ambient_temperature,
            gbw_canopy,
            leaf_width,
            RH,
            direct_gsw_estimate,
            layer_wind_speed);

        double leaf_temperature_dir = ambient_temperature + et_direct.Deltat;  // degrees C

        photosynthesis_outputs direct_photo =
            c3photoC(
                tr_param, iabs_dir, leaf_temperature_dir, ambient_temperature,
                RH, Vcmax_at_25, Jmax_at_25,
                Tp_at_25, RL_at_25, b0, b1, Gs_min, Catm, atmospheric_pressure,
                o2, StomataWS,
                electrons_per_carboxylation, electrons_per_oxygenation,
                beta_PSII, et_direct.gbw_molecular);

        // Calculations for shaded leaves. First, estimate stomatal conductance
        // by assuming the leaf has the same temperature as the air. Then, use
        // energy balance to get a better temperature estimate using that value
        // of stomatal conductance. Get the final estimate of stomatal
        // conductance using the new value of the leaf temperature.
        double iabs_diff = light_profile.shaded_absorbed_ppfd;    // micromol / m^2 /s
        double j_diff = light_profile.shaded_absorbed_shortwave;  // J / m^2 / s
        double pLeafshade = light_profile.shaded_fraction;        // dimensionless
        double Leafshade = lai_per_layer * pLeafshade;            // dimensionless

        double diffuse_gsw_estimate =
            c3photoC(
                tr_param, iabs_diff, ambient_temperature, ambient_temperature,
                RH, Vcmax_at_25, Jmax_at_25,
                Tp_at_25, RL_at_25, b0, b1, Gs_min, Catm, atmospheric_pressure,
                o2, StomataWS,
                electrons_per_carboxylation, electrons_per_oxygenation,
                beta_PSII, gbw_guess)
                .Gs;  // mol / m^2 / s

        energy_balance_outputs et_diffuse = leaf_energy_balance(
            absorbed_longwave,
            j_diff,
            atmospheric_pressure,
            ambient_temperature,
            gbw_canopy,
            leaf_width,
            RH,
            diffuse_gsw_estimate,
            layer_wind_speed);

        double leaf_temperature_Idiffuse = ambient_temperature + et_diffuse.Deltat;  // degrees C

        photosynthesis_outputs diffuse_photo =
            c3photoC(
                tr_param, iabs_diff, leaf_temperature_Idiffuse, ambient_temperature,
                RH, Vcmax_at_25,
                Jmax_at_25, Tp_at_25, RL_at_25, b0, b1, Gs_min, Catm,
                atmospheric_pressure, o2, StomataWS,
                electrons_per_carboxylation,
                electrons_per_oxygenation, beta_PSII,
                et_diffuse.gbw_molecular);

        // Combine sunlit and shaded leaves
//        canopy += Leafsun * direct_photo  + Leafshade * diffuse_photo;
        canopy.assim += Leafsun * direct_photo.Assim + Leafshade * diffuse_photo.Assim;             // micromol / m^2 / s
        canopy.transpiration += Leafsun * et_direct.TransR + Leafshade * et_diffuse.TransR;                 // mmol / m^2 / s
        canopy.carboxylation += Leafsun * direct_photo.GrossAssim + Leafshade * diffuse_photo.GrossAssim;  // micromol / m^2 / s
        canopy.photorespiration += Leafsun * direct_photo.Rp + Leafshade * diffuse_photo.Rp;                 // micromol / m^2 / s
        canopy.leaf_respiration += Leafsun * direct_photo.RL + Leafshade * diffuse_photo.RL;                 // micromol / m^2 / s

        canopy.penman += Leafsun * et_direct.EPenman + Leafshade * et_diffuse.EPenman;        // mmol / m^2 / s
        canopy.priestly += Leafsun * et_direct.EPriestly + Leafshade * et_diffuse.EPriestly;    // mmol / m^2 / s
        canopy.stomatal_vapor_conductance += Leafsun * direct_photo.Gs + Leafshade * diffuse_photo.Gs;  // mol / m^2 / s
    }

    // Calculate the rate of whole-plant growth respiration
    canopy.whole_plant_growth_respiration = growth_resp(canopy.assim, growth_respiration_fraction);  // micromol / m^2 / s
    canopy.assim -= canopy.whole_plant_growth_respiration;
    // For transpiration, we need to convert mmol / m^2 / s into Mg / ha / hr
    // using the molar mass of water in kg / mol, which can be accomplished by
    // the following conversion factor:
    // (3600 s / hr) * (1e-3 mol / mmol) * (1e-3 Mg / kg) * (1e4 m^2 / ha)
    // = 36 s * mol * Mg * m^2 / (hr * mmol * kg * ha)
    double constexpr cf2 = physical_constants::molar_mass_of_water * 36;  // (Mg / ha / hr) / (mmol / m^2 / s)
    canopy.transpiration *= cf2;
    return canopy;
}
