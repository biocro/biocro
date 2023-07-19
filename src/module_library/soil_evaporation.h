#ifndef SOIL_EVAPORATION_H
#define SOIL_EVAPORATION_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "BioCro.h"  // For SoilEvapo

namespace standardBML
{
class soil_evaporation : public direct_module
{
   public:
    soil_evaporation(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          lai{get_input(input_quantities, "lai")},
          temp{get_input(input_quantities, "temp")},
          solar{get_input(input_quantities, "solar")},
          soil_water_content{get_input(input_quantities, "soil_water_content")},
          soil_field_capacity{get_input(input_quantities, "soil_field_capacity")},
          soil_wilting_point{get_input(input_quantities, "soil_wilting_point")},
          windspeed{get_input(input_quantities, "windspeed")},
          rh{get_input(input_quantities, "rh")},
          rsec{get_input(input_quantities, "rsec")},
          soil_clod_size{get_input(input_quantities, "soil_clod_size")},
          soil_reflectance{get_input(input_quantities, "soil_reflectance")},
          soil_transmission{get_input(input_quantities, "soil_transmission")},
          specific_heat_of_air{get_input(input_quantities, "specific_heat_of_air")},
          par_energy_content{get_input(input_quantities, "par_energy_content")},

          // Get pointers to output quantities
          soil_evaporation_rate_op{get_op(output_quantities, "soil_evaporation_rate")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_evaporation"; }

   private:
    // Pointers to input quantities
    double const& lai;
    double const& temp;
    double const& solar;
    double const& soil_water_content;
    double const& soil_field_capacity;
    double const& soil_wilting_point;
    double const& windspeed;
    double const& rh;
    double const& rsec;
    double const& soil_clod_size;
    double const& soil_reflectance;
    double const& soil_transmission;
    double const& specific_heat_of_air;
    double const& par_energy_content;

    // Pointers to output quantities
    double* soil_evaporation_rate_op;

    // Main operation
    void do_operation() const;
};

string_vector soil_evaporation::get_inputs()
{
    return {
        "lai",                  // dimensionless
        "temp",                 // degrees C
        "solar",                // micromol / m^2 / s
        "soil_water_content",   // dimensionless
        "soil_field_capacity",  // dimensionless
        "soil_wilting_point",   // dimensionless
        "windspeed",            // m / s
        "rh",                   // dimensionless
        "rsec",
        "soil_clod_size",
        "soil_reflectance",
        "soil_transmission",
        "specific_heat_of_air",  // J / kg / K
        "par_energy_content"     // J / micromol
    };
}

string_vector soil_evaporation::get_outputs()
{
    return {
        "soil_evaporation_rate"  // Mg / ha / hr
    };
}

void soil_evaporation::do_operation() const
{
    // Collect inputs and make calculations
    // SoilEvapo(...) is located in AuxBioCro.cpp
    double soilEvap = SoilEvapo(
        lai, 0.68, temp, solar, soil_water_content, soil_field_capacity,
        soil_wilting_point, windspeed, rh, rsec, soil_clod_size,
        soil_reflectance, soil_transmission, specific_heat_of_air,
        par_energy_content);  // kg / m^2 / s

    // Convert units for consistency with canopy_transpiration_rate and
    // two_layer_soil_profile's output
    soilEvap *= 3600 * 1e-3 * 10000;  // Mg / ha / hr

    // Update the output quantity list
    update(soil_evaporation_rate_op, soilEvap);
}

}  // namespace standardBML
#endif
