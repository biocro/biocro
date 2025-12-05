#ifndef SOIL_EVAPORATION2_H
#define SOIL_EVAPORATION2_H

#include <algorithm>  // for std::min, std::max
#include "../framework/module.h"
#include "../framework/state_map.h"
#include "soil_evaporation_functions.h"

namespace standardBML
{
/**
 *  @class soil_evaporation2
 *
 *  @brief This module is based on the SOILEV subroutine from DSSAT,
 *  which calculates a daily soil evaporation rate. Here, we have made several
 *  changes to allow hourly calculation to match BioCro.
 *  Only compatible with a fixed-step Euler solver.
 *
 *  ### Model overview
 *
 *  ### Source
 *
 *  - Ritchie, J. T. (1972), Model for predicting evaporation from a row crop
 *  with incomplete cover, Water Resour. Res., 8(5), 1204–1213,
 *  doi:10.1029/WR008i005p01204.
 *  - DSSAT Fortran source code:
 *  github.com/DSSAT/dssat-csm-os/blob/develop/SPAM/SOILEV.for
 */
class soil_evaporation2 : public differential_module
{
   public:
    soil_evaporation2(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module(true),

          // Get references to input quantities
          skc{get_input(input_quantities, "skc")},
          kcbmax{get_input(input_quantities, "kcbmax")},
          doy{get_input(input_quantities, "doy")},
          lat{get_input(input_quantities, "lat")},
          elevation{get_input(input_quantities, "elevation")},
          lai{get_input(input_quantities, "lai")},
          bare_soil_albedo_max{get_input(input_quantities, "bare_soil_albedo_max")},
          k_diffuse{get_input(input_quantities, "k_diffuse")},
          windspeed{get_input(input_quantities, "windspeed")},
          rh{get_input(input_quantities, "rh")},
          par_energy_content{get_input(input_quantities, "par_energy_content")},
          // height{get_input(input_quantities, "height")},
          sumes1{get_input(input_quantities, "sumes1")},
          sumes2{get_input(input_quantities, "sumes2")},
          days_stage2{get_input(input_quantities, "days_stage2")},
          hours_per_day{get_input(input_quantities, "hours_per_day")},
          temp{get_input(input_quantities, "temp")},
          solar{get_input(input_quantities, "solar")},
          soil_evaporation_rate{get_input(input_quantities, "soil_evaporation_rate")},
          infiltrated_water{get_input(input_quantities, "infiltrated_water")},

          soil_depth_1{get_input(input_quantities, "soil_depth_1")},
          soil_wilting_point_1{get_input(input_quantities, "soil_wilting_point_1")},
          soil_field_capacity_1{get_input(input_quantities, "soil_field_capacity_1")},
          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")},
          deltaS_1{get_input(input_quantities, "deltaS_1")},
          deltaU_1{get_input(input_quantities, "deltaU_1")},
          // deltaT_1{get_input(input_quantities, "deltaT_1")},

          // Parameters for layer 2
          soil_depth_2{get_input(input_quantities, "soil_depth_2")},
          soil_wilting_point_2{get_input(input_quantities, "soil_wilting_point_2")},
          soil_field_capacity_2{get_input(input_quantities, "soil_field_capacity_2")},
          soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},
          deltaS_2{get_input(input_quantities, "deltaS_2")},
          deltaU_2{get_input(input_quantities, "deltaU_2")},
          // deltaT_2{get_input(input_quantities, "deltaT_2")},

          // Parameters for layer 3
          soil_depth_3{get_input(input_quantities, "soil_depth_3")},
          soil_wilting_point_3{get_input(input_quantities, "soil_wilting_point_3")},
          soil_field_capacity_3{get_input(input_quantities, "soil_field_capacity_3")},
          soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},
          deltaS_3{get_input(input_quantities, "deltaS_3")},
          deltaU_3{get_input(input_quantities, "deltaU_3")},
          // deltaT_3{get_input(input_quantities, "deltaT_3")},

          // Parameters for layer 4
          soil_depth_4{get_input(input_quantities, "soil_depth_4")},
          soil_wilting_point_4{get_input(input_quantities, "soil_wilting_point_4")},
          soil_field_capacity_4{get_input(input_quantities, "soil_field_capacity_4")},
          soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},
          deltaS_4{get_input(input_quantities, "deltaS_4")},
          deltaU_4{get_input(input_quantities, "deltaU_4")},
          // deltaT_4{get_input(input_quantities, "deltaT_4")},

          // Parameters for layer 5
          soil_depth_5{get_input(input_quantities, "soil_depth_5")},
          soil_wilting_point_5{get_input(input_quantities, "soil_wilting_point_5")},
          soil_field_capacity_5{get_input(input_quantities, "soil_field_capacity_5")},
          soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},
          deltaS_5{get_input(input_quantities, "deltaS_5")},
          deltaU_5{get_input(input_quantities, "deltaU_5")},
          // deltaT_5{get_input(input_quantities, "deltaT_5")},

          // Parameters for layer 6
          soil_depth_6{get_input(input_quantities, "soil_depth_6")},
          soil_wilting_point_6{get_input(input_quantities, "soil_wilting_point_6")},
          soil_field_capacity_6{get_input(input_quantities, "soil_field_capacity_6")},
          soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},
          deltaS_6{get_input(input_quantities, "deltaS_6")},
          deltaU_6{get_input(input_quantities, "deltaU_6")},
          // deltaT_6{get_input(input_quantities, "deltaT_6")},
          // Get pointers to output quantities
          sumes1_op{get_op(output_quantities, "sumes1")},
          sumes2_op{get_op(output_quantities, "sumes2")},
          days_stage2_op{get_op(output_quantities, "days_stage2")},
          soil_evaporation_rate_op{get_op(output_quantities, "soil_evaporation_rate")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_evaporation2"; }

   private:
    // Pointers to input quantities
    double const& skc;
    double const& kcbmax;
    double const& doy;
    double const& lat;
    double const& elevation;
    double const& lai;
    double const& bare_soil_albedo_max;
    double const& k_diffuse;
    double const& windspeed;
    double const& rh;
    double const& par_energy_content;
    // double const& height;
    double const& sumes1;
    double const& sumes2;
    double const& days_stage2;
    double const& hours_per_day;
    double const& temp;
    double const& solar;
    double const& soil_evaporation_rate;
    double const& infiltrated_water;

    double const& soil_depth_1;
    double const& soil_wilting_point_1;
    double const& soil_field_capacity_1;
    double const& soil_water_content_1;
    double const& deltaS_1;
    double const& deltaU_1;

    // Inputs for layer 2
    double const& soil_depth_2;
    double const& soil_wilting_point_2;
    double const& soil_field_capacity_2;
    double const& soil_water_content_2;
    double const& deltaS_2;
    double const& deltaU_2;

    // Inputs for layer 3
    double const& soil_depth_3;
    double const& soil_wilting_point_3;
    double const& soil_field_capacity_3;
    double const& soil_water_content_3;
    double const& deltaS_3;
    double const& deltaU_3;

    // Inputs for layer 4
    double const& soil_depth_4;
    double const& soil_wilting_point_4;
    double const& soil_field_capacity_4;
    double const& soil_water_content_4;
    double const& deltaS_4;
    double const& deltaU_4;

    // Inputs for layer 5
    double const& soil_depth_5;
    double const& soil_wilting_point_5;
    double const& soil_field_capacity_5;
    double const& soil_water_content_5;
    double const& deltaS_5;
    double const& deltaU_5;

    // Inputs for layer 6
    double const& soil_depth_6;
    double const& soil_wilting_point_6;
    double const& soil_field_capacity_6;
    double const& soil_water_content_6;
    double const& deltaS_6;
    double const& deltaU_6;

    // Pointers to output quantities
    double* sumes1_op;
    double* sumes2_op;
    double* days_stage2_op;
    double* soil_evaporation_rate_op;

    // Main operation
    void do_operation() const;
};

string_vector soil_evaporation2::get_inputs()
{
    return {
        "skc",  // Basal crop coefficient
        "kcbmax",
        "doy",                    // day of the year
        "lat",                    // latitude of the location
        "elevation",              // altitude in meters
        "lai",                    // Healthy leaf area index (m2[leaf] / m2[ground])
        "bare_soil_albedo_max",   // Bare soil albedo (fraction) - dimensionless
        "k_diffuse",              // light extinction coefficient
        "windspeed",              // m/s
        "rh",                     // fraction. dimensionless
        "par_energy_content",     // J / micromol
        "sumes1",                 // Cumulative soil evaporation in stage 1 (mm)
        "sumes2",                 // Cumulative soil evaporation in stage 2 (mm)
        "days_stage2",            // Days elapsed in Stage-2 evaporation (decimal allowed)
        "hours_per_day",          //
        "temp",                   // degrees C
        "solar",                  // micromol / m^2 / s
        "soil_evaporation_rate",  // Actual soil evaporation rate (mm/hr)
        "infiltrated_water",      // Water available for infiltration - rainfall minus runoff plus net irrigation (mm)
        "soil_depth_1",
        "soil_wilting_point_1",
        "soil_field_capacity_1",
        "soil_water_content_1",
        "deltaS_1",
        "deltaU_1",

        "soil_depth_2",
        "soil_wilting_point_2",
        "soil_field_capacity_2",
        "soil_water_content_2",
        "deltaS_2",
        "deltaU_2",

        "soil_depth_3",
        "soil_wilting_point_3",
        "soil_field_capacity_3",
        "soil_water_content_3",
        "deltaS_3",
        "deltaU_3",

        "soil_depth_4",
        "soil_wilting_point_4",
        "soil_field_capacity_4",
        "soil_water_content_4",
        "deltaS_4",
        "deltaU_4",

        "soil_depth_5",
        "soil_wilting_point_5",
        "soil_field_capacity_5",
        "soil_water_content_5",
        "deltaS_5",
        "deltaU_5",

        "soil_depth_6",
        "soil_wilting_point_6",
        "soil_field_capacity_6",
        "soil_water_content_6",
        "deltaS_6",
        "deltaU_6"};
}

string_vector soil_evaporation2::get_outputs()
{
    return {
        "sumes1",       // Cumulative soil evaporation in stage 1 (mm)
        "sumes2",       // Cumulative soil evaporation in stage 2 (mm)
        "days_stage2",  // Days elapsed in Stage-2 evaporation (decimal allowed)
        "soil_evaporation_rate"};
}

void soil_evaporation2::do_operation() const
{
    using std::max;
    using std::min;

    int constexpr nlayers = 6;
    double constexpr canopyHeight = 1.0;  // m
    // soil hydraulic properties. Houston black clay
    // See Table 1 in Ritchie (1972), https://doi.org/10.1029/WR008i005p01204
    double constexpr soil_evaporation_alpha = 3.5;  // mm/day^(0.5)

    // Upper Limit of Stage 1 Cumulative Evaporation. Houston black clay.
    // Table 1. Ritchie (1972)
    double constexpr evap_limit = 6.0;  // mm

    double actual_soil_evap = soil_evaporation_rate;
    double sumes1_temp = sumes1;
    double sumes2_temp = sumes2;
    double days_stage2_temp = days_stage2;
    double old_soil_evap = soil_evaporation_rate;
    double soil_depth[] = {
        soil_depth_1,
        soil_depth_2,
        soil_depth_3,
        soil_depth_4,
        soil_depth_5,
        soil_depth_6};

    double soil_water_content[] = {
        soil_water_content_1,
        soil_water_content_2,
        soil_water_content_3,
        soil_water_content_4,
        soil_water_content_5,
        soil_water_content_6};

    double soil_field_capacity[] = {
        soil_field_capacity_1,
        soil_field_capacity_2,
        soil_field_capacity_3,
        soil_field_capacity_4,
        soil_field_capacity_5,
        soil_field_capacity_6};

    double soil_wilting_point[] = {
        soil_wilting_point_1,
        soil_wilting_point_2,
        soil_wilting_point_3,
        soil_wilting_point_4,
        soil_wilting_point_5,
        soil_wilting_point_6};

    double swdeltS[] = {
        deltaS_1,
        deltaS_2,
        deltaS_3,
        deltaS_4,
        deltaS_5,
        deltaS_6};

    double swdeltU[] = {
        deltaU_1,
        deltaU_2,
        deltaU_3,
        deltaU_4,
        deltaU_5,
        deltaU_6};

    double const surface_soil_depth_in_mm = soil_depth[0] * 10.0;  // mm

    // Soil albedo modification with water content
    double wet_soil_albedo = surface_albedo(
        lai,
        bare_soil_albedo_max,
        soil_water_content[0],
        soil_field_capacity[0]);
    // Potential soil evaporation (PET.for - PSE function at line 1442)
    double potential_et = potential_evapotranspiration(
        solar,
        temp,
        lai,
        wet_soil_albedo,
        par_energy_content);

    // Reference Height computation
    double reference_et = reference_evapotranspiration(
        doy,
        solar,
        temp,
        lat,  // latitude
        elevation,
        windspeed,
        rh,
        wet_soil_albedo,
        par_energy_content);

    double potential_soil_evap = potential_soil_evaporation(
        skc,
        kcbmax,
        k_diffuse,
        lai,
        canopyHeight,
        potential_et,
        reference_et);
    if (potential_soil_evap > 1e-6) {
        // Ritchie soil evaporation routine
        // Calculate the availability of soil water
        double sw_avail[nlayers];
        for (int l = 0; l < nlayers; l++) {
            sw_avail[l] = max(0.0, soil_water_content[l] + swdeltS[l] + swdeltU[l]);
        }
        // Set air dry water content for top soil layer
        // Here the 30 seems to still be in cm.
        // See Fortran source here: https://github.com/DSSAT/dssat-csm-os/blob/develop/SPAM/SOILEV.for
        double soil_water_air_dry = 0.9 - 0.00038 * pow((soil_depth[0] - 30.0), 2);
        // Adjust soil evaporation, and the sum of stage 1 (SUMES1) and stage 2
        // (SUMES2) evaporation based on infiltration (WINF), potential
        // soil evaporation (EOS), and stage 1 evaporation (evap_limit = U).

        if ((sumes1 >= evap_limit) && (infiltrated_water >= sumes2)) {
            // Stage 1 Evaporation
            double temp_wat_infil = infiltrated_water - sumes2;  // Interim value of WINF, water available for infiltration (mm)
            sumes1_temp = evap_limit - temp_wat_infil;
            sumes2_temp = 0.0;
            days_stage2_temp = 0.0;
            if (temp_wat_infil > evap_limit) sumes1_temp = 0.0;
            evap_str evap_comp;
            evap_comp = supplemetal_evap_computation(
                potential_soil_evap,
                sumes1_temp,
                sumes2_temp,
                evap_limit,
                soil_evaporation_alpha,
                days_stage2_temp);
            sumes1_temp = evap_comp.sumes1;
            sumes2_temp = evap_comp.sumes2;
            days_stage2_temp = evap_comp.days_stage2;
            actual_soil_evap = evap_comp.actual_soil_evap;
        } else if ((sumes1 >= evap_limit) && (infiltrated_water < sumes2)) {
            // Stage 2 Evaporation
            days_stage2_temp = days_stage2 + 1.0 / hours_per_day;
            actual_soil_evap = soil_evaporation_alpha * pow(days_stage2_temp, 0.5) - sumes2;
            if (infiltrated_water > 0.0) {
                double esx = 0.8 * infiltrated_water;  // Interim value of evaporation rate for Stage 2 evaporation
                if (esx <= actual_soil_evap) esx = actual_soil_evap + infiltrated_water;
                if (esx > potential_soil_evap) esx = potential_soil_evap;
                actual_soil_evap = esx;
            } else if (actual_soil_evap > potential_soil_evap) {
                actual_soil_evap = potential_soil_evap;
            }
            sumes2_temp = sumes2 + actual_soil_evap - infiltrated_water;
            days_stage2_temp = pow((sumes2_temp / soil_evaporation_alpha), 2);
        } else if (infiltrated_water >= sumes1) {
            // Stage 1 evaporation
            sumes1_temp = 0.0;
            evap_str evap_comp;
            evap_comp = supplemetal_evap_computation(
                potential_soil_evap,
                sumes1_temp,
                sumes2_temp,
                evap_limit,
                soil_evaporation_alpha,
                days_stage2_temp);
            sumes1_temp = evap_comp.sumes1;
            sumes2_temp = evap_comp.sumes2;
            days_stage2_temp = evap_comp.days_stage2;
            actual_soil_evap = evap_comp.actual_soil_evap;
        } else {
            // Stage 1 evaporation
            sumes1_temp = sumes1 - infiltrated_water;
            evap_str evap_comp;
            evap_comp = supplemetal_evap_computation(
                potential_soil_evap,
                sumes1_temp,
                sumes2_temp,
                evap_limit,
                soil_evaporation_alpha,
                days_stage2_temp);
            sumes1_temp = evap_comp.sumes1;
            sumes2_temp = evap_comp.sumes2;
            days_stage2_temp = evap_comp.days_stage2;
            actual_soil_evap = evap_comp.actual_soil_evap;
        }
        // -----------------------------------------------------------------------
        //    Soil evaporation can not be larger than the current extractable soil
        //    water in the top layer.
        //    If available soil water is less than soil evaporation, adjust first
        //    and second stage evaporation and soil evaporation accordingly
        // -----------------------------------------------------------------------

        double sw_avail_evap = (soil_water_content[0] - soil_wilting_point[0] *
                                                            soil_water_air_dry) *
                               surface_soil_depth_in_mm;  // Available water for soil evaporation (mm)
        sw_avail_evap = max(0.0, sw_avail_evap);

        if (sw_avail_evap < actual_soil_evap) {
            if ((sumes1_temp >= evap_limit) && (sumes2_temp > actual_soil_evap)) {
                sumes2_temp = sumes2_temp - actual_soil_evap + sw_avail_evap;
                days_stage2_temp = pow((sumes2_temp / soil_evaporation_alpha), 2);
                actual_soil_evap = sw_avail_evap;
            } else if ((sumes1_temp >= evap_limit) && (sumes2_temp < actual_soil_evap) &&
                       (sumes2_temp > 0.0)) {
                sumes1_temp = sumes1_temp - (actual_soil_evap - sumes2_temp);
                sumes2_temp = max(sumes1_temp + sw_avail_evap - evap_limit, 0.0);
                sumes1_temp = min(sumes1_temp + sw_avail_evap, evap_limit);
                days_stage2_temp = pow((sumes2_temp / soil_evaporation_alpha), 2);
                actual_soil_evap = sw_avail_evap;
            } else {
                sumes1_temp = sumes1_temp - actual_soil_evap + sw_avail_evap;
                actual_soil_evap = sw_avail_evap;
            }
        }
        //-----------------------------------------------------------------------
        // Available water = SW - air dry limit + infil. or sat. flow
        double sw_min = max(0.0, sw_avail[0] - soil_water_air_dry * soil_wilting_point[0]);

        // Limit actual_soil_evap to between zero and avail water in soil layer 1
        if (actual_soil_evap > sw_min * surface_soil_depth_in_mm) {
            actual_soil_evap = sw_min * surface_soil_depth_in_mm;
        }

        actual_soil_evap = max(actual_soil_evap, 0.0);
    }
    double delta_sumes1 = sumes1_temp - sumes1;
    double delta_sumes2 = sumes2_temp - sumes2;
    double delta_days_stage2 = days_stage2_temp - days_stage2;
    double delta_actual_soil_evap = actual_soil_evap - old_soil_evap;
    // Update the output quantity list
    update(sumes1_op, delta_sumes1);
    update(sumes2_op, delta_sumes2);
    update(days_stage2_op, delta_days_stage2);
    update(soil_evaporation_rate_op, delta_actual_soil_evap);
}

}  // namespace standardBML
#endif
