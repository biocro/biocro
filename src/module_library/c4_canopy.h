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
          absorbed_longwave{get_input(input_quantities, "absorbed_longwave")},
          alpha1{get_input(input_quantities, "alpha1")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          atmospheric_scattering{get_input(input_quantities, "atmospheric_scattering")},
          atmospheric_transmittance{get_input(input_quantities, "atmospheric_transmittance")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},
          beta{get_input(input_quantities, "beta")},
          Catm{get_input(input_quantities, "Catm")},
          chil{get_input(input_quantities, "chil")},
          cosine_zenith_angle{get_input(input_quantities, "cosine_zenith_angle")},
          gbw_canopy{get_input(input_quantities, "gbw_canopy")},
          Gs_min{get_input(input_quantities, "Gs_min")},
          kparm{get_input(input_quantities, "kparm")},
          kpLN{get_input(input_quantities, "kpLN")},
          k_diffuse{get_input(input_quantities, "k_diffuse")},
          lai{get_input(input_quantities, "lai")},
          LeafN{get_input(input_quantities, "LeafN")},
          leafwidth{get_input(input_quantities, "leafwidth")},
          leaf_reflectance_nir{get_input(input_quantities, "leaf_reflectance_nir")},
          leaf_reflectance_par{get_input(input_quantities, "leaf_reflectance_par")},
          leaf_transmittance_nir{get_input(input_quantities, "leaf_transmittance_nir")},
          leaf_transmittance_par{get_input(input_quantities, "leaf_transmittance_par")},
          lnfun{get_input(input_quantities, "lnfun")},
          lowerT{get_input(input_quantities, "lowerT")},
          nalphab0{get_input(input_quantities, "nalphab0")},
          nalphab1{get_input(input_quantities, "nalphab1")},
          nileafn{get_input(input_quantities, "nileafn")},
          nkln{get_input(input_quantities, "nkln")},
          nkpLN{get_input(input_quantities, "nkpLN")},
          nlayers{get_input(input_quantities, "nlayers")},
          nlnb0{get_input(input_quantities, "nlnb0")},
          nlnb1{get_input(input_quantities, "nlnb1")},
          nRdb0{get_input(input_quantities, "nRdb0")},
          nRdb1{get_input(input_quantities, "nRdb1")},
          nvmaxb0{get_input(input_quantities, "nvmaxb0")},
          nvmaxb1{get_input(input_quantities, "nvmaxb1")},
          par_energy_content{get_input(input_quantities, "par_energy_content")},
          par_energy_fraction{get_input(input_quantities, "par_energy_fraction")},
          Rd{get_input(input_quantities, "Rd")},
          rh{get_input(input_quantities, "rh")},
          solar{get_input(input_quantities, "solar")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          temp{get_input(input_quantities, "temp")},
          theta{get_input(input_quantities, "theta")},
          upperT{get_input(input_quantities, "upperT")},
          vmax1{get_input(input_quantities, "vmax1")},
          windspeed{get_input(input_quantities, "windspeed")},

          // Get pointers to output quantities
          canopy_assimilation_rate_CO2_op{get_op(output_quantities, "canopy_assimilation_rate_CO2")},
          canopy_transpiration_rate_op{get_op(output_quantities, "canopy_transpiration_rate")},
          canopy_conductance_op{get_op(output_quantities, "canopy_conductance")},
          GrossAssim_CO2_op{get_op(output_quantities, "GrossAssim_CO2")},
          canopy_photorespiration_rate_CO2_op{get_op(output_quantities, "canopy_photorespiration_rate_CO2")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "c4_canopy"; }

   private:
    // References to input quantities
    double const& absorbed_longwave;
    double const& alpha1;
    double const& atmospheric_pressure;
    double const& atmospheric_scattering;
    double const& atmospheric_transmittance;
    double const& b0;
    double const& b1;
    double const& beta;
    double const& Catm;
    double const& chil;
    double const& cosine_zenith_angle;
    double const& gbw_canopy;
    double const& Gs_min;
    double const& kparm;
    double const& kpLN;
    double const& k_diffuse;
    double const& lai;
    double const& LeafN;
    double const& leafwidth;
    double const& leaf_reflectance_nir;
    double const& leaf_reflectance_par;
    double const& leaf_transmittance_nir;
    double const& leaf_transmittance_par;
    double const& lnfun;
    double const& lowerT;
    double const& nalphab0;
    double const& nalphab1;
    double const& nileafn;
    double const& nkln;
    double const& nkpLN;
    double const& nlayers;
    double const& nlnb0;
    double const& nlnb1;
    double const& nRdb0;
    double const& nRdb1;
    double const& nvmaxb0;
    double const& nvmaxb1;
    double const& par_energy_content;
    double const& par_energy_fraction;
    double const& Rd;
    double const& rh;
    double const& solar;
    double const& StomataWS;
    double const& temp;
    double const& theta;
    double const& upperT;
    double const& vmax1;
    double const& windspeed;

    // Pointers to output quantities
    double* canopy_assimilation_rate_CO2_op;
    double* canopy_transpiration_rate_op;
    double* canopy_conductance_op;
    double* GrossAssim_CO2_op;
    double* canopy_photorespiration_rate_CO2_op;

    // Main operation
    void do_operation() const;
};

string_vector c4_canopy::get_inputs()
{
    return {
        "absorbed_longwave",  // J / m^2 / s
        "alpha1",
        "atmospheric_pressure",       // Pa
        "atmospheric_scattering",     // dimensionless
        "atmospheric_transmittance",  // dimensionless
        "b0",                         // mol / m^2 / s
        "b1",                         // dimensionless
        "beta",                       // dimensionless
        "Catm",                       // ppm
        "chil",                       // dimensionless
        "cosine_zenith_angle",        // dimensionless
        "gbw_canopy",                 // m / s
        "Gs_min",                     // mol / m^2 / s
        "kparm",
        "kpLN",
        "k_diffuse",  // dimensionless
        "lai",        // dimensionless from m^2 leaf / m^2 ground
        "LeafN",
        "leafwidth",               // m
        "leaf_reflectance_nir",    // dimensionless
        "leaf_reflectance_par",    // dimensionless
        "leaf_transmittance_nir",  // dimensionless
        "leaf_transmittance_par",  // dimensionless
        "lnfun",                   // not a physical quantity
        "lowerT",                  // degrees C
        "nalphab0",
        "nalphab1",
        "nileafn",
        "nkln",
        "nkpLN",
        "nlayers",  // not a physical quantity
        "nlnb0",
        "nlnb1",
        "nRdb0",
        "nRdb1",
        "nvmaxb0",
        "nvmaxb1",
        "par_energy_content",   // J / micromol
        "par_energy_fraction",  // dimensionless
        "Rd",                   // micromol / m^2 / s
        "rh",                   // dimensionless from Pa / Pa
        "solar",                // micromol / m^2 / s
        "StomataWS",            // dimensionless
        "temp",                 // degrees C
        "theta",                // dimensionless
        "upperT",               // degrees C
        "vmax1",                // micromol / m^2 / s
        "windspeed"             // m / s
    };
}

string_vector c4_canopy::get_outputs()
{
    return {
        "canopy_assimilation_rate_CO2",     // micromol CO2 / m^2 / s
        "canopy_transpiration_rate",        // Mg / ha / hr
        "canopy_conductance",               // mol / m^2 / s
        "GrossAssim_CO2",                   // micromol CO2 / m^2 / s
        "canopy_photorespiration_rate_CO2"  // micromol / m^2 / s
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
        nitroP,
        absorbed_longwave,
        alpha1,
        temp,
        atmospheric_pressure,
        atmospheric_scattering,
        atmospheric_transmittance,
        b0,
        b1,
        beta,
        Catm,
        chil,
        cosine_zenith_angle,
        gbw_canopy,
        Gs_min,
        kparm,
        kpLN,
        k_diffuse,
        lai,
        LeafN,
        leafwidth,
        leaf_reflectance_nir,
        leaf_reflectance_par,
        leaf_transmittance_nir,
        leaf_transmittance_par,
        lowerT,
        par_energy_content,
        par_energy_fraction,
        Rd,
        rh,
        solar,
        StomataWS,
        theta,
        upperT,
        vmax1,
        windspeed,
        lnfun,
        nlayers);

    // Update the parameter list
    update(canopy_assimilation_rate_CO2_op, can_result.Assim);     // micromol / m^2 /s
    update(canopy_transpiration_rate_op, can_result.Trans);        // Mg / ha / hr
    update(canopy_conductance_op, can_result.canopy_conductance);  // mol / m^2 / s
    update(GrossAssim_CO2_op, can_result.GrossAssim);              // micromol / m^2 /s
    update(canopy_photorespiration_rate_CO2_op, can_result.Rp);    // micromol / m^2 /s
}
}  // namespace standardBML
#endif
