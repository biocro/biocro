#ifndef C3_CANOPY_H
#define C3_CANOPY_H

#include "../modules.h"
#include "../state_map.h"

class c3_canopy : public direct_module
{
   public:
    c3_canopy(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          direct_module("c3_canopy"),

          // Get references to input quantities
          lai{get_input(input_quantities, "lai")},
          time{get_input(input_quantities, "time")},
          solar{get_input(input_quantities, "solar")},
          temp{get_input(input_quantities, "temp")},
          rh{get_input(input_quantities, "rh")},
          windspeed{get_input(input_quantities, "windspeed")},
          lat{get_input(input_quantities, "lat")},
          nlayers{get_input(input_quantities, "nlayers")},
          vmax{get_input(input_quantities, "vmax")},
          jmax{get_input(input_quantities, "jmax")},
          tpu_rate_max{get_input(input_quantities, "tpu_rate_max")},
          Rd{get_input(input_quantities, "Rd")},
          Catm{get_input(input_quantities, "Catm")},
          O2{get_input(input_quantities, "O2")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},
          Gs_min{get_input(input_quantities, "Gs_min")},
          theta{get_input(input_quantities, "theta")},
          kd{get_input(input_quantities, "kd")},
          heightf{get_input(input_quantities, "heightf")},
          LeafN{get_input(input_quantities, "LeafN")},
          kpLN{get_input(input_quantities, "kpLN")},
          lnb0{get_input(input_quantities, "lnb0")},
          lnb1{get_input(input_quantities, "lnb1")},
          lnfun{get_input(input_quantities, "lnfun")},
          chil{get_input(input_quantities, "chil")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          specific_heat_of_air{get_input(input_quantities, "specific_heat_of_air")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          growth_respiration_fraction{get_input(input_quantities, "growth_respiration_fraction")},
          water_stress_approach{get_input(input_quantities, "water_stress_approach")},
          electrons_per_carboxylation{get_input(input_quantities, "electrons_per_carboxylation")},
          electrons_per_oxygenation{get_input(input_quantities, "electrons_per_oxygenation")},
          absorptivity_par{get_input(input_quantities, "absorptivity_par")},
          par_energy_content{get_input(input_quantities, "par_energy_content")},
          par_energy_fraction{get_input(input_quantities, "par_energy_fraction")},
          leaf_transmittance{get_input(input_quantities, "leaf_transmittance")},
          leaf_reflectance{get_input(input_quantities, "leaf_reflectance")},

          // Get pointers to output quantities
          canopy_assimilation_rate_op{get_op(output_quantities, "canopy_assimilation_rate")},
          canopy_transpiration_rate_op{get_op(output_quantities, "canopy_transpiration_rate")},
          GrossAssim_op{get_op(output_quantities, "GrossAssim")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // References to input quantities
    double const& lai;
    double const& time;
    double const& solar;
    double const& temp;
    double const& rh;
    double const& windspeed;
    double const& lat;
    double const& nlayers;
    double const& vmax;
    double const& jmax;
    double const& tpu_rate_max;
    double const& Rd;
    double const& Catm;
    double const& O2;
    double const& b0;
    double const& b1;
    double const& Gs_min;
    double const& theta;
    double const& kd;
    double const& heightf;
    double const& LeafN;
    double const& kpLN;
    double const& lnb0;
    double const& lnb1;
    double const& lnfun;
    double const& chil;
    double const& StomataWS;
    double const& specific_heat_of_air;
    double const& atmospheric_pressure;
    double const& growth_respiration_fraction;
    double const& water_stress_approach;
    double const& electrons_per_carboxylation;
    double const& electrons_per_oxygenation;
    double const& absorptivity_par;
    double const& par_energy_content;
    double const& par_energy_fraction;
    double const& leaf_transmittance;
    double const& leaf_reflectance;

    // Pointers to output quantities
    double* canopy_assimilation_rate_op;
    double* canopy_transpiration_rate_op;
    double* GrossAssim_op;

    // Main operation
    void do_operation() const;
};

#endif
