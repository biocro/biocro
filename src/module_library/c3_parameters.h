#ifndef C3_PARAMETERS_H
#define C3_PARAMETERS_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "c3_temperature_response.h"

namespace standardBML
{
/**
 * @class c3_parameters
 *
 * @brief Uses `c3_temperature_response()` to calculate the values of key C3
 * photosynthesis parameters at leaf temperature.
 */
class c3_parameters : public direct_module
{
   public:
    c3_parameters(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          gm_Ha{get_input(input_quantities, "gm_Ha")},
          gm_Hd{get_input(input_quantities, "gm_Hd")},
          gm_S{get_input(input_quantities, "gm_S")},
          Gstar_Ea{get_input(input_quantities, "Gstar_Ea")},
          Jmax_Ea{get_input(input_quantities, "Jmax_Ea")},
          Kc_Ea{get_input(input_quantities, "Kc_Ea")},
          Ko_Ea{get_input(input_quantities, "Ko_Ea")},
          phi_PSII_0{get_input(input_quantities, "phi_PSII_0")},
          phi_PSII_1{get_input(input_quantities, "phi_PSII_1")},
          phi_PSII_2{get_input(input_quantities, "phi_PSII_2")},
          RL_Ea{get_input(input_quantities, "RL_Ea")},
          theta_0{get_input(input_quantities, "theta_0")},
          theta_1{get_input(input_quantities, "theta_1")},
          theta_2{get_input(input_quantities, "theta_2")},
          Tleaf{get_input(input_quantities, "Tleaf")},
          Tp_Ha{get_input(input_quantities, "Tp_Ha")},
          Tp_Hd{get_input(input_quantities, "Tp_Hd")},
          Tp_S{get_input(input_quantities, "Tp_S")},
          Vcmax_Ea{get_input(input_quantities, "Vcmax_Ea")},

          // Get pointers to output quantities
          gm_norm_op{get_op(output_quantities, "gm_norm")},
          Gstar_norm_op{get_op(output_quantities, "Gstar_norm")},
          Jmax_norm_op{get_op(output_quantities, "Jmax_norm")},
          Kc_norm_op{get_op(output_quantities, "Kc_norm")},
          Ko_norm_op{get_op(output_quantities, "Ko_norm")},
          phi_PSII_op{get_op(output_quantities, "phi_PSII")},
          RL_norm_op{get_op(output_quantities, "RL_norm")},
          theta_op{get_op(output_quantities, "theta")},
          Tp_norm_op{get_op(output_quantities, "Tp_norm")},
          Vcmax_norm_op{get_op(output_quantities, "Vcmax_norm")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "c3_parameters"; }

   private:
    // References to input quantities
    double const& gm_Ha;
    double const& gm_Hd;
    double const& gm_S;
    double const& Gstar_Ea;
    double const& Jmax_Ea;
    double const& Kc_Ea;
    double const& Ko_Ea;
    double const& phi_PSII_0;
    double const& phi_PSII_1;
    double const& phi_PSII_2;
    double const& RL_Ea;
    double const& theta_0;
    double const& theta_1;
    double const& theta_2;
    double const& Tleaf;
    double const& Tp_Ha;
    double const& Tp_Hd;
    double const& Tp_S;
    double const& Vcmax_Ea;

    // Pointers to output quantities
    double* gm_norm_op;
    double* Gstar_norm_op;
    double* Jmax_norm_op;
    double* Kc_norm_op;
    double* Ko_norm_op;
    double* phi_PSII_op;
    double* RL_norm_op;
    double* theta_op;
    double* Tp_norm_op;
    double* Vcmax_norm_op;

    // Main operation
    void do_operation() const;
};

string_vector c3_parameters::get_inputs()
{
    return {
        "gm_Ha",       // J / mol
        "gm_Hd",       // J / mol
        "gm_S",        // J / K / mol
        "Gstar_Ea",    // J / mol
        "Jmax_Ea",     // J / mol
        "Kc_Ea",       // J / mol
        "Ko_Ea",       // J / mol
        "phi_PSII_0",  // dimensionless
        "phi_PSII_1",  // (degrees C)^(-1)
        "phi_PSII_2",  // (degrees C)^(-2)
        "RL_Ea",       // J / mol
        "theta_0",     // dimensionless
        "theta_1",     // (degrees C)^(-1)
        "theta_2",     // (degrees C)^(-2)
        "Tleaf",       // degrees C
        "Tp_Ha",       // J / mol
        "Tp_Hd",       // J / mol
        "Tp_S",        // J / K / mol
        "Vcmax_Ea"     // J / mol
    };
}

string_vector c3_parameters::get_outputs()
{
    return {
        "gm_norm",     // dimensionless
        "Gstar_norm",  // dimensionless
        "Jmax_norm",   // dimensionless
        "Kc_norm",     // dimensionless
        "Ko_norm",     // dimensionless
        "phi_PSII",    // dimensionless
        "RL_norm",     // dimensionless
        "theta",       // dimensionless
        "Tp_norm",     // dimensionless
        "Vcmax_norm"   // dimensionless
    };
}

void c3_parameters::do_operation() const
{
    // Combine temperature response parameters
    c3_temperature_response_parameters const tr_param{
        gm_Ha,
        gm_Hd,
        gm_S,
        Gstar_Ea,
        Jmax_Ea,
        Kc_Ea,
        Ko_Ea,
        phi_PSII_0,
        phi_PSII_1,
        phi_PSII_2,
        RL_Ea,
        theta_0,
        theta_1,
        theta_2,
        Tp_Ha,
        Tp_Hd,
        Tp_S,
        Vcmax_Ea};

    // Calculate values of key parameters at leaf temperature
    c3_param_at_tleaf c3_param = c3_temperature_response(tr_param, Tleaf);

    // Update the output quantity list
    update(gm_norm_op, c3_param.gm_norm);
    update(Gstar_norm_op, c3_param.Gstar_norm);
    update(Jmax_norm_op, c3_param.Jmax_norm);
    update(Kc_norm_op, c3_param.Kc_norm);
    update(Ko_norm_op, c3_param.Ko_norm);
    update(phi_PSII_op, c3_param.phi_PSII);
    update(RL_norm_op, c3_param.RL_norm);
    update(theta_op, c3_param.theta);
    update(Tp_norm_op, c3_param.Tp_norm);
    update(Vcmax_norm_op, c3_param.Vcmax_norm);
}

}  // namespace standardBML
#endif
