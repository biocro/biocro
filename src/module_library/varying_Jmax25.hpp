#ifndef VARYING_JMAX25_H
#define VARYING_JMAX25_H
#include "../framework/module.h"
#include "../framework/state_map.h"

// Added by Yufeng He. 02-13-2023

namespace standardBML
{
class varying_Jmax25 : public direct_module
{
   public:
    varying_Jmax25(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities
          DVI{get_input(input_quantities, "DVI")},
          jmax_mature{get_input(input_quantities, "jmax_mature")},
          sf_jmax{get_input(input_quantities, "sf_jmax")},

          // Get pointers to output quantities
          jmax_op{get_op(output_quantities, "jmax")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "varying_Jmax25"; }

   private:
    // Pointers to input quantities
    double const& DVI;
    double const& jmax_mature;
    double const& sf_jmax;

    // Pointers to output quantities
    double* jmax_op;

    // Main operation
    void do_operation() const;
};

string_vector varying_Jmax25::get_inputs()
{
    return {
        "DVI",         // dimensionless
        "sf_jmax",     // dimensionless. A constant scaling factor for jmax
        "jmax_mature"  // micromol/m^2/s.
    };
}

string_vector varying_Jmax25::get_outputs()
{
    return {
        "jmax"         // micromol/m^2/s
    };
}

void varying_Jmax25::do_operation() const
{
    // I use 0.6 here based on Ed's 2021 measurement.
    // The general rule is to reduce jmax for the emergence period, but
    // before the vegetative period.
    // This works in my optimization tests, but it's open to more tests.
    // The sf_jmax is a scaling factor for jmax_mature.
    double DVI_threshold = 0.6; 
    double jmax_tmp;
    if (DVI < DVI_threshold) {
        jmax_tmp = jmax_mature * sf_jmax;
    } else {
        jmax_tmp = jmax_mature;
    }
    // Update the output quantity list
    update(jmax_op, jmax_tmp);
}

}  // namespace standardBML
#endif
