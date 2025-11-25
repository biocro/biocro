#ifndef SOIL_EVAPORATION2_H
#define SOIL_EVAPORATION2_H
#include <algorithm>  // for std::min, std::max
#include <math.h>
#include <Rinternals.h>
#include "../framework/module.h"
#include "../framework/state_map.h"
#include "soil_evaporation_functions.h"
// #include "BioCro.h"  // For SoilEvapo
using namespace std;

namespace standardBML
{
class soil_evaporation2 : public differential_module
{
   public:
    soil_evaporation2(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module(),

          // Get references to input quantities
          skc{get_input(input_quantities, "skc")},
          kcbmax{get_input(input_quantities, "kcbmax")},
          doy{get_input(input_quantities, "doy")},
          lat{get_input(input_quantities, "lat")},
          elevation{get_input(input_quantities, "elevation")},
          lai{get_input(input_quantities, "lai")},
          bare_soil_albedo{get_input(input_quantities, "bare_soil_albedo")},
          kd{get_input(input_quantities, "kd")},
          windspeed{get_input(input_quantities, "windspeed")},
          rh{get_input(input_quantities, "rh")},
          // height{get_input(input_quantities, "height")},
          sumes1{get_input(input_quantities, "sumes1")},
          sumes2{get_input(input_quantities, "sumes2")},
          time_factor{get_input(input_quantities, "time_factor")},
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
          time_factor_op{get_op(output_quantities, "time_factor")},
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
    double const& bare_soil_albedo;
    double const& kd;
    double const& windspeed;
    double const& rh;
    // double const& height;
    double const& sumes1;
    double const& sumes2;
    double const& time_factor;
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
    double* time_factor_op;
    double* soil_evaporation_rate_op;

    // Main operation
    void do_operation() const;
};

string_vector soil_evaporation2::get_inputs()
{
    return {
        "skc",                  // Basal crop coefficient
        "kcbmax",
        "doy",                  // day of the year
        "lat",                  // latitude of the location
        "elevation",            // altitude in meters
        "lai",                  // Healthy leaf area index (m2[leaf] / m2[ground])
        "bare_soil_albedo",     // Bare soil albedo (fraction) - dimensionless
        "kd",                   // light extinction coefficient
        "windspeed",            // m/s
        "rh",                   // fraction. dimensionless
        // "height",               // Canopy height (m)
        "sumes1",               // Cumulative soil evaporation in stage 1 (mm)
        "sumes2",               // Cumulative soil evaporation in stage 2 (mm)
        "time_factor",          // Time factor for hourly temperature calculations
        "temp",                 // degrees C
        "solar",                // micromol / m^2 / s
        "soil_evaporation_rate",// Actual soil evaporation rate (mm/hr)
        "infiltrated_water",    // Water available for infiltration - rainfall minus runoff plus net irrigation (mm)
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
        "deltaU_6"
    };
}

string_vector soil_evaporation2::get_outputs()
{
    return {
        "sumes1",     // Cumulative soil evaporation in stage 1 (mm)
        "sumes2",     // Cumulative soil evaporation in stage 2 (mm)
        "time_factor", //time factor for hourly temperature calculations
        "soil_evaporation_rate"
    };
}

void soil_evaporation2::do_operation() const
{
    int nlayers = 6;
    double canopyHeight = 1.0;
    double evap_limit = 0.25;   // 6/24 cm/hr. Default Evaporation
                                // limit (cm) from SOILDYN.for, line 468 (variable U)
    double actual_soil_evap = soil_evaporation_rate;
    double sumes1_temp = sumes1;
    double sumes2_temp = sumes2;
    double time_factor_temp = time_factor;
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
    // SOILDYN.for, line 1488-1566
    // Soil albedo modification with water content
    double wet_soil_albedo =  soil_albedo(
        temp,
        lai,
        bare_soil_albedo,
        soil_water_content,
        soil_field_capacity);
    // Potential soil evaporation (PET.for - PSE function at line 1442)
    double potential_et = potential_evapotranspiration(
        solar,
        temp,
        lai,
        wet_soil_albedo
    );
//    //Rprintf("potential_et is: %f (MJ/m2/hr)\n", potential_et);

    // Reference Height computation
    double reference_et = reference_evapotranspiration(
        doy,
        solar,
        temp,
        lat,             //latitude
        elevation,
        windspeed,
        rh,
        wet_soil_albedo
    );
//    //Rprintf("reference_et is: %f (MJ/m2/hr)\n", reference_et);

    double potential_soil_evap = potential_soil_evaporation(
        skc,
        kcbmax,
        kd,
        lai,
        canopyHeight,
        potential_et,
        reference_et
    );
//    //Rprintf("potential_soil_evap is: %f (MJ/m2/hr)\n", potential_soil_evap);
    // SPAM.for line 353
    if (potential_soil_evap  > 1e-6){
        // Rprintf("potential_soil_evap is: %f (MJ/m2/hr)\n", potential_soil_evap);
        // Ritchie soil evaporation routine
        // Calculate the availability of soil water
        double sw_avail[nlayers];
        for (int l = 0; l < nlayers; l++) {
            sw_avail[l] = max(0.0, soil_water_content[l] + swdeltS[l] + swdeltU[l]);
        }
        // SOILEV.for
        // Set air dry water content for top soil layer
        double soil_water_air_dry = 0.9-0.00038 * pow((soil_depth[0]-30.0), 2);
        // Adjust soil evaporation, and the sum of stage 1 (SUMES1) and stage 2
        // (SUMES2) evaporation based on infiltration (WINF), potential
        // soil evaporation (EOS), and stage 1 evaporation (evap_limit = U).
        // CALL ESUP(EOS, SUMES1, SUMES2, U, ES, T)  !Supplementary calcs

        if ((sumes1 >= evap_limit) && (infiltrated_water >= sumes2)) {
//            Rprintf("case 1: sumes 1 >= evap_limit and winf >= sumes2 \n");
            // Stage 1 Evaporation
            double temp_wat_infil = infiltrated_water - sumes2; // Interim value of WINF, water available for infiltration (mm)
            sumes1_temp = evap_limit - temp_wat_infil;
            sumes2_temp = 0.0;
            time_factor_temp = 0.0;
            if (temp_wat_infil > evap_limit) sumes1_temp = 0.0;
            // CALL ESUP(EOS, SUMES1, SUMES2, U, ES, T)  !Supplementary calcs
            evap_str evap_comp;
            evap_comp = supplemetal_evap_computation(
                potential_soil_evap,
                sumes1_temp,
                sumes2_temp,
                evap_limit,
                time_factor_temp
            );
            sumes1_temp = evap_comp.sumes1;
            sumes2_temp = evap_comp.sumes2;
            time_factor_temp = evap_comp.time_factor;
            actual_soil_evap = evap_comp.actual_soil_evap;
//            Rprintf("Case 1: Actual soil evap is: %f \n", actual_soil_evap);
        }
        else if ((sumes1 >= evap_limit) && (infiltrated_water < sumes2)) {
            //Rprintf("case 2: sumes 1 >= U and winf < sumes2 \n");
            // Stage 2 Evaporation
            time_factor_temp = time_factor + 1.0;
            actual_soil_evap = 3.5 * pow(time_factor_temp, 0.5) - sumes2;
            if (infiltrated_water > 0.0) {
              double esx = 0.8 * infiltrated_water; // Interim value of evaporation rate for Stage 2 evaporation
              if (esx <= actual_soil_evap) esx = actual_soil_evap + infiltrated_water;
              if (esx > potential_soil_evap) esx = potential_soil_evap;
              actual_soil_evap = esx;
            }
            else if (actual_soil_evap > potential_soil_evap) {
              actual_soil_evap = potential_soil_evap;
            }
            sumes2_temp = sumes2 + actual_soil_evap - infiltrated_water;
            time_factor_temp = pow((sumes2_temp/3.5), 2);
            //Rprintf("Case 2: Actual soil evap is: %f \n", actual_soil_evap);
        }
        else if (infiltrated_water >= sumes1) {
            //Rprintf("case 3: winf >= sumes1 \n");
            // Stage 1 evaporation
            sumes1_temp = 0.0;
            // CALL ESUP(EOS, SUMES1, SUMES2, U, ES, T)  !Supplementary calcs
            evap_str evap_comp;
            evap_comp = supplemetal_evap_computation(
                potential_soil_evap,
                sumes1_temp,
                sumes2_temp,
                evap_limit,
                time_factor_temp
            );
            sumes1_temp = evap_comp.sumes1;
            sumes2_temp = evap_comp.sumes2;
            time_factor_temp = evap_comp.time_factor;
            actual_soil_evap = evap_comp.actual_soil_evap;
              //Rprintf("Case 3: Actual soil evap is: %f \n", actual_soil_evap);
        }
        else {
            //Rprintf("case 4: else \n");
            // Stage 1 evaporation
            sumes1_temp = sumes1 - infiltrated_water;
            // CALL ESUP(EOS, SUMES1, SUMES2, U, ES, T)  !Supplementary calcs
            evap_str evap_comp;
            evap_comp = supplemetal_evap_computation(
                potential_soil_evap,
                sumes1_temp,
                sumes2_temp,
                evap_limit,
                time_factor_temp
            );
            sumes1_temp = evap_comp.sumes1;
            sumes2_temp = evap_comp.sumes2;
            time_factor_temp = evap_comp.time_factor;
            actual_soil_evap = evap_comp.actual_soil_evap;
              //Rprintf("Case 4: Actual soil evap is: %f \n", actual_soil_evap);
        }
  // -----------------------------------------------------------------------
  //    Soil evaporation can not be larger than the current extractable soil
  //    water in the top layer.
  //    If available soil water is less than soil evaporation, adjust first
  //    and second stage evaporation and soil evaporation accordingly
  // -----------------------------------------------------------------------

        double sw_avail_evap = (soil_water_content[0] - soil_wilting_point[0] *
                  soil_water_air_dry) * soil_depth[0] * 10.0; // Available water for soil evaporation (mm/d)
        sw_avail_evap = max(0.0,sw_avail_evap);

        if (sw_avail_evap < actual_soil_evap) {
           if ((sumes1_temp >= evap_limit) &&  (sumes2_temp > actual_soil_evap)) {
              sumes2_temp = sumes2_temp - actual_soil_evap + sw_avail_evap;
              time_factor_temp = pow((sumes2_temp/3.5), 2);
              actual_soil_evap = sw_avail_evap;
            }
            else if ((sumes1_temp >= evap_limit) && (sumes2_temp < actual_soil_evap) &&
                   (sumes2_temp > 0.0)) {
              sumes1_temp = sumes1_temp - (actual_soil_evap - sumes2_temp);
              sumes2_temp = max(sumes1_temp + sw_avail_evap - evap_limit, 0.0);
              sumes1_temp = min(sumes1_temp + sw_avail_evap, evap_limit);
              time_factor_temp = pow((sumes2_temp/3.5), 2);
              actual_soil_evap = sw_avail_evap;
            }
           else {
              sumes1_temp = sumes1_temp - actual_soil_evap + sw_avail_evap;
              actual_soil_evap = sw_avail_evap;
           }
        }
  //-----------------------------------------------------------------------
        // Available water = SW - air dry limit + infil. or sat. flow
        double sw_min = max(0.0, sw_avail[0] - soil_water_air_dry * soil_wilting_point[0]);

        // Limit actual_soil_evap to between zero and avail water in soil layer 1
        if (actual_soil_evap > sw_min * soil_depth[0] * 10.0) {
          actual_soil_evap = sw_min * soil_depth[0] * 10.0;
        }

        actual_soil_evap = max(actual_soil_evap, 0.0);
    }
    double delta_sumes1 = sumes1_temp - sumes1;
    double delta_sumes2 = sumes2_temp - sumes2;
    double delta_time_factor = time_factor_temp - time_factor;
    double delta_actual_soil_evap = actual_soil_evap - old_soil_evap;
    // Update the output quantity list
    update(sumes1_op, delta_sumes1);
    update(sumes2_op, delta_sumes2);
    update(time_factor_op, delta_time_factor);
    update(soil_evaporation_rate_op, delta_actual_soil_evap);
}

}  // namespace BioCroWater
#endif
