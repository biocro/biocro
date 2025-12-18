#ifndef SOIL_WATER_FLOW_FUNCTIONS_H
#define SOIL_WATER_FLOW_FUNCTIONS_H

#include <vector>
#include "../framework/module.h"
#include "../framework/state_map.h"
static int const max_soil_layers = 10;

/**
 * @brief Structure for storing the output from `infil` and `satflo`.
 */
struct infilWater_str {
    double drain;                        // Drainage rate from soil profile (mm/hr)
    double drn[max_soil_layers];         // Drainage rate through soil layer l (cm/hr)
    double excess_water;                 // Excess water to be added to runoff (cm/hr)
    double sw_delta_S[max_soil_layers];  // Change in soil water content due to drainage in layer l
                                         // (cm3 [water] / cm3 [soil])
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


struct soil_water_profile {
    
    struct soil_layer {
        double const& depth,
        double const& water_content,
        double const& field_capacity;
        double const& saturation_capacity;
        double const& wilting_point;
        double const& saturated_conductivity;
        
        soil_layer(state_map const& input_quantities, state_map* output_quantities, int layer) :
            depth{get_input(input_quantities, "soil_depth_" + std::to_string(layer))},
            water_content{get_input(input_quantities, "soil_water_content_" + std::to_string(layer))},        
            field_capacity{get_input(input_quantities, "soil_field_capacity_" + std::to_string(layer))},
            saturation_capacity{get_input(input_quantities, "soil_saturation_capacity_" + std::to_string(layer))},
            wilting_point{get_input(input_quantities, "soil_wilting_point_" + std::to_string(layer))} {
                }    
        
    };
    
    std::vector<soil_layer> layers;
    
    soil_water_profile(state_map const& input_quantities, state_map* output_quantities)  {
        for (size_t i = 0; i < 6; ++i){
            layers.emplace_back(soil_layer(input_quantities, output_quantities, i));
        }
    }
    

    
    // formerly esw
    double extractable_water(const soil_layer& layer) const {
        return layer.field_capacity - layer.wilting_point;
    }
    
    // formerly thet1 and thet2
    // I think this should have a better name. Excess water conten?
    double theta(const soil_layer& layer) const {
        double esw = extractable_water(layer);
        double th = std::min(layer.water_content - layer.wilting_point, esw);
        return std::max(0.0, th);
    }


    double average_diffusivity(const soil_layer& layer, const soil_layer& next_layer) const {
        
        double avg_theta  = (theta(layer) + theta(next_layer))/2;
        
        constexpr double magic_num_1 = 35.4; 
        double u = magic_num_1  * avg_theta;

        constexpr double min_diffusivity = 0.88; 
        double out = min_diffusivity * std::exp(u); 
        
        constexpr double max_diffusivity = 100;
        return std::min(out, max_diffusivity); 

    }
    
    // is this a gradient ? spatial derivative?
    double grad(const soil_layer& layer, const soil_layer& next_layer) const {
             
        double a = theta(next_layer) / extractable_water(next_layer) - theta(layer) / extractable_water(layer);
        double w = layer.depth / (layer.depth + next_layer.depth);
        double weighted_extractable_water = w * extractable_water(layer) + extractable_water(next_layer) * (1 - w);
        return a * weighted_extractable_water;
    }

    double upflow(const soil_layer& layer, const soil_layer& next_layer) const {
        double avg_depth = (layer.depth + next_layer.depth) * 0.5;
        return dbar(layer, next_layer) * grad(layer, next_layer) / avg_depth;  // cm / hr    
    }

//      double flowfix;  // Adjustment amount for upward flow calculations to prevent a
//                         // soil layer from exceeding the saturation content (cm3/cm3)
//                         // Upward flow from layer M to layer L
//        if (upflow[l] > 0.0) {
//            // Rprintf("upflow is greater than 0.0 %f. \n", upflow[l]);
//            if (swtemp[l] <= soil_field_capacity[l]) {
//                swtemp[l] = swtemp[l] + upflow[l] / soil_depth[l];
//                sw_inf[l] = sw_inf[l] + upflow[l] / soil_depth[l];
//                if (swtemp[l] > soil_field_capacity[l] || sw_inf[l] >
//                                                              soil_saturation_capacity[l]) {
//                    flowfix = std::max({0.0,
//                                        (swtemp[l] - soil_field_capacity[l]) * soil_depth[l],
//                                        (sw_inf[l] - soil_saturation_capacity[l]) * soil_depth[l]});
//                    flowfix = std::min(upflow[l], flowfix);
//                    // Rprintf("New flowfix is %f \n", flowfix);
//                    upflow[l] = upflow[l] - flowfix;
//                    swtemp[l] = soil_water_old + upflow[l] / soil_depth[l];
//                }
//            } else {  // No upward flow if swtemp > soil_field_capacity
//                upflow[l] = 0.0;
//            }
//            if (upflow[l] / soil_depth[m] > sw_avail[m]) {
//                upflow[l] = sw_avail[m] * soil_depth[m];
//                swtemp[l] = soil_water_old + upflow[l] / soil_depth[l];
//            }
//            swtemp[m] = swtemp[m] - upflow[l] / soil_depth[m];
//        } else if (upflow[l] < 0.0) {  // Downward flow from layer L to layer M
//            if (swtemp[l] >= soil_wilting_point[l]) {
//                if (fabs(upflow[l] / soil_depth[l]) > sw_avail[l]) {
//                    upflow[l] = -sw_avail[l] * soil_depth[l];
//                }
//
//                swtemp[l] = swtemp[l] + upflow[l] / soil_depth[l];
//                swtemp[m] = swtemp[m] - upflow[l] / soil_depth[m];
//                sw_inf[m] = sw_inf[m] - upflow[l] / soil_depth[m];
//
//                if (sw_inf[m] > soil_saturation_capacity[m]) {
//                    flowfix = std::min(fabs(upflow[l]), (sw_inf[m] -
//                                                         soil_saturation_capacity[m]) *
//                                                            soil_depth[m]);
//                    upflow[l] = upflow[l] + flowfix;
//                    swtemp[l] = soil_water_old + upflow[l] / soil_depth[l];
//                    swtemp[m] = swtemp[m] - flowfix / soil_depth[m];
//                }
//            } else {  // No downward flow if SWTEMP < LL
//                upflow[l] = 0.0;
//            }
//        }
//    }
//    for (int l = 0; l < nlayers; l++) {
//        swdeltU[l] = swtemp[l] - soil_water_content[l];
//    }
//
//    for (int l = 0; l < nlayers; l++) {
//        return_value.upwardFlo[l] = upflow[l];  // Not used anymore. Can be removed
//        return_value.sw_delta_U[l] = swdeltU[l];
//    }
};

#endif
