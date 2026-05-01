#include <algorithm>  // for std::min, std::max
#include <cmath>      // for exp, fabs
#include "soil_water_flow_functions.h"

/**
 *  @brief Calculates soil water infiltration for a multi-layer soil profile.
 *
 *  ### Model overview
 *
 *  This module is based on a model for soil water infiltration originally
 *  presented in Ritchie (1998).
 *
 *  This model determines the total downward flux of water flowing out of each
 *  soil layer. The downward flux has two components, infiltration and drainage.
 *  Infiltration is due to water from upper layers flowing downward; some of the
 *  potential infiltration may be absorbed by the soil layer (increasing its
 *  water content) or passed on to lower layers. Drainage is downward flow that
 *  would occur even in the absence of infiltration. Because the layers in the
 *  soil profile are linked together, the total downward flux out of one layer
 *  becomes the potential infiltration of the layer below it.
 *
 *  The overall rationale for this model is explained in the beginning of the
 *  section of Ritchie (1998) titled "drainage":
 *
 *  > For soil water redistribution during infiltration water is moved downward
 *  > from the top soil layer to lower layers in a cascading approach. Drainage
 *  > from a layer takes place only when the soil water content is between field
 *  > saturation and the drained upper limit.
 *
 *  This requires a layer-by-layer approach. At the end, there may be drainage
 *  out of the bottom layer:
 *
 *  > After calculating the water movement through all soil layers, drainage
 *  > from the bottom layer of the profile... is converted to mm and set equal
 *  > to `DRAIN`. DRAIN then represents the total outflow from the lowest layer
 *  > of the soil profile and is an available output variable for those
 *  > interested in the time course of drainage out of the soil profile.
 *
 *  It is also possible that the soil profile cannot accommodate all potential
 *  infiltration. Any water that cannot be absorbed by the soil is considered
 *  "excess" water that contributes to runoff.
 *
 *  ### Model implementation
 *
 *  Ritchie (1998) provides equations that can be used to implement the model.
 *  Rather than basing our code directly on this resource, we have also looked
 *  to DSSAT, and our code is largely based on its INFIL subroutine.
 *
 *  The DSSAT submodule describes these calculations as follows:
 *
 *  > Infiltration is distributed according to the holding capacity of each
 *  > horizon (SAT(L)-DUL(L)). If the soil water content of a layer reaches
 *  > saturation, the excess amount of infiltration is added to the next soil
 *  > layer(s), depending on their water holding capacity and actual soil water
 *  > content. Then saturated flow is calculated.
 *
 *  ### Source
 *
 *  - [Ritchie, J. T. "Soil water balance and plant water stress" in "Understanding Options for Agricultural Production"
 *    (eds Tsuji, G. Y., Hoogenboom, G. & Thornton, P. K.) 41–54 (Springer Netherlands, Dordrecht, 1998)]
 *    (https://doi.org/10.1007/978-94-017-3624-4_3)
 *
 *  - DSSAT Fortran source code:
 *    https://github.com/DSSAT/dssat-csm-os/blob/develop/Soil/SoilWater/INFIL.for
 */
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
)
{
    // Hard-coded constants
    double constexpr eps_sfc = 0.003;   // m^3 / m^3 - small threshold value of soil field capacity
    double constexpr eps_sw = 0.0001;   // cm - small threshold value of soil water
    double constexpr mm_per_cm = 10.0;  // mm / cm
    double constexpr swconrf = 0.9;     // dimensionless - swcon reduction factor

    // Initialize layer-dependent variables
    double downward_flux[nlayers];  // cm / hr   - Total downward water flux (drainage and infiltration)
    double swtemp[nlayers];         // m^3 / m^3 - Soil water content

    for (int l = 0; l < nlayers; l++) {
        downward_flux[l] = 0.0;             // cm / hr
        swtemp[l] = soil_water_content[l];  // m^3 / m^3
    }

    // Initialize non-layer-dependent variables
    double excess = 0.0;                                           // cm
    double potential_infiltration = infiltration_rate * timestep;  // cm

    // For each layer, determine the downward flux and the new soil water
    // content
    for (int l = 0; l < nlayers; l++) {
        // Initialize the drainage rate that would occur in the absence of any
        // infiltration
        double drainage_rate = 0.0;  // cm / hr

        // The holding capacity of the soil layer is the amount of water it
        // could absorb before reaching its saturation capacity
        double hold = (soil_saturation_capacity[l] - swtemp[l]) * soil_depth[l];  // cm

        if (potential_infiltration > eps_sw && potential_infiltration > hold) {
            // The potential infiltration that could occur during the next time
            // step exceeds the holding capacity of the soil layer.
            //
            // In this case, the water content in this layer is increased up to
            // its saturation capacity, and water in excess of the holding
            // capacity is passed directly to the layer below.

            // Calculate the drainage rate that would occur when the soil layer
            // is fully saturated, in the absence of any infiltration
            drainage_rate = swcon * (soil_saturation_capacity[l] - soil_field_capacity[l]) * soil_depth[l];  // cm / hr

            // Reduce the soil water conductivity constant in the top layer to
            // allow increased evaporation from wet soil surfaces
            if (l == 0) {
                drainage_rate = swconrf * drainage_rate;  // cm / hr
            }

            // Infiltrated water in excess of the holding capacity is drained
            // from this layer and passed directly to the layer below, adding
            // to the drainage that would otherwise occur
            downward_flux[l] = (potential_infiltration - hold) / timestep + drainage_rate;  // cm / hr

            // The total flux out of the soil layer cannot exceed the saturated
            // soil conductivity
            if (soil_saturated_conductivity[l] > 0.0 && downward_flux[l] > soil_saturated_conductivity[l]) {
                // Adjust the total flux and drainage rate to avoid going over
                // the upper flux limit
                downward_flux[l] = soil_saturated_conductivity[l];                              // cm / hr
                drainage_rate = downward_flux[l] - (potential_infiltration - hold) / timestep;  // cm / hr
            }

            // Adjust the soil water content to account for water gained by
            // infiltration and lost through flux to lower soil layers
            swtemp[l] = swtemp[l] + (potential_infiltration - downward_flux[l] * timestep) / soil_depth[l];  // m^3 / m^3

            // The soil water content cannot exceed the saturation capacity
            if (swtemp[l] > soil_saturation_capacity[l]) {
                // Restrict the soil water content and determine the amount of
                // excess water
                double tmpexcess = (swtemp[l] - soil_saturation_capacity[l]) * soil_depth[l];  // cm
                swtemp[l] = soil_saturation_capacity[l];                                       // m^3 / m^3

                // If there is excess water in the top soil layer, it cannot be
                // redistributed
                if (l == 0 && tmpexcess > 0.0) {
                    excess = excess + tmpexcess;  // cm
                }

                // If there is excess water in any other soil layer,
                // redistribute it to the layers above
                if (l > 0) {
                    for (int lk = l - 1; lk >= 0; lk--) {
                        if (tmpexcess < eps_sw) {
                            // There is no more excess water to redistribute, so
                            // we are done
                            break;
                        }

                        // This layer can absorb either its holding capacity or
                        // the remaining excess water, whichever is smaller
                        hold = std::min(
                            (soil_saturation_capacity[lk] - swtemp[lk]) * soil_depth[lk],
                            tmpexcess);  // cm

                        // Adjust water content to account for excess water
                        // redistribution
                        swtemp[lk] = swtemp[lk] + hold / soil_depth[lk];  // m^3 / m^3

                        // Adjust flux from this layer to account for excess
                        // water redistribution
                        downward_flux[lk] = std::max((downward_flux[lk] - tmpexcess / timestep), 0.0);  // cm / hr

                        // Get the remaining excess water
                        tmpexcess = tmpexcess - hold;

                        // If this is the top soil layer, the remaining excess
                        // water cannot be redistributed to a higher layer
                        if (lk == 0 && tmpexcess > 0.0) {
                            excess = excess + tmpexcess;
                        }
                    }
                }
            }

            // The potential infiltration for the next layer (below this one)
            // is equal to the total flux out of this layer
            potential_infiltration = downward_flux[l] * timestep;  // cm
        } else {
            // The potential infiltration that could occur during the next time
            // step will not exceed the holding capacity of the soil layer.
            //
            // In this case, the infiltrated water is absorbed by this layer,
            // and no infiltrated water is passed to the layer below.

            // Adjust the soil water content to account for water gained by
            // infiltration
            swtemp[l] = swtemp[l] + potential_infiltration / soil_depth[l];  // m^3 / m^3

            // The soil water content cannot exceed the saturation capacity;
            // drainage to lower layers would prevent this from happening
            if (swtemp[l] >= soil_field_capacity[l] + eps_sfc) {
                // Calculate the drainage rate
                drainage_rate = swcon * (swtemp[l] - soil_field_capacity[l]) * soil_depth[l];  // cm / hr

                // Reduce the soil water conductivity constant in the top layer
                // to allow increased evaporation from wet soil surfaces
                if (l == 0) {
                    drainage_rate = swconrf * drainage_rate;  // cm / hr
                }

                // No infiltrated water is passed on to the next layer, so the
                // downward flux is only determined by drainage
                downward_flux[l] = drainage_rate;  // cm / hr

                // The total flux out of the soil layer cannot exceed the
                // saturated soil conductivity
                if (soil_saturated_conductivity[l] > 0.0 && downward_flux[l] > soil_saturated_conductivity[l]) {
                    // Adjust the total flux and drainage rate to avoid going
                    // over the upper flux limit
                    downward_flux[l] = soil_saturated_conductivity[l];  // cm / hr
                    drainage_rate = downward_flux[l];                   // cm / hr
                }

                // Adjust the soil water content to account for water lost
                // through drainage to lower soil layers
                swtemp[l] = swtemp[l] - downward_flux[l] / soil_depth[l];  // m^3 / m^3

                // The potential infiltration for the next layer (below this one)
                // is equal to the total flux out of this layer
                potential_infiltration = downward_flux[l] * timestep;  // cm
            } else {
                // There is no additional drainage out of this soil layer
                potential_infiltration = 0.0;  // cm
                downward_flux[l] = 0.0;        // cm / hr
            }
        }
    }

    infilWater_str return_value;

    // Any potential infiltration not absorbed by the soil profile flows out of
    // the bottom layer
    return_value.overall_drainage_rate =
        potential_infiltration * mm_per_cm / timestep;  // mm / hr

    // Any excess infiltration at the top layer will contribute to runoff
    return_value.excess_water_rate = excess * mm_per_cm / timestep;  // mm / hr

    for (int l = 0; l < nlayers; l++) {
        return_value.downward_flux[l] = downward_flux[l];                // cm / hr
        return_value.sw_delta_S[l] = swtemp[l] - soil_water_content[l];  // m^3 / m^3
    }

    return return_value;
}

/**
 *  @brief Calculates saturated water flow through a multi-layer soil profile.
 *
 *  ### Model implementation
 *
 *  It is unknown whether these calculations are described in a publication.
 *  This function is based on the subroutine `SATFLO` from DSSAT. Here we have
 *  converted the units from a "per day" basis to a "per hour" basis.
 *
 *  The DSSAT submodule describes these calculations as follows:
 *
 *  > Calculates saturated flow on days with no rain or irrigation.
 *  >
 *  > Saturated flow is calculated for days with no irr or rain. Drainage is
 *  > reduced when the flux exceeds the rate allowed by the saturated soil
 *  > hydraulic conductivity, assuming unit gradient.  This allows for perched
 *  > water tables in profile. Prevents flux from exceeding the most limiting
 *  > layer below it.  If the sat. hyd. cond values are missing (neg) assume no
 *  > perching of water table.
 *
 *  ### Source
 *
 *  - DSSAT Fortran source code:
 *    https://github.com/DSSAT/dssat-csm-os/blob/develop/Soil/SoilWater/SATFLO.for
 */
infilWater_str satflo(
    int const nlayers,                           // not a physical quantity
    double const swcon,                          // hr^(-1)
    double const soil_depth[],                   // cm
    double const soil_saturation_capacity[],     // m^3 / m^3
    double const soil_field_capacity[],          // m^3 / m^3
    double const soil_water_content[],           // m^3 / m^3
    double const soil_saturated_conductivity[],  // cm / hr
    double const timestep                        // hr
)
{
    // Specify hard-coded parameters
    double constexpr eps_sfc = 0.003;   // m^3 / m^3 - small threshold value of soil field capacity
    double constexpr mm_per_cm = 10.0;  // mm / cm

    // Initialize layer-dependent variables
    double downward_flux[nlayers];  // cm / hr   - Total downward water flux (drainage and infiltration)
    double swtemp[nlayers];         // m^3 / m^3 - Soil water content

    for (int l = 0; l < nlayers; l++) {
        downward_flux[l] = 0.0;             // cm / hr
        swtemp[l] = soil_water_content[l];  // m^3 / m^3
    }

    // For each layer, determine the downward flux and the new soil water
    // content
    for (int l = 0; l < nlayers; l++) {
        // Drainage will occur if the soil water content in this layer is above
        // the field capacity
        double const drainage_rate =
            swtemp[l] >= soil_field_capacity[l] + eps_sfc
                ? std::max(0.0, swcon * (swtemp[l] - soil_field_capacity[l]) * soil_depth[l])
                : 0;  // cm / hr

        // Water freely drains downward from the top layer. For lower layers,
        // any excess water beyond the holding capacity is passed downwards,
        // including drainage from the layer above.
        if (l == 0) {
            // This is the top layer
            downward_flux[l] = drainage_rate;  // cm / hr
        } else {
            // The holding capacity of the soil layer is the amount of water it
            // could absorb before reaching its saturation capacity
            double const hold =
                swtemp[l] < soil_field_capacity[l]
                    ? (soil_field_capacity[l] - swtemp[l]) * soil_depth[l]
                    : 0;  // cm

            // Total water drained
            double const total_drained =
                (downward_flux[l - 1] + drainage_rate) * timestep - hold;  // cm

            // Total downward flow rate
            downward_flux[l] = std::max(total_drained / timestep, 0.0);
        }

        // The total flux out of the soil layer cannot exceed the saturated soil
        // conductivity
        if (soil_saturated_conductivity[l] > 0.0 && downward_flux[l] > soil_saturated_conductivity[l]) {
            // Adjust the total flux to avoid going over the upper flux limit
            downward_flux[l] = soil_saturated_conductivity[l];  // cm / hr
        }
    }
    // Compute volumetric water contents after drainage in a day.
    // Prevent water content in any layer from exceeding saturation
    // as water drains down in the profile.

    // Calculate the new soil water content in each of the lower soil layers. If
    // there is excess water in a layer, redistribute it to the layer above
    for (int l = nlayers - 1; l >= 1; l--) {
        // Keep a record of the initial soil water content in case the flux
        // needs to be adjusted
        double const soil_water_old = swtemp[l];  // m^3 / m^3

        // The soil water content increases due to downward flux from the layer
        // above and decreases due to downward flux out of this layer
        swtemp[l] = swtemp[l] + (downward_flux[l - 1] - downward_flux[l]) / soil_depth[l];  // m^3 / m^3

        // The soil water content cannot exceed the saturation capacity
        if (swtemp[l] > soil_saturation_capacity[l]) {
            // Limit the new water content to the saturation capacity
            swtemp[l] = soil_saturation_capacity[l];

            // Find the amount of water this layer absorbed to reach its
            // saturation capacity
            double const absorbed =
                (soil_saturation_capacity[l] - soil_water_old) * soil_depth[l];  // cm

            // Conservation of mass requires that
            //
            //   absorbed = (mass_from_above - mass_to_below)
            //            = (rate_from_above - rate_to_below) * timestep
            //
            // So, solving for rate_from_above, we can see that
            //
            //   rate_from_above = absorbed / timestep + rate_to_below
            //
            // Here we adjust the rate from above to match this value
            downward_flux[l - 1] =
                std::max(0.0, absorbed / timestep + downward_flux[l]);  // cm / hr
        }
    }

    // Get the new soil water content in the top layer
    swtemp[0] = swtemp[0] - downward_flux[0] / soil_depth[0];  // m^3 / m^3

    infilWater_str return_value;

    // The drainage rate for the profile as a whole is the downward flux out of
    // the lowest layer
    return_value.overall_drainage_rate =
        downward_flux[nlayers - 1] * mm_per_cm;  // mm / hr

    // There is no excess water at the soil surface
    return_value.excess_water_rate = 0.0;  // mm / hr

    for (int l = 0; l < nlayers; l++) {
        return_value.downward_flux[l] = downward_flux[l];                // cm / hr
        return_value.sw_delta_S[l] = swtemp[l] - soil_water_content[l];  // m^3 / m^3
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
    int ist;  //Beginning soil layer for upward flow calculations (=1 for
              // layers 1 through 5, =2 for lower layers)
    double upflow[nlayers];
    double swdeltU[nlayers];
    double swtemp[nlayers];  //Soil water content in layer L (temporary value)

    // Calculated flow will be limited by SW_INF and SW_AVAIL
    double sw_inf[nlayers];  // Soil water content in layer L including computed upward flow (cm3 [water] / cm3 [soil])

    double esw[nlayers];
    for (int l = 0; l < nlayers; l++) {
        upflow[l] = 0.0;
        swdeltU[l] = 0.0;
        swtemp[l] = soil_water_content[l];  //Soil water content in layer l (temporary value)
        sw_inf[l] = sw_avail[l];            // Includes saturated flow
        sw_avail[l] = std::max(0.0, sw_avail[l] - soil_wilting_point[l]);
        esw[l] = soil_field_capacity[l] - soil_wilting_point[l];  //Plant extractable soil water by layer (= DUL - LL)
    }

    if (soil_depth[0] >= 5.0)  // soil_depth in cm
        ist = 0;
    else
        ist = 1;

    for (int l = ist; l < nlayers - 1; l++) {
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

        constexpr double min_diffusivity = 0.88 / 24.0;  // 0.88 cm / day
        constexpr double max_diffusivity = 100 / 24.0;   // 100 cm / day
        double dbar = min_diffusivity * exp(35.4 * ((thet1 * soil_depth[l] + thet2 * soil_depth[m]) / (soil_depth[l] + soil_depth[m])) * 0.5);
        dbar = std::min(dbar, max_diffusivity);
        // Rprintf("dbar is %f \n", dbar);

        double grad = (thet2 / esw[m] - thet1 / esw[l]) * (esw[m] * soil_depth[m] + esw[l] * soil_depth[l]) / (soil_depth[m] + soil_depth[l]);
        // Rprintf("grad is %f \n", grad);

        upflow[l] = dbar * grad / ((soil_depth[l] + soil_depth[m]) * 0.5);  // to cm/hr
        // Rprintf("upflow is %f \n", upflow[l]);
        double flowfix;  // Adjustment amount for upward flow calculations to prevent a
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
            } else {  // No upward flow if swtemp > soil_field_capacity
                upflow[l] = 0.0;
            }
            if (upflow[l] / soil_depth[m] > sw_avail[m]) {
                upflow[l] = sw_avail[m] * soil_depth[m];
                swtemp[l] = soil_water_old + upflow[l] / soil_depth[l];
            }
            swtemp[m] = swtemp[m] - upflow[l] / soil_depth[m];
        } else if (upflow[l] < 0.0) {  // Downward flow from layer L to layer M
            if (swtemp[l] >= soil_wilting_point[l]) {
                if (fabs(upflow[l] / soil_depth[l]) > sw_avail[l]) {
                    upflow[l] = -sw_avail[l] * soil_depth[l];
                }

                swtemp[l] = swtemp[l] + upflow[l] / soil_depth[l];
                swtemp[m] = swtemp[m] - upflow[l] / soil_depth[m];
                sw_inf[m] = sw_inf[m] - upflow[l] / soil_depth[m];

                if (sw_inf[m] > soil_saturation_capacity[m]) {
                    flowfix = std::min(fabs(upflow[l]), (sw_inf[m] -
                                                         soil_saturation_capacity[m]) *
                                                            soil_depth[m]);
                    upflow[l] = upflow[l] + flowfix;
                    swtemp[l] = soil_water_old + upflow[l] / soil_depth[l];
                    swtemp[m] = swtemp[m] - flowfix / soil_depth[m];
                }
            } else {  // No downward flow if SWTEMP < LL
                upflow[l] = 0.0;
            }
        }
    }
    for (int l = 0; l < nlayers; l++) {
        swdeltU[l] = swtemp[l] - soil_water_content[l];
    }

    for (int l = 0; l < nlayers; l++) {
        return_value.upwardFlo[l] = upflow[l];  // Not used anymore. Can be removed
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

    double total_tile_flow = 0.0;  // TDFC
    double swdeltT[nlayers];
    double drn[nlayers];
    for (int l = 0; l < nlayers; l++) {
        drn[l] = 0.0;
        swdeltT[l] = 0.0;
    }
    // Compute hydraulic head above drain defined as top-most saturated layer above the
    // drain. All saturated layers above the drain must be continuous. Compute soil water
    // available to drain (water in saturated zone in exceedance of soil field capacity).
    double head = 0.0;       // cm
    int topsat;              // top-most saturated layer above the drain.
    double tdf_avail = 0.0;  // Soil water available to drain, cm
    for (int l = td_layer_num; l >= 0; l--) {
        if (soil_water_content[l] >= (0.98 * soil_saturation_capacity[l])) {
            // Rprintf("the value of SW_%i is > 0.9 * SAT_%i \n", l,l);

            head = head + soil_depth[l];
            tdf_avail = tdf_avail + (soil_water_content[l] + sw_delta_S[l] -
                                     soil_field_capacity[l]) *
                                        soil_depth[l];
            topsat = l;
        } else {
            // Rprintf("the value of SW_%i is %f \n", l,soil_water_content[l]);
            // Total head includes saturated portion of first unsaturated layer.

            // Rprintf("the value of SW_%i is smaller than SAT_%i \n", l,l);
            head = head + ((soil_water_content[l] - soil_field_capacity[l]) /
                           (soil_saturation_capacity[l] - soil_field_capacity[l])) *
                              soil_depth[l];
            head = std::max(head, 0.0);
            tdf_avail = tdf_avail + (soil_water_content[l] + sw_delta_S[l] -
                                     soil_field_capacity[l]) *
                                        soil_depth[l];
            topsat = l;
            break;
        }
    }

    double tile_drain_conductivity = tile_drainage_rate * head;  // Conductivity of tile drain cm/hr
    double cumulative_tile_flow = 0.0;                           // cm/hr

    // Drain water from tile layer if layer is saturated
    if (head > 0.0) {
        cumulative_tile_flow = std::min(tile_drain_conductivity, tdf_avail);

        // Redistribute water from upper layers
        // Assume that water is limited by user-specified tile drainage rate
        // rather than by each layer's Ksat.
        double drn_total = 0.0;  // cm/hr
        double swdeltT_total = 0.0;
        double excess = 0.0;
        for (int l = topsat; l <= td_layer_num; l++) {  // top saturated layer down to tiledrain
            if (drn_total < cumulative_tile_flow) {
                // Rprintf("the value of drn_total is : %f \n", drn_total);
                // Reduce soil water from top saturated layers until tile
                // drainage capacity is met.
                swdeltT[l] = -(soil_water_content[l] + sw_delta_S[l] -
                               soil_field_capacity[l]);
                // Rprintf("the value of swdeltT[%i] is : %f \n", l, swdeltT[l]);
                drn_total = drn_total - swdeltT[l] * soil_depth[l];
                drn[l] = drn[l] + drn_total;
                if (drn_total > cumulative_tile_flow) {
                    // Rprintf("the value of drn_total > cumulative_tile_flow");
                    excess = drn_total - cumulative_tile_flow;
                    swdeltT[l] = swdeltT[l] + excess / soil_depth[l];
                    drn[l] = drn[l] - excess;
                }
            } else {
                // Lower soil layers will remain at saturation
                swdeltT[l] = 0.0;
                drn[l] = drn[l] + cumulative_tile_flow;
            }

            swdeltT_total = swdeltT_total + swdeltT[l] * soil_depth[l];
        }

        total_tile_flow = total_tile_flow + cumulative_tile_flow;
    } else {
        cumulative_tile_flow = 0.0;
    }
    return_value.head = head;
    return_value.topsat = topsat;
    return_value.tile_drain_conductivity = tile_drain_conductivity;
    return_value.tdf_avail = tdf_avail;
    return_value.cumulative_tile_flow = cumulative_tile_flow;
    return_value.total_tile_flow = total_tile_flow;
    for (int l = 0; l < nlayers; l++) {
        return_value.sw_delta_T[l] = swdeltT[l];
    }

    return return_value;
}
