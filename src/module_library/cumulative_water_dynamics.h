#ifndef STANDARDBML_CUMULATIVE_WATER_DYNAMICS_H
#define STANDARDBML_CUMULATIVE_WATER_DYNAMICS_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class cumulative_water_dynamics
 *
 *  @brief Enables calculations of cumulative water dynamics.
 *
 *  Cumulative water dynamics will be included in the simulation output as
 *  differential quantities called:
 *  - ``'canopy_transpiration'``: The cumulative water lost through canopy
 *    transpiration
 *  - ``'soil_evaporation'``: The cumulative water lost through evaporation from
 *    the soil surface
 *  - ``'tile_flow'``: The cumulative water lost due to tile drainage
 *  - ``'drainage'``: The cumulative water lost due to drainage out of the
 *    lowest soil layer
 *  - ``'excess_water'``: The cumulative water lost due to excess infiltration
 *    that was not absorbed by the soil
 *  - ``'irrigation'``: The cumulative water available from irrigation
 *  - ``'total_precip'``: The cumulative water available from precipitation
 *  - ``'surface_runoff'``: The cumulative water lost due to surface runoff
 *  - ``'unmet_demand'``: The cumulative water that was transpired even
 *    though it was not actually extractable from the soil
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
          drainage_rate{get_input(input_quantities, "drainage_rate")},
          excess_water_rate{get_input(input_quantities, "excess_water_rate")},
          irrigation_rate{get_input(input_quantities, "irrigation_rate")},
          precip{get_input(input_quantities, "precip")},
          soil_evaporation_rate{get_input(input_quantities, "soil_evaporation_rate")},
          surface_runoff_rate{get_input(input_quantities, "surface_runoff_rate")},
          tile_flow_rate{get_input(input_quantities, "tile_flow_rate")},
          unmet_demand_rate{get_input(input_quantities, "unmet_demand_rate")},

          // Get pointers to output quantities
          canopy_transpiration_op{get_op(output_quantities, "canopy_transpiration")},
          drainage_op{get_op(output_quantities, "drainage")},
          excess_water_op{get_op(output_quantities, "excess_water")},
          irrigation_op{get_op(output_quantities, "irrigation")},
          soil_evaporation_op{get_op(output_quantities, "soil_evaporation")},
          surface_runoff_op{get_op(output_quantities, "surface_runoff")},
          tile_flow_op{get_op(output_quantities, "tile_flow")},
          total_precip_op{get_op(output_quantities, "total_precip")},
          unmet_demand_op{get_op(output_quantities, "unmet_demand")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "cumulative_water_dynamics"; }

   private:
    // References to input quantities
    double const& canopy_transpiration_rate;
    double const& drainage_rate;
    double const& excess_water_rate;
    double const& irrigation_rate;
    double const& precip;
    double const& soil_evaporation_rate;
    double const& surface_runoff_rate;
    double const& tile_flow_rate;
    double const& unmet_demand_rate;

    // Pointers to output quantities
    double* canopy_transpiration_op;
    double* drainage_op;
    double* excess_water_op;
    double* irrigation_op;
    double* soil_evaporation_op;
    double* surface_runoff_op;
    double* tile_flow_op;
    double* total_precip_op;
    double* unmet_demand_op;

    // Main operation
    void do_operation() const;
};

string_vector cumulative_water_dynamics::get_inputs()
{
    return {
        "canopy_transpiration_rate",  // Mg / ha / hr
        "drainage_rate",              // Mg / ha / hr
        "excess_water_rate",          // Mg / ha / hr
        "irrigation_rate",            // Mg / ha / hr
        "precip",                     // mm / hr
        "soil_evaporation_rate",      // Mg / ha / hr
        "surface_runoff_rate",        // Mg / ha / hr
        "tile_flow_rate",             // Mg / ha / hr
        "unmet_demand_rate"           // Mg / ha / hr

    };
}

string_vector cumulative_water_dynamics::get_outputs()
{
    return {
        "canopy_transpiration",  // Mg / ha / hr
        "drainage",              // Mg / ha / hr
        "excess_water",          // Mg / ha / hr
        "irrigation",            // Mg / ha / hr
        "soil_evaporation",      // Mg / ha / hr
        "surface_runoff",        // Mg / ha / hr
        "tile_flow",             // Mg / ha / hr
        "total_precip",          // Mg / ha / hr
        "unmet_demand"           // Mg / ha / hr
    };
}

void cumulative_water_dynamics::do_operation() const
{
    // Define unit conversion constants
    double constexpr mm_to_Mg_per_ha = 10;  // (Mg / ha) / mm

    // Use `update` to set outputs
    update(canopy_transpiration_op, canopy_transpiration_rate);
    update(drainage_op, drainage_rate);
    update(excess_water_op, excess_water_rate);
    update(soil_evaporation_op, soil_evaporation_rate);
    update(surface_runoff_op, surface_runoff_rate);
    update(tile_flow_op, tile_flow_rate);
    update(irrigation_op, irrigation_rate);
    update(total_precip_op, precip * mm_to_Mg_per_ha);
    update(unmet_demand_op, unmet_demand_rate);
}

}  // namespace standardBML
#endif
