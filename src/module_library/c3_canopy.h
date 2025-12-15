#ifndef C3_CANOPY_H
#define C3_CANOPY_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
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
          kpLN{get_input(input_quantities, "kpLN")},
          lai{get_input(input_quantities, "lai")},
          leaf_reflectance_nir{get_input(input_quantities, "leaf_reflectance_nir")},
          leaf_reflectance_par{get_input(input_quantities, "leaf_reflectance_par")},
          leaf_transmittance_nir{get_input(input_quantities, "leaf_transmittance_nir")},
          leaf_transmittance_par{get_input(input_quantities, "leaf_transmittance_par")},
          LeafN{get_input(input_quantities, "LeafN")},
          leafwidth{get_input(input_quantities, "leafwidth")},
          lnb0{get_input(input_quantities, "lnb0")},
          lnb1{get_input(input_quantities, "lnb1")},
          lnfun{get_input(input_quantities, "lnfun")},
          nlayers{get_input(input_quantities, "nlayers")},
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
    double const& kpLN;
    double const& lai;
    double const& leaf_reflectance_nir;
    double const& leaf_reflectance_par;
    double const& leaf_transmittance_nir;
    double const& leaf_transmittance_par;
    double const& LeafN;
    double const& leafwidth;
    double const& lnb0;
    double const& lnb1;
    double const& lnfun;
    double const& nlayers;
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

}  // namespace standardBML
#endif
