#ifndef STANDARDBML_VC_C4_H
#define STANDARDBML_VC_C4_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "von_caemmerer_c4_model_core.h"

namespace standardBML
{
/**
 * @class vc_c4
 *
 * @brief Put documentation here.
 *
 */
class vc_c4 : public direct_module
{
   public:
    vc_c4(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          alpha_psii{get_input(input_quantities, "alpha_psii")},
          ao{get_input(input_quantities, "ao")},
          Cm{get_input(input_quantities, "Cm")},
          f_cyc{get_input(input_quantities, "f_cyc")},
          gamma_star{get_input(input_quantities, "gamma_star")},
          gbs{get_input(input_quantities, "gbs")},
          J{get_input(input_quantities, "J")},
          Kc{get_input(input_quantities, "Kc")},
          Ko{get_input(input_quantities, "Ko")},
          Kp{get_input(input_quantities, "Kp")},
          Om{get_input(input_quantities, "Om")},
          Rd{get_input(input_quantities, "Rd")},
          Rm{get_input(input_quantities, "Rm")},
          Vcmax{get_input(input_quantities, "Vcmax")},
          Vpmax{get_input(input_quantities, "Vpmax")},
          Vpr{get_input(input_quantities, "Vpr")},
          x_etr{get_input(input_quantities, "x_etr")},

          // Get pointers to output quantities
          An_op{get_op(output_quantities, "An")},
          Ac_op{get_op(output_quantities, "Ac")},
          Aj_op{get_op(output_quantities, "Aj")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "vc_c4"; }

   private:
    // References to input quantities
    double const& alpha_psii;
    double const& ao;
    double const& Cm;
    double const& f_cyc;
    double const& gamma_star;
    double const& gbs;
    double const& J;
    double const& Kc;
    double const& Ko;
    double const& Kp;
    double const& Om;
    double const& Rd;
    double const& Rm;
    double const& Vcmax;
    double const& Vpmax;
    double const& Vpr;
    double const& x_etr;

    // Pointers to output quantities
    double* An_op;
    double* Ac_op;
    double* Aj_op;

    // Main operation
    void do_operation() const;
};

string_vector vc_c4::get_inputs()
{
    return {
        "alpha_psii",  // dimensionless
        "ao",          // dimensionless ratio of diffusitivites O2 : CO2
        "Cm",          // Pa (CO2 mesophyll)
        "f_cyc",       // dimensionless
        "gamma_star",  // dimensionless ratio CO2 : O2 Pa
        "gbs",         // micromol / m^2 / s / Pa
        "J",           // micromol / m^2 / s
        "Kc",          // Pa
        "Ko",          // Pa
        "Kp",          // Pa
        "Om",          // Pa (O2 mesophyll)
        "Rd",          // micromol / m^2 / s
        "Rm",          // micromol / m^2 / s
        "Vcmax",       // micromol / m^2 / s
        "Vpmax",       // micromol / m^2 / s
        "Vpr",         // micromol / m^2 / s
        "x_etr"        // dimensionless
    };
}

string_vector vc_c4::get_outputs()
{
    return {
        "An",  // micromol / m^2 / s
        "Ac",  // micromol / m^2 / s
        "Aj"   // micromol / m^2 / s
    };
}

void vc_c4::do_operation() const
{
    // Make calculations here

    vc_c4_result res = vc_c4_module_core(
        alpha_psii,
        ao,
        Cm,
        f_cyc,
        gamma_star,
        gbs,
        J,
        Kc,
        Ko,
        Kp,
        Om,
        Rd,
        Rm,
        Vcmax,
        Vpmax,
        Vpr,
        x_etr);
    // Use `update` to set outputs
    update(An_op, res.An);
    update(Ac_op, res.Ac);
    update(Aj_op, res.Aj);

}

}  // namespace standardBML
#endif
