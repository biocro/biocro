#ifndef SLA_LINEAR_H
#define SLA_LINEAR_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class sla_linear
 *
 *  @brief Calculates the specific leaf area as a linear function of thermal
 *  time.
 *
 *  As plants age, they tend to produce thicker leaves. Thus, the leaf mass per
 *  area increases, while its inverse, the specific leaf area, decreases.
 *
 *  Here, this process is modeled as a linear dependence of specific leaf area
 *  (`Sp`) on thermal time (`TTc`):
 *
 *  > `Sp = iSp - Sp_thermal_time_decay * TTc`,
 *
 *  where `iSp` is the initial value of the specific leaf area and
 *  `Sp_thermal_time_decay` is the slope.
 *
 *  For an alternate way to model specific leaf area, see the `sla_logistic`
 *  module.
 */
class sla_linear : public direct_module
{
   public:
    sla_linear(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          iSp{get_input(input_quantities, "iSp")},
          Sp_thermal_time_decay{get_input(input_quantities, "Sp_thermal_time_decay")},
          TTc{get_input(input_quantities, "TTc")},

          // Get pointers to output quantities
          Sp_op{get_op(output_quantities, "Sp")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "sla_linear"; }

   private:
    // References to input quantities
    double const& iSp;
    double const& Sp_thermal_time_decay;
    double const& TTc;

    // Pointers to output quantities
    double* Sp_op;

    // Main operation
    void do_operation() const;
};

string_vector sla_linear::get_inputs()
{
    return {
        "iSp",                   // Ha / Mg
        "TTc",                   // degree C * day
        "Sp_thermal_time_decay"  // Ha / Mg / degree C / day
    };
}

string_vector sla_linear::get_outputs()
{
    return {
        "Sp"  // Ha / Mg
    };
}

void sla_linear::do_operation() const
{
    update(Sp_op, iSp - TTc * Sp_thermal_time_decay);
}

}  // namespace standardBML
#endif
