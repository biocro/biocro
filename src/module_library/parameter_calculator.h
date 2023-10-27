#ifndef PARAMETER_CALCULATOR_H
#define PARAMETER_CALCULATOR_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class parameter_calculator : public direct_module
{
   public:
    parameter_calculator(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          iSp_ip{get_ip(input_quantities, "iSp")},
          TTc_ip{get_ip(input_quantities, "TTc")},
          Sp_thermal_time_decay_ip{get_ip(input_quantities, "Sp_thermal_time_decay")},
          Leaf_ip{get_ip(input_quantities, "Leaf")},
          LeafN_0_ip{get_ip(input_quantities, "LeafN_0")},
          LeafN_ip{get_ip(input_quantities, "LeafN")},
          vmax_n_intercept_ip{get_ip(input_quantities, "vmax_n_intercept")},
          vmax1_ip{get_ip(input_quantities, "vmax1")},
          alphab1_ip{get_ip(input_quantities, "alphab1")},
          alpha1_ip{get_ip(input_quantities, "alpha1")},

          // Get pointers to output quantities
          Sp_op{get_op(output_quantities, "Sp")},
          lai_op{get_op(output_quantities, "lai")},
          vmax_op{get_op(output_quantities, "vmax")},
          alpha_op{get_op(output_quantities, "alpha")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "parameter_calculator"; }

   private:
    // Pointers to input quantities
    const double* iSp_ip;
    const double* TTc_ip;
    const double* Sp_thermal_time_decay_ip;
    const double* Leaf_ip;
    const double* LeafN_0_ip;
    const double* LeafN_ip;
    const double* vmax_n_intercept_ip;
    const double* vmax1_ip;
    const double* alphab1_ip;
    const double* alpha1_ip;

    // Pointers to output quantities
    double* Sp_op;
    double* lai_op;
    double* vmax_op;
    double* alpha_op;

    // Main operation
    void do_operation() const;
};

string_vector parameter_calculator::get_inputs()
{
    return {
        "iSp",
        "TTc",
        "Sp_thermal_time_decay",
        "Leaf",
        "LeafN_0",
        "LeafN",
        "vmax_n_intercept",
        "vmax1",
        "alphab1",
        "alpha1"};
}

string_vector parameter_calculator::get_outputs()
{
    return {
        "Sp",
        "lai",
        "vmax",
        "alpha"};
}

void parameter_calculator::do_operation() const
{
    // Collect inputs and make calculations
    double Sp = (*iSp_ip) - (*TTc_ip) * (*Sp_thermal_time_decay_ip);

    // Update the output quantity list
    update(Sp_op, Sp);
    update(lai_op, (*Leaf_ip) * Sp);
    update(vmax_op, ((*LeafN_0_ip) - (*LeafN_ip)) * (*vmax_n_intercept_ip) + (*vmax1_ip));
    update(alpha_op, ((*LeafN_0_ip) - (*LeafN_ip)) * (*alphab1_ip) + (*alpha1_ip));
}

}  // namespace standardBML
#endif
