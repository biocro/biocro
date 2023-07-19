#ifndef C4_CANOPY_H
#define C4_CANOPY_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "CanAC.h"  // For CanAC

namespace standardBML
{
class c4_canopy : public direct_module
{
   public:
    c4_canopy(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          nileafn{get_input(input_quantities, "nileafn")},
          nkln{get_input(input_quantities, "nkln")},
          nvmaxb1{get_input(input_quantities, "nvmaxb1")},
          nvmaxb0{get_input(input_quantities, "nvmaxb0")},
          nalphab1{get_input(input_quantities, "nalphab1")},
          nalphab0{get_input(input_quantities, "nalphab0")},
          nRdb1{get_input(input_quantities, "nRdb1")},
          nRdb0{get_input(input_quantities, "nRdb0")},
          nkpLN{get_input(input_quantities, "nkpLN")},
          nlnb0{get_input(input_quantities, "nlnb0")},
          nlnb1{get_input(input_quantities, "nlnb1")},
          lai{get_input(input_quantities, "lai")},
          cosine_zenith_angle{get_input(input_quantities, "cosine_zenith_angle")},
          solar{get_input(input_quantities, "solar")},
          temp{get_input(input_quantities, "temp")},
          rh{get_input(input_quantities, "rh")},
          windspeed{get_input(input_quantities, "windspeed")},
          nlayers{get_input(input_quantities, "nlayers")},
          vmax1{get_input(input_quantities, "vmax1")},
          alpha1{get_input(input_quantities, "alpha1")},
          kparm{get_input(input_quantities, "kparm")},
          beta{get_input(input_quantities, "beta")},
          Rd{get_input(input_quantities, "Rd")},
          Catm{get_input(input_quantities, "Catm")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},
          Gs_min{get_input(input_quantities, "Gs_min")},
          theta{get_input(input_quantities, "theta")},
          kd{get_input(input_quantities, "kd")},
          chil{get_input(input_quantities, "chil")},
          LeafN{get_input(input_quantities, "LeafN")},
          kpLN{get_input(input_quantities, "kpLN")},
          lnfun{get_input(input_quantities, "lnfun")},
          upperT{get_input(input_quantities, "upperT")},
          lowerT{get_input(input_quantities, "lowerT")},
          leafwidth{get_input(input_quantities, "leafwidth")},
          et_equation{get_input(input_quantities, "et_equation")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          specific_heat_of_air{get_input(input_quantities, "specific_heat_of_air")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          atmospheric_transmittance{get_input(input_quantities, "atmospheric_transmittance")},
          atmospheric_scattering{get_input(input_quantities, "atmospheric_scattering")},
          absorptivity_par{get_input(input_quantities, "absorptivity_par")},
          par_energy_content{get_input(input_quantities, "par_energy_content")},
          par_energy_fraction{get_input(input_quantities, "par_energy_fraction")},
          leaf_transmittance{get_input(input_quantities, "leaf_transmittance")},
          leaf_reflectance{get_input(input_quantities, "leaf_reflectance")},
          minimum_gbw{get_input(input_quantities, "minimum_gbw")},

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
    static std::string get_name() { return "c4_canopy"; }

   private:
    // References to input quantities
    double const& nileafn;
    double const& nkln;
    double const& nvmaxb1;
    double const& nvmaxb0;
    double const& nalphab1;
    double const& nalphab0;
    double const& nRdb1;
    double const& nRdb0;
    double const& nkpLN;
    double const& nlnb0;
    double const& nlnb1;
    double const& lai;
    double const& cosine_zenith_angle;
    double const& solar;
    double const& temp;
    double const& rh;
    double const& windspeed;
    double const& nlayers;
    double const& vmax1;
    double const& alpha1;
    double const& kparm;
    double const& beta;
    double const& Rd;
    double const& Catm;
    double const& b0;
    double const& b1;
    double const& Gs_min;
    double const& theta;
    double const& kd;
    double const& chil;
    double const& LeafN;
    double const& kpLN;
    double const& lnfun;
    double const& upperT;
    double const& lowerT;
    double const& leafwidth;
    double const& et_equation;
    double const& StomataWS;
    double const& specific_heat_of_air;
    double const& atmospheric_pressure;
    double const& atmospheric_transmittance;
    double const& atmospheric_scattering;
    double const& absorptivity_par;
    double const& par_energy_content;
    double const& par_energy_fraction;
    double const& leaf_transmittance;
    double const& leaf_reflectance;
    double const& minimum_gbw;

    // Pointers to output quantities
    double* canopy_assimilation_rate_op;
    double* canopy_transpiration_rate_op;
    double* canopy_conductance_op;
    double* GrossAssim_op;
    double* canopy_photorespiration_rate_op;

    // Main operation
    void do_operation() const;
};

string_vector c4_canopy::get_inputs()
{
    return {
        "nileafn",
        "nkln",
        "nvmaxb1",
        "nvmaxb0",
        "nalphab1",
        "nalphab0",
        "nRdb1",
        "nRdb0",
        "nkpLN",
        "nlnb0",
        "nlnb1",
        "lai",
        "cosine_zenith_angle",
        "solar",
        "temp",
        "rh",
        "windspeed",
        "nlayers",
        "vmax1",
        "alpha1",
        "kparm",
        "beta",
        "Rd",
        "Catm",
        "b0",
        "b1",
        "Gs_min",  // mol / m^2 / s
        "theta",
        "kd",
        "chil",
        "LeafN",
        "kpLN",
        "lnfun",
        "upperT",
        "lowerT",
        "leafwidth",
        "et_equation",
        "StomataWS",
        "specific_heat_of_air",       // J / kg / K
        "atmospheric_pressure",       // Pa
        "atmospheric_transmittance",  // dimensionless
        "atmospheric_scattering",     // dimensionless
        "absorptivity_par",           // dimensionless
        "par_energy_content",         // J / micromol
        "par_energy_fraction",        // dimensionless
        "leaf_transmittance",         // dimensionless
        "leaf_reflectance",           // dimensionless
        "minimum_gbw"                 // mol / m^2 / s
    };
}

string_vector c4_canopy::get_outputs()
{
    return {
        "canopy_assimilation_rate",     // Mg / ha / hr
        "canopy_transpiration_rate",    // Mg / ha / hr
        "canopy_conductance",           // Mg / ha / hr
        "GrossAssim",                   // Mg / ha / hr
        "canopy_photorespiration_rate"  // Mg / ha / hr
    };
}

void c4_canopy::do_operation() const
{
    // Collect inputs and make calculations
    struct nitroParms nitroP;
    nitroP.ileafN = nileafn;
    nitroP.kln = nkln;
    nitroP.Vmaxb1 = nvmaxb1;
    nitroP.Vmaxb0 = nvmaxb0;
    nitroP.alphab1 = nalphab1;
    nitroP.alphab0 = nalphab0;
    nitroP.Rdb1 = nRdb1;
    nitroP.Rdb0 = nRdb0;
    nitroP.kpLN = nkpLN;
    nitroP.lnb0 = nlnb0;
    nitroP.lnb1 = nlnb1;

    canopy_photosynthesis_outputs can_result = CanAC(
        lai, cosine_zenith_angle, solar, temp, rh, windspeed, nlayers, vmax1,
        alpha1, kparm, beta, Rd, Catm, b0, b1, Gs_min, theta, kd, chil, LeafN,
        kpLN, lnfun, upperT, lowerT, nitroP, leafwidth, et_equation, StomataWS,
        specific_heat_of_air, atmospheric_pressure, atmospheric_transmittance,
        atmospheric_scattering, absorptivity_par, par_energy_content,
        par_energy_fraction, leaf_transmittance, leaf_reflectance, minimum_gbw);

    // Update the parameter list
    update(canopy_assimilation_rate_op, can_result.Assim);         // Mg / ha / hr
    update(canopy_transpiration_rate_op, can_result.Trans);        // Mg / ha / hr
    update(canopy_conductance_op, can_result.canopy_conductance);  // mmol / m^2 / s
    update(GrossAssim_op, can_result.GrossAssim);                  // Mg / ha / hr
    update(canopy_photorespiration_rate_op, can_result.Rp);        // Mg / ha / hr
}

}  // namespace standardBML
#endif
