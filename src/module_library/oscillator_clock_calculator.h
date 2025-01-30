#ifndef OSCILLATOR_CLOCK_CALCULATOR_H
#define OSCILLATOR_CLOCK_CALCULATOR_H

#include <cmath>
#include "../framework/constants.h"
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class oscillator_clock_calculator : public direct_module
{
   public:
    oscillator_clock_calculator(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          time_ip{get_ip(input_quantities, "time")},
          kick_strength_ip{get_ip(input_quantities, "kick_strength")},
          night_tracker_ip{get_ip(input_quantities, "night_tracker")},
          day_tracker_ip{get_ip(input_quantities, "day_tracker")},
          light_ip{get_ip(input_quantities, "light")},
          dawn_b_ip{get_ip(input_quantities, "dawn_b")},
          dawn_a_ip{get_ip(input_quantities, "dawn_a")},
          dusk_b_ip{get_ip(input_quantities, "dusk_b")},
          dusk_a_ip{get_ip(input_quantities, "dusk_a")},
          ref_b_ip{get_ip(input_quantities, "ref_b")},
          ref_a_ip{get_ip(input_quantities, "ref_a")},

          // Get pointers to output quantities
          dawn_kick_op{get_op(output_quantities, "dawn_kick")},
          dusk_kick_op{get_op(output_quantities, "dusk_kick")},
          dawn_phase_op{get_op(output_quantities, "dawn_phase")},
          dusk_phase_op{get_op(output_quantities, "dusk_phase")},
          ref_phase_op{get_op(output_quantities, "ref_phase")},
          dawn_radius_op{get_op(output_quantities, "dawn_radius")},
          dusk_radius_op{get_op(output_quantities, "dusk_radius")},
          ref_radius_op{get_op(output_quantities, "ref_radius")},
          day_length_op{get_op(output_quantities, "day_length")},
          night_length_op{get_op(output_quantities, "night_length")},
          sunrise_op{get_op(output_quantities, "sunrise")},
          sunset_op{get_op(output_quantities, "sunset")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "oscillator_clock_calculator"; }

   private:
    // Pointers to input quantities
    const double* time_ip;
    const double* kick_strength_ip;
    const double* night_tracker_ip;
    const double* day_tracker_ip;
    const double* light_ip;
    const double* dawn_b_ip;
    const double* dawn_a_ip;
    const double* dusk_b_ip;
    const double* dusk_a_ip;
    const double* ref_b_ip;
    const double* ref_a_ip;

    // Pointers to output quantities
    double* dawn_kick_op;
    double* dusk_kick_op;
    double* dawn_phase_op;
    double* dusk_phase_op;
    double* ref_phase_op;
    double* dawn_radius_op;
    double* dusk_radius_op;
    double* ref_radius_op;
    double* day_length_op;
    double* night_length_op;
    double* sunrise_op;
    double* sunset_op;

    // Main operation
    void do_operation() const;
};

string_vector oscillator_clock_calculator::get_inputs()
{
    return {
        "time",
        "kick_strength",
        "night_tracker",
        "day_tracker",
        "light",
        "dawn_b",
        "dawn_a",
        "dusk_b",
        "dusk_a",
        "ref_b",
        "ref_a"};
}

string_vector oscillator_clock_calculator::get_outputs()
{
    return {
        "dawn_kick",
        "dusk_kick",
        "dawn_phase",
        "dusk_phase",
        "ref_phase",
        "dawn_radius",
        "dusk_radius",
        "ref_radius",
        "day_length",
        "night_length",
        "sunrise",
        "sunset"};
}

/// This is like atan2 but with a range of [0, 2pi) instead of (-pi, pi]: When
/// the point (x, y) is in the third or fourth quadrant (i.e., when y < 0), add
/// 2pi to the value in (-pi, 0) returned by atan2 so that dawn_phase is always
/// in the interval [0, 2pi).
inline double range_adjusted_atan2(double y, double x)
{
    using math_constants::pi;
    return (y >= 0) ? atan2(y, x)
                    : atan2(y, x) + 2 * pi;
}

void oscillator_clock_calculator::do_operation() const
{
    //////////////////////////////////////////
    // Collect inputs and make calculations //
    //////////////////////////////////////////

    using math_constants::pi;

    // Get the current time value
    const double time = *time_ip;
    const double hour = std::fmod(time,  24);

    // Get the current light value
    const double light = *light_ip;

    // Get the current state of the night and day trackers
    const double night_tracker = *night_tracker_ip;
    const double day_tracker = *day_tracker_ip;

    // Get the kick strength
    const double kick_strength = *kick_strength_ip;

    // Calculate the kicks
    // The dawn kick is created by taking the night tracker value during
    //  the day, which is just a short decay portion
    // The dusk kick is created by taking the day tracker value during
    //  the night, which is just a short decay portion
    const double dawn_kick = light * kick_strength * night_tracker;
    const double dusk_kick = (1.0 - light) * kick_strength * day_tracker;

    // Get the current state of the dawn and dusk tracking oscillators
    const double dawn_b = *dawn_b_ip;
    const double dawn_a = *dawn_a_ip;
    const double dusk_b = *dusk_b_ip;
    const double dusk_a = *dusk_a_ip;
    const double ref_b = *ref_b_ip;
    const double ref_a = *ref_a_ip;

    // Calculate the dawn phase angle, which is zero around dawn and increases
    // throughout the day.
    const double dawn_phase = range_adjusted_atan2(dawn_b, dawn_a);

    // Calculate the dusk phase angle, which is zero around dusk and increases
    // throughout the night.
    const double dusk_phase = range_adjusted_atan2(dusk_b, dusk_a);

    // Calculate the reference phase angle, which is not coupled to the light.
    const double ref_phase = range_adjusted_atan2(ref_b, ref_a);

    // Calculate the day and night length indicators (in hours):
    const double day_length = dusk_phase > dawn_phase ? (dawn_phase - dusk_phase + 2 * pi) * 12.0 / pi
                                                      : (dawn_phase - dusk_phase) * 12.0 / pi;
    const double night_length = dawn_phase > dusk_phase ? (dusk_phase - dawn_phase + 2 * pi) * 12.0 / pi
                                                        : (dusk_phase - dawn_phase) * 12.0 / pi;

    // Calculate the sunrise and sunset times
    const double sunrise = dawn_phase * 12 / pi < hour ? (hour - dawn_phase * 12 / pi)
                                                       : (hour - dawn_phase * 12 / pi + 24.0);
    const double sunset = dusk_phase * 12 / pi < hour ? (hour - dusk_phase * 12 / pi)
                                                      : (hour - dusk_phase * 12 / pi + 24.0);

    //////////////////////////////////////
    // Update the output quantity list //
    //////////////////////////////////////

    update(dawn_kick_op, dawn_kick);
    update(dusk_kick_op, dusk_kick);
    update(dawn_phase_op, dawn_phase);
    update(dusk_phase_op, dusk_phase);
    update(ref_phase_op, ref_phase);
    update(dawn_radius_op, sqrt(dawn_a * dawn_a + dawn_b * dawn_b));
    update(dusk_radius_op, sqrt(dusk_a * dusk_a + dusk_b * dusk_b));
    update(ref_radius_op, sqrt(ref_a * ref_a + ref_b * ref_b));
    update(day_length_op, day_length);
    update(night_length_op, night_length);
    update(sunrise_op, sunrise);
    update(sunset_op, sunset);
}

}  // namespace standardBML
#endif
