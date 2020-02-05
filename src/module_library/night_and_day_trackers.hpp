#ifndef NIGHT_AND_DAY_TRACKERS_H
#define NIGHT_AND_DAY_TRACKERS_H

#include "../modules.h"

class night_and_day_trackers : public DerivModule {
    public:
        night_and_day_trackers(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
            // Define basic module properties by passing its name to its parent class
            DerivModule("night_and_day_trackers"),
            // Get pointers to input parameters
            tracker_rate_ip(get_ip(input_parameters, "tracker_rate")),
            light_ip(get_ip(input_parameters, "light")),
            night_tracker_ip(get_ip(input_parameters, "night_tracker")),
            day_tracker_ip(get_ip(input_parameters, "day_tracker")),
            // Get pointers to output parameters
            night_tracker_op(get_op(output_parameters, "night_tracker")),
            day_tracker_op(get_op(output_parameters, "day_tracker"))
        {}
        static std::vector<std::string> get_inputs();
        static std::vector<std::string> get_outputs();
    private:
        // Pointers to input parameters
        const double* tracker_rate_ip;
        const double* light_ip;
        const double* night_tracker_ip;
        const double* day_tracker_ip;
        // Pointers to output parameters
        double* night_tracker_op;
        double* day_tracker_op;
        // Main operation
        void do_operation() const;
};

std::vector<std::string> night_and_day_trackers::get_inputs() {
    return {
        "tracker_rate",
        "light",
        "night_tracker",
        "day_tracker"
    };
}

std::vector<std::string> night_and_day_trackers::get_outputs() {
    return {
        "night_tracker",
        "day_tracker"
    };
}

void night_and_day_trackers::do_operation() const {
    //////////////////////////////////////////
    // Collect inputs and make calculations //
    //////////////////////////////////////////
    
    // Get the tracker rate
    double tracker_rate = *tracker_rate_ip;
    
    // Get the current light value
    double light = *light_ip;
    
    // Get the current values of the night and day trackers
    double night_tracker = *night_tracker_ip;
    double day_tracker = *day_tracker_ip;
    
    //////////////////////////////////////
    // Update the output parameter list //
    //////////////////////////////////////
    
    // The day and night trackers are produced at a light-dependent rate and decay exponentially independent
    //  of the light level
    // These quantities were inspired by the light-sensitive protein P in
    //  Locke at. al, Journal of Theoretical Biology 234, 383SPA393 (2005)
    update(night_tracker_op, tracker_rate * ((1.0 - light) - night_tracker));
    update(day_tracker_op, tracker_rate * (light - day_tracker));
}

#endif