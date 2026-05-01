#ifndef SOIL_WATER_FLOW_FUNCTIONS_H
#define SOIL_WATER_FLOW_FUNCTIONS_H

static int const max_soil_layers = 10;

/**
 * @brief Structure for storing the output from `infil` and `satflo`.
 */
struct infilWater_str {
    double overall_drainage_rate;           // mm / hr       - Drainage rate from the lowest soil layer
    double downward_flux[max_soil_layers];  // cm / hr       - Rate of downward water flow out of this layer
    double excess_water_rate;               // mm / hr       - Rate of excess water appearing at soil surface
    double sw_delta_S[max_soil_layers];     // dimensionless - Change in soil water content due to downflow
};

struct upwardFlo_str {
    double upwardFlo[max_soil_layers];   // Movement of water between unsaturated soil layers due to soil evaporation
    double sw_delta_U[max_soil_layers];  // Change in soil water content due to evaporation and/or upward
                                         // flow in layer l (cm3 [water] / cm3 [soil])
};

struct tileDrain_str {
    double head;
    double tdf_avail;
    double topsat;                       // Top saturated layer above drain
    double tile_drain_conductivity;      // Tile capacity to drain water
    double cumulative_tile_flow;         // Cumulative Tile drain flow, cm/hr
    double total_tile_flow;              // Sum of tile drain flow from beginning of model run, cm
    double sw_delta_T[max_soil_layers];  // Change in soil water content due to tile drainage
                                         // flow in layer l (cm3 [water] / cm3 [soil])
};

infilWater_str infil(
    int const nlayers,                           // not a physical quantity
    double const infiltration_rate,              // cm / hr
    double const swcon,                          // hr^(-1)
    double const soil_depth[],                   // cm
    double const soil_saturation_capacity[],     // dimensionless from (m^3 water) / (m^3 soil)
    double const soil_field_capacity[],          // dimensionless from (m^3 water) / (m^3 soil)
    double const soil_water_content[],           // dimensionless from (m^3 water) / (m^3 soil)
    double const soil_saturated_conductivity[],  // cm / hr
    double const timestep                        // hr
);

infilWater_str satflo(
    int const nlayers,                           // not a physical quantity
    double const swcon,                          // hr^(-1)
    double const soil_depth[],                   // cm
    double const soil_saturation_capacity[],     // dimensionless from (m^3 water) / (m^3 soil)
    double const soil_field_capacity[],          // dimensionless from (m^3 water) / (m^3 soil)
    double const soil_water_content[],           // dimensionless from (m^3 water) / (m^3 soil)
    double const soil_saturated_conductivity[],  // cm / hr
    double const timestep                        // hr
);

upwardFlo_str up_flow(
    int nlayers,
    double sw_avail[],
    double soil_depth[],
    double soil_saturation_capacity[],
    double soil_wilting_point[],
    double soil_field_capacity[],
    double soil_water_content[]);

tileDrain_str tile_flow(
    int nlayers,
    int td_layer_num,
    double tile_drainage_rate,
    double soil_depth[],
    double soil_water_content[],
    double soil_field_capacity[],
    double soil_saturation_capacity[],
    double sw_delta_S[]);

#endif
