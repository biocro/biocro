#ifndef SOIL_WATER_FLOW_FUNCTIONS_H
#define SOIL_WATER_FLOW_FUNCTIONS_H

static int const max_soil_layers = 10;

/**
 * @brief Structure for storing the output from `infil` and `satflo`.
 */
struct infilWater_str {
    double overall_drainage_rate;           // mm / hr   - Drainage rate from the lowest soil layer
    double downward_flux[max_soil_layers];  // cm / hr   - Rate of downward water flow out of this layer
    double excess_water_rate;               // mm / hr   - Rate of excess water appearing at soil surface
    double sw_delta_S[max_soil_layers];     // m^3 / m^3 - Change in soil water content due to downflow
};

struct upwardFlo_str {
    double upwardFlo[max_soil_layers];   // cm / hr   - Movement of water between unsaturated soil layers due to upward flow
    double sw_delta_U[max_soil_layers];  // m^3 / m^3 - Change in soil water content due to upward flow
};

struct tileDrain_str {
    double head;                         // cm
    double tdf_avail;                    // cm
    double topsat;                       // Not a physical quantity - Top saturated layer above drain
    double tile_drain_conductivity;      // cm / hr                 - Tile capacity to drain water
    double tile_flow_rate;               // cm / hr                 - Tile drain flow rate
    double sw_delta_T[max_soil_layers];  // m^3 / m^3               - Change in soil water content due to tile drainage
};

infilWater_str infil(
    int const nlayers,                           // not a physical quantity
    double const infiltration_rate,              // cm / hr
    double const swcon,                          // hr^(-1)
    double const soil_depth[],                   // cm
    double const soil_saturation_capacity[],     // m^3 / m^3
    double const soil_field_capacity[],          // m^3 / m^3
    double const soil_water_content[],           // m^3 / m^3
    double const soil_saturated_conductivity[],  // cm / hr
    double const timestep                        // hr
);

infilWater_str satflo(
    int const nlayers,                           // not a physical quantity
    double const swcon,                          // hr^(-1)
    double const soil_depth[],                   // cm
    double const soil_saturation_capacity[],     // m^3 / m^3
    double const soil_field_capacity[],          // m^3 / m^3
    double const soil_water_content[],           // m^3 / m^3
    double const soil_saturated_conductivity[],  // cm / hr
    double const timestep                        // hr
);

upwardFlo_str up_flow(
    int const nlayers,                        // not a physical quantity
    double const swc_plus_sat[],              // m^3 / m^3
    double const soil_depth[],                // cm
    double const soil_saturation_capacity[],  // m^3 / m^3
    double const soil_wilting_point[],        // m^3 / m^3
    double const soil_field_capacity[],       // m^3 / m^3
    double const soil_water_content[],        // m^3 / m^3
    double const timestep                     // hr
);

tileDrain_str tile_flow(
    int const nlayers,                        // not a physical quantity
    int const td_layer_num,                   // not a physical quantity
    double const tile_drainage_rate,          // 1 / hr
    double const soil_depth[],                // cm
    double const soil_water_content[],        // m^3 / m^3
    double const soil_field_capacity[],       // m^3 / m^3
    double const soil_saturation_capacity[],  // m^3 / m^3
    double const sw_delta_S[],                // m^3 / m^3
    double const timestep                     // hr
);

#endif
