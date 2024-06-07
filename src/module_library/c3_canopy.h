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
          growth_respiration_fraction{get_input(input_quantities, "growth_respiration_fraction")},
          gbw_canopy{get_input(input_quantities, "gbw_canopy")},
          Gs_min{get_input(input_quantities, "Gs_min")},
          heightf{get_input(input_quantities, "heightf")},
          jmax{get_input(input_quantities, "jmax")},
          kpLN{get_input(input_quantities, "kpLN")},
          k_diffuse{get_input(input_quantities, "k_diffuse")},
          lai{get_input(input_quantities, "lai")},
          LeafN{get_input(input_quantities, "LeafN")},
          leafwidth{get_input(input_quantities, "leafwidth")},
          leaf_reflectance_nir{get_input(input_quantities, "leaf_reflectance_nir")},
          leaf_reflectance_par{get_input(input_quantities, "leaf_reflectance_par")},
          leaf_transmittance_nir{get_input(input_quantities, "leaf_transmittance_nir")},
          leaf_transmittance_par{get_input(input_quantities, "leaf_transmittance_par")},
          lnb0{get_input(input_quantities, "lnb0")},
          lnb1{get_input(input_quantities, "lnb1")},
          lnfun{get_input(input_quantities, "lnfun")},
          nlayers{get_input(input_quantities, "nlayers")},
          O2{get_input(input_quantities, "O2")},
          par_energy_content{get_input(input_quantities, "par_energy_content")},
          par_energy_fraction{get_input(input_quantities, "par_energy_fraction")},
          Rd{get_input(input_quantities, "Rd")},
          rh{get_input(input_quantities, "rh")},
          solar{get_input(input_quantities, "solar")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          temp{get_input(input_quantities, "temp")},
          theta{get_input(input_quantities, "theta")},
          tpu_rate_max{get_input(input_quantities, "tpu_rate_max")},
          vmax{get_input(input_quantities, "vmax")},
          windspeed_height{get_input(input_quantities, "windspeed_height")},
          windspeed{get_input(input_quantities, "windspeed")},

          // Get pointers to output quantities
          canopy_assimilation_rate_op{get_op(output_quantities, "canopy_assimilation_rate")},
          canopy_transpiration_rate_op{get_op(output_quantities, "canopy_transpiration_rate")},
          canopy_conductance_op{get_op(output_quantities, "canopy_conductance")},
          GrossAssim_op{get_op(output_quantities, "GrossAssim")},
          canopy_photorespiration_rate_op{get_op(output_quantities, "canopy_photorespiration_rate")}
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
    double const& growth_respiration_fraction;
    double const& gbw_canopy;
    double const& Gs_min;
    double const& heightf;
    double const& jmax;
    double const& kpLN;
    double const& k_diffuse;
    double const& lai;
    double const& LeafN;
    double const& leafwidth;
    double const& leaf_reflectance_nir;
    double const& leaf_reflectance_par;
    double const& leaf_transmittance_nir;
    double const& leaf_transmittance_par;
    double const& lnb0;
    double const& lnb1;
    double const& lnfun;
    double const& nlayers;
    double const& O2;
    double const& par_energy_content;
    double const& par_energy_fraction;
    double const& Rd;
    double const& rh;
    double const& solar;
    double const& StomataWS;
    double const& temp;
    double const& theta;
    double const& tpu_rate_max;
    double const& vmax;
    double const& windspeed_height;
    double const& windspeed;

    // Pointers to output quantities
    double* canopy_assimilation_rate_op;
    double* canopy_transpiration_rate_op;
    double* canopy_conductance_op;
    double* GrossAssim_op;
    double* canopy_photorespiration_rate_op;

    // Main operation
    void do_operation() const;
};

}  // namespace standardBML
#endif
