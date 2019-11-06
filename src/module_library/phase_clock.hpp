#ifndef PHASE_CLOCK_H
#define PHASE_CLOCK_H

#include "../modules.h"

class phase_clock : public DerivModule {
    public:
        phase_clock(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
            // Define basic module properties by passing its name to its parent class
            DerivModule("phase_clock"),
            // Get pointers to input parameters
            phi_ip(get_ip(input_parameters, "phi")),
            light_ip(get_ip(input_parameters, "light")),
            clock_dead_width_ip(get_ip(input_parameters, "clock_dead_width")),
            clock_width_asymm_ip(get_ip(input_parameters, "clock_width_asymm")),
            clock_area_asymm_ip(get_ip(input_parameters, "clock_area_asymm")),
            clock_r_scale_ip(get_ip(input_parameters, "clock_r_scale")),
            clock_period_ip(get_ip(input_parameters, "clock_period")),
            // Get pointers to output parameters
            phi_op(get_op(output_parameters, "phi"))
        {}
        static std::vector<std::string> get_inputs();
        static std::vector<std::string> get_outputs();
    private:
        // Pointers to input parameters
        const double* phi_ip;
        const double* light_ip;
        const double* clock_dead_width_ip;
        const double* clock_width_asymm_ip;
        const double* clock_area_asymm_ip;
        const double* clock_r_scale_ip;
        const double* clock_period_ip;
        // Pointers to output parameters
        double* phi_op;
        // Main operation
        void do_operation() const;
};

std::vector<std::string> phase_clock::get_inputs() {
    return {
        "phi",
        "light",
        "clock_dead_width",
        "clock_width_asymm",
        "clock_area_asymm",
        "clock_r_scale",
        "clock_period"
    };
}

std::vector<std::string> phase_clock::get_outputs() {
    return {
        "phi"
    };
}

void phase_clock::do_operation() const {
    //////////////////////////////////////////
    // Collect inputs and make calculations //
    //////////////////////////////////////////
    
    // Get the current phase value
    const double phi = *phi_ip;
    
    // Bring phi back to the [0, 2*pi) range
    const double phi_mod = phi - 2.0 * M_PI * floor(phi / (2.0 * M_PI));
    
    // Get the current light value
    const double light = *light_ip;
    
    // Get the response function parameters
    const double d = *clock_dead_width_ip;
    const double delta = *clock_width_asymm_ip;
    const double epsilon = *clock_area_asymm_ip;
    const double s = *clock_r_scale_ip;
    
    // Determine a, b, alpha, and beta for the response function
    const double a = M_PI * (1.0 - d + delta);
    const double b = M_PI * (1.0 - d - delta);;
    const double alpha = s * (1.0 + 0.5 * epsilon);
    const double beta = s * (1.0 - 0.5 * epsilon);
    
    // Calculate the response
    double R;
    if(0.0 <= phi_mod && phi_mod < a) R = -6.0 * alpha * phi_mod * (phi_mod - a) / (a * a * a);
    else if(phi_mod < 2.0 * M_PI - b) R = 0.0;
    else if(phi_mod < 2.0 * M_PI) R = 6.0 * beta * (phi_mod - 2.0 * M_PI) * (phi_mod - 2.0 * M_PI + b) / (b * b * b);
    else throw std::logic_error(std::string("Thrown by phase_clock: something is wrong with phi_mod!\n"));
    
    // Get the intrinsic clock period
    const double natural_period = *clock_period_ip;             // Natural period in hours
    const double natural_freq = 2.0 * M_PI / natural_period;    // Corresponding angular frequency in radians per hour
    
    //////////////////////////////////////
    // Update the output parameter list //
    //////////////////////////////////////
    
    update(phi_op, natural_freq * (1 + R * light));
}

#endif