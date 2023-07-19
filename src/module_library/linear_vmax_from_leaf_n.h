#ifndef LINEAR_VMAX_FROM_LEAF_N_H
#define LINEAR_VMAX_FROM_LEAF_N_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class linear_vmax_from_leaf_n : public direct_module
{
   public:
    linear_vmax_from_leaf_n(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          LeafN_0_ip{get_ip(input_quantities, "LeafN_0")},
          LeafN_ip{get_ip(input_quantities, "LeafN")},
          vmax_n_intercept_ip{get_ip(input_quantities, "vmax_n_intercept")},
          vmax1_ip{get_ip(input_quantities, "vmax1")},

          // Get pointers to output quantities
          vmax_op{get_op(output_quantities, "vmax")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "linear_vmax_from_leaf_n"; }

   private:
    // Pointers to input quantities
    const double* LeafN_0_ip;
    const double* LeafN_ip;
    const double* vmax_n_intercept_ip;
    const double* vmax1_ip;

    // Pointers to output quantities
    double* vmax_op;

    // Main operation
    void do_operation() const;
};

string_vector linear_vmax_from_leaf_n::get_inputs()
{
    return {
        "LeafN_0",           //
        "LeafN",             //
        "vmax_n_intercept",  //
        "vmax1"              //
    };
}

string_vector linear_vmax_from_leaf_n::get_outputs()
{
    return {
        "vmax"  //
    };
}

void linear_vmax_from_leaf_n::do_operation() const
{
    // Collect inputs and make calculations
    double LeafN_0 = *LeafN_0_ip;
    double LeafN = *LeafN_ip;
    double vmax_n_intercept = *vmax_n_intercept_ip;
    double vmax1 = *vmax1_ip;

    // Update the output quantity list
    update(vmax_op, (LeafN_0 - LeafN) * vmax_n_intercept + vmax1);
}

}  // namespace standardBML
#endif
