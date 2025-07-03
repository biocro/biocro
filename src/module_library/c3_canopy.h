#ifndef C3_CANOPY_H
#define C3_CANOPY_H

#include <cmath>  // For floor
#include <vector>
#include <string>

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "../framework/constants.h"  // for molar_mass_of_water, molar_mass_of_glucose

#include "c3_temperature_response.h"  // for c3_temperature_response_parameters
#include "BioCro.h"                   // for WINDprof
#include "c3photo.h"                  // for c3photoC
#include "leaf_energy_balance.h"      // for leaf_energy_balance
#include "lightME.h"                  // for lightME
#include "respiration.h"              // for growth_resp
#include "sunML.h"                    // for sunML
#include "c3CanAC.h"

namespace standardBML
{

const size_t nlayers__ = 10;

std::vector<const double*> get_profile(state_map const& input_quantities, std::string variable)
{
    std::vector<const double*> out;
    out.reserve(nlayers__);
    for (size_t i = 0; i < nlayers__; ++i)
        out.emplace_back(get_ip(input_quantities, variable + "_" + std::to_string(i)));
    return out;
}

class c3_canopy : public direct_module
{
   public:
    c3_canopy(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          absorbed_longwave{get_input(input_quantities, "absorbed_longwave")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          atmospheric_scattering{get_input(input_quantities, "atmospheric_scattering")},
          atmospheric_transmittance{get_input(input_quantities, "atmospheric_transmittance")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},
          beta_PSII{get_input(input_quantities, "beta_PSII")},
          Catm{get_input(input_quantities, "Catm")},
          chil{get_input(input_quantities, "chil")},
          cosine_zenith_angle{get_input(input_quantities, "cosine_zenith_angle")},
          electrons_per_carboxylation{get_input(input_quantities, "electrons_per_carboxylation")},
          electrons_per_oxygenation{get_input(input_quantities, "electrons_per_oxygenation")},
          gbw_canopy{get_input(input_quantities, "gbw_canopy")},
          growth_respiration_fraction{get_input(input_quantities, "growth_respiration_fraction")},
          Gs_min{get_input(input_quantities, "Gs_min")},
          Gstar_c{get_input(input_quantities, "Gstar_c")},
          Gstar_Ea{get_input(input_quantities, "Gstar_Ea")},
          heightf{get_input(input_quantities, "heightf")},
          Jmax_at_25{get_input(input_quantities, "Jmax_at_25")},
          Jmax_c{get_input(input_quantities, "Jmax_c")},
          Jmax_Ea{get_input(input_quantities, "Jmax_Ea")},
          k_diffuse{get_input(input_quantities, "k_diffuse")},
          Kc_c{get_input(input_quantities, "Kc_c")},
          Kc_Ea{get_input(input_quantities, "Kc_Ea")},
          Ko_c{get_input(input_quantities, "Ko_c")},
          Ko_Ea{get_input(input_quantities, "Ko_Ea")},
          lai{get_input(input_quantities, "lai")},
          leaf_reflectance_nir{get_input(input_quantities, "leaf_reflectance_nir")},
          leaf_reflectance_par{get_input(input_quantities, "leaf_reflectance_par")},
          leaf_transmittance_nir{get_input(input_quantities, "leaf_transmittance_nir")},
          leaf_transmittance_par{get_input(input_quantities, "leaf_transmittance_par")},
          leaf_width{get_input(input_quantities, "leaf_width")},
          O2{get_input(input_quantities, "O2")},
          par_energy_content{get_input(input_quantities, "par_energy_content")},
          par_energy_fraction{get_input(input_quantities, "par_energy_fraction")},
          phi_PSII_0{get_input(input_quantities, "phi_PSII_0")},
          phi_PSII_1{get_input(input_quantities, "phi_PSII_1")},
          phi_PSII_2{get_input(input_quantities, "phi_PSII_2")},
          rh{get_input(input_quantities, "rh")},
          RL_at_25{get_input(input_quantities, "RL_at_25")},
          RL_c{get_input(input_quantities, "RL_c")},
          RL_Ea{get_input(input_quantities, "RL_Ea")},
          solar{get_input(input_quantities, "solar")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          temp{get_input(input_quantities, "temp")},
          theta_0{get_input(input_quantities, "theta_0")},
          theta_1{get_input(input_quantities, "theta_1")},
          theta_2{get_input(input_quantities, "theta_2")},
          Tp_at_25{get_input(input_quantities, "Tp_at_25")},
          Tp_c{get_input(input_quantities, "Tp_c")},
          Tp_Ha{get_input(input_quantities, "Tp_Ha")},
          Tp_Hd{get_input(input_quantities, "Tp_Hd")},
          Tp_S{get_input(input_quantities, "Tp_S")},
          Vcmax_at_25{get_input(input_quantities, "Vcmax_at_25")},
          Vcmax_c{get_input(input_quantities, "Vcmax_c")},
          Vcmax_Ea{get_input(input_quantities, "Vcmax_Ea")},
          windspeed{get_input(input_quantities, "windspeed")},
          windspeed_height{get_input(input_quantities, "windspeed_height")},
          jmax_fraction{get_profile(input_quantities, "jmax_fraction")},
          vcmax_fraction{get_profile(input_quantities, "vcmax_fraction")},

          // Get pointers to output quantities
          canopy_assimilation_molar_flux_op{get_op(output_quantities, "canopy_assimilation_molar_flux")},
          canopy_conductance_op{get_op(output_quantities, "canopy_conductance")},
          canopy_gross_assimilation_molar_flux_op{get_op(output_quantities, "canopy_gross_assimilation_molar_flux")},
          canopy_non_photorespiratory_CO2_release_rate_op{get_op(output_quantities, "canopy_non_photorespiratory_CO2_release_molar_flux")},
          canopy_photorespiration_molar_flux_op{get_op(output_quantities, "canopy_photorespiration_molar_flux")},
          canopy_transpiration_rate_op{get_op(output_quantities, "canopy_transpiration_rate")},
          whole_plant_growth_respiration_molar_flux_op{get_op(output_quantities, "whole_plant_growth_respiration_molar_flux")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "c3_canopy"; }

   private:
    // References to input quantities
    double const& absorbed_longwave;
    double const& atmospheric_pressure;
    double const& atmospheric_scattering;
    double const& atmospheric_transmittance;
    double const& b0;
    double const& b1;
    double const& beta_PSII;
    double const& Catm;
    double const& chil;
    double const& cosine_zenith_angle;
    double const& electrons_per_carboxylation;
    double const& electrons_per_oxygenation;
    double const& gbw_canopy;
    double const& growth_respiration_fraction;
    double const& Gs_min;
    double const& Gstar_c;
    double const& Gstar_Ea;
    double const& heightf;
    double const& Jmax_at_25;
    double const& Jmax_c;
    double const& Jmax_Ea;
    double const& k_diffuse;
    double const& Kc_c;
    double const& Kc_Ea;
    double const& Ko_c;
    double const& Ko_Ea;
    double const& lai;
    double const& leaf_reflectance_nir;
    double const& leaf_reflectance_par;
    double const& leaf_transmittance_nir;
    double const& leaf_transmittance_par;
    double const& leaf_width;
    double const& O2;
    double const& par_energy_content;
    double const& par_energy_fraction;
    double const& phi_PSII_0;
    double const& phi_PSII_1;
    double const& phi_PSII_2;
    double const& rh;
    double const& RL_at_25;
    double const& RL_c;
    double const& RL_Ea;
    double const& solar;
    double const& StomataWS;
    double const& temp;
    double const& theta_0;
    double const& theta_1;
    double const& theta_2;
    double const& Tp_at_25;
    double const& Tp_c;
    double const& Tp_Ha;
    double const& Tp_Hd;
    double const& Tp_S;
    double const& Vcmax_at_25;
    double const& Vcmax_c;
    double const& Vcmax_Ea;
    double const& windspeed;
    double const& windspeed_height;

    std::vector<const double*> jmax_fraction;
    std::vector<const double*> vcmax_fraction;

    // Pointers to output quantities
    double* canopy_assimilation_molar_flux_op;
    double* canopy_conductance_op;
    double* canopy_gross_assimilation_molar_flux_op;
    double* canopy_non_photorespiratory_CO2_release_rate_op;
    double* canopy_photorespiration_molar_flux_op;
    double* canopy_transpiration_rate_op;
    double* whole_plant_growth_respiration_molar_flux_op;

    // Main operation
    void do_operation() const;
};

string_vector c3_canopy::get_inputs()
{
    string_vector out = {
        "absorbed_longwave",            // J / m^2 / s
        "atmospheric_pressure",         // Pa
        "atmospheric_scattering",       // dimensionless
        "atmospheric_transmittance",    // dimensionless
        "b0",                           // mol / m^2 / s
        "b1",                           // dimensionless
        "beta_PSII",                    // dimensionless (fraction of absorbed light that reaches photosystem II)
        "Catm",                         // ppm
        "chil",                         // dimensionless
        "cosine_zenith_angle",          // dimensionless
        "electrons_per_carboxylation",  // electron / carboxylation
        "electrons_per_oxygenation",    // electron / oxygenation
        "gbw_canopy",                   // m / s
        "growth_respiration_fraction",  // dimensionless
        "Gs_min",                       // mol / m^2 / s
        "Gstar_c",                      // dimensionless
        "Gstar_Ea",                     // J / mol
        "heightf",                      // m^(-1)
        "Jmax_at_25",                   // micromol / m^2 / s
        "Jmax_c",                       // dimensionless
        "Jmax_Ea",                      // J / mol
        "k_diffuse",                    // dimensionless
        "Kc_c",                         // dimensionless
        "Kc_Ea",                        // J / mol
        "Ko_c",                         // dimensionless
        "Ko_Ea",                        // J / mol
        "lai",                          // dimensionless
        "leaf_reflectance_nir",         // dimensionless
        "leaf_reflectance_par",         // dimensionless
        "leaf_transmittance_nir",       // dimensionless
        "leaf_transmittance_par",       // dimensionless
        "leaf_width",                   // m
        "O2",                           // mmol / mol
        "par_energy_content",           // J / micromol
        "par_energy_fraction",          // dimensionless
        "phi_PSII_0",                   // dimensionless
        "phi_PSII_1",                   // (degrees C)^(-1)
        "phi_PSII_2",                   // (degrees C)^(-2)
        "rh",                           // dimensionless
        "RL_at_25",                     // micromol / m^2 / s
        "RL_c",                         // dimensionless
        "RL_Ea",                        // J / mol
        "solar",                        // micromol / m^2 / s
        "StomataWS",                    // dimensionless
        "temp",                         // degrees C
        "theta_0",                      // dimensionless
        "theta_1",                      // (degrees C)^(-1)
        "theta_2",                      // (degrees C)^(-2)
        "Tp_at_25",                     // micromol / m^2 / s
        "Tp_c",                         // dimensionless
        "Tp_Ha",                        // J / mol
        "Tp_Hd",                        // J / mol
        "Tp_S",                         // J / K / mol
        "Vcmax_at_25",                  // micromol / m^2 / s
        "Vcmax_c",                      // dimensionless
        "Vcmax_Ea",                     // J / mol
        "windspeed",                    // m / s
        "windspeed_height"              // m
    };

    for (size_t i = 0; i < nlayers__; ++i) {
        out.push_back("vcmax_fraction_" + std::to_string(i));
        out.push_back("jmax_fraction_" + std::to_string(i));
    }
    return out;
}

string_vector c3_canopy::get_outputs()
{
    return {
        "canopy_assimilation_molar_flux",                      // micromol / m^2 / s
        "canopy_conductance",                                  // mol / m^2 / s
        "canopy_gross_assimilation_molar_flux",                // micromol / m^2 / s
        "canopy_non_photorespiratory_CO2_release_molar_flux",  // micromol / m^2 / s
        "canopy_photorespiration_molar_flux",                  // micromol / m^2 / s
        "canopy_transpiration_rate",                           // Mg / ha / hr
        "whole_plant_growth_respiration_molar_flux"            // micromol / m^2 / s
    };
}

void c3_canopy::do_operation() const
{
    // Combine temperature response parameters
    c3_temperature_response_parameters const tr_param{
        Gstar_c,
        Gstar_Ea,
        Jmax_c,
        Jmax_Ea,
        Kc_c,
        Kc_Ea,
        Ko_c,
        Ko_Ea,
        phi_PSII_0,
        phi_PSII_1,
        phi_PSII_2,
        RL_c,
        RL_Ea,
        theta_0,
        theta_1,
        theta_2,
        Tp_c,
        Tp_Ha,
        Tp_Hd,
        Tp_S,
        Vcmax_c,
        Vcmax_Ea};

    Light_model const light_model = lightME(
        cosine_zenith_angle,
        atmospheric_pressure,
        atmospheric_transmittance,
        atmospheric_scattering);

    // q_dir: flux through a plane perpendicular to the rays of the sun
    // q_diff: flux through any surface
    double const q_dir = light_model.direct_fraction * solar;    // micromol / m^2 / s
    double const q_diff = light_model.diffuse_fraction * solar;  // micromol / m^2 / s

    const Light_profile light_profile = sunML(
        q_dir,
        q_diff,
        chil,
        cosine_zenith_angle,
        heightf,
        k_diffuse,
        lai,
        leaf_reflectance_nir,
        leaf_reflectance_par,
        leaf_transmittance_nir,
        leaf_transmittance_par,
        par_energy_content,
        par_energy_fraction,
        nlayers__);

    double const laic = lai / nlayers__;  // dimensionless

    std::vector<double> wind_speed_profile(nlayers__);
    WINDprof(windspeed, lai, wind_speed_profile);  // Modifies wind_speed_profile

    double CanopyA{0.0};             // micromol / m^2 / s
    double GCanopyA{0.0};            // micromol / m^2 / s
    double canopy_rp{0.0};           // micromol / m^2 / s
    double canopy_RL{0.0};           // micromol / m^2 / s
    double CanopyT{0.0};             // mmol / m^2 / s
    double CanopyPe{0.0};            // mmol / m^2 / s
    double CanopyPr{0.0};            // mmol / m^2 / s
    double canopy_conductance{0.0};  // mmol / m^2 / s

    double gbw_guess{1.2};  // mol / m^2 / s

    for (size_t i = 0; i < nlayers__; ++i) {
        // Calculations that are the same for sunlit and shaded leaves
        size_t current_layer = nlayers__ - 1 - i;

        double layer_wind_speed = wind_speed_profile[current_layer];  // m / s

        double Vcmax_at_25_ = Vcmax_at_25 * (*vcmax_fraction[current_layer]);
        double Jmax_at_25_ = Jmax_at_25 * (*jmax_fraction[current_layer]);

        // Calculations for sunlit leaves. First, estimate stomatal conductance
        // by assuming the leaf has the same temperature as the air. Then, use
        // energy balance to get a better temperature estimate using that value
        // of stomatal conductance. Get the final estimate of stomatal
        // conductance using the new value of the leaf temperature.
        double iabs_dir = light_profile.sunlit_absorbed_ppfd[current_layer];    // micromol / m^2 / s
        double j_dir = light_profile.sunlit_absorbed_shortwave[current_layer];  // J / m^2 / s
        double pLeafsun = light_profile.sunlit_fraction[current_layer];         // dimensionless
        double Leafsun = laic * pLeafsun;                                       // dimensionless

        double direct_gsw_estimate =
            c3photoC(
                tr_param, iabs_dir, temp, temp,
                rh, Vcmax_at_25_, Jmax_at_25_,
                Tp_at_25, RL_at_25, b0, b1, Gs_min, Catm, atmospheric_pressure,
                O2, StomataWS,
                electrons_per_carboxylation, electrons_per_oxygenation,
                beta_PSII, gbw_guess)
                .Gs;  // mol / m^2 / s

        energy_balance_outputs et_direct = leaf_energy_balance(
            absorbed_longwave,
            j_dir,
            atmospheric_pressure,
            temp,
            gbw_canopy,
            leaf_width,
            rh,
            direct_gsw_estimate,
            layer_wind_speed);

        double leaf_temperature_dir = temp + et_direct.Deltat;  // degrees C

        photosynthesis_outputs direct_photo =
            c3photoC(
                tr_param, iabs_dir, leaf_temperature_dir, temp,
                rh, Vcmax_at_25_, Jmax_at_25_,
                Tp_at_25, RL_at_25, b0, b1, Gs_min, Catm, atmospheric_pressure,
                O2, StomataWS,
                electrons_per_carboxylation, electrons_per_oxygenation,
                beta_PSII, et_direct.gbw_molecular);

        // Calculations for shaded leaves. First, estimate stomatal conductance
        // by assuming the leaf has the same temperature as the air. Then, use
        // energy balance to get a better temperature estimate using that value
        // of stomatal conductance. Get the final estimate of stomatal
        // conductance using the new value of the leaf temperature.
        double iabs_diff = light_profile.shaded_absorbed_ppfd[current_layer];    // micromol / m^2 /s
        double j_diff = light_profile.shaded_absorbed_shortwave[current_layer];  // J / m^2 / s
        double pLeafshade = light_profile.shaded_fraction[current_layer];        // dimensionless
        double Leafshade = laic * pLeafshade;                                    // dimensionless

        double diffuse_gsw_estimate =
            c3photoC(
                tr_param, iabs_diff, temp, temp,
                rh, Vcmax_at_25_, Jmax_at_25_,
                Tp_at_25, RL_at_25, b0, b1, Gs_min, Catm, atmospheric_pressure,
                O2, StomataWS,
                electrons_per_carboxylation, electrons_per_oxygenation,
                beta_PSII, gbw_guess)
                .Gs;  // mol / m^2 / s

        energy_balance_outputs et_diffuse = leaf_energy_balance(
            absorbed_longwave,
            j_diff,
            atmospheric_pressure,
            temp,
            gbw_canopy,
            leaf_width,
            rh,
            diffuse_gsw_estimate,
            layer_wind_speed);

        double leaf_temperature_Idiffuse = temp + et_diffuse.Deltat;  // degrees C

        photosynthesis_outputs diffuse_photo =
            c3photoC(
                tr_param, iabs_diff, leaf_temperature_Idiffuse, temp,
                rh, Vcmax_at_25_,
                Jmax_at_25_, Tp_at_25, RL_at_25, b0, b1, Gs_min, Catm,
                atmospheric_pressure, O2, StomataWS,
                electrons_per_carboxylation,
                electrons_per_oxygenation, beta_PSII,
                et_diffuse.gbw_molecular);

        // Combine sunlit and shaded leaves
        CanopyA += Leafsun * direct_photo.Assim + Leafshade * diffuse_photo.Assim;             // micromol / m^2 / s
        CanopyT += Leafsun * et_direct.TransR + Leafshade * et_diffuse.TransR;                 // mmol / m^2 / s
        GCanopyA += Leafsun * direct_photo.GrossAssim + Leafshade * diffuse_photo.GrossAssim;  // micromol / m^2 / s
        canopy_rp += Leafsun * direct_photo.Rp + Leafshade * diffuse_photo.Rp;                 // micromol / m^2 / s
        canopy_RL += Leafsun * direct_photo.RL + Leafshade * diffuse_photo.RL;                 // micromol / m^2 / s

        CanopyPe += Leafsun * et_direct.EPenman + Leafshade * et_diffuse.EPenman;        // mmol / m^2 / s
        CanopyPr += Leafsun * et_direct.EPriestly + Leafshade * et_diffuse.EPriestly;    // mmol / m^2 / s
        canopy_conductance += Leafsun * direct_photo.Gs + Leafshade * diffuse_photo.Gs;  // mol / m^2 / s
    }

    // Calculate the rate of whole-plant growth respiration
    double const whole_plant_gr =
        growth_resp(CanopyA, growth_respiration_fraction);  // micromol / m^2 / s

    // For transpiration, we need to convert mmol / m^2 / s into Mg / ha / hr
    // using the molar mass of water in kg / mol, which can be accomplished by
    // the following conversion factor:
    // (3600 s / hr) * (1e-3 mol / mmol) * (1e-3 Mg / kg) * (1e4 m^2 / ha)
    // = 36 s * mol * Mg * m^2 / (hr * mmol * kg * ha)
    double constexpr cf2 = physical_constants::molar_mass_of_water * 36;  // (Mg / ha / hr) / (mmol / m^2 / s)

    // Update the output quantity list
    update(canopy_assimilation_molar_flux_op, CanopyA - whole_plant_gr);   // micromol / m^2 / s
    update(canopy_conductance_op, canopy_conductance);                     // mol / m^2 / s
    update(canopy_gross_assimilation_molar_flux_op, GCanopyA);             // micromol / m^2 / s
    update(canopy_non_photorespiratory_CO2_release_rate_op, canopy_RL);    // micromol / m^2 / s
    update(canopy_photorespiration_molar_flux_op, canopy_rp);              // micromol / m^2 / s
    update(canopy_transpiration_rate_op, CanopyT * cf2);                   // Mg / ha / hr
    update(whole_plant_growth_respiration_molar_flux_op, whole_plant_gr);  // micromol / m^2 / s
}

}  // namespace standardBML
#endif
