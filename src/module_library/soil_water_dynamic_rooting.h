#ifndef SOIL_WATER_DYNAMIC_ROOTING_H
#define SOIL_WATER_DYNAMIC_ROOTING_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class soil_water_dynamic_rooting
 *
 *  @brief Allows max_rooting_layer to change over time
 *
 */
class soil_water_dynamic_rooting : public direct_module
{
   public:
    soil_water_dynamic_rooting(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          DVI{get_input(input_quantities, "DVI")},

          // Get pointers to output quantities
          max_rooting_layer_op{get_op(output_quantities, "max_rooting_layer")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_water_dynamic_rooting"; }

   private:
    // Pointers to input quantities
    double const& DVI;

    // Pointers to output quantities
    double* max_rooting_layer_op;

    // Main operation
    void do_operation() const;
};

string_vector soil_water_dynamic_rooting::get_inputs()
{
    return {
        "DVI"  // dimensionless
    };
}

string_vector soil_water_dynamic_rooting::get_outputs()
{
    return {
        "max_rooting_layer"  //
    };
}

void soil_water_dynamic_rooting::do_operation() const
{
    double max_rooting_layer{};
    if (DVI < 1.0) {
        max_rooting_layer = 3;
    } else if (DVI < 1.5) {
        max_rooting_layer = 4;
    } else {
        max_rooting_layer = 5;
    }
    update(max_rooting_layer_op, max_rooting_layer);
}

}  // namespace standardBML
#endif
