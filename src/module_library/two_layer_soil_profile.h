#ifndef TWO_LAYER_SOIL_PROFILE_H
#define TWO_LAYER_SOIL_PROFILE_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "AuxBioCro.h"  // For soilML_str
#include "BioCro.h"     // For soilML

namespace standardBML
{
class two_layer_soil_profile : public differential_module
{
   public:
    two_layer_soil_profile(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          cws1{get_input(input_quantities, "cws1")},
          cws2{get_input(input_quantities, "cws2")},
          soil_depth1{get_input(input_quantities, "soil_depth1")},
          soil_depth2{get_input(input_quantities, "soil_depth2")},
          soil_depth3{get_input(input_quantities, "soil_depth3")},
          precip{get_input(input_quantities, "precip")},
          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")},
          soil_field_capacity{get_input(input_quantities, "soil_field_capacity")},
          soil_wilting_point{get_input(input_quantities, "soil_wilting_point")},
          soil_saturation_capacity{get_input(input_quantities, "soil_saturation_capacity")},
          soil_air_entry{get_input(input_quantities, "soil_air_entry")},
          soil_saturated_conductivity{get_input(input_quantities, "soil_saturated_conductivity")},
          soil_b_coefficient{get_input(input_quantities, "soil_b_coefficient")},
          soil_sand_content{get_input(input_quantities, "soil_sand_content")},
          phi1{get_input(input_quantities, "phi1")},
          phi2{get_input(input_quantities, "phi2")},
          wsFun{get_input(input_quantities, "wsFun")},
          Root{get_input(input_quantities, "Root")},
          lai{get_input(input_quantities, "lai")},
          temp{get_input(input_quantities, "temp")},
          solar{get_input(input_quantities, "solar")},
          windspeed{get_input(input_quantities, "windspeed")},
          rh{get_input(input_quantities, "rh")},
          hydrDist{get_input(input_quantities, "hydrDist")},
          rfl{get_input(input_quantities, "rfl")},
          rsec{get_input(input_quantities, "rsec")},
          rsdf{get_input(input_quantities, "rsdf")},
          soil_clod_size{get_input(input_quantities, "soil_clod_size")},
          soil_reflectance{get_input(input_quantities, "soil_reflectance")},
          soil_transmission{get_input(input_quantities, "soil_transmission")},
          specific_heat_of_air{get_input(input_quantities, "specific_heat_of_air")},
          soil_water_content{get_input(input_quantities, "soil_water_content")},
          par_energy_content{get_input(input_quantities, "par_energy_content")},

          // Get pointers to output quantities
          cws1_op{get_op(output_quantities, "cws1")},
          cws2_op{get_op(output_quantities, "cws2")},
          soil_water_content_op{get_op(output_quantities, "soil_water_content")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "two_layer_soil_profile"; }

   private:
    // References to input quantities
    double const& cws1;
    double const& cws2;
    double const& soil_depth1;
    double const& soil_depth2;
    double const& soil_depth3;
    double const& precip;
    double const& canopy_transpiration_rate;
    double const& soil_field_capacity;
    double const& soil_wilting_point;
    double const& soil_saturation_capacity;
    double const& soil_air_entry;
    double const& soil_saturated_conductivity;
    double const& soil_b_coefficient;
    double const& soil_sand_content;
    double const& phi1;
    double const& phi2;
    double const& wsFun;
    double const& Root;
    double const& lai;
    double const& temp;
    double const& solar;
    double const& windspeed;
    double const& rh;
    double const& hydrDist;
    double const& rfl;
    double const& rsec;
    double const& rsdf;
    double const& soil_clod_size;
    double const& soil_reflectance;
    double const& soil_transmission;
    double const& specific_heat_of_air;
    double const& soil_water_content;
    double const& par_energy_content;

    // Pointers to output quantities
    double* cws1_op;
    double* cws2_op;
    double* soil_water_content_op;

    // Main operation
    void do_operation() const;
};

string_vector two_layer_soil_profile::get_inputs()
{
    return {
        "cws1",
        "cws2",
        "soil_depth1",
        "soil_depth2",
        "soil_depth3",
        "precip",
        "canopy_transpiration_rate",
        "soil_field_capacity",
        "soil_wilting_point",
        "soil_saturation_capacity",
        "soil_air_entry",
        "soil_saturated_conductivity",
        "soil_b_coefficient",
        "soil_sand_content",
        "phi1",
        "phi2",
        "wsFun",
        "Root",
        "lai",        // dimensionless
        "temp",       // degrees C
        "solar",      // micromol / m^2 / s
        "windspeed",  // m / s
        "rh",         // dimensionless
        "hydrDist",
        "rfl",
        "rsec",
        "rsdf",
        "soil_clod_size",
        "soil_reflectance",
        "soil_transmission",
        "specific_heat_of_air",  // J / kg / mol
        "soil_water_content",    // dimensionless
        "par_energy_content"     // J / micromol
    };
}

string_vector two_layer_soil_profile::get_outputs()
{
    return {
        "cws1",
        "cws2",
        "soil_water_content"  // dimensionless
    };
}

void two_layer_soil_profile::do_operation() const
{
    // Collect inputs and make calculations
    double cws[] = {cws1, cws2};
    double soil_depths[] = {soil_depth1, soil_depth2, soil_depth3};

    struct soilML_str soilMLS = soilML(
        precip, canopy_transpiration_rate, cws, soil_depth3, soil_depths,
        soil_field_capacity, soil_wilting_point, soil_saturation_capacity,
        soil_air_entry, soil_saturated_conductivity, soil_b_coefficient,
        soil_sand_content, phi1, phi2, wsFun, 2 /* Always uses 2 layers */,
        Root, lai, 0.68, temp, solar, windspeed, rh, hydrDist, rfl, rsec, rsdf,
        soil_clod_size, soil_reflectance, soil_transmission,
        specific_heat_of_air, par_energy_content);

    double layer_one_depth = soil_depth2 - soil_depth1;
    double layer_two_depth = soil_depth3 - soil_depth2;

    double cws_mean =
        (soilMLS.cws[0] * layer_one_depth + soilMLS.cws[1] * layer_two_depth) /
        (layer_one_depth + layer_two_depth);

    // Update the output quantity list
    update(cws1_op, soilMLS.cws[0] - cws1);
    update(cws2_op, soilMLS.cws[1] - cws2);
    update(soil_water_content_op, cws_mean - soil_water_content);
}

}  // namespace standardBML
#endif
