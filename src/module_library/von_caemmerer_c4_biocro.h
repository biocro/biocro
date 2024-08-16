
#ifndef VON_CAEMMERER_C4_BIOCRO_H
#define VON_CAEMMERER_C4_BIOCRO_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "von_caemmerer_c4_assimilation.h"

namespace standardBML
{
/**
 * @class von_caemmerer_c4_biocro
 *
 * @brief Put Documentation Here.
 *
 */
class von_caemmerer_c4_biocro : public direct_module
{
   public:
    von_caemmerer_c4_biocro(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities
          Cm{get_input(input_quantities, "Cm")},
          Jmax{get_input(input_quantities, "Jmax")},
          Kc{get_input(input_quantities, "Kc")},
          Ko{get_input(input_quantities, "Ko")},
          Kp{get_input(input_quantities, "Kp")},
          POm{get_input(input_quantities, "POm")},
          Qin{get_input(input_quantities, "Qin")},
          RL{get_input(input_quantities, "RL")},
          Rm_frac{get_input(input_quantities, "Rm_frac")},
          Vcmax{get_input(input_quantities, "Vcmax")},
          Vpmax{get_input(input_quantities, "Vpmax")},
          Vpr{get_input(input_quantities, "Vpr")},
          absorptance{get_input(input_quantities, "absorptance")},
          alpha_psii{get_input(input_quantities, "alpha_psii")},
          ao{get_input(input_quantities, "ao")},
          f_spectral{get_input(input_quantities, "f_spectral")},
          gamma_star{get_input(input_quantities, "gamma_star")},
          gbs{get_input(input_quantities, "gbs")},
          rho{get_input(input_quantities, "rho")},
          theta{get_input(input_quantities, "theta")},
          x_etr{get_input(input_quantities, "x_etr")},

          // Get pointers to output quantities
          assimilation_op{get_op(output_quantities, "assimilation")},
          Aj_op{get_op(output_quantities, "Aj")},
          Ac_op{get_op(output_quantities, "Ac")},
          J_op{get_op(output_quantities, "J")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "von_caemmerer_c4_biocro"; }

   private:
    // References to input quantities
    double const& Cm;
    double const& Jmax;
    double const& Kc;
    double const& Ko;
    double const& Kp;
    double const& POm;
    double const& Qin;
    double const& RL;
    double const& Rm_frac;
    double const& Vcmax;
    double const& Vpmax;
    double const& Vpr;
    double const& absorptance;
    double const& alpha_psii;
    double const& ao;
    double const& f_spectral;
    double const& gamma_star;
    double const& gbs;
    double const& rho;
    double const& theta;
    double const& x_etr;

    // Pointers to output quantities
    double* assimilation_op;
    double* Aj_op;
    double* Ac_op;
    double* J_op;

    // Main operation
    void do_operation() const;
};

string_vector von_caemmerer_c4_biocro::get_inputs()
{
    return {
        "Cm",
        "Jmax",
        "Kc",
        "Ko",
        "Kp",
        "POm",
        "Qin",
        "RL",
        "Rm_frac",
        "Vcmax",
        "Vpmax",
        "Vpr",
        "absorptance",
        "alpha_psii",
        "ao",
        "f_spectral",
        "gamma_star",
        "gbs",
        "rho",
        "theta",
        "x_etr",
    };
}

string_vector von_caemmerer_c4_biocro::get_outputs()
{
    return {
        "assimilation",
        "Aj",
        "Ac",
        "J",
    };
}

void von_caemmerer_c4_biocro::do_operation() const
{
    // bundle parameters into parameter objects
    Rate_type rate = {Vcmax, Vpmax, Vpr};
    K_type K = {Kc, Ko, Kp};
    Light_Param_type light = {Jmax, absorptance, f_spectral, rho, theta, x_etr};
    VC_C4_param_type params = {RL, Rm_frac, K, rate, light, alpha_psii, ao, gamma_star, gbs};

    // call vc_c4_assim for assimilation rates
    VC_C4_output out = vc_c4_assim(Cm, POm, Qin, params);
    update(assimilation_op, out.assimilation);
    update(Aj_op, out.light_limited_assimilation);
    update(Ac_op, out.enzyme_limited_assimilation);
    update(J_op, out.electron_transport_rate);
}

}  // namespace standardBML
#endif
