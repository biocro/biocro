#ifndef LIGHT_FROM_SOLAR_H
#define LIGHT_FROM_SOLAR_H

#include "../modules.h"

class light_from_solar : public SteadyModule {
    public:
        light_from_solar(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
            // Define basic module properties by passing its name to its parent class
            SteadyModule("light_from_solar"),
            // Get pointers to input parameters
            solar_ip(get_ip(input_parameters, "solar")),
            light_threshold_ip(get_ip(input_parameters, "light_threshold")),
            light_exp_at_zero_ip(get_ip(input_parameters, "light_exp_at_zero")),
            // Get pointers to output parameters
            light_op(get_op(output_parameters, "light"))
        {}
        static std::vector<std::string> get_inputs();
        static std::vector<std::string> get_outputs();
    private:
        // Pointers to input parameters
        const double* solar_ip;
        const double* light_threshold_ip;
        const double* light_exp_at_zero_ip;
        // Pointers to output parameters
        double* light_op;
        // Main operation
        void do_operation() const;
};

std::vector<std::string> light_from_solar::get_inputs() {
    return {
        "solar",
        "light_threshold",
        "light_exp_at_zero"
    };
}

std::vector<std::string> light_from_solar::get_outputs() {
    return {
        "light"
    };
}

void light_from_solar::do_operation() const {
    //////////////////////////////////////////
    // Collect inputs and make calculations //
    //////////////////////////////////////////
    
    // Get the current level of solar radiation
    double solar = *solar_ip;
    
    // Get the light threshold
    double light_threshold = *light_threshold_ip;
    
    // Get the light exponent at zero
    double light_exp_at_zero = *light_exp_at_zero_ip;
    
    // Check whether the plant is illuminated
    // Rather than simply using a step function (i.e., light = solar > 0),
    //  this logistic function smoothly turns on as the solar radiation
    //  increases
    // Coefficients have been chosen so that light:
    //  (1) roughly equals exp(-light_exp_at_zero) for solar = 0
    //  (2) roughly equals 1 - exp(-light_exp_at_zero) for solar = light_threshold
    //  (3) equals 1/2 for solar = light_threshold/2
    double light = 1.0 / (1.0 + exp(-2.0 * light_exp_at_zero * (solar - 0.5 * light_threshold) / light_threshold));
    
    //////////////////////////////////////
    // Update the output parameter list //
    //////////////////////////////////////
    
    update(light_op, light);
}

#endif
