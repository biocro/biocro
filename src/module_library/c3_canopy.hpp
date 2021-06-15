#ifndef C3_CANOPY_H
#define C3_CANOPY_H

#include "../modules.h"
#include "../state_map.h"

class c3_canopy : public SteadyModule
{
   public:
    c3_canopy(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("c3_canopy"),

          // Get pointers to input quantities
          lai_ip(get_ip(input_quantities, "lai")),
          time_ip(get_ip(input_quantities, "time")),
          solar_ip(get_ip(input_quantities, "solar")),
          temp_ip(get_ip(input_quantities, "temp")),
          rh_ip(get_ip(input_quantities, "rh")),
          windspeed_ip(get_ip(input_quantities, "windspeed")),
          lat_ip(get_ip(input_quantities, "lat")),
          nlayers_ip(get_ip(input_quantities, "nlayers")),
          vmax_ip(get_ip(input_quantities, "vmax")),
          jmax_ip(get_ip(input_quantities, "jmax")),
          tpu_rate_max_ip(get_ip(input_quantities, "tpu_rate_max")),
          Rd_ip(get_ip(input_quantities, "Rd")),
          Catm_ip(get_ip(input_quantities, "Catm")),
          O2_ip(get_ip(input_quantities, "O2")),
          b0_ip(get_ip(input_quantities, "b0")),
          b1_ip(get_ip(input_quantities, "b1")),
          Gs_min_ip(get_ip(input_quantities, "Gs_min")),
          theta_ip(get_ip(input_quantities, "theta")),
          kd_ip(get_ip(input_quantities, "kd")),
          heightf_ip(get_ip(input_quantities, "heightf")),
          LeafN_ip(get_ip(input_quantities, "LeafN")),
          kpLN_ip(get_ip(input_quantities, "kpLN")),
          lnb0_ip(get_ip(input_quantities, "lnb0")),
          lnb1_ip(get_ip(input_quantities, "lnb1")),
          lnfun_ip(get_ip(input_quantities, "lnfun")),
          chil_ip(get_ip(input_quantities, "chil")),
          StomataWS_ip(get_ip(input_quantities, "StomataWS")),
          specific_heat_of_air_ip(get_ip(input_quantities, "specific_heat_of_air")),
          atmospheric_pressure_ip(get_ip(input_quantities, "atmospheric_pressure")),
          growth_respiration_fraction_ip(get_ip(input_quantities, "growth_respiration_fraction")),
          water_stress_approach_ip(get_ip(input_quantities, "water_stress_approach")),
          electrons_per_carboxylation_ip(get_ip(input_quantities, "electrons_per_carboxylation")),
          electrons_per_oxygenation_ip(get_ip(input_quantities, "electrons_per_oxygenation")),
          absorptivity_par_ip(get_ip(input_quantities, "absorptivity_par")),
          par_energy_content_ip(get_ip(input_quantities, "par_energy_content")),

          // Get pointers to output quantities
          canopy_assimilation_rate_op(get_op(output_quantities, "canopy_assimilation_rate")),
          canopy_transpiration_rate_op(get_op(output_quantities, "canopy_transpiration_rate")),
          GrossAssim_op(get_op(output_quantities, "GrossAssim"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // Pointers to input quantities
    const double* lai_ip;
    const double* time_ip;
    const double* solar_ip;
    const double* temp_ip;
    const double* rh_ip;
    const double* windspeed_ip;
    const double* lat_ip;
    const double* nlayers_ip;
    const double* vmax_ip;
    const double* jmax_ip;
    const double* tpu_rate_max_ip;
    const double* Rd_ip;
    const double* Catm_ip;
    const double* O2_ip;
    const double* b0_ip;
    const double* b1_ip;
    const double* Gs_min_ip;
    const double* theta_ip;
    const double* kd_ip;
    const double* heightf_ip;
    const double* LeafN_ip;
    const double* kpLN_ip;
    const double* lnb0_ip;
    const double* lnb1_ip;
    const double* lnfun_ip;
    const double* chil_ip;
    const double* StomataWS_ip;
    const double* specific_heat_of_air_ip;
    const double* atmospheric_pressure_ip;
    const double* growth_respiration_fraction_ip;
    const double* water_stress_approach_ip;
    const double* electrons_per_carboxylation_ip;
    const double* electrons_per_oxygenation_ip;
    const double* absorptivity_par_ip;
    const double* par_energy_content_ip;

    // Pointers to output quantities
    double* canopy_assimilation_rate_op;
    double* canopy_transpiration_rate_op;
    double* GrossAssim_op;

    // Main operation
    void do_operation() const;
};

#endif
