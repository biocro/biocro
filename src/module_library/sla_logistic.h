#ifndef SLA_LOGISTIC_H
#define SLA_LOGISTIC_H

#include <cmath>  // for exp
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class sla_logistic : public direct_module
{
   public:
    sla_logistic(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          Sp_k{get_input(input_quantities, "Sp_k")},
          Sp_max{get_input(input_quantities, "Sp_max")},
          Sp_min{get_input(input_quantities, "Sp_min")},
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
    double const& Sp_k;
    double const& Sp_max;
    double const& Sp_min;
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
        "Sp_k",      // (degree C * day)^(-1)
        "Sp_max",    // Ha / Mg
        "Sp_min",    // Ha / Mg
        "Sp_TTc_0",  // degree C * day
        "TTc"        // degree C * day
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
    update(Sp_op,
           Sp_min + (Sp_max - Sp_min) / (1.0 + exp(-Sp_k * (TTc - Sp_TTc_0))));
}

}  // namespace standardBML
#endif
