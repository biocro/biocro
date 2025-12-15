#ifndef C4_LEAF_PHOTOSYNTHESIS_H
#define C4_LEAF_PHOTOSYNTHESIS_H

#include "../framework/state_map.h"
#include "../framework/module.h"

namespace standardBML
{
/**
 * @class c4_leaf_photosynthesis
 *
 * @brief Uses the method from `CanAC()` to calculate leaf photosynthesis
 * parameters for C4 plants
 */
class c4_leaf_photosynthesis : public direct_module
{
   public:
    c4_leaf_photosynthesis(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          absorbed_longwave{get_input(input_quantities, "absorbed_longwave")},
          absorbed_shortwave{get_input(input_quantities, "absorbed_shortwave")},
          alpha1{get_input(input_quantities, "alpha1")},
          ambient_temperature{get_input(input_quantities, "temp")},
          atmospheric_pressure{get_input(input_quantities, "atmospheric_pressure")},
          b0{get_input(input_quantities, "b0")},
          b1{get_input(input_quantities, "b1")},
          beta{get_input(input_quantities, "beta")},
          Catm{get_input(input_quantities, "Catm")},
          gbw_canopy{get_input(input_quantities, "gbw_canopy")},
          Gs_min{get_input(input_quantities, "Gs_min")},
          incident_ppfd{get_input(input_quantities, "incident_ppfd")},
          kparm{get_input(input_quantities, "kparm")},
          leafwidth{get_input(input_quantities, "leafwidth")},
          lowerT{get_input(input_quantities, "lowerT")},
          rh{get_input(input_quantities, "rh")},
          RL_at_25{get_input(input_quantities, "RL_at_25")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          theta{get_input(input_quantities, "theta")},
          upperT{get_input(input_quantities, "upperT")},
          Vcmax_at_25{get_input(input_quantities, "Vcmax_at_25")},
          windspeed{get_input(input_quantities, "windspeed")},

          // Get pointers to output quantities
          Assim_op{get_op(output_quantities, "Assim")},
          Ci_op{get_op(output_quantities, "Ci")},
          Cs_op{get_op(output_quantities, "Cs")},
          EPenman_op{get_op(output_quantities, "EPenman")},
          EPriestly_op{get_op(output_quantities, "EPriestly")},
          gbw_op{get_op(output_quantities, "gbw")},
          GrossAssim_op{get_op(output_quantities, "GrossAssim")},
          Gs_op{get_op(output_quantities, "Gs")},
          leaf_temperature_op{get_op(output_quantities, "leaf_temperature")},
          RHs_op{get_op(output_quantities, "RHs")},
          RH_canopy_op{get_op(output_quantities, "RH_canopy")},
          RL_op{get_op(output_quantities, "RL")},
          Rp_op{get_op(output_quantities, "Rp")},
          TransR_op{get_op(output_quantities, "TransR")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "c4_leaf_photosynthesis"; }

   private:
    // References to input quantities
    double const& absorbed_longwave;
    double const& absorbed_shortwave;
    double const& alpha1;
    double const& ambient_temperature;
    double const& atmospheric_pressure;
    double const& b0;
    double const& b1;
    double const& beta;
    double const& Catm;
    double const& gbw_canopy;
    double const& Gs_min;
    double const& incident_ppfd;
    double const& kparm;
    double const& leafwidth;
    double const& lowerT;
    double const& rh;
    double const& RL_at_25;
    double const& StomataWS;
    double const& theta;
    double const& upperT;
    double const& Vcmax_at_25;
    double const& windspeed;

    // Pointers to output quantities
    double* Assim_op;
    double* Ci_op;
    double* Cs_op;
    double* EPenman_op;
    double* EPriestly_op;
    double* gbw_op;
    double* GrossAssim_op;
    double* Gs_op;
    double* leaf_temperature_op;
    double* RHs_op;
    double* RH_canopy_op;
    double* RL_op;
    double* Rp_op;
    double* TransR_op;

    // Main operation
    void do_operation() const;
};

}  // namespace standardBML
#endif
