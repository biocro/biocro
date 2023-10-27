#ifndef GRIMM_SOYBEAN_FLOWERING_H
#define GRIMM_SOYBEAN_FLOWERING_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class grimm_soybean_flowering
 *
 *  @brief Model for soybean development and flowering based on Grimm et al.
 *  (1993).
 *
 *  See `grimm_soybean_flowering_calculator` for details.
 */
class grimm_soybean_flowering : public differential_module
{
   public:
    grimm_soybean_flowering(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          grimm_rate{get_input(input_quantities, "grimm_rate")},

          // Get pointers to output quantities
          grimm_physiological_age_op{get_op(output_quantities, "grimm_physiological_age")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "grimm_soybean_flowering"; }

   private:
    // References to input quantities
    double const& grimm_rate;

    // Pointers to output quantities
    double* grimm_physiological_age_op;

    // Main operation
    void do_operation() const;
};

string_vector grimm_soybean_flowering::get_inputs()
{
    return {
        "grimm_rate"  // physiological days / hr
    };
}

string_vector grimm_soybean_flowering::get_outputs()
{
    return {
        "grimm_physiological_age"  // physiological days / hr
    };
}

void grimm_soybean_flowering::do_operation() const
{
    // Update the output quantity list
    update(grimm_physiological_age_op, grimm_rate);
}

}  // namespace standardBML
#endif
