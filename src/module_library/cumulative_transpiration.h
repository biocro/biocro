#ifndef STANDARDBML_CUMULATIVE_TRANSPIRATION_H
#define STANDARDBML_CUMULATIVE_TRANSPIRATION_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class cumulative_transpiration
 *
 * @brief Enables calculations of cumulative transpiration, which will be
 * included in the simulation output as a differential quantity called
 * ``'canopy_transpiration'``.
 *
 */
class cumulative_transpiration : public differential_module
{
   public:
    cumulative_transpiration(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")},

          // Get pointers to output quantities
          canopy_transpiration_op{get_op(output_quantities, "canopy_transpiration")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "cumulative_transpiration"; }

   private:
    // References to input quantities
    double const& canopy_transpiration_rate;

    // Pointers to output quantities
    double* canopy_transpiration_op;

    // Main operation
    void do_operation() const;
};

string_vector cumulative_transpiration::get_inputs()
{
    return {
        "canopy_transpiration_rate"  // Mg / ha / hr
    };
}

string_vector cumulative_transpiration::get_outputs()
{
    return {
        "canopy_transpiration"  // Mg / ha / hr
    };
}

void cumulative_transpiration::do_operation() const
{
    update(canopy_transpiration_op, canopy_transpiration_rate);
}

}  // namespace standardBML
#endif
