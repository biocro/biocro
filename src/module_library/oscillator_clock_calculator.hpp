#ifndef OSCILLATOR_CLOCK_CALCULATOR_H
#define OSCILLATOR_CLOCK_CALCULATOR_H

#include "../modules.h"

class oscillator_clock_calculator : public SteadyModule {
    public:
        oscillator_clock_calculator(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
            // Define basic module properties by passing its name to its parent class
            SteadyModule("oscillator_clock_calculator"),
            // Get pointers to input parameters
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
            day_length_op(get_op(output_parameters, "day_length")),
            night_length_op(get_op(output_parameters, "night_length"))
        {}
        static std::vector<std::string> get_inputs();
        static std::vector<std::string> get_outputs();
    private:
        // Pointers to input parameters
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
        double* day_length_op;
        double* night_length_op;
        // Main operation
        void do_operation() const;
};

std::vector<std::string> oscillator_clock_calculator::get_inputs() {
    return {
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
        "day_length",
        "night_length"
    };
}

void oscillator_clock_calculator::do_operation() const {
    //////////////////////////////////////////
    // Collect inputs and make calculations //
    //////////////////////////////////////////
    
    // Get the current light value
    double light = *light_ip;
    
    // Get the current state of the night and day trackers
    double night_tracker = *night_tracker_ip;
    double day_tracker = *day_tracker_ip;
    
    // Get the kick strength
    double kick_strength = *kick_strength_ip;
    
    // Calculate the kicks
    // The dawn kick is created by taking the night tracker value during
    //  the day, which is just a short decay portion
    // The dusk kick is created by taking the day tracker value during
    //  the night, which is just a short decay portion
    double dawn_kick = light * kick_strength * night_tracker;
    double dusk_kick = (1.0 - light) * kick_strength * day_tracker;
    
    // Get the current state of the dawn and dusk tracking oscillators
    double dawn_b = *dawn_b_ip;
    double dawn_a = *dawn_a_ip;
    double dusk_b = *dusk_b_ip;
    double dusk_a = *dusk_a_ip;
    double ref_b = *ref_b_ip;
    double ref_a = *ref_a_ip;
    
    // Calculate the dawn phase angle, which is zero around dawn
    //  and increases throughout the day
    double dawn_phase = atan2(dawn_b, dawn_a);      // Output lies within [-pi,pi]
    if(dawn_phase < 0) dawn_phase += 2 * M_PI;      // Change output domain to [0,2*pi)
    
    // Calculate the dusk phase angle, which is zero around dusk
    //  and increases throughout the night
    double dusk_phase = atan2(dusk_b, dusk_a);      // Output lies within [-pi,pi]
    if(dusk_phase < 0) dusk_phase += 2  *M_PI;      // Change output domain to [0,2*pi)
    
    // Calculate the reference phase angle, which is not coupled
    //  to the light
    double ref_phase = atan2(ref_b, ref_a);         // Output lies within [-pi,pi]
    if(ref_phase < 0) ref_phase += 2 * M_PI;        // Change output domain to [0,2*pi)
    
    // Calculate the day and night length indicators
    double day_length = dusk_phase > dawn_phase ? (dawn_phase - dusk_phase + 2 * M_PI) * 12.0 / M_PI : (dawn_phase - dusk_phase) * 12.0 / M_PI;
    double night_length = dawn_phase > dusk_phase ? (dusk_phase - dawn_phase + 2 * M_PI) * 12.0 / M_PI : (dusk_phase - dawn_phase) * 12.0 / M_PI;
    
    //////////////////////////////////////
    // Update the output parameter list //
    //////////////////////////////////////
    
    update(dawn_kick_op, dawn_kick);
    update(dusk_kick_op, dusk_kick);
    update(dawn_phase_op, dawn_phase);
    update(dusk_phase_op, dusk_phase);
    update(ref_phase_op, ref_phase);
    update(day_length_op, day_length);
    update(night_length_op, night_length);
}

#endif