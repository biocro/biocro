#ifndef STOMATA_WATER_STRESS_RESISTANCE_H
#define STOMATA_WATER_STRESS_RESISTANCE_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class stomata_water_stress_resistance: public direct_module 
{
   public:
    stomata_water_stress_resistance(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          Catm_ip{get_ip(input_quantities, "Catm")},
          uptake_laststep_ip{get_ip(input_quantities, "uptake_laststep")},
          soil_field_capacity_ip{get_ip(input_quantities, "soil_field_capacity")},
          soil_wilting_point_ip{get_ip(input_quantities, "soil_wilting_point")},
          soil_water_content_ip{get_ip(input_quantities, "soil_water_content")},

          // Get pointers to output quantities
          StomataWS_op{get_op(output_quantities, "StomataWS")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "stomata_water_stress_resistance"; }

   private:
    // Pointers to input quantities
    const double* Catm_ip;
    const double* uptake_laststep_ip;
    const double* soil_field_capacity_ip;
    const double* soil_wilting_point_ip;
    const double* soil_water_content_ip;

    // Pointers to output quantities
    double* StomataWS_op;

    // Main operation
    void do_operation() const;
};

string_vector stomata_water_stress_resistance::get_inputs()
{
    return {
        "Catm",
        "uptake_laststep",
        "soil_field_capacity",
        "soil_wilting_point",
        "soil_water_content"};
}

string_vector stomata_water_stress_resistance::get_outputs()
{
    return {
        "StomataWS"};
}

void stomata_water_stress_resistance::do_operation() const
{
    double resistance_base = 0.01;    // higher = faster response
    // Collect inputs and make calculations
    double Catm = *Catm_ip;
// eCO2 plants are 5x more sensitive to flow/drying
// As xylem ABA increased (drought signal), the eCO2 plants closed their stomata more aggressively than ambient plants
    if (Catm > 500) resistance_base *= 5.0;
    double uptake_laststep    = *uptake_laststep_ip;
    double soil_wilting_point = *soil_wilting_point_ip;
    double soil_field_capacity = *soil_field_capacity_ip;
    double soil_water_content = *soil_water_content_ip;
//starts linear stress a bit lower than the fc, mimicing the "Readily Available Water" plateau
    double RAW_sf = 0.8;
    double slope  = 1.0 / (RAW_sf*soil_field_capacity - soil_wilting_point);
    double intercept = 1.0 - RAW_sf*soil_field_capacity * slope;
     
    double x = std::min(std::max(slope * soil_water_content + intercept, 1e-10), 1.0);

//    The Safety Valve: Dynamic Resistance
    // At x=1 (Wet): R = base. 
    // At x=0.5 (Dry): R = 2*base. (StomataWS drops significantly).
    double resistance_dynamic = resistance_base / x ;

    double f_ws = x - resistance_dynamic * std::abs(uptake_laststep); 
    f_ws   = std::min(std::max(f_ws, 1e-10),1.0);

    // Update the output quantity list
    update(StomataWS_op, f_ws); 
}

}  // namespace standardBML
#endif
