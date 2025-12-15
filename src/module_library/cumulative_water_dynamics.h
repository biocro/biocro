#ifndef STANDARDBML_CUMULATIVE_WATER_DYNAMICS_H
#define STANDARDBML_CUMULATIVE_WATER_DYNAMICS_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class cumulative_water_dynamics
 *
 * @brief Enables calculations of cumulative water dynamics.
 *
 * Cumulative water dynamics will be included in the simulation output as
 * differential quantities called:
 * - ``'canopy_transpiration'``: The cumulative water lost through canopy transpiration
 * - ``'soil_evaporation'``: The cumulative water lost through evaporation from the soil surface
 * - ``'total_precip'``: The cumulative water gained by the soil from precipitation
 */
class cumulative_water_dynamics : public differential_module
{
   public:
    cumulative_water_dynamics(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")},
          precip{get_input(input_quantities, "precip")},
          soil_evaporation_rate{get_input(input_quantities, "soil_evaporation_rate")},

          // Get pointers to output quantities
          canopy_transpiration_op{get_op(output_quantities, "canopy_transpiration")},
          soil_evaporation_op{get_op(output_quantities, "soil_evaporation")},
          total_precip_op{get_op(output_quantities, "total_precip")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "cumulative_water_dynamics"; }

   private:
    // References to input quantities
    double const& canopy_transpiration_rate;
    double const& precip;
    double const& soil_evaporation_rate;

    // Pointers to output quantities
    double* canopy_transpiration_op;
    double* soil_evaporation_op;
    double* total_precip_op;

    // Main operation
    void do_operation() const;
};

string_vector cumulative_water_dynamics::get_inputs()
{
    return {
        "canopy_transpiration_rate",  // Mg / ha / hr
        "precip",                     // mm / hr
        "soil_evaporation_rate"       // Mg / ha / hr
    };
}

string_vector cumulative_water_dynamics::get_outputs()
{
    return {
        "canopy_transpiration",  // Mg / ha / hr
        "soil_evaporation",      // Mg / ha / hr
        "total_precip",          // Mg / ha / hr
    };
}

void cumulative_water_dynamics::do_operation() const
{
    // We will need to convert the rainfall rate from mm / hr to Mg / ha / hr.
    // Across 1 ha, 1 mm of rain has a total volume of 10 m^3. We will assume a
    // density of 1 g / mL, so this amount of rain has a mass of 10 Mg. In other
    // words, 1 mm / hr of rainfall is equivalent to 10 Mg / ha / hr.

    // Use `update` to set outputs
    update(canopy_transpiration_op, canopy_transpiration_rate);
    update(soil_evaporation_op, soil_evaporation_rate);
    update(total_precip_op, precip * 10);
}

}  // namespace standardBML
#endif
