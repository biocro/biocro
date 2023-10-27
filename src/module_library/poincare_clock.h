#ifndef POINCARE_CLOCK_H
#define POINCARE_CLOCK_H

#include <cmath>
#include "../framework/constants.h"
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class poincare_clock : public differential_module
{
   public:
    poincare_clock(state_map const& input_quantities, state_map* output_quantities)
        : differential_module{},

          // Get pointers to input quantities
          dawn_kick_ip{get_ip(input_quantities, "dawn_kick")},
          dusk_kick_ip{get_ip(input_quantities, "dusk_kick")},
          dawn_b_ip{get_ip(input_quantities, "dawn_b")},
          dawn_a_ip{get_ip(input_quantities, "dawn_a")},
          dusk_b_ip{get_ip(input_quantities, "dusk_b")},
          dusk_a_ip{get_ip(input_quantities, "dusk_a")},
          ref_b_ip{get_ip(input_quantities, "ref_b")},
          ref_a_ip{get_ip(input_quantities, "ref_a")},
          clock_gamma_ip{get_ip(input_quantities, "clock_gamma")},
          clock_r0_ip{get_ip(input_quantities, "clock_r0")},
          clock_period_ip{get_ip(input_quantities, "clock_period")},

          // Get pointers to output quantities
          dawn_b_op{get_op(output_quantities, "dawn_b")},
          dawn_a_op{get_op(output_quantities, "dawn_a")},
          dusk_b_op{get_op(output_quantities, "dusk_b")},
          dusk_a_op{get_op(output_quantities, "dusk_a")},
          ref_b_op{get_op(output_quantities, "ref_b")},
          ref_a_op{get_op(output_quantities, "ref_a")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "poincare_clock"; }

   private:
    // Pointers to input quantities
    const double* dawn_kick_ip;
    const double* dusk_kick_ip;
    const double* dawn_b_ip;
    const double* dawn_a_ip;
    const double* dusk_b_ip;
    const double* dusk_a_ip;
    const double* ref_b_ip;
    const double* ref_a_ip;
    const double* clock_gamma_ip;
    const double* clock_r0_ip;
    const double* clock_period_ip;

    // Pointers to output quantities
    double* dawn_b_op;
    double* dawn_a_op;
    double* dusk_b_op;
    double* dusk_a_op;
    double* ref_b_op;
    double* ref_a_op;

    // Main operation
    void do_operation() const;
};

string_vector poincare_clock::get_inputs()
{
    return {
        "dawn_kick",
        "dusk_kick",
        "dawn_b",
        "dawn_a",
        "dusk_b",
        "dusk_a",
        "ref_b",
        "ref_a",
        "clock_gamma",
        "clock_r0",
        "clock_period"};
}

string_vector poincare_clock::get_outputs()
{
    return {
        "dawn_b",
        "dawn_a",
        "dusk_b",
        "dusk_a",
        "ref_b",
        "ref_a"};
}

void poincare_clock::do_operation() const
{
    //////////////////////////////////////////
    // Collect inputs and make calculations //
    //////////////////////////////////////////

    using math_constants::pi;

    // Get the current values of the dawn and dusk kicks
    double dawn_kick = *dawn_kick_ip;
    double dusk_kick = *dusk_kick_ip;

    // Get the current state of the dawn tracking oscillator
    double dawn_b = *dawn_b_ip;
    double dawn_a = *dawn_a_ip;

    // Get the current state of the dusk tracking oscillator
    double dusk_b = *dusk_b_ip;
    double dusk_a = *dusk_a_ip;

    // Get the current state of the reference oscillator
    double ref_b = *ref_b_ip;
    double ref_a = *ref_a_ip;

    // Define the constants for the tracking oscillators
    const double natural_period = *clock_period_ip;         // Natural period in hours
    const double natural_freq = 2.0 * pi / natural_period;  // Corresponding angular frequency in radians per hour
    const double gamma = *clock_gamma_ip;
    const double r_0 = *clock_r0_ip;

    // Calculate the friction terms for the tracking oscillators
    const double dawn_friction = gamma * (r_0 - sqrt(dawn_a * dawn_a + dawn_b * dawn_b));
    const double dusk_friction = gamma * (r_0 - sqrt(dusk_a * dusk_a + dusk_b * dusk_b));
    const double ref_friction = gamma * (r_0 - sqrt(ref_a * ref_a + ref_b * ref_b));

    //////////////////////////////////////
    // Update the output quantity list //
    //////////////////////////////////////

    // The dawn tracking oscillator is driven by a small kick at dawn
    update(dawn_b_op, dawn_friction * dawn_b + natural_freq * dawn_a);
    update(dawn_a_op, dawn_friction * dawn_a - natural_freq * dawn_b + dawn_kick);

    // The dusk tracking oscillator is driven by a small kick at dusk
    update(dusk_b_op, dusk_friction * dusk_b + natural_freq * dusk_a);
    update(dusk_a_op, dusk_friction * dusk_a - natural_freq * dusk_b + dusk_kick);

    // The reference oscillator is not driven
    update(ref_b_op, ref_friction * ref_b + natural_freq * ref_a);
    update(ref_a_op, ref_friction * ref_a - natural_freq * ref_b);
}

}  // namespace standardBML
#endif
