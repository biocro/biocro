#ifndef NIGHT_AND_DAY_TRACKERS_H
#define NIGHT_AND_DAY_TRACKERS_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class night_and_day_trackers : public differential_module
{
   public:
    night_and_day_trackers(state_map const& input_quantities, state_map* output_quantities)
        : differential_module{},

          // Get pointers to input quantities
          tracker_rate_ip{get_ip(input_quantities, "tracker_rate")},
          light_ip{get_ip(input_quantities, "light")},
          night_tracker_ip{get_ip(input_quantities, "night_tracker")},
          day_tracker_ip{get_ip(input_quantities, "day_tracker")},

          // Get pointers to output quantities
          night_tracker_op{get_op(output_quantities, "night_tracker")},
          day_tracker_op{get_op(output_quantities, "day_tracker")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "night_and_day_trackers"; }

   private:
    // Pointers to input quantities
    const double* tracker_rate_ip;
    const double* light_ip;
    const double* night_tracker_ip;
    const double* day_tracker_ip;

    // Pointers to output quantities
    double* night_tracker_op;
    double* day_tracker_op;

    // Main operation
    void do_operation() const;
};

string_vector night_and_day_trackers::get_inputs()
{
    return {
        "tracker_rate",   //
        "light",          //
        "night_tracker",  //
        "day_tracker"     //
    };
}

string_vector night_and_day_trackers::get_outputs()
{
    return {
        "night_tracker",  //
        "day_tracker"     //
    };
}

void night_and_day_trackers::do_operation() const
{
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
    // Update the output quantity list //
    //////////////////////////////////////

    // The day and night trackers are produced at a light-dependent rate and decay exponentially independent
    //  of the light level
    // These quantities were inspired by the light-sensitive protein P in
    //  Locke at. al, Journal of Theoretical Biology 234, 383SPA393 (2005)
    update(night_tracker_op, tracker_rate * ((1.0 - light) - night_tracker));
    update(day_tracker_op, tracker_rate * (light - day_tracker));
}

}  // namespace standardBML
#endif
