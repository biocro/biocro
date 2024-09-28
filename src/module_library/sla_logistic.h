#ifndef SLA_LOGISTIC_H
#define SLA_LOGISTIC_H

#include <cmath>  // for exp
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class sla_logistic
 *
 *  @brief Calculates the specific leaf area as a logistic function of thermal
 *  time.
 *
 *  As plants age, they tend to produce thicker leaves. Thus, the leaf mass per
 *  area increases, while its inverse, the specific leaf area, decreases.
 *
 *  Here, this process is modeled as a logistic dependence of specific leaf area
 *  (`Sp`) on thermal time (`TTc`):
 *
 *  > `Sp = Sp_initial + (Sp_final - Sp_initial) / (1.0 + exp(-Sp_k * (TTc - Sp_TTc_0))`,
 *
 *  where `Sp_initial` and `Sp_final` are the initial and final values of
 *  specific leaf area, respectively, `Sp_k` sets the width of the transition
 *  from `Sp_initial` to `Sp_final`, and `Sp_TTc_0` sets the midpoint of the
 *  transition.
 *
 *  For an alternate way to model specific leaf area, see the `sla_linear`
 *  module.
 */
class sla_logistic : public direct_module
{
   public:
    sla_logistic(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          Sp_final{get_input(input_quantities, "Sp_final")},
          Sp_initial{get_input(input_quantities, "Sp_initial")},
          Sp_k{get_input(input_quantities, "Sp_k")},
          Sp_TTc_0{get_input(input_quantities, "Sp_TTc_0")},
          TTc{get_input(input_quantities, "TTc")},

          // Get pointers to output quantities
          Sp_op{get_op(output_quantities, "Sp")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "sla_logistic"; }

   private:
    // References to input quantities
    double const& Sp_final;
    double const& Sp_initial;
    double const& Sp_k;
    double const& Sp_TTc_0;
    double const& TTc;

    // Pointers to output quantities
    double* Sp_op;

    // Main operation
    void do_operation() const;
};

string_vector sla_logistic::get_inputs()
{
    return {
        "Sp_final",    // Ha / Mg
        "Sp_initial",  // Ha / Mg
        "Sp_k",        // (degree C * day)^(-1)
        "Sp_TTc_0",    // degree C * day
        "TTc"          // degree C * day
    };
}

string_vector sla_logistic::get_outputs()
{
    return {
        "Sp"  // Ha / Mg
    };
}

void sla_logistic::do_operation() const
{
    update(Sp_op, Sp_initial + (Sp_final - Sp_initial) / (1.0 + exp(-Sp_k * (TTc - Sp_TTc_0))));
}

}  // namespace standardBML
#endif
