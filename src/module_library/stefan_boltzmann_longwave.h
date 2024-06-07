#ifndef STEFAN_BOLTZMANN_LONGWAVE_H
#define STEFAN_BOLTZMANN_LONGWAVE_H

#include <cmath>  // for pow
#include "../framework/module.h"
#include "../framework/state_map.h"
#include "../framework/constants.h"  // for celsius_to_kelvin, stefan_boltzmann

namespace standardBML
{
/**
 * @class stefan_boltzmann_longwave
 *
 * @brief
 */
class stefan_boltzmann_longwave : public direct_module
{
   public:
    stefan_boltzmann_longwave(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          emissivity_sky{get_input(input_quantities, "emissivity_sky")},
          temp{get_input(input_quantities, "temp")},

          // Get pointers to output quantities
          absorbed_longwave_op{get_op(output_quantities, "absorbed_longwave")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "stefan_boltzmann_longwave"; }

   private:
    // References to input quantities
    double const& emissivity_sky;
    double const& temp;

    // Pointers to output quantities
    double* absorbed_longwave_op;

    // Main operation
    void do_operation() const;
};

string_vector stefan_boltzmann_longwave::get_inputs()
{
    return {
        "emissivity_sky",  // dimensionless
        "temp"             // degrees C
    };
}

string_vector stefan_boltzmann_longwave::get_outputs()
{
    return {
        "absorbed_longwave"  // J / m^2 / s
    };
}

void stefan_boltzmann_longwave::do_operation() const
{
    double const absorbed_longwave =
        emissivity_sky * physical_constants::stefan_boltzmann *
        pow(conversion_constants::celsius_to_kelvin + temp, 4);  // J / m^2 / s

    // Update the output quantity list
    update(absorbed_longwave_op, absorbed_longwave);  // J / m^2 / s
}

}  // namespace standardBML
#endif
