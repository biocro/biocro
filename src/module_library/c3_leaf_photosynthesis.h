#ifndef C3_LEAF_PHOTOSYNTHESIS_H
#define C3_LEAF_PHOTOSYNTHESIS_H

#include "../framework/state_map.h"
#include "../framework/module.h"

namespace standardBML
{
/**
 * @class c3_leaf_photosynthesis
 *
 * @brief Uses the method from `c3CanAC()` to calculate leaf photosynthesis
 * parameters for C3 plants
 */
class c3_leaf_photosynthesis : public direct_module
{
   public:
    c3_leaf_photosynthesis(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          absorbed_longwave{get_input(input_quantities, "absorbed_longwave")},
          absorbed_ppfd{get_input(input_quantities, "absorbed_ppfd")},
          absorbed_shortwave{get_input(input_quantities, "absorbed_shortwave")},
          ambient_temperature{get_input(input_quantities, "temp")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},
          beta_PSII{get_input(input_quantities, "beta_PSII")},
          Catm{get_input(input_quantities, "Catm")},
          electrons_per_carboxylation{get_input(input_quantities, "electrons_per_carboxylation")},
          electrons_per_oxygenation{get_input(input_quantities, "electrons_per_oxygenation")},
          gbw_canopy{get_input(input_quantities, "gbw_canopy")},
          Gs_min{get_input(input_quantities, "Gs_min")},
          height{get_input(input_quantities, "height")},
          jmax{get_input(input_quantities, "jmax")},
          leafwidth{get_input(input_quantities, "leafwidth")},
          O2{get_input(input_quantities, "O2")},
          Rd{get_input(input_quantities, "Rd")},
          rh{get_input(input_quantities, "rh")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          theta{get_input(input_quantities, "theta")},
          tpu_rate_max{get_input(input_quantities, "tpu_rate_max")},
          vmax1{get_input(input_quantities, "vmax1")},
          windspeed{get_input(input_quantities, "windspeed")},

          // Get pointers to output quantities
          Assim_op{get_op(output_quantities, "Assim")},
          GrossAssim_op{get_op(output_quantities, "GrossAssim")},
          Rp_op{get_op(output_quantities, "Rp")},
          Ci_op{get_op(output_quantities, "Ci")},
          Gs_op{get_op(output_quantities, "Gs")},
          Cs_op{get_op(output_quantities, "Cs")},
          RHs_op{get_op(output_quantities, "RHs")},
          TransR_op{get_op(output_quantities, "TransR")},
          EPenman_op{get_op(output_quantities, "EPenman")},
          EPriestly_op{get_op(output_quantities, "EPriestly")},
          leaf_temperature_op{get_op(output_quantities, "leaf_temperature")},
          gbw_op{get_op(output_quantities, "gbw")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "c3_leaf_photosynthesis"; }

   private:
    // References to input quantities
    double const& absorbed_longwave;
    double const& absorbed_ppfd;
    double const& absorbed_shortwave;
    double const& ambient_temperature;
    double const& atmospheric_pressure;
    double const& b0;
    double const& b1;
    double const& beta_PSII;
    double const& Catm;
    double const& electrons_per_carboxylation;
    double const& electrons_per_oxygenation;
    double const& gbw_canopy;
    double const& Gs_min;
    double const& height;
    double const& jmax;
    double const& leafwidth;
    double const& O2;
    double const& Rd;
    double const& rh;
    double const& StomataWS;
    double const& theta;
    double const& tpu_rate_max;
    double const& vmax1;
    double const& windspeed;

    // Pointers to output quantities
    double* Assim_op;
    double* GrossAssim_op;
    double* Rp_op;
    double* Ci_op;
    double* Gs_op;
    double* Cs_op;
    double* RHs_op;
    double* TransR_op;
    double* EPenman_op;
    double* EPriestly_op;
    double* leaf_temperature_op;
    double* gbw_op;

    // Main operation
    void do_operation() const;
};

}  // namespace standardBML
#endif
