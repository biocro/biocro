#include <algorithm>  // for std::min, std::max
#include <cmath>
#include <Rinternals.h>
#include "soil_water_flow_functions.h"
using namespace std;
/**
 * @brief Calculates soil infiltration for a multi-layer soil profile.
 */
infilWater_str infil(
    int nlayers,
    double potential_infiltration,
    double swcon,
    double soil_depth[],
    double soil_saturation_capacity[],
    double soil_field_capacity[],
    double soil_water_content[],
    double soil_saturated_conductivity[]) // Saturated hydraulic conductivity in layer L (cm/hr)
{
    infilWater_str return_value;
    double drn[nlayers];      // Drainage rate through soil layer L (cm/hr)
    double swdelts[nlayers];  // Change in soil water content due to drainage in layer L
    double drainage_rate;     // Drainage rate from soil profile (cm/hr)

    double swtemp[nlayers];  // = soil_water_content;
    for (int l = 0; l < nlayers; l++){
        drn[l] = 0.0;
        swdelts[l] = 0.0;
        swtemp[l] = soil_water_content[l];
    }
    double excess = 0.0;  // Excess water to be added to runoff (cm/hr)
    double tmpexcess = 0.0;
    for (int l = 0; l < nlayers; l++) {
        double hold = (soil_saturation_capacity[l] - swtemp[l]) * soil_depth[l];

        // Rprintf("the value of hold is : %f \n", hold);
        // Rprintf("the value of potential_infiltration is : %f \n", potential_infiltration);

        if (potential_infiltration > 0.0001 && potential_infiltration > hold) {
            // Rprintf("PINF is greater than 0.0001, and holding capacity. \n");
            if (l == 0) {
                // reduce SWCON in top layer to allow for increased evaporation for wet soils
                drainage_rate = 0.9 * swcon * (soil_saturation_capacity[l] - 
                    soil_field_capacity[l]) * soil_depth[l]; // cm/hr
            } else {
                drainage_rate = swcon * (soil_saturation_capacity[l] - 
                    soil_field_capacity[l]) * soil_depth[l];
            }
            // Rprintf("drainage_rate: %f \n", drainage_rate);
            drn[l] = potential_infiltration - hold + drainage_rate;  // cm/hr
            // Rprintf("the value of drn[%i]: %f \n", l, drn[l]);
            // Rprintf("the value of HOLD is %f \n", hold);
            // Rprintf("the value of PINF is %f \n", potential_infiltration);
            if (soil_saturated_conductivity[l] > 0.0 && drn[l] > soil_saturated_conductivity[l]) {
                // Rprintf("drn%i > soil_saturated_conductivity_%i \n", l,l);
                drn[l] = soil_saturated_conductivity[l];
                drainage_rate = drn[l] + hold - potential_infiltration;
            }

            swtemp[l] = swtemp[l] + (potential_infiltration - drn[l]) / soil_depth[l];
            // Rprintf("swtemp[%i] is: %f \n", l,swtemp[l]);
            if (swtemp[l] > soil_saturation_capacity[l]) {
                // Rprintf("swtemp%i > soil_saturation_capacity_%i \n", l,l);
                tmpexcess = (swtemp[l] - soil_saturation_capacity[l]) * soil_depth[l]; //cm/hr
                swtemp[l] = soil_saturation_capacity[l];
                if (l == 0 && tmpexcess > 0.0) {
                    excess = excess + tmpexcess;
                }
                // If there is excess water, redistribute it in layers above.
                if (l > 0) {
                    for (int lk = l - 1; lk >= 0; lk--) {
                        // Rprintf("tmpexcess is %f \n", tmpexcess);
                        if (tmpexcess < 0.0001) break;
                        hold = std::min((soil_saturation_capacity[lk] - swtemp[lk]) * 
                                soil_depth[lk], tmpexcess);
                        swtemp[lk] = swtemp[lk] + hold / soil_depth[lk];
                        // Rprintf("swtemp[%i] is %f \n", lk,swtemp[lk]);
                        drn[lk] = std::max((drn[lk] - tmpexcess), 0.0);
                        // Rprintf("drn[%i] is %f \n", lk,drn[lk]);
                        tmpexcess = tmpexcess - hold;
                        if (lk == 0 && tmpexcess > 0.0) {
                            excess = excess + tmpexcess;
                        }
                    }
                }
            }
            potential_infiltration = drn[l];

        } else {
            // Rprintf("PINF is less than 0.0001. \n");
            swtemp[l] = swtemp[l] + potential_infiltration / soil_depth[l];

            if (swtemp[l] >= (soil_field_capacity[l] + 0.003)) {
                if (l == 0) {
                    drainage_rate = 0.9 * (swtemp[l] - soil_field_capacity[l]) * 
                    swcon * soil_depth[l];
                } else {
                    drainage_rate = (swtemp[l] - soil_field_capacity[l]) * swcon * 
                    soil_depth[l];
                }
                drn[l] = drainage_rate;
                if (soil_saturated_conductivity[l] > 0.0 && drn[l] > soil_saturated_conductivity[l]) {
                    drn[l] = soil_saturated_conductivity[l];
                    drainage_rate = drn[l];
                }
                swtemp[l] = swtemp[l] - drainage_rate / soil_depth[l];
                potential_infiltration = drainage_rate;
            } else {
                potential_infiltration = 0.0;
                drn[l] = 0.0;
            }
        }
    }
    double drain = potential_infiltration * 10.0;  // in mm
    for (int l = 0; l < nlayers; l++) {
        swdelts[l] = swtemp[l] - soil_water_content[l];
    }
    return_value.drain = drain;
    return_value.excess_water = excess;

    for (int l = 0; l < nlayers; l++) {
        return_value.drn[l] = drn[l];
        return_value.sw_delta_S[l] = swdelts[l];
    }

    return return_value;
}

/**
 * @brief Calculates saturated water flow through a multi-layer soil profile.
 */
infilWater_str satflo(
    int nlayers,
    double potential_infiltration,
    double swcon,
    double soil_depth[],
    double soil_saturation_capacity[],
    double soil_field_capacity[],
    double soil_water_content[],
    double soil_saturated_conductivity[])
{
    infilWater_str return_value;
    double drn[nlayers];      // Drainage rate through soil layer L (cm/hr)
    double swdelts[nlayers];  // Change in soil water content due to drainage in layer l
    // double drainage_rate = 0.0; // Drainage rate from soil profile (cm/hr)

    double swtemp[nlayers];  //Soil water content in layer L (temporary value)
    for (int l = 0; l < nlayers; l++){
        drn[l] = 0.0;
        swdelts[l] = 0.0;
        swtemp[l] = soil_water_content[l];
    }

    double drmx[nlayers];  // Calculated maximum saturated flow from layer l (cm/hr)

    // double excess = 0.0; //Excess water to be added to runoff (cm/hr)
    for (int l = 0; l < nlayers; l++) {
        drn[l] = 0.0;
        drmx[l] = 0.0;
        // double hold = (soil_saturation_capacity[l] - swtemp[l]) * soil_depth[l];
        if (swtemp[l] >= soil_field_capacity[l] + 0.003) {
            // Rprintf("swtemp[%i] > soil_field_capacity_%i. \n", l,l);
            drmx[l] = (swtemp[l] - soil_field_capacity[l]) * swcon * soil_depth[l];
            drmx[l] = std::max(0.0, drmx[l]);
        }
        if (l == 0) {
            drn[l] = drmx[l];
        } else {
            double hold = 0.0;
            if (swtemp[l] < soil_field_capacity[l]) {
                hold = (soil_field_capacity[l] - swtemp[l]) * soil_depth[l];
            }
            drn[l] = std::max((drn[l - 1] + drmx[l] - hold), 0.0);
        }
        // Rprintf("drn[%i]: %f \n", l, drn[l]);
        if (soil_saturated_conductivity[l] > 0.0 && 
            drn[l] > soil_saturated_conductivity[l]) {
                drn[l] = soil_saturated_conductivity[l];
        }
    }
    // Compute volumetric water contents after drainage in a day.
    // Prevent water content in any layer from exceeding saturation
    // as water drains down in the profile.

    for (int l = nlayers - 1; l >= 1; l--) {
        double soil_water_old = swtemp[l];
        swtemp[l] = swtemp[l] + (drn[l - 1] - drn[l]) / soil_depth[l];
        if (swtemp[l] > soil_saturation_capacity[l]) {
            drn[l - 1] = std::max(0.0, ((soil_saturation_capacity[l] - soil_water_old) * 
                soil_depth[l] + drn[l]));
            swtemp[l] = soil_saturation_capacity[l];
        }
    }
    swtemp[0] = swtemp[0] - drn[0] / soil_depth[0];
    double drain = drn[nlayers-1] * 10.0; // in mm

    for (int l = 0; l < nlayers; l++) {
        swdelts[l] = swtemp[l] - soil_water_content[l];
    }

    return_value.drain = drain;
    return_value.excess_water = 0.0;

    for (int l = 0; l < nlayers; l++) {
        return_value.drn[l] = drn[l];
        return_value.sw_delta_S[l] = swdelts[l];
    }

    return return_value;
}


upwardFlo_str up_flow(
    int nlayers, 
    double sw_avail[],
    double soil_depth[], 
    double soil_saturation_capacity[], 
    double soil_wilting_point[],
    double soil_field_capacity[], 
    double soil_water_content[])
{
    // SW_AVAIL(L) Soil water content in layer L available for evaporation,
    //            plant extraction, or movement through soil
    upwardFlo_str return_value;
    int ist; //Beginning soil layer for upward flow calculations (=1 for
             // layers 1 through 5, =2 for lower layers)
    double upflow[nlayers];
    double swdeltU[nlayers];
    double swtemp[nlayers]; //Soil water content in layer L (temporary value)

    // Calculated flow will be limited by SW_INF and SW_AVAIL
    double sw_inf[nlayers]; // Soil water content in layer L including computed upward flow (cm3 [water] / cm3 [soil])

    double esw[nlayers];
    for (int l = 0; l < nlayers; l++){
        upflow[l] = 0.0;
        swdeltU[l] = 0.0;
        swtemp[l] = soil_water_content[l]; //Soil water content in layer l (temporary value)
        sw_inf[l] = sw_avail[l];  // Includes saturated flow
        sw_avail[l] = std::max(0.0, sw_avail[l] - soil_wilting_point[l]);
        esw[l] = soil_field_capacity[l] - soil_wilting_point[l]; //Plant extractable soil water by layer (= DUL - LL)
    }

    if (soil_depth[0] >= 5.0)    // soil_depth in cm
        ist = 0;
    else
        ist = 1;

    for (int l = ist; l < nlayers - 1; l++){
        int m = l + 1;
        double soil_water_old = swtemp[l];
        double thet1 = std::min(swtemp[l] - soil_wilting_point[l], esw[l]);
        // Rprintf("New thet1 is %f \n", thet1);
        thet1 = std::max(0.0, thet1);
        // Rprintf("New thet1 is %f \n", thet1);

        double thet2 = std::min(swtemp[m] - soil_wilting_point[m], esw[m]);
        // Rprintf("thet2 is %f \n", thet2);
        thet2 = std::max(0.0, thet2);
        // Rprintf("thet2 is %f \n", thet2);

        double dbar = 0.88 * exp(35.4 * ((thet1 * soil_depth[l] + thet2 * soil_depth[m]) /
            (soil_depth[l] + soil_depth[m])) * 0.5);
        dbar = std::min(dbar, 100.0);
        // Rprintf("dbar is %f \n", dbar);

        // Rprintf("dbar is %f \n", dbar);

        double grad = (thet2/esw[m] - thet1/esw[l]) * (esw[m] * soil_depth[m] +
            esw[l] * soil_depth[l]) / (soil_depth[m] + soil_depth[l]);
        // Rprintf("grad is %f \n", grad);

       upflow[l] = dbar * grad / ((soil_depth[l] + soil_depth[m]) * 0.5); // to cm/hr
       // Rprintf("upflow is %f \n", upflow[l]);
       double flowfix; // Adjustment amount for upward flow calculations to prevent a 
       // soil layer from exceeding the saturation content (cm3/cm3)
       // Upward flow from layer M to layer L
        if (upflow[l] > 0.0) {
            // Rprintf("upflow is greater than 0.0 %f. \n", upflow[l]);
            if (swtemp[l] <= soil_field_capacity[l]) {
                swtemp[l] = swtemp[l] + upflow[l] / soil_depth[l];
                sw_inf[l] = sw_inf[l] + upflow[l] / soil_depth[l];
                if (swtemp[l] > soil_field_capacity[l] || sw_inf[l] > 
                    soil_saturation_capacity[l]) {
                    flowfix = std::max({0.0, 
                        (swtemp[l] - soil_field_capacity[l]) * soil_depth[l], 
                        (sw_inf[l] - soil_saturation_capacity[l]) * soil_depth[l]});
                    flowfix = std::min(upflow[l], flowfix);
                    // Rprintf("New flowfix is %f \n", flowfix);
                    upflow[l] = upflow[l] - flowfix;
                    swtemp[l] = soil_water_old + upflow[l] / soil_depth[l];
                }
            } else { // No upward flow if swtemp > soil_field_capacity
                upflow[l] = 0.0;
            }
            if (upflow[l] / soil_depth[m] > sw_avail[m]) {
                upflow[l] = sw_avail[m] * soil_depth[m];
                swtemp[l] = soil_water_old + upflow[l] / soil_depth[l];
            }
            swtemp[m] = swtemp[m] - upflow[l] / soil_depth[m];
        } else if (upflow[l] < 0.0) { // Downward flow from layer L to layer M
            if (swtemp[l] >= soil_wilting_point[l]) {
                if (fabs(upflow[l] / soil_depth[l]) > sw_avail[l]) {
                    upflow[l] = -sw_avail[l] * soil_depth[l];
                }

                swtemp[l] = swtemp[l] + upflow[l] / soil_depth[l];
                swtemp[m] = swtemp[m] - upflow[l] / soil_depth[m];
                sw_inf[m] = sw_inf[m] - upflow[l] / soil_depth[m];

                if (sw_inf[m] > soil_saturation_capacity[m]) {
                    flowfix = std::min(fabs(upflow[l]), (sw_inf[m] - 
                                        soil_saturation_capacity[m]) * soil_depth[m]);
                    upflow[l] = upflow[l] + flowfix;
                    swtemp[l] = soil_water_old + upflow[l] / soil_depth[l];
                    swtemp[m] = swtemp[m] - flowfix / soil_depth[m];
                }
            } else { // No downward flow if SWTEMP < LL
                upflow[l] = 0.0;
            }
        }
    }
    for (int l = 0; l < nlayers; l++){
        swdeltU[l] = swtemp[l] - soil_water_content[l];
    }

    for(int l = 0; l < nlayers; l++){
      return_value.upwardFlo[l] = upflow[l]; // Not used anymore. Can be removed
      return_value.sw_delta_U[l] = swdeltU[l];
    }
    return return_value;
}


tileDrain_str tile_flow(
    int nlayers, 
    int td_layer_num,                
    double tile_drainage_rate,          // 1/hr
    double soil_depth[],                // cm
    double soil_water_content[],        // (cm3 [water] / cm3 [soil])
    double soil_field_capacity[],       // (cm3 [water] / cm3 [soil])
    double soil_saturation_capacity[],  // (cm3 [water] / cm3 [soil])
    double sw_delta_S[])                // (cm3 [water] / cm3 [soil])
{

    tileDrain_str return_value;

    double total_tile_flow = 0.0; // TDFC
    double swdeltT[nlayers];
    double drn[nlayers];
    for (int l = 0; l < nlayers; l++){
        drn[l] = 0.0;
        swdeltT[l] = 0.0;
    }
    // Compute hydraulic head above drain defined as top-most saturated layer above the 
    // drain. All saturated layers above the drain must be continuous. Compute soil water
    // available to drain (water in saturated zone in exceedance of soil field capacity).
    double head = 0.0;      // cm
    int topsat;             // top-most saturated layer above the drain.
    double tdf_avail = 0.0; // Soil water available to drain, cm
    for (int l = td_layer_num; l >= 0; l--){

        if (soil_water_content[l] >= (0.98 * soil_saturation_capacity[l])) {
            // Rprintf("the value of SW_%i is > 0.9 * SAT_%i \n", l,l);

            head = head + soil_depth[l];
            tdf_avail = tdf_avail + (soil_water_content[l] + sw_delta_S[l] -
                            soil_field_capacity[l]) * soil_depth[l];
            topsat = l;
        }
        else {
            // Rprintf("the value of SW_%i is %f \n", l,soil_water_content[l]);
        // Total head includes saturated portion of first unsaturated layer.

            // Rprintf("the value of SW_%i is smaller than SAT_%i \n", l,l);
            head = head + ((soil_water_content[l] - soil_field_capacity[l]) / 
                          (soil_saturation_capacity[l] - soil_field_capacity[l])) * 
                          soil_depth[l];
            head = std::max(head, 0.0);
            tdf_avail = tdf_avail + (soil_water_content[l] + sw_delta_S[l] -
                          soil_field_capacity[l]) * soil_depth[l];
            topsat = l;
            break;
        }
    }

    double tile_drain_conductivity = tile_drainage_rate * head;  // Conductivity of tile drain cm/hr
    double cumulative_tile_flow = 0.0; // cm/hr

    // Drain water from tile layer if layer is saturated
    if (head > 0.0){
        cumulative_tile_flow = std::min(tile_drain_conductivity, tdf_avail);

        // Redistribute water from upper layers
        // Assume that water is limited by user-specified tile drainage rate
        // rather than by each layer's Ksat.
        double drn_total = 0.0;      // cm/hr
        double swdeltT_total = 0.0;
        double excess = 0.0;
        for (int l = topsat; l <= td_layer_num; l++){ // top saturated layer down to tiledrain 
            if (drn_total <  cumulative_tile_flow){
                // Rprintf("the value of drn_total is : %f \n", drn_total);
                // Reduce soil water from top saturated layers until tile
                // drainage capacity is met.
                swdeltT[l] = -(soil_water_content[l] + sw_delta_S[l] - 
                                soil_field_capacity[l]); 
                // Rprintf("the value of swdeltT[%i] is : %f \n", l, swdeltT[l]);
                drn_total = drn_total - swdeltT[l] *  soil_depth[l];
                drn[l] = drn[l] + drn_total;
                if (drn_total > cumulative_tile_flow) {
                    // Rprintf("the value of drn_total > cumulative_tile_flow");
                    excess = drn_total - cumulative_tile_flow;
                    swdeltT[l] = swdeltT[l] + excess / soil_depth[l];
                    drn[l] = drn[l] - excess;
                }   
            }
            else {
                // Lower soil layers will remain at saturation
                swdeltT[l] = 0.0;
                drn[l] = drn[l] + cumulative_tile_flow;
            }
            
            swdeltT_total = swdeltT_total + swdeltT[l] * soil_depth[l];
        }

        total_tile_flow = total_tile_flow + cumulative_tile_flow;  
    }
    else {
      cumulative_tile_flow = 0.0;
    }
    return_value.head = head;
    return_value.topsat = topsat;
    return_value.tile_drain_conductivity = tile_drain_conductivity;
    return_value.tdf_avail = tdf_avail;
    return_value.cumulative_tile_flow = cumulative_tile_flow;
    return_value.total_tile_flow = total_tile_flow;
    for(int l = 0; l < nlayers; l++){
      return_value.sw_delta_T[l] = swdeltT[l];
    }

    return return_value;
}
