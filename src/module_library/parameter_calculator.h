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
          Sp{get_input(input_quantities, "Sp")},
          Leaf{get_input(input_quantities, "Leaf")},
          LeafN_0{get_input(input_quantities, "LeafN_0")},
          LeafN{get_input(input_quantities, "LeafN")},
          vmax_n_intercept{get_input(input_quantities, "vmax_n_intercept")},
          vmax1{get_input(input_quantities, "vmax1")},
          alphab1{get_input(input_quantities, "alphab1")},
          alpha1{get_input(input_quantities, "alpha1")},

          // Get pointers to output quantities
          lai_op{get_op(output_quantities, "lai")},
          vmax_op{get_op(output_quantities, "vmax")},
          alpha_op{get_op(output_quantities, "alpha")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "parameter_calculator"; }

   private:
    // References to input quantities
    double const& Sp;
    double const& Leaf;
    double const& LeafN_0;
    double const& LeafN;
    double const& vmax_n_intercept;
    double const& vmax1;
    double const& alphab1;
    double const& alpha1;

    // Pointers to output quantities
    double* lai_op;
    double* vmax_op;
    double* alpha_op;

    // Main operation
    void do_operation() const;
};

string_vector parameter_calculator::get_inputs()
{
    return {
        "Sp",                // Ha / Mg
        "Leaf",              // Mg / Ha
        "LeafN_0",           //
        "LeafN",             //
        "vmax_n_intercept",  //
        "vmax1",             // micromol / m^2 / s
        "alphab1",           //
        "alpha1"             //
    };
}

string_vector parameter_calculator::get_outputs()
{
    return {
        "lai",   // dimensionless
        "vmax",  // micromol / m^2 / s
        "alpha"  //
    };
}

void parameter_calculator::do_operation() const
{
    update(lai_op, Leaf * Sp);
    update(vmax_op, (LeafN_0 - LeafN) * vmax_n_intercept + vmax1);
    update(alpha_op, (LeafN_0 - LeafN) * alphab1 + alpha1);
}

}  // namespace standardBML
#endif
