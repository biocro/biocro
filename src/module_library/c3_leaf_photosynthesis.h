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
          Gstar_c{get_input(input_quantities, "Gstar_c")},
          Gstar_Ea{get_input(input_quantities, "Gstar_Ea")},
          height{get_input(input_quantities, "height")},
          jmax{get_input(input_quantities, "jmax")},
          Jmax_c{get_input(input_quantities, "Jmax_c")},
          Jmax_Ea{get_input(input_quantities, "Jmax_Ea")},
          Kc_c{get_input(input_quantities, "Kc_c")},
          Kc_Ea{get_input(input_quantities, "Kc_Ea")},
          Ko_c{get_input(input_quantities, "Ko_c")},
          Ko_Ea{get_input(input_quantities, "Ko_Ea")},
          leafwidth{get_input(input_quantities, "leafwidth")},
          O2{get_input(input_quantities, "O2")},
          phi_PSII_0{get_input(input_quantities, "phi_PSII_0")},
          phi_PSII_1{get_input(input_quantities, "phi_PSII_1")},
          phi_PSII_2{get_input(input_quantities, "phi_PSII_2")},
          Rd{get_input(input_quantities, "Rd")},
          Rd_c{get_input(input_quantities, "Rd_c")},
          Rd_Ea{get_input(input_quantities, "Rd_Ea")},
          rh{get_input(input_quantities, "rh")},
          StomataWS{get_input(input_quantities, "StomataWS")},
          theta_0{get_input(input_quantities, "theta_0")},
          theta_1{get_input(input_quantities, "theta_1")},
          theta_2{get_input(input_quantities, "theta_2")},
          Tp_c{get_input(input_quantities, "Tp_c")},
          Tp_Ha{get_input(input_quantities, "Tp_Ha")},
          Tp_Hd{get_input(input_quantities, "Tp_Hd")},
          Tp_S{get_input(input_quantities, "Tp_S")},
          tpu_rate_max{get_input(input_quantities, "tpu_rate_max")},
          Vcmax_c{get_input(input_quantities, "Vcmax_c")},
          Vcmax_Ea{get_input(input_quantities, "Vcmax_Ea")},
          vmax1{get_input(input_quantities, "vmax1")},
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
          Rp_op{get_op(output_quantities, "Rp")},
          TransR_op{get_op(output_quantities, "TransR")}
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
    double const& Gstar_c;
    double const& Gstar_Ea;
    double const& height;
    double const& jmax;
    double const& Jmax_c;
    double const& Jmax_Ea;
    double const& Kc_c;
    double const& Kc_Ea;
    double const& Ko_c;
    double const& Ko_Ea;
    double const& leafwidth;
    double const& O2;
    double const& phi_PSII_0;
    double const& phi_PSII_1;
    double const& phi_PSII_2;
    double const& Rd;
    double const& Rd_c;
    double const& Rd_Ea;
    double const& rh;
    double const& StomataWS;
    double const& theta_0;
    double const& theta_1;
    double const& theta_2;
    double const& Tp_c;
    double const& Tp_Ha;
    double const& Tp_Hd;
    double const& Tp_S;
    double const& tpu_rate_max;
    double const& Vcmax_c;
    double const& Vcmax_Ea;
    double const& vmax1;
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
    double* Rp_op;
    double* TransR_op;

    // Main operation
    void do_operation() const;
};

}  // namespace standardBML
#endif
