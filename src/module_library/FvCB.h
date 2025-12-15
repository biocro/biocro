#ifndef FVCB_H
#define FVCB_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "FvCB_assim.h"

namespace standardBML
{
/**
 * @class FvCB
 *
 * @brief Uses `FvCB_assim()` to evaluate the Farquhar-von-Caemmerer-Berry model
 * for C3 photosynthesis.
 */
class FvCB : public direct_module
{
   public:
    FvCB(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          Ci{get_input(input_quantities, "Ci")},
          Gstar{get_input(input_quantities, "Gstar")},
          J{get_input(input_quantities, "J")},
          Kc{get_input(input_quantities, "Kc")},
          Ko{get_input(input_quantities, "Ko")},
          Oi{get_input(input_quantities, "Oi")},
          RL{get_input(input_quantities, "RL")},
          TPU{get_input(input_quantities, "TPU")},
          Vcmax{get_input(input_quantities, "Vcmax")},
          alpha_TPU{get_input(input_quantities, "alpha_TPU")},
          electrons_per_carboxylation{get_input(input_quantities, "electrons_per_carboxylation")},
          electrons_per_oxygenation{get_input(input_quantities, "electrons_per_oxygenation")},

          // Get pointers to output quantities
          An_op{get_op(output_quantities, "An")},
          Ac_op{get_op(output_quantities, "Ac")},
          Aj_op{get_op(output_quantities, "Aj")},
          Ap_op{get_op(output_quantities, "Ap")},
          Vc_op{get_op(output_quantities, "Vc")},
          Wc_op{get_op(output_quantities, "Wc")},
          Wj_op{get_op(output_quantities, "Wj")},
          Wp_op{get_op(output_quantities, "Wp")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "FvCB"; }

   private:
    // References to input quantities
    double const& Ci;
    double const& Gstar;
    double const& J;
    double const& Kc;
    double const& Ko;
    double const& Oi;
    double const& RL;
    double const& TPU;
    double const& Vcmax;
    double const& alpha_TPU;
    double const& electrons_per_carboxylation;
    double const& electrons_per_oxygenation;

    // Pointers to output quantities
    double* An_op;
    double* Ac_op;
    double* Aj_op;
    double* Ap_op;
    double* Vc_op;
    double* Wc_op;
    double* Wj_op;
    double* Wp_op;

    // Main operation
    void do_operation() const;
};

string_vector FvCB::get_inputs()
{
    return {
        "Ci",                           // micromol / mol
        "Gstar",                        // micromol / mol
        "J",                            // micromol / mol
        "Kc",                           // mmol / mol
        "Ko",                           // mmol / mol
        "Oi",                           // micromol / m^2 / s
        "RL",                           // micromol / m^2 / s
        "TPU",                          // micromol / m^2 / s
        "Vcmax",                        // micromol / m^2 / s
        "alpha_TPU",                    // dimensionless
        "electrons_per_carboxylation",  // self-explanatory units
        "electrons_per_oxygenation"     // self-explanatory units
    };
}

string_vector FvCB::get_outputs()
{
    return {
        "An",  // micromol / m^2 / s
        "Ac",  // micromol / m^2 / s
        "Aj",  // micromol / m^2 / s
        "Ap",  // micromol / m^2 / s
        "Vc",  // micromol / m^2 / s
        "Wc",  // micromol / m^2 / s
        "Wj",  // micromol / m^2 / s
        "Wp"   // micromol / m^2 / s
    };
}

void FvCB::do_operation() const
{
    FvCB_outputs result = FvCB_assim(
        Ci,
        Gstar,
        J,
        Kc,
        Ko,
        Oi,
        RL,
        TPU,
        Vcmax,
        alpha_TPU,
        electrons_per_carboxylation,
        electrons_per_oxygenation);

    // Update the output quantity list
    update(An_op, result.An);
    update(Ac_op, result.Ac);
    update(Aj_op, result.Aj);
    update(Ap_op, result.Ap);
    update(Vc_op, result.Vc);
    update(Wc_op, result.Wc);
    update(Wj_op, result.Wj);
    update(Wp_op, result.Wp);
}

}  // namespace standardBML
#endif
