#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <cmath>  // for pow
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class harmonic_oscillator : public differential_module
{
   public:
    harmonic_oscillator(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          mass{get_input(input_quantities, "mass")},
          spring_constant{get_input(input_quantities, "spring_constant")},
          position{get_input(input_quantities, "position")},
          velocity{get_input(input_quantities, "velocity")},

          // Get pointers to output quantities
          position_op{get_op(output_quantities, "position")},
          velocity_op{get_op(output_quantities, "velocity")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "harmonic_oscillator"; }

   private:
    // References to input quantities
    double const& mass;
    double const& spring_constant;
    double const& position;
    double const& velocity;

    // Pointers to output quantities
    double* position_op;
    double* velocity_op;

    // Main operation
    void do_operation() const;
};

string_vector harmonic_oscillator::get_inputs()
{
    return {
        "mass",             // kg
        "spring_constant",  // N / m
        "position",         // m
        "velocity"          // m / s
    };
}

string_vector harmonic_oscillator::get_outputs()
{
    return {
        "position",  // m / s
        "velocity"   // m / s^2
    };
}

void harmonic_oscillator::do_operation() const
{
    update(position_op, velocity);
    update(velocity_op, -1.0 * spring_constant * position / mass);
}

class harmonic_energy : public direct_module
{
   public:
    harmonic_energy(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          mass{get_input(input_quantities, "mass")},
          spring_constant{get_input(input_quantities, "spring_constant")},
          position{get_input(input_quantities, "position")},
          velocity{get_input(input_quantities, "velocity")},

          // Get pointers to output quantities
          kinetic_energy_op{get_op(output_quantities, "kinetic_energy")},
          spring_energy_op{get_op(output_quantities, "spring_energy")},
          total_energy_op{get_op(output_quantities, "total_energy")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "harmonic_energy"; }

   private:
    // Pointers to input quantities
    double const& mass;
    double const& spring_constant;
    double const& position;
    double const& velocity;

    // Pointers to output quantities
    double* kinetic_energy_op;
    double* spring_energy_op;
    double* total_energy_op;

    // Main operation
    void do_operation() const;
};

string_vector harmonic_energy::get_inputs()
{
    return {
        "mass",             // kg
        "spring_constant",  // N / m
        "position",         // m
        "velocity"          // m / s
    };
}

string_vector harmonic_energy::get_outputs()
{
    return {
        "kinetic_energy",  // J
        "spring_energy",   // J
        "total_energy"     // J
    };
}

void harmonic_energy::do_operation() const
{
    double const kinetic_energy = 0.5 * mass * pow(velocity, 2);
    double const spring_energy = 0.5 * spring_constant * pow(position, 2);

    update(kinetic_energy_op, kinetic_energy);
    update(spring_energy_op, spring_energy);
    update(total_energy_op, kinetic_energy + spring_energy);
}

}  // namespace standardBML
#endif
