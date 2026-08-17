#include <algorithm>  // for std::min, std::max
#include <cmath>      // for exp, fabs
#include <vector>
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
 *  Note: For some reason, this function should not be used when the potential
 *  amount of infiltration is very small. Instead, the `satflo` function should
 *  be used. This approach is taken in the `soil_water_downflow` module.
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
    std::vector<double> downward_flux(nlayers);  // cm / hr   - Total downward water flux (drainage and infiltration)
    std::vector<double> swtemp(nlayers);         // m^3 / m^3 - Soil water content

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
 *  For some reason, the `infil` function cannot be used when the potential
 *  infiltration rate is very low. Instead, this function is used. This approach
 *  is taken in the `soil_water_downflow` module.
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
 *  It is possible that these calculations are described in Suleiman and
 *  Ritchie (2004), or one of the references cited in that paper.
 *
 *  ### Source
 *
 *  - DSSAT Fortran source code:
 *    https://github.com/DSSAT/dssat-csm-os/blob/develop/Soil/SoilWater/SATFLO.for
 *
 *  - [Suleiman, A. A. & Ritchie, J. T. "MODIFICATIONS TO THE DSSAT VERTICAL DRAINAGE MODEL FOR
 *    MORE ACCURATE SOIL WATER DYNAMICS ESTIMATION. Soil Science 169, 745 (2004)]
 *    (https://doi.org/10.1097/01.ss.0000148740.90616.fd)
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
    std::vector<double> downward_flux(nlayers, 0.0);  // cm / hr   - Total downward water flux (drainage and infiltration)
    std::vector<double> swtemp(nlayers);              // m^3 / m^3 - Soil water content

    for (int l = 0; l < nlayers; l++) {
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

/**
 *  @brief Calculates upward water flow due to evaporation through a multi-layer
 *  soil profile.
 *
 *  ### Model overview
 *
 *  This module is based on a model for soil water infiltration originally
 *  presented in Ritchie (1998). It can be found in the section titled
 *  "Evapotranspiration and upward flow."
 *
 *  One of the parameter values reported in Ritchie (1998) was found by fitting
 *  data from Rose (1968).
 *
 *  ### Model implementation
 *
 *  Ritchie (1998) provides equations that can be used to implement the model.
 *  Rather than basing our code directly on this resource, we have also looked
 *  to DSSAT, and our code is largely based on its UP_FLOW subroutine. This
 *  subroutine is included in the file `WBSUBS.for`.
 *
 *  Here we have converted the units from a "per day" basis to a "per hour"
 *  basis.
 *
 *  ### Source
 *
 *  - [Ritchie, J. T. "Soil water balance and plant water stress" in "Understanding Options for Agricultural Production"
 *    (eds Tsuji, G. Y., Hoogenboom, G. & Thornton, P. K.) 41–54 (Springer Netherlands, Dordrecht, 1998)]
 *    (https://doi.org/10.1007/978-94-017-3624-4_3)
 *
 *  - [Rose, D. A. "Water movement in porous materials III. Evaporation of water
 *    from soil." J. Phys. D: Appl. Phys. 1, 1779 (1968)]
 *    (https://doi.org/10.1088/0022-3727/1/12/327)
 *
 *  - DSSAT Fortran source code:
 *    https://github.com/DSSAT/dssat-csm-os/blob/develop/Soil/SoilWater/WBSUBS.for
 *
 *  @param [in] swc_plus_sat Soil water content estimated at the next time step
 *              after accounting for downward flow (m^3 / m^3)
 */
upwardFlo_str up_flow(
    int const nlayers,                        // not a physical quantity
    double const swc_plus_sat[],              // m^3 / m^3
    double const soil_depth[],                // cm
    double const soil_saturation_capacity[],  // m^3 / m^3
    double const soil_wilting_point[],        // m^3 / m^3
    double const soil_field_capacity[],       // m^3 / m^3
    double const soil_water_content[],        // m^3 / m^3
    double const timestep                     // hr
)
{
    // Specify hard-coded parameter values
    double constexpr hours_per_day = 24.0;               // hr / day
    double constexpr max_dbar = 100;                     // cm / day
    double constexpr rose_const = 35.4;                  // dimensionless - fitted by Ritchie (1998) from data reported in Rose (1968)
    double constexpr soil_diffusivity = 0.88;            // cm / day
    double constexpr surface_thickness_threshold = 5.0;  // cm

    double const max_dbar_hr = max_dbar / hours_per_day;                  // cm / hr
    double const soil_diffusivity_hr = soil_diffusivity / hours_per_day;  // cm / hr

    // Initialize layer-dependent values
    std::vector<double> upward_flux(nlayers);  // cm / hr
    std::vector<double> swtemp(nlayers);       // m^3 / m^3 - soil water content (temporary value)
    std::vector<double> sw_inf(nlayers);       // m^3 / m^3 - soil water content including computed upward flow
    std::vector<double> sw_avail(nlayers);     // m^3 / m^3 - soil water content available for evaporation, plant extraction, or movement through soil
    std::vector<double> esw(nlayers);          // m^3 / m^3 - plant extractable soil water

    // Calculated flow will be limited by SW_INF and SW_AVAIL

    for (int l = 0; l < nlayers; l++) {
        upward_flux[l] = 0.0;                                                  // cm / hr
        swtemp[l] = soil_water_content[l];                                     // m^3 / m^3
        sw_inf[l] = swc_plus_sat[l];                                           // m^3 / m^3
        sw_avail[l] = std::max(0.0, swc_plus_sat[l] - soil_wilting_point[l]);  // m^3 / m^3
        esw[l] = soil_field_capacity[l] - soil_wilting_point[l];               // m^3 / m^3
    }

    // Determine the beginning soil layer for upward flow calculations;
    // typically the second layer is used, unless the first layer is very thick
    int const first = soil_depth[0] >= surface_thickness_threshold ? 0 : 1;

    for (int l = first; l < nlayers - 1; l++) {
        int m = l + 1;

        double const soil_water_old = swtemp[l];  // m^3 / m^3

        double const thet1 =
            std::max(0.0, std::min(swtemp[l] - soil_wilting_point[l], esw[l]));  // m^3 / m^3

        double const thet2 =
            std::max(0.0, std::min(swtemp[m] - soil_wilting_point[m], esw[m]));  // m^3 / m^3

        // Note from EL on 2026-05-05: Our version of this equation is different
        // from DSSAT, where `theta_avg_dssat = 0.5 * theta_avg_biocro`. We
        // believe the factor of 0.5 in the DSSAT code is a typo.
        double const theta_avg =
            (thet1 * soil_depth[l] + thet2 * soil_depth[m]) /
            (soil_depth[l] + soil_depth[m]);  // m^3 / m^3

        double const dbar =
            std::min(max_dbar_hr, soil_diffusivity_hr * exp(rose_const * theta_avg));  // cm / hr

        double const grad =
            (thet2 / esw[m] - thet1 / esw[l]) *
            (esw[m] * soil_depth[m] + esw[l] * soil_depth[l]) /
            (soil_depth[m] + soil_depth[l]);  // m^3 / m^3

        upward_flux[l] = dbar * grad / ((soil_depth[l] + soil_depth[m]) * 0.5);  // cm / hr

        // Adjust upward flow calculations to ensure no soil layer exceeds its
        // saturation water content at the next time step
        if (upward_flux[l] > 0.0) {
            // Water is flowing upward from layer M to layer L
            if (swtemp[l] <= soil_field_capacity[l]) {
                swtemp[l] = swtemp[l] + upward_flux[l] * timestep / soil_depth[l];  // m^3 / m^3
                sw_inf[l] = sw_inf[l] + upward_flux[l] * timestep / soil_depth[l];  // m^3 / m^3

                if (swtemp[l] > soil_field_capacity[l] || sw_inf[l] > soil_saturation_capacity[l]) {
                    double flowfix =
                        std::max({0.0,
                                  (swtemp[l] - soil_field_capacity[l]) * soil_depth[l],
                                  (sw_inf[l] - soil_saturation_capacity[l]) * soil_depth[l]});  // cm
                    flowfix = std::min(upward_flux[l] * timestep, flowfix);                     // cm

                    upward_flux[l] = upward_flux[l] - flowfix / timestep;                    // cm / hr
                    swtemp[l] = soil_water_old + upward_flux[l] * timestep / soil_depth[l];  // m^3 / m^3
                }
            } else {
                // No upward flow if swtemp > soil_field_capacity
                upward_flux[l] = 0.0;  // cm / hr
            }

            // Adjust upward flow calculations to ensure the flow does not
            // transport more water than is available
            if (upward_flux[l] * timestep / soil_depth[m] > sw_avail[m]) {
                upward_flux[l] = sw_avail[m] * soil_depth[m] / timestep;                 // cm / hr
                swtemp[l] = soil_water_old + upward_flux[l] * timestep / soil_depth[l];  // m^3 / m^3
            }

            swtemp[m] = swtemp[m] - upward_flux[l] * timestep / soil_depth[m];  // m^3 / m^3
        } else if (upward_flux[l] < 0.0) {
            // Water is flowing downward from layer L to layer M
            if (swtemp[l] >= soil_wilting_point[l]) {
                if (fabs(upward_flux[l] * timestep / soil_depth[l]) > sw_avail[l]) {
                    upward_flux[l] = -sw_avail[l] * soil_depth[l] / timestep;  // cm / hr
                }

                swtemp[l] = swtemp[l] + upward_flux[l] * timestep / soil_depth[l];  // m^3 / m^3
                swtemp[m] = swtemp[m] - upward_flux[l] * timestep / soil_depth[m];  // m^3 / m^3
                sw_inf[m] = sw_inf[m] - upward_flux[l] * timestep / soil_depth[m];  // m^3 / m^3

                if (sw_inf[m] > soil_saturation_capacity[m]) {
                    double flowfix =
                        std::min(fabs(upward_flux[l] * timestep),
                                 (sw_inf[m] - soil_saturation_capacity[m]) * soil_depth[m]);  // cm

                    upward_flux[l] = upward_flux[l] + flowfix / timestep;                    // cm / hr
                    swtemp[l] = soil_water_old + upward_flux[l] * timestep / soil_depth[l];  // m^3 / m^3
                    swtemp[m] = swtemp[m] - flowfix / soil_depth[m];                         // m^3 / m^3
                }
            } else {
                // No downward flow if SWTEMP < LL
                upward_flux[l] = 0.0;  // cm / hr
            }
        }
    }

    upwardFlo_str return_value;

    for (int l = 0; l < nlayers; l++) {
        return_value.upwardFlo[l] = upward_flux[l];                      // cm / hr
        return_value.sw_delta_U[l] = swtemp[l] - soil_water_content[l];  // m^3 / m^3
    }

    return return_value;
}

/**
 *  @brief Calculates flow through a multi-layer soil profile due to tile
 *  drainage.
 *
 *  ### Model implementation
 *
 *  It is unknown whether these calculations are described in a publication.
 *  This function is based on the subroutine `TILEDRAIN` from DSSAT. Here we
 *  have converted the units from a "per day" basis to a "per hour" basis.
 *
 *  ### Source
 *
 *  - DSSAT Fortran source code:
 *    https://github.com/DSSAT/dssat-csm-os/blob/develop/Soil/SoilWater/TILEDRAIN.for
 */
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
)
{
    // Specify hard-coded parameter values
    double constexpr sat_thresh = 0.98;  // dimensionless

    // Initialize layer-dependent values
    std::vector<double> drn(nlayers);      // cm
    std::vector<double> swdeltT(nlayers);  // m^3 / m^3

    for (int l = 0; l < nlayers; l++) {
        drn[l] = 0.0;      // cm
        swdeltT[l] = 0.0;  // m^3 / m^3
    }

    // Initialize non-layer-dependent variables
    double head = 0.0;       // cm
    double tile_flow = 0.0;  // cm - The amount of water drained in a timestep
    double tdf_avail = 0.0;  // cm - Soil water available to drain
    int topsat = 0;          // top-most saturated layer above the drain

    // Compute hydraulic head above drain defined as top-most saturated layer
    // above the drain. All saturated layers above the drain must be continuous.
    // Compute soil water available to drain (water in saturated zone in
    // exceedance of soil field capacity).
    for (int l = td_layer_num; l >= 0; l--) {
        if (soil_water_content[l] >= sat_thresh * soil_saturation_capacity[l]) {
            head = head + soil_depth[l];  // cm

            tdf_avail = tdf_avail +
                        (soil_water_content[l] + sw_delta_S[l] - soil_field_capacity[l]) *
                            soil_depth[l];  // cm

            topsat = l;
        } else {
            // Total head includes saturated portion of first unsaturated layer.
            head = head + ((soil_water_content[l] - soil_field_capacity[l]) /
                           (soil_saturation_capacity[l] - soil_field_capacity[l])) *
                              soil_depth[l];

            head = std::max(head, 0.0);  // cm

            tdf_avail = tdf_avail +
                        (soil_water_content[l] + sw_delta_S[l] - soil_field_capacity[l]) *
                            soil_depth[l];  // cm

            topsat = l;
            break;
        }
    }

    double const tile_drain_conductivity = tile_drainage_rate * head;  // cm / hr

    // Drain water from tile layer if layer is saturated
    if (head > 0.0) {
        tile_flow = std::min(tile_drain_conductivity * timestep, tdf_avail);  // cm

        // Redistribute water from upper layers. Assume that water is limited by
        // user-specified tile drainage rate rather than by each layer's Ksat.
        double drn_total = 0.0;  // cm
        double excess = 0.0;     // cm

        for (int l = topsat; l <= td_layer_num; l++) {  // top saturated layer down to tiledrain
            if (drn_total < tile_flow) {
                // Reduce soil water from top saturated layers until tile
                // drainage capacity is met.
                swdeltT[l] = -(soil_water_content[l] + sw_delta_S[l] -
                               soil_field_capacity[l]);  // m^3 / m^3

                drn_total = drn_total - swdeltT[l] * soil_depth[l];  // cm
                drn[l] = drn[l] + drn_total;                         // cm

                if (drn_total > tile_flow) {
                    excess = drn_total - tile_flow;                    // cm
                    swdeltT[l] = swdeltT[l] + excess / soil_depth[l];  // m^3 / m^3
                    drn[l] = drn[l] - excess;                          // cm
                }
            } else {
                // Lower soil layers will remain at saturation
                swdeltT[l] = 0.0;             // m^3 / m^3
                drn[l] = drn[l] + tile_flow;  // cm
            }
        }
    } else {
        tile_flow = 0.0;  // cm
    }

    tileDrain_str return_value;

    return_value.head = head;                                        // cm
    return_value.topsat = topsat;                                    // not a physical quantity
    return_value.tile_drain_conductivity = tile_drain_conductivity;  // cm / hr
    return_value.tdf_avail = tdf_avail;                              // cm
    return_value.tile_flow_rate = tile_flow / timestep;              // cm / hr

    for (int l = 0; l < nlayers; l++) {
        return_value.sw_delta_T[l] = swdeltT[l];  // m^3 / m^3
    }

    return return_value;
}
