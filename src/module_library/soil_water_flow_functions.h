#ifndef SOIL_WATER_FLOW_FUNCTIONS_H
#define SOIL_WATER_FLOW_FUNCTIONS_H

static int const max_soil_layers = 10;

/**
 * @brief Structure for storing the output from `infil` and `satflo`.
 */
struct infilWater_str {
    double drain;         // Drainage rate from soil profile (mm/hr)
    double drn[max_soil_layers];         // Drainage rate through soil layer l (cm/hr)
    double excess_water;  // Excess water to be added to runoff (cm/hr)
    double sw_delta_S[max_soil_layers];  // Change in soil water content due to drainage in layer l
                          // (cm3 [water] / cm3 [soil])
};

struct upwardFlo_str {
  double upwardFlo[max_soil_layers];     // Movement of water between unsaturated soil layers due to soil evaporation
  double sw_delta_U[max_soil_layers];    // Change in soil water content due to evaporation and/or upward
                                         // flow in layer l (cm3 [water] / cm3 [soil])
};

struct tileDrain_str {
    double head;
    double tdf_avail;
    double topsat;                         // Top saturated layer above drain
    double tile_drain_conductivity;        // Tile capacity to drain water
    double cumulative_tile_flow;           // Cumulative Tile drain flow, cm/hr
    double total_tile_flow;                // Sum of tile drain flow from beginning of model run, cm
    double sw_delta_T[max_soil_layers];    // Change in soil water content due to tile drainage
                                           // flow in layer l (cm3 [water] / cm3 [soil])
};

infilWater_str infil(
    int nlayers,
    double potential_infiltration,
    double swcon,
    double soil_depth[],
    double soil_saturation_capacity[],
    double soil_field_capacity[],
    double soil_water_content[],
    double soil_saturated_conductivity[]);

infilWater_str satflo(
    int nlayers,
    double potential_infiltration,
    double swcon,
    double soil_depth[],
    double soil_saturation_capacity[],
    double soil_field_capacity[],
    double soil_water_content[],
    double soil_saturated_conductivity[]);

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
