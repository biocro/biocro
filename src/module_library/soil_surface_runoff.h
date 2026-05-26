#ifndef SOIL_SURFACE_RUNOFF_H
#define SOIL_SURFACE_RUNOFF_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class soil_surface_runoff
 *
 * @brief Calculates overland water runoff from the top two soil layers due to
 * inflow from precipitation and irrigation.
 *
 * It is based off the RNOFF subroutine from DSSAT
 * (https://github.com/DSSAT/dssat-csm-os/blob/develop/Soil/SoilWater/RNOFF.for)
 *
 * The DSSAT code implements the "Williams-SCS curve number technique."
 * Williams et al. (2011) (https://doi.org/10.1061/(ASCE)HE.1943-5584.0000529)
 *
 * See also
 * https://www.hec.usace.army.mil/confluence/rasdocs/ras1dtechref/6.4/overview-of-optional-capabilities/modeling-precipitation-and-infiltration/curve-number
 *
 */
class soil_surface_runoff : public direct_module
{
   public:
    soil_surface_runoff(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities
          precip{get_input(input_quantities, "precip")},
          irrigation_rate{get_input(input_quantities, "irrigation_rate")},
          curve_number{get_input(input_quantities, "curve_number")},

          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")},
          soil_wilting_point_1{get_input(input_quantities, "soil_wilting_point_1")},
          soil_saturation_capacity_1{get_input(input_quantities, "soil_saturation_capacity_1")},

          soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},
          soil_wilting_point_2{get_input(input_quantities, "soil_wilting_point_2")},
          soil_saturation_capacity_2{get_input(input_quantities, "soil_saturation_capacity_2")},

          soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},
          soil_wilting_point_3{get_input(input_quantities, "soil_wilting_point_3")},
          soil_saturation_capacity_3{get_input(input_quantities, "soil_saturation_capacity_3")},

          soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},
          soil_wilting_point_4{get_input(input_quantities, "soil_wilting_point_4")},
          soil_saturation_capacity_4{get_input(input_quantities, "soil_saturation_capacity_4")},

          soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},
          soil_wilting_point_5{get_input(input_quantities, "soil_wilting_point_5")},
          soil_saturation_capacity_5{get_input(input_quantities, "soil_saturation_capacity_5")},

          soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},
          soil_wilting_point_6{get_input(input_quantities, "soil_wilting_point_6")},
          soil_saturation_capacity_6{get_input(input_quantities, "soil_saturation_capacity_6")},

          // Get pointers to output quantities
          surface_runoff_rate_op{get_op(output_quantities, "surface_runoff_rate")},
          soil_storage_op{get_op(output_quantities, "soil_storage")},
          pb_op{get_op(output_quantities, "pb")},
          soil_initial_abstraction_op{get_op(output_quantities, "soil_initial_abstraction")},
          available_water_op{get_op(output_quantities, "available_water")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_surface_runoff"; }

   private:
    // References to input quantities
    double const& precip;
    double const& irrigation_rate;
    double const& curve_number;

    double const& soil_water_content_1;
    double const& soil_wilting_point_1;
    double const& soil_saturation_capacity_1;

    double const& soil_water_content_2;
    double const& soil_wilting_point_2;
    double const& soil_saturation_capacity_2;

    double const& soil_water_content_3;
    double const& soil_wilting_point_3;
    double const& soil_saturation_capacity_3;

    double const& soil_water_content_4;
    double const& soil_wilting_point_4;
    double const& soil_saturation_capacity_4;

    double const& soil_water_content_5;
    double const& soil_wilting_point_5;
    double const& soil_saturation_capacity_5;

    double const& soil_water_content_6;
    double const& soil_wilting_point_6;
    double const& soil_saturation_capacity_6;

    // Pointers to output quantities
    double* surface_runoff_rate_op;
    double* soil_storage_op;
    double* pb_op;
    double* soil_initial_abstraction_op;
    double* available_water_op;

    // Main operation
    void do_operation() const;
};

string_vector soil_surface_runoff::get_inputs()
{
    return {
        "precip",                      // mm / hr
        "irrigation_rate",             // Mg / ha / hr
        "curve_number",                // unitless
        "soil_water_content_1",        // m^3 / m^3
        "soil_wilting_point_1",        // m^3 / m^3
        "soil_saturation_capacity_1",  // m^3 / m^3
        "soil_water_content_2",        // m^3 / m^3
        "soil_wilting_point_2",        // m^3 / m^3
        "soil_saturation_capacity_2",  // m^3 / m^3
        "soil_water_content_3",        // m^3 / m^3
        "soil_wilting_point_3",        // m^3 / m^3
        "soil_saturation_capacity_3",  // m^3 / m^3
        "soil_water_content_4",        // m^3 / m^3
        "soil_wilting_point_4",        // m^3 / m^3
        "soil_saturation_capacity_4",  // m^3 / m^3
        "soil_water_content_5",        // m^3 / m^3
        "soil_wilting_point_5",        // m^3 / m^3
        "soil_saturation_capacity_5",  // m^3 / m^3
        "soil_water_content_6",        // m^3 / m^3
        "soil_wilting_point_6",        // m^3 / m^3
        "soil_saturation_capacity_6"   // m^3 / m^3
    };
}

string_vector soil_surface_runoff::get_outputs()
{
    return {
        "surface_runoff_rate",       // Mg / ha / hr
        "soil_storage",              // mm
        "pb",                        // mm
        "soil_initial_abstraction",  // unitless
        "available_water"            // mm / hr
    };
}

void soil_surface_runoff::do_operation() const
{
    // Define hard-coded constants
    double constexpr mm_per_inch = 254;
    double constexpr mm_to_Mg_per_ha = 10;  // (Mg / ha) / mm
    double constexpr one_hour = 1;          // hour
    int constexpr nlayers = 6;

    double available_water = precip + irrigation_rate / mm_to_Mg_per_ha;  // mm / hr
    double soil_storage = mm_per_inch * (100.0 / curve_number - 1.0);     // mm

    double soil_water_content[] = {
        soil_water_content_1,
        soil_water_content_2,
        soil_water_content_3,
        soil_water_content_4,
        soil_water_content_5,
        soil_water_content_6};

    double soil_wilting_point[] = {
        soil_wilting_point_1,
        soil_wilting_point_2,
        soil_wilting_point_3,
        soil_wilting_point_4,
        soil_wilting_point_5,
        soil_wilting_point_6};

    double soil_saturation_capacity[] = {
        soil_saturation_capacity_1,
        soil_saturation_capacity_2,
        soil_saturation_capacity_3,
        soil_saturation_capacity_4,
        soil_saturation_capacity_5,
        soil_saturation_capacity_6};

    for (int l = 0; l < nlayers; l++) {
        if (soil_water_content[l] < soil_wilting_point[l]) {  // m^3 / m ^3
            if (l == 0) {
                double soil_water_air_dry = 0.30 * soil_wilting_point[l];  // m^3 / m ^3
                if (soil_water_content[l] < soil_water_air_dry)            // m^3 / m ^3
                    soil_water_content[l] = soil_water_air_dry;            // m^3 / m ^3
            } else
                soil_water_content[l] = soil_wilting_point[l];  // m^3 / m ^3
        }
    }
    // Initial abstraction ratio
    // Runoff is related to the average soil water content of the top two layers of soil
    double soil_initial_abstraction =
        0.15 * ((soil_saturation_capacity[0] - soil_water_content[0]) /
                    (soil_saturation_capacity[0] - soil_wilting_point[0] * 0.5) +
                (soil_saturation_capacity[1] - soil_water_content[1]) /
                    (soil_saturation_capacity[1] - soil_wilting_point[1] * 0.5));  // unitless ratio

    soil_initial_abstraction = std::max(0.0, soil_initial_abstraction);  // unitless ratio

    double excess_water = available_water * one_hour;  // mm
    // Determine threshold amount of rainfall that will occur before
    //      runoff starts (mm)
    double pb = excess_water - soil_initial_abstraction * soil_storage;  // mm

    // Equation 1 in Williams (2012)
    double surface_runoff = 0.0;         // mm
    double constexpr threshold = 0.001;  // mm / hr
    if (available_water > threshold) {
        if (pb > 0.0) {
            surface_runoff = std::pow(pb, 2) / (excess_water +
                                                (1 - soil_initial_abstraction) * soil_storage);  // mm
        }
    }

    // Update the output quantity list
    update(surface_runoff_rate_op, surface_runoff * mm_to_Mg_per_ha / one_hour);  // Mg / ha / hr
    update(soil_storage_op, soil_storage);
    update(pb_op, pb);
    update(soil_initial_abstraction_op, soil_initial_abstraction);
    update(available_water_op, available_water);
}

}  // namespace standardBML
#endif
