#ifndef BIOMASS_LEAF_N_LIMITATION_H
#define BIOMASS_LEAF_N_LIMITATION_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "../framework/constants.h"  // For calculation_constants::eps_zero
#include <cmath>                     // For std::abs

namespace standardBML
{
class biomass_leaf_n_limitation : public direct_module
{
   public:
    biomass_leaf_n_limitation(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          LeafN_0_ip{get_ip(input_quantities, "LeafN_0")},
          Leaf_ip{get_ip(input_quantities, "Leaf")},
          Stem_ip{get_ip(input_quantities, "Stem")},
          kln_ip{get_ip(input_quantities, "kln")},

          // Get pointers to output quantities
          LeafN_op{get_op(output_quantities, "LeafN")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "biomass_leaf_n_limitation"; }

   private:
    // Pointers to input quantities
    const double* LeafN_0_ip;
    const double* Leaf_ip;
    const double* Stem_ip;
    const double* kln_ip;

    // Pointers to output quantities
    double* LeafN_op;

    // Main operation
    void do_operation() const;
};

string_vector biomass_leaf_n_limitation::get_inputs()
{
    return {
        "LeafN_0",  //
        "Leaf",     //
        "Stem",     //
        "kln"       //
    };
}

string_vector biomass_leaf_n_limitation::get_outputs()
{
    return {
        "LeafN"  //
    };
}

void biomass_leaf_n_limitation::do_operation() const
{
    // Collect inputs and make calculations
    double leaf_n;
    if (std::abs((*Leaf_ip) + (*Stem_ip)) < calculation_constants::eps_zero) {
        leaf_n = *LeafN_0_ip;
    } else {
        leaf_n = (*LeafN_0_ip) * pow((*Leaf_ip) + (*Stem_ip), -1.0 * (*kln_ip));
    }

    // Update the output quantity list
    update(LeafN_op, std::min(*LeafN_0_ip, leaf_n));
}

}  // namespace standardBML
#endif
