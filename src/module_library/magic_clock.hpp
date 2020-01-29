#ifndef MAGIC_CLOCK_H
#define MAGIC_CLOCK_H

#include <cmath>
#include "../constants.h"
#include "../modules.h"

class magic_clock : public DerivModule {
    public:
        magic_clock(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
            // Define basic module properties by passing its name to its parent class
            DerivModule("magic_clock"),
            // Get pointers to input parameters
            dawn_kick_ip(get_ip(input_parameters, "dawn_kick")),
            dusk_kick_ip(get_ip(input_parameters, "dusk_kick")),
            dawn_b_ip(get_ip(input_parameters, "dawn_b")),
            dawn_a_ip(get_ip(input_parameters, "dawn_a")),
            dusk_b_ip(get_ip(input_parameters, "dusk_b")),
            dusk_a_ip(get_ip(input_parameters, "dusk_a")),
            ref_b_ip(get_ip(input_parameters, "ref_b")),
            ref_a_ip(get_ip(input_parameters, "ref_a")),
            clock_gamma_ip(get_ip(input_parameters, "clock_gamma")),
            clock_r0_ip(get_ip(input_parameters, "clock_r0")),
            clock_period_ip(get_ip(input_parameters, "clock_period")),
            // Get pointers to output parameters
            dawn_b_op(get_op(output_parameters, "dawn_b")),
            dawn_a_op(get_op(output_parameters, "dawn_a")),
            dusk_b_op(get_op(output_parameters, "dusk_b")),
            dusk_a_op(get_op(output_parameters, "dusk_a")),
            ref_b_op(get_op(output_parameters, "ref_b")),
            ref_a_op(get_op(output_parameters, "ref_a"))
        {}
        static std::vector<std::string> get_inputs();
        static std::vector<std::string> get_outputs();
    private:
        // Pointers to input parameters
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
        // Pointers to output parameters
        double* dawn_b_op;
        double* dawn_a_op;
        double* dusk_b_op;
        double* dusk_a_op;
        double* ref_b_op;
        double* ref_a_op;
        // Main operation
        void do_operation() const;
};

std::vector<std::string> magic_clock::get_inputs() {
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
        "clock_period"
    };
}

std::vector<std::string> magic_clock::get_outputs() {
    return {
        "dawn_b",
        "dawn_a",
        "dusk_b",
        "dusk_a",
        "ref_b",
        "ref_a"
    };
}

void magic_clock::do_operation() const {
    //////////////////////////////////////////
    // Collect inputs and make calculations //
    //////////////////////////////////////////

    using math_constants::pi;
    
    // Get the current values of the dawn and dusk kicks
    const double dawn_kick = *dawn_kick_ip;
    const double dusk_kick = *dusk_kick_ip;
    
    // Get the current state of the dawn tracking oscillator
    const double dawn_b = *dawn_b_ip;
    const double dawn_a = *dawn_a_ip;
    
    // Get the current state of the dusk tracking oscillator
    const double dusk_b = *dusk_b_ip;
    const double dusk_a = *dusk_a_ip;
    
    // Get the current state of the reference oscillator
    const double ref_b = *ref_b_ip;
    const double ref_a = *ref_a_ip;
    
    // Define the constants for the tracking oscillators
    const double natural_period = *clock_period_ip;           // Natural period in hours
    const double natural_freq = 2.0 * pi / natural_period;    // Corresponding angular frequency in radians per hour
    const double gamma = *clock_gamma_ip;
    const double r_0 = *clock_r0_ip;
    
    // Calculate the friction terms for the tracking oscillators
    const double dawn_friction = gamma * (r_0 - sqrt(dawn_a * dawn_a + dawn_b * dawn_b));
    const double dusk_friction = gamma * (r_0 - sqrt(dusk_a * dusk_a + dusk_b * dusk_b));
    const double  ref_friction = gamma * (r_0 - sqrt( ref_a *  ref_a +  ref_b *  ref_b));
    
    //////////////////////////////////////
    // Update the output parameter list //
    //////////////////////////////////////
    
    // The dawn tracking oscillator is driven by a small kick at dawn
    // The kick is created by taking the night tracker value during the day,
    //  which is just a short decay portion
    update(dawn_b_op, natural_freq * dawn_a);
    update(dawn_a_op, dawn_friction * dawn_a - natural_freq * dawn_b + dawn_kick);
    
    // The dusk tracking oscillator is driven by a small kick at dusk
    // The kick is created by taking the day tracker value during the night,
    //  which is just a short decay portion
    update(dusk_b_op, natural_freq * dusk_a);
    update(dusk_a_op, dusk_friction * dusk_a - natural_freq * dusk_b + dusk_kick);
    
    // The reference oscillator is not driven
    update(ref_b_op, natural_freq * ref_a);
    update(ref_a_op, ref_friction * ref_a - natural_freq * ref_b);
}

#endif
