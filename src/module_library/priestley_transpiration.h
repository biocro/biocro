#ifndef PRIESTLEY_TRANSPIRATION_H
#define PRIESTLEY_TRANSPIRATION_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class priestley_transpiration : public direct_module
{
   public:
    priestley_transpiration(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          slope_water_vapor_ip{get_ip(input_quantities, "slope_water_vapor")},
          psychrometric_parameter_ip{get_ip(input_quantities, "psychrometric_parameter")},
          latent_heat_vaporization_of_water_ip{get_ip(input_quantities, "latent_heat_vaporization_of_water")},
          PhiN_ip{get_ip(input_quantities, "PhiN")},

          // Get pointers to output quantities
          transpiration_rate_op{get_op(output_quantities, "transpiration_rate")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "priestley_transpiration"; }

   private:
    // Pointers to input quantities
    const double* slope_water_vapor_ip;
    const double* psychrometric_parameter_ip;
    const double* latent_heat_vaporization_of_water_ip;
    const double* PhiN_ip;

    // Pointers to output quantities
    double* transpiration_rate_op;

    // Main operation
    void do_operation() const;
};

string_vector priestley_transpiration::get_inputs()
{
    return {
        "slope_water_vapor",
        "psychrometric_parameter",
        "latent_heat_vaporization_of_water",
        "PhiN"};
}

string_vector priestley_transpiration::get_outputs()
{
    return {
        "transpiration_rate"};
}

void priestley_transpiration::do_operation() const
{
    // Collect input quantities and make calculations

    double slope_water_vapor = *slope_water_vapor_ip;       // kg / m^3 / K
    double psychr_parameter = *psychrometric_parameter_ip;  // kg / m^3 / K
    double LHV = *latent_heat_vaporization_of_water_ip;     // J / kg
    double PhiN = *PhiN_ip;                                 // In penman_monteith_transpiration, PhiN is retrieved from "leaf_net_irradiance" (EBL)

    double evapotranspiration = 1.26 * slope_water_vapor * PhiN / (LHV * (slope_water_vapor + psychr_parameter));

    // Update the output quantity list
    update(transpiration_rate_op, evapotranspiration);  // kg / m^2 / s. Leaf area basis.
}

}  // namespace standardBML
#endif
