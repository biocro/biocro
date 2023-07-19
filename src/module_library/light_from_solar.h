#ifndef LIGHT_FROM_SOLAR_H
#define LIGHT_FROM_SOLAR_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class light_from_solar : public direct_module
{
   public:
    light_from_solar(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          solar_ip{get_ip(input_quantities, "solar")},
          light_threshold_ip{get_ip(input_quantities, "light_threshold")},
          light_exp_at_zero_ip{get_ip(input_quantities, "light_exp_at_zero")},

          // Get pointers to output quantities
          light_op{get_op(output_quantities, "light")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "light_from_solar"; }

   private:
    // Pointers to input quantities
    const double* solar_ip;
    const double* light_threshold_ip;
    const double* light_exp_at_zero_ip;

    // Pointers to output quantities
    double* light_op;

    // Main operation
    void do_operation() const;
};

string_vector light_from_solar::get_inputs()
{
    return {
        "solar",             //
        "light_threshold",   //
        "light_exp_at_zero"  //
    };
}

string_vector light_from_solar::get_outputs()
{
    return {
        "light"  //
    };
}

void light_from_solar::do_operation() const
{
    //////////////////////////////////////////
    // Collect inputs and make calculations //
    //////////////////////////////////////////

    // Get the current level of solar radiation
    double solar = *solar_ip;

    // Get the light threshold
    double light_threshold = *light_threshold_ip;

    // Get the light exponent at zero
    double light_exp_at_zero = *light_exp_at_zero_ip;

    // Check whether the plant is illuminated
    // Rather than simply using a step function (i.e., light = solar > 0),
    //  this logistic function smoothly turns on as the solar radiation
    //  increases
    // Coefficients have been chosen so that light:
    //  (1) roughly equals exp(-light_exp_at_zero) for solar = 0
    //  (2) roughly equals 1 - exp(-light_exp_at_zero) for solar = light_threshold
    //  (3) equals 1/2 for solar = light_threshold/2
    double light = 1.0 / (1.0 + exp(-2.0 * light_exp_at_zero * (solar - 0.5 * light_threshold) / light_threshold));

    //////////////////////////////////////
    // Update the output quantity list //
    //////////////////////////////////////

    update(light_op, light);
}

}  // namespace standardBML
#endif
