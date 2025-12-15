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
          LeafN{get_input(input_quantities, "LeafN")},
          LeafN_0{get_input(input_quantities, "LeafN_0")},
          Vcmax_at_25_n_offset{get_input(input_quantities, "Vcmax_at_25_n_offset")},
          Vcmax_at_25_n_slope{get_input(input_quantities, "Vcmax_at_25_n_slope")},

          // Get pointers to output quantities
          Vcmax_at_25_op{get_op(output_quantities, "Vcmax_at_25")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "linear_vmax_from_leaf_n"; }

   private:
    // Pointers to input quantities
    double const& LeafN;
    double const& LeafN_0;
    double const& Vcmax_at_25_n_offset;
    double const& Vcmax_at_25_n_slope;

    // Pointers to output quantities
    double* Vcmax_at_25_op;

    // Main operation
    void do_operation() const;
};

string_vector linear_vmax_from_leaf_n::get_inputs()
{
    return {
        "LeafN",                 //
        "LeafN_0",               //
        "Vcmax_at_25_n_offset",  // micromol / m^2 / s
        "Vcmax_at_25_n_slope"    //
    };
}

string_vector linear_vmax_from_leaf_n::get_outputs()
{
    return {
        "Vcmax_at_25"  // micromol / m^2 / s
    };
}

void linear_vmax_from_leaf_n::do_operation() const
{
    // Update the output quantity list
    update(Vcmax_at_25_op, (LeafN_0 - LeafN) * Vcmax_at_25_n_slope + Vcmax_at_25_n_offset);
}

}  // namespace standardBML
#endif
