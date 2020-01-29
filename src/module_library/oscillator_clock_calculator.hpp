#ifndef OSCILLATOR_CLOCK_CALCULATOR_H
#define OSCILLATOR_CLOCK_CALCULATOR_H

#include <cmath>
#include "../constants.h"
#include "../modules.h"

class oscillator_clock_calculator : public SteadyModule {
    public:
        oscillator_clock_calculator(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
            // Define basic module properties by passing its name to its parent class
            SteadyModule("oscillator_clock_calculator"),
            // Get pointers to input parameters
            doy_dbl_ip(get_ip(input_parameters, "doy_dbl")),
            kick_strength_ip(get_ip(input_parameters, "kick_strength")),
            night_tracker_ip(get_ip(input_parameters, "night_tracker")),
            day_tracker_ip(get_ip(input_parameters, "day_tracker")),
            light_ip(get_ip(input_parameters, "light")),
            dawn_b_ip(get_ip(input_parameters, "dawn_b")),
            dawn_a_ip(get_ip(input_parameters, "dawn_a")),
            dusk_b_ip(get_ip(input_parameters, "dusk_b")),
            dusk_a_ip(get_ip(input_parameters, "dusk_a")),
            ref_b_ip(get_ip(input_parameters, "ref_b")),
            ref_a_ip(get_ip(input_parameters, "ref_a")),
            // Get pointers to output parameters
            dawn_kick_op(get_op(output_parameters, "dawn_kick")),
            dusk_kick_op(get_op(output_parameters, "dusk_kick")),
            dawn_phase_op(get_op(output_parameters, "dawn_phase")),
            dusk_phase_op(get_op(output_parameters, "dusk_phase")),
            ref_phase_op(get_op(output_parameters, "ref_phase")),
            dawn_radius_op(get_op(output_parameters, "dawn_radius")),
            dusk_radius_op(get_op(output_parameters, "dusk_radius")),
            ref_radius_op(get_op(output_parameters, "ref_radius")),
            day_length_op(get_op(output_parameters, "day_length")),
            night_length_op(get_op(output_parameters, "night_length")),
            sunrise_op(get_op(output_parameters, "sunrise")),
            sunset_op(get_op(output_parameters, "sunset"))
        {}
        static std::vector<std::string> get_inputs();
        static std::vector<std::string> get_outputs();
    private:
        // Pointers to input parameters
        const double* doy_dbl_ip;
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
        // Pointers to output parameters
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

std::vector<std::string> oscillator_clock_calculator::get_inputs() {
    return {
        "doy_dbl",
        "kick_strength",
        "night_tracker",
        "day_tracker",
        "light",
        "dawn_b",
        "dawn_a",
        "dusk_b",
        "dusk_a",
        "ref_b",
        "ref_a"
    };
}

std::vector<std::string> oscillator_clock_calculator::get_outputs() {
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
        "sunset"
    };
}

/// This is like atan2 but with a range of [0, 2pi) instead of (-pi, pi]: When
/// the point (x, y) is in the third or fourth quadrant (i.e., when y < 0), add
/// 2pi to the value in (-pi, 0) returned by atan2 so that dawn_phase is always
/// in the interval [0, 2pi).
inline double range_adjusted_atan2(double y, double x) {
    using math_constants::pi;
    return (y >= 0) ? atan2(y, x)
                    : atan2(y, x) + 2 * pi;
}


void oscillator_clock_calculator::do_operation() const {
    //////////////////////////////////////////
    // Collect inputs and make calculations //
    //////////////////////////////////////////

    using math_constants::pi;
    
    // Get the current time value
    const double doy_dbl = *doy_dbl_ip;
    const double hour = 24.0 * (doy_dbl - floor(doy_dbl));
    
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
    const double day_length   = dusk_phase > dawn_phase ? (dawn_phase - dusk_phase + 2 * pi) * 12.0 / pi
                              :                           (dawn_phase - dusk_phase)          * 12.0 / pi;
    const double night_length = dawn_phase > dusk_phase ? (dusk_phase - dawn_phase + 2 * pi) * 12.0 / pi
                              :                           (dusk_phase - dawn_phase)          * 12.0 / pi;
                              
    // Calculate the sunrise and sunset times
    const double sunrise = dawn_phase * 12 / pi < hour ? (hour - dawn_phase * 12 / pi)
                         :                               (hour - dawn_phase * 12 / pi + 24.0);
    const double sunset  = dusk_phase * 12 / pi < hour ? (hour - dusk_phase * 12 / pi)
                         :                               (hour - dusk_phase * 12 / pi + 24.0);
    
    //////////////////////////////////////
    // Update the output parameter list //
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

#endif
