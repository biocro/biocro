#ifndef C4_CANOPY_H
#define C4_CANOPY_H

#include "../modules.h"
#include "AuxBioCro.h"  // For nitroParms and Can_Str
#include "BioCro.h"     // For CanAC

class c4_canopy : public SteadyModule
{
   public:
    c4_canopy(
        const std::unordered_map<std::string, double>* input_quantities,
        std::unordered_map<std::string, double>* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("c4_canopy"),

          // Get pointers to input quantities
          nileafn_ip(get_ip(input_quantities, "nileafn")),
          nkln_ip(get_ip(input_quantities, "nkln")),
          nvmaxb1_ip(get_ip(input_quantities, "nvmaxb1")),
          nvmaxb0_ip(get_ip(input_quantities, "nvmaxb0")),
          nalphab1_ip(get_ip(input_quantities, "nalphab1")),
          nalphab0_ip(get_ip(input_quantities, "nalphab0")),
          nRdb1_ip(get_ip(input_quantities, "nRdb1")),
          nRdb0_ip(get_ip(input_quantities, "nRdb0")),
          nkpLN_ip(get_ip(input_quantities, "nkpLN")),
          nlnb0_ip(get_ip(input_quantities, "nlnb0")),
          nlnb1_ip(get_ip(input_quantities, "nlnb1")),
          lai_ip(get_ip(input_quantities, "lai")),
          time_ip(get_ip(input_quantities, "time")),
          solar_ip(get_ip(input_quantities, "solar")),
          temp_ip(get_ip(input_quantities, "temp")),
          rh_ip(get_ip(input_quantities, "rh")),
          windspeed_ip(get_ip(input_quantities, "windspeed")),
          lat_ip(get_ip(input_quantities, "lat")),
          nlayers_ip(get_ip(input_quantities, "nlayers")),
          vmax1_ip(get_ip(input_quantities, "vmax1")),
          alpha1_ip(get_ip(input_quantities, "alpha1")),
          kparm_ip(get_ip(input_quantities, "kparm")),
          beta_ip(get_ip(input_quantities, "beta")),
          Rd_ip(get_ip(input_quantities, "Rd")),
          Catm_ip(get_ip(input_quantities, "Catm")),
          b0_ip(get_ip(input_quantities, "b0")),
          b1_ip(get_ip(input_quantities, "b1")),
          Gs_min_ip(get_ip(input_quantities, "Gs_min")),
          theta_ip(get_ip(input_quantities, "theta")),
          kd_ip(get_ip(input_quantities, "kd")),
          chil_ip(get_ip(input_quantities, "chil")),
          heightf_ip(get_ip(input_quantities, "heightf")),
          LeafN_ip(get_ip(input_quantities, "LeafN")),
          kpLN_ip(get_ip(input_quantities, "kpLN")),
          lnb0_ip(get_ip(input_quantities, "lnb0")),
          lnb1_ip(get_ip(input_quantities, "lnb1")),
          lnfun_ip(get_ip(input_quantities, "lnfun")),
          upperT_ip(get_ip(input_quantities, "upperT")),
          lowerT_ip(get_ip(input_quantities, "lowerT")),
          leafwidth_ip(get_ip(input_quantities, "leafwidth")),
          et_equation_ip(get_ip(input_quantities, "et_equation")),
          StomataWS_ip(get_ip(input_quantities, "StomataWS")),
          specific_heat_of_air_ip(get_ip(input_quantities, "specific_heat_of_air")),
          atmospheric_pressure_ip(get_ip(input_quantities, "atmospheric_pressure")),
          water_stress_approach_ip(get_ip(input_quantities, "water_stress_approach")),
          absorptivity_par_ip(get_ip(input_quantities, "absorptivity_par")),

          // Get pointers to output quantities
          canopy_assimilation_rate_op(get_op(output_quantities, "canopy_assimilation_rate")),
          canopy_transpiration_rate_op(get_op(output_quantities, "canopy_transpiration_rate")),
          canopy_conductance_op(get_op(output_quantities, "canopy_conductance")),
          GrossAssim_op(get_op(output_quantities, "GrossAssim"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input quantities
    const double* nileafn_ip;
    const double* nkln_ip;
    const double* nvmaxb1_ip;
    const double* nvmaxb0_ip;
    const double* nalphab1_ip;
    const double* nalphab0_ip;
    const double* nRdb1_ip;
    const double* nRdb0_ip;
    const double* nkpLN_ip;
    const double* nlnb0_ip;
    const double* nlnb1_ip;
    const double* lai_ip;
    const double* time_ip;
    const double* solar_ip;
    const double* temp_ip;
    const double* rh_ip;
    const double* windspeed_ip;
    const double* lat_ip;
    const double* nlayers_ip;
    const double* vmax1_ip;
    const double* alpha1_ip;
    const double* kparm_ip;
    const double* beta_ip;
    const double* Rd_ip;
    const double* Catm_ip;
    const double* b0_ip;
    const double* b1_ip;
    const double* Gs_min_ip;
    const double* theta_ip;
    const double* kd_ip;
    const double* chil_ip;
    const double* heightf_ip;
    const double* LeafN_ip;
    const double* kpLN_ip;
    const double* lnb0_ip;
    const double* lnb1_ip;
    const double* lnfun_ip;
    const double* upperT_ip;
    const double* lowerT_ip;
    const double* leafwidth_ip;
    const double* et_equation_ip;
    const double* StomataWS_ip;
    const double* specific_heat_of_air_ip;
    const double* atmospheric_pressure_ip;
    const double* water_stress_approach_ip;
    const double* absorptivity_par_ip;

    // Pointers to output quantities
    double* canopy_assimilation_rate_op;
    double* canopy_transpiration_rate_op;
    double* canopy_conductance_op;
    double* GrossAssim_op;

    // Main operation
    void do_operation() const;
};

std::vector<std::string> c4_canopy::get_inputs()
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
        "time",
        "solar",
        "temp",
        "rh",
        "windspeed",
        "lat",
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
        "heightf",
        "LeafN",
        "kpLN",
        "lnb0",
        "lnb1",
        "lnfun",
        "upperT",
        "lowerT",
        "leafwidth",
        "et_equation",
        "StomataWS",
        "specific_heat_of_air",   // J / kg / K
        "atmospheric_pressure",   // Pa
        "water_stress_approach",  // dimensionless switch
        "absorptivity_par"        // dimensionless
    };
}

std::vector<std::string> c4_canopy::get_outputs()
{
    return {
        "canopy_assimilation_rate",   // Mg / ha / hr
        "canopy_transpiration_rate",  // Mg / ha / hr
        "canopy_conductance",         // Mg / ha / hr
        "GrossAssim"                  // Mg / ha / hr
    };
}

void c4_canopy::do_operation() const
{
    // Collect inputs and make calculations
    int doy = floor(*time_ip);                // Round time down to get the day of year
    double hour = 24.0 * ((*time_ip) - doy);  // Get the fractional part as the hour

    struct nitroParms nitroP;
    nitroP.ileafN = *nileafn_ip;
    nitroP.kln = *nkln_ip;
    nitroP.Vmaxb1 = *nvmaxb1_ip;
    nitroP.Vmaxb0 = *nvmaxb0_ip;
    nitroP.alphab1 = *nalphab1_ip;
    nitroP.alphab0 = *nalphab0_ip;
    nitroP.Rdb1 = *nRdb1_ip;
    nitroP.Rdb0 = *nRdb0_ip;
    nitroP.kpLN = *nkpLN_ip;
    nitroP.lnb0 = *nlnb0_ip;
    nitroP.lnb1 = *nlnb1_ip;

    // CanAC is located in CanAC.cpp
    struct Can_Str can_result = CanAC(
        *lai_ip, doy, hour, *solar_ip, *temp_ip,
        *rh_ip, *windspeed_ip, *lat_ip, *nlayers_ip, *vmax1_ip,
        *alpha1_ip, *kparm_ip, *beta_ip, *Rd_ip, *Catm_ip,
        *b0_ip, *b1_ip, *Gs_min_ip * 1e3, *theta_ip, *kd_ip, *chil_ip,
        *heightf_ip, *LeafN_ip, *kpLN_ip, *lnb0_ip, *lnb1_ip,
        *lnfun_ip, *upperT_ip, *lowerT_ip, nitroP, *leafwidth_ip,
        *et_equation_ip, *StomataWS_ip, *specific_heat_of_air_ip,
        *atmospheric_pressure_ip, *water_stress_approach_ip,
        *absorptivity_par_ip);

    // Update the parameter list
    update(canopy_assimilation_rate_op, can_result.Assim);   // Mg / ha / hr.
    update(canopy_transpiration_rate_op, can_result.Trans);  // Mg / ha / hr.
    update(canopy_conductance_op, can_result.canopy_conductance);
    update(GrossAssim_op, can_result.GrossAssim);
}

#endif
