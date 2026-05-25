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
 *  - ``'total_drainage'``: The cumulative water lost due to drainage out of the
 *    lowest soil layer
 *  - ``'total_excess_water'``: The cumulative water lost due to excess
 *    infiltration that was not absorbed by the soil
 *  - ``'total_irrigation'``: The cumulative water available from irrigation
 *  - ``'total_precip'``: The cumulative water available from precipitation
 *  - ``'total_surface_runoff'``: The cumulative water lost due to surface
 *    runoff
 *  - ``'total_unmet_demand'``: The cumulative water that was transpired even
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
          drain{get_input(input_quantities, "drain")},
          excess_water{get_input(input_quantities, "excess_water")},
          irrigation{get_input(input_quantities, "irrigation")},
          precip{get_input(input_quantities, "precip")},
          soil_evaporation_rate{get_input(input_quantities, "soil_evaporation_rate")},
          surface_runoff{get_input(input_quantities, "surface_runoff")},
          tile_flow_rate{get_input(input_quantities, "tile_flow_rate")},
          unmet_demand{get_input(input_quantities, "unmet_demand")},

          // Get pointers to output quantities
          canopy_transpiration_op{get_op(output_quantities, "canopy_transpiration")},
          soil_evaporation_op{get_op(output_quantities, "soil_evaporation")},
          tile_flow_op{get_op(output_quantities, "tile_flow")},
          total_drainage_op{get_op(output_quantities, "total_drainage")},
          total_excess_water_op{get_op(output_quantities, "total_excess_water")},
          total_irrigation_op{get_op(output_quantities, "total_irrigation")},
          total_precip_op{get_op(output_quantities, "total_precip")},
          total_surface_runoff_op{get_op(output_quantities, "total_surface_runoff")},
          total_unmet_demand_op{get_op(output_quantities, "total_unmet_demand")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "cumulative_water_dynamics"; }

   private:
    // References to input quantities
    double const& canopy_transpiration_rate;
    double const& drain;
    double const& excess_water;
    double const& irrigation;
    double const& precip;
    double const& soil_evaporation_rate;
    double const& surface_runoff;
    double const& tile_flow_rate;
    double const& unmet_demand;

    // Pointers to output quantities
    double* canopy_transpiration_op;
    double* soil_evaporation_op;
    double* tile_flow_op;
    double* total_drainage_op;
    double* total_excess_water_op;
    double* total_irrigation_op;
    double* total_precip_op;
    double* total_surface_runoff_op;
    double* total_unmet_demand_op;

    // Main operation
    void do_operation() const;
};

string_vector cumulative_water_dynamics::get_inputs()
{
    return {
        "canopy_transpiration_rate",  // Mg / ha / hr
        "drain",                      // mm / hr
        "excess_water",               // mm / hr
        "irrigation",                 // mm / hr
        "precip",                     // mm / hr
        "soil_evaporation_rate",      // Mg / ha / hr
        "surface_runoff",             // mm / hr
        "tile_flow_rate",             // cm / hr
        "unmet_demand"                // Mg / ha / hr

    };
}

string_vector cumulative_water_dynamics::get_outputs()
{
    return {
        "canopy_transpiration",  // Mg / ha / hr
        "soil_evaporation",      // Mg / ha / hr
        "tile_flow",             // Mg / ha / hr
        "total_drainage",        // Mg / ha / hr
        "total_excess_water",    // Mg / ha / hr
        "total_irrigation",      // Mg / ha / hr
        "total_precip",          // Mg / ha / hr
        "total_surface_runoff",  // Mg / ha / hr
        "total_unmet_demand"     // Mg / ha / hr
    };
}

void cumulative_water_dynamics::do_operation() const
{
    // Define unit conversion constants
    double constexpr cm_to_mm = 10;         // 10 mm / cm
    double constexpr mm_to_Mg_per_ha = 10;  // (Mg / ha) / mm

    // Use `update` to set outputs
    update(canopy_transpiration_op, canopy_transpiration_rate);
    update(soil_evaporation_op, soil_evaporation_rate);
    update(tile_flow_op, tile_flow_rate * cm_to_mm * mm_to_Mg_per_ha);
    update(total_drainage_op, drain * mm_to_Mg_per_ha);
    update(total_excess_water_op, excess_water * mm_to_Mg_per_ha);
    update(total_irrigation_op, irrigation * mm_to_Mg_per_ha);
    update(total_precip_op, precip * mm_to_Mg_per_ha);
    update(total_surface_runoff_op, surface_runoff * mm_to_Mg_per_ha);
    update(total_unmet_demand_op, unmet_demand);
}

}  // namespace standardBML
#endif
