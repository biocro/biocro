#ifndef ED_LONG_WAVE_ENERGY_LOSS_H
#define ED_LONG_WAVE_ENERGY_LOSS_H

#include <cmath>  // For pow
#include "../modules.h"
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
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_long_wave_energy_loss"),
          // Get pointers to input parameters
          temperature_leaf_ip(get_ip(input_parameters, "temperature_leaf")),
          temperature_air_ip(get_ip(input_parameters, "temp")),
          emissivity_leaf_ip(get_ip(input_parameters, "emissivity_leaf")),
          // Get pointers to output parameters
          long_wave_energy_loss_leaf_op(get_op(output_parameters, "long_wave_energy_loss_leaf"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* temperature_leaf_ip;
    const double* temperature_air_ip;
    const double* emissivity_leaf_ip;
    // Pointers to output parameters
    double* long_wave_energy_loss_leaf_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_long_wave_energy_loss::get_inputs()
{
    return {
        "temperature_leaf",  // deg. C
        "temp",              // deg. C
        "emissivity_leaf"    // dimensionless
    };
}

std::vector<std::string> ed_long_wave_energy_loss::get_outputs()
{
    return {
        "long_wave_energy_loss_leaf"  // W / m^2
    };
}

void ed_long_wave_energy_loss::do_operation() const
{
    using physical_constants::celsius_to_kelvin;
    using physical_constants::stefan_boltzmann;

    update(long_wave_energy_loss_leaf_op,
           *emissivity_leaf_ip * stefan_boltzmann * (pow(*temperature_leaf_ip + celsius_to_kelvin, 4) - pow(*temperature_air_ip + celsius_to_kelvin, 4)));
}

#endif
