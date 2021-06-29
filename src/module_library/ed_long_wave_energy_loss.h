#ifndef ED_LONG_WAVE_ENERGY_LOSS_H
#define ED_LONG_WAVE_ENERGY_LOSS_H

#include <cmath>  // For pow
#include "../modules.h"
#include "../state_map.h"
#include "../constants.h"

/**
 * @class ed_long_wave_energy_loss
 *
 * @brief Uses the Stefan-Boltzmann law to calculate long-wave energy losses
 * from the leaf to the air. Currently only intended for use by Ed.
 */
class ed_long_wave_energy_loss : public SteadyModule
{
   public:
    ed_long_wave_energy_loss(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_long_wave_energy_loss"),

          // Get references to input quantities
          temperature_leaf{get_input(input_quantities, "temperature_leaf")},
          temperature_air{get_input(input_quantities, "temp")},
          emissivity_leaf{get_input(input_quantities, "emissivity_leaf")},

          // Get pointers to output quantities
          long_wave_energy_loss_leaf_op{get_op(output_quantities, "long_wave_energy_loss_leaf")}

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // References to input quantities
    double const& temperature_leaf;
    double const& temperature_air;
    double const& emissivity_leaf;

    // Pointers to output quantities
    double* long_wave_energy_loss_leaf_op;

    // Main operation
    void do_operation() const override;
};

string_vector ed_long_wave_energy_loss::get_inputs()
{
    return {
        "temperature_leaf",  // deg. C
        "temp",              // deg. C
        "emissivity_leaf"    // dimensionless
    };
}

string_vector ed_long_wave_energy_loss::get_outputs()
{
    return {
        "long_wave_energy_loss_leaf"  // W / m^2
    };
}

void ed_long_wave_energy_loss::do_operation() const
{
    using conversion_constants::celsius_to_kelvin;
    using physical_constants::stefan_boltzmann;

    double const leaf_temp_term =
        pow(temperature_leaf + celsius_to_kelvin, 4);  // K^4

    double const air_temp_term =
        pow(temperature_leaf + celsius_to_kelvin, 4);  // K^4

    double const net_energy_loss_leaf =
        emissivity_leaf * stefan_boltzmann *
        (leaf_temp_term - air_temp_term);  // W / m^2

    update(long_wave_energy_loss_leaf_op, net_energy_loss_leaf);
}

#endif
