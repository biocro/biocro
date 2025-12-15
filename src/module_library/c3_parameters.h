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
          Gstar_c{get_input(input_quantities, "Gstar_c")},
          Gstar_Ea{get_input(input_quantities, "Gstar_Ea")},
          Jmax_c{get_input(input_quantities, "Jmax_c")},
          Jmax_Ea{get_input(input_quantities, "Jmax_Ea")},
          Kc_c{get_input(input_quantities, "Kc_c")},
          Kc_Ea{get_input(input_quantities, "Kc_Ea")},
          Ko_c{get_input(input_quantities, "Ko_c")},
          Ko_Ea{get_input(input_quantities, "Ko_Ea")},
          phi_PSII_0{get_input(input_quantities, "phi_PSII_0")},
          phi_PSII_1{get_input(input_quantities, "phi_PSII_1")},
          phi_PSII_2{get_input(input_quantities, "phi_PSII_2")},
          RL_c{get_input(input_quantities, "RL_c")},
          RL_Ea{get_input(input_quantities, "RL_Ea")},
          theta_0{get_input(input_quantities, "theta_0")},
          theta_1{get_input(input_quantities, "theta_1")},
          theta_2{get_input(input_quantities, "theta_2")},
          Tleaf{get_input(input_quantities, "Tleaf")},
          Tp_c{get_input(input_quantities, "Tp_c")},
          Tp_Ha{get_input(input_quantities, "Tp_Ha")},
          Tp_Hd{get_input(input_quantities, "Tp_Hd")},
          Tp_S{get_input(input_quantities, "Tp_S")},
          Vcmax_c{get_input(input_quantities, "Vcmax_c")},
          Vcmax_Ea{get_input(input_quantities, "Vcmax_Ea")},

          // Get pointers to output quantities
          Gstar_op{get_op(output_quantities, "Gstar")},
          Jmax_norm_op{get_op(output_quantities, "Jmax_norm")},
          Kc_op{get_op(output_quantities, "Kc")},
          Ko_op{get_op(output_quantities, "Ko")},
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
    double const& Gstar_c;
    double const& Gstar_Ea;
    double const& Jmax_c;
    double const& Jmax_Ea;
    double const& Kc_c;
    double const& Kc_Ea;
    double const& Ko_c;
    double const& Ko_Ea;
    double const& phi_PSII_0;
    double const& phi_PSII_1;
    double const& phi_PSII_2;
    double const& RL_c;
    double const& RL_Ea;
    double const& theta_0;
    double const& theta_1;
    double const& theta_2;
    double const& Tleaf;
    double const& Tp_c;
    double const& Tp_Ha;
    double const& Tp_Hd;
    double const& Tp_S;
    double const& Vcmax_c;
    double const& Vcmax_Ea;

    // Pointers to output quantities
    double* Gstar_op;
    double* Jmax_norm_op;
    double* Kc_op;
    double* Ko_op;
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
        "Gstar_c",     // dimensionless
        "Gstar_Ea",    // J / mol
        "Jmax_c",      // dimensionless
        "Jmax_Ea",     // J / mol
        "Kc_c",        // dimensionless
        "Kc_Ea",       // J / mol
        "Ko_c",        // dimensionless
        "Ko_Ea",       // J / mol
        "phi_PSII_0",  // dimensionless
        "phi_PSII_1",  // (degrees C)^(-1)
        "phi_PSII_2",  // (degrees C)^(-2)
        "RL_c",        // dimensionless
        "RL_Ea",       // J / mol
        "theta_0",     // dimensionless
        "theta_1",     // (degrees C)^(-1)
        "theta_2",     // (degrees C)^(-2)
        "Tleaf",       // degrees C
        "Tp_c",        // dimensionless
        "Tp_Ha",       // J / mol
        "Tp_Hd",       // J / mol
        "Tp_S",        // J / K / mol
        "Vcmax_c",     // dimensionless
        "Vcmax_Ea"     // J / mol
    };
}

string_vector c3_parameters::get_outputs()
{
    return {
        "Gstar",      // micromol / mol
        "Jmax_norm",  // dimensionless
        "Kc",         // micromol / mol
        "Ko",         // mmol / mol
        "phi_PSII",   // dimensionless
        "RL_norm",    // dimensionless
        "theta",      // dimensionless
        "Tp_norm",    // dimensionless
        "Vcmax_norm"  // dimensionless
    };
}

void c3_parameters::do_operation() const
{
    // Combine temperature response parameters
    c3_temperature_response_parameters const tr_param{
        Gstar_c,
        Gstar_Ea,
        Jmax_c,
        Jmax_Ea,
        Kc_c,
        Kc_Ea,
        Ko_c,
        Ko_Ea,
        phi_PSII_0,
        phi_PSII_1,
        phi_PSII_2,
        RL_c,
        RL_Ea,
        theta_0,
        theta_1,
        theta_2,
        Tp_c,
        Tp_Ha,
        Tp_Hd,
        Tp_S,
        Vcmax_c,
        Vcmax_Ea};

    // Calculate values of key parameters at leaf temperature
    c3_param_at_tleaf c3_param = c3_temperature_response(tr_param, Tleaf);

    // Update the output quantity list
    update(Gstar_op, c3_param.Gstar);
    update(Jmax_norm_op, c3_param.Jmax_norm);
    update(Kc_op, c3_param.Kc);
    update(Ko_op, c3_param.Ko);
    update(phi_PSII_op, c3_param.phi_PSII);
    update(RL_norm_op, c3_param.RL_norm);
    update(theta_op, c3_param.theta);
    update(Tp_norm_op, c3_param.Tp_norm);
    update(Vcmax_norm_op, c3_param.Vcmax_norm);
}

}  // namespace standardBML
#endif
