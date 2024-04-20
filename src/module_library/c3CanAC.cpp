#include <vector>
#include "c3CanAC.h"
#include "BioCro.h"                  // for WINDprof, c3EvapoTrans
#include "c3photo.h"                 // for c3photoC
#include "lightME.h"                 // for lightME
#include "sunML.h"                   // for sunML
#include "../framework/constants.h"  // for molar_mass_of_water, molar_mass_of_glucose

canopy_photosynthesis_outputs c3CanAC(
    double LAI,                  // dimensionless
    double cosine_zenith_angle,  // hr
    double solarR,               // micromol / m^2 / s
    double ambient_temperature,  // degrees C
    double RH,                   // Pa / Pa
    double WindSpeed,            // m / s
    int nlayers,                 // dimensionless
    double Vmax,
    double Jmax,
    double tpu_rate_max,
    double Rd,
    double Catm,  // ppm
    double o2,
    double b0,
    double b1,
    double Gs_min,  // mol / m^2 / s
    double theta,   // dimensionless
    double kd,
    double heightf,
    double leafN,
    double kpLN,
    double lnb0,
    double lnb1,
    int lnfun,  // dimensionless switch
    double chil,
    double StomataWS,                    // dimensionless
    double specific_heat_of_air,         // J / kg / K
    double atmospheric_pressure,         // Pa
    double atmospheric_transmittance,    // dimensionless
    double atmospheric_scattering,       // dimensionless
    double growth_respiration_fraction,  // dimensionless
    double electrons_per_carboxylation,  // self-explanatory units
    double electrons_per_oxygenation,    // self-explanatory units
    double absorptivity_par,             // dimensionless
    double par_energy_content,           // J / micromol
    double par_energy_fraction,          // dimensionless
    double leaf_transmittance,           // dimensionless
    double leaf_reflectance,             // dimensionless
    double minimum_gbw,                  // mol / m^2 / s
    double WindSpeedHeight,              // m
    double beta_PSII                     // dimensionless (fraction of absorbed light that reaches photosystem II)
)
{
    struct Light_model light_model = lightME(
        cosine_zenith_angle,
        atmospheric_pressure,
        atmospheric_transmittance,
        atmospheric_scattering);

    // q_dir: flux through a plane perpendicular to the rays of the sun
    // q_diff: flux through any surface
    double q_dir = light_model.direct_fraction * solarR;    // micromol / m^2 / s
    double q_diff = light_model.diffuse_fraction * solarR;  // micromol / m^2 / s

    struct Light_profile light_profile =
        sunML(q_dir, q_diff, LAI, nlayers, cosine_zenith_angle, kd, chil, absorptivity_par,
              heightf, par_energy_content, par_energy_fraction,
              leaf_transmittance, leaf_reflectance);

    double LAIc = LAI / nlayers;  // dimensionless

    std::vector<double> wind_speed_profile(nlayers);
    WINDprof(WindSpeed, LAI, wind_speed_profile);  // Modifies wind_speed_profile

    std::vector<double> leafN_profile(nlayers);
    LNprof(leafN, LAI, kpLN, leafN_profile);  // Modifies leafN_profile

    double CanopyA{0.0};             // micromol / m^2 / s
    double GCanopyA{0.0};            // micromol / m^2 / s
    double canopy_rp{0.0};           // micromol / m^2 / s
    double CanopyT{0.0};             // mmol / m^2 / s
    double CanopyPe{0.0};            // mmol / m^2 / s
    double CanopyPr{0.0};            // mmol / m^2 / s
    double canopy_conductance{0.0};  // mmol / m^2 / s

    double gbw_guess{1.2};  // mol / m^2 / s

    for (int i = 0; i < nlayers; ++i) {
        // Calculations that are the same for sunlit and shaded leaves
        int current_layer = nlayers - 1 - i;
        double leafN_lay = leafN_profile[current_layer];

        double vmax1;
        if (lnfun == 0) {
            vmax1 = Vmax;
        } else {
            vmax1 = leafN_lay * lnb1 + lnb0;
        }

        double layer_wind_speed = wind_speed_profile[current_layer];             // m / s
        double CanHeight = light_profile.height[current_layer];                  // m
        double j_avg = light_profile.average_absorbed_shortwave[current_layer];  // J / m^2 / s

        // Calculations for sunlit leaves. First, estimate stomatal conductance
        // by assuming the leaf has the same temperature as the air. Then, use
        // energy balance to get a better temperature estimate using that value
        // of stomatal conductance. Get the final estimate of stomatal
        // conductance using the new value of the leaf temperature.
        double iabs_dir = light_profile.sunlit_absorbed_ppfd[current_layer];  // micromole / m^2 / s
        double pLeafsun = light_profile.sunlit_fraction[current_layer];       // dimensionless
        double Leafsun = LAIc * pLeafsun;                                     // dimensionless

        double direct_gsw_estimate =
            c3photoC(
                iabs_dir, ambient_temperature, ambient_temperature,
                RH, vmax1, Jmax,
                tpu_rate_max, Rd, b0, b1, Gs_min, Catm, atmospheric_pressure,
                o2, theta, StomataWS,
                electrons_per_carboxylation, electrons_per_oxygenation,
                beta_PSII, gbw_guess)
                .Gs;  // mmol / m^2 / s

        struct ET_Str et_direct =
            c3EvapoTrans(
                j_avg, ambient_temperature, RH, layer_wind_speed,
                CanHeight, specific_heat_of_air, direct_gsw_estimate,
                minimum_gbw, WindSpeedHeight);

        double leaf_temperature_dir = ambient_temperature + et_direct.Deltat;  // degrees C

        photosynthesis_outputs direct_photo =
            c3photoC(
                iabs_dir, leaf_temperature_dir, ambient_temperature,
                RH, vmax1, Jmax,
                tpu_rate_max, Rd, b0, b1, Gs_min, Catm, atmospheric_pressure,
                o2, theta, StomataWS,
                electrons_per_carboxylation, electrons_per_oxygenation,
                beta_PSII, et_direct.boundary_layer_conductance);

        // Calculations for shaded leaves. First, estimate stomatal conductance
        // by assuming the leaf has the same temperature as the air. Then, use
        // energy balance to get a better temperature estimate using that value
        // of stomatal conductance. Get the final estimate of stomatal
        // conductance using the new value of the leaf temperature.
        double iabs_diff = light_profile.shaded_absorbed_ppfd[current_layer];  // micromole / m^2 /s
        double pLeafshade = light_profile.shaded_fraction[current_layer];      // dimensionless
        double Leafshade = LAIc * pLeafshade;                                  // dimensionless

        double diffuse_gsw_estimate =
            c3photoC(
                iabs_diff, ambient_temperature, ambient_temperature,
                RH, vmax1, Jmax,
                tpu_rate_max, Rd, b0, b1, Gs_min, Catm, atmospheric_pressure,
                o2, theta, StomataWS,
                electrons_per_carboxylation, electrons_per_oxygenation,
                beta_PSII, gbw_guess)
                .Gs;  // mmol / m^2 / s

        struct ET_Str et_diffuse =
            c3EvapoTrans(
                j_avg, ambient_temperature, RH, layer_wind_speed,
                CanHeight, specific_heat_of_air, diffuse_gsw_estimate,
                minimum_gbw, WindSpeedHeight);

        double leaf_temperature_Idiffuse = ambient_temperature + et_diffuse.Deltat;  // degrees C

        photosynthesis_outputs diffuse_photo =
            c3photoC(
                iabs_diff, leaf_temperature_Idiffuse, ambient_temperature,
                RH, vmax1,
                Jmax, tpu_rate_max, Rd, b0, b1, Gs_min, Catm,
                atmospheric_pressure, o2, theta, StomataWS,
                electrons_per_carboxylation,
                electrons_per_oxygenation, beta_PSII,
                et_diffuse.boundary_layer_conductance);

        // Combine sunlit and shaded leaves
        CanopyA += Leafsun * direct_photo.Assim + Leafshade * diffuse_photo.Assim;             // micromol / m^2 / s
        CanopyT += Leafsun * et_direct.TransR + Leafshade * et_diffuse.TransR;                 // mmol / m^2 / s
        GCanopyA += Leafsun * direct_photo.GrossAssim + Leafshade * diffuse_photo.GrossAssim;  // micromol / m^2 / s
        canopy_rp += Leafsun * direct_photo.Rp + Leafshade * diffuse_photo.Rp;                 // micromol / m^2 / s

        CanopyPe += Leafsun * et_direct.EPenman + Leafshade * et_diffuse.EPenman;        // mmol / m^2 / s
        CanopyPr += Leafsun * et_direct.EPriestly + Leafshade * et_diffuse.EPriestly;    // mmol / m^2 / s
        canopy_conductance += Leafsun * direct_photo.Gs + Leafshade * diffuse_photo.Gs;  // mmol / m^2 / s
    }

    // For assimilation, we need to convert micromol / m^2 / s into
    // Mg / ha / hr, assuming that all carbon is converted into biomass in the
    // form of glucose (C6H12O6), i.e., six assimilated CO2 molecules contribute
    // one glucose molecule. Using the molar mass of glucose in kg / mol, the
    // conversion can be accomplished with the following factor:
    // (1 glucose / 6 CO2) * (3600 s / hr) * (1e-6 mol / micromol) *
    //     (1e-3 Mg / kg) * (1e4 m^2 / ha)
    // = 6e-3 s * mol * Mg * m^2 / (hr * micromol * kg * ha)
    const double cf = physical_constants::molar_mass_of_glucose * 6e-3;  // (Mg / ha / hr) / (micromol / m^2 / s)

    // For transpiration, we need to convert mmol / m^2 / s into Mg / ha / hr
    // using the molar mass of water in kg / mol, which can be accomplished by
    // the following conversion factor:
    // (3600 s / hr) * (1e-3 mol / mmol) * (1e-3 Mg / kg) * (1e4 m^2 / ha)
    // = 36 s * mol * Mg * m^2 / (hr * mmol * kg * ha)
    const double cf2 = physical_constants::molar_mass_of_water * 36;  // (Mg / ha / hr) / (mmol / m^2 / s)

    canopy_photosynthesis_outputs ans;
    ans.Assim = CanopyA * (1.0 - growth_respiration_fraction) * cf;  // Mg / ha / hr
    ans.GrossAssim = GCanopyA * cf;                                  // Mg / ha / hr
    ans.Rp = canopy_rp * cf;                                         // Mg / ha / hr
    ans.Trans = CanopyT * cf2;                                       // Mg / ha / hr
    ans.canopy_transpiration_penman = CanopyPe;                      // mmol / m^2 / s
    ans.canopy_transpiration_priestly = CanopyPr;                    // mmol / m^2 / s
    ans.canopy_conductance = canopy_conductance;                     // mmol / m^2 / s

    return ans;
}
