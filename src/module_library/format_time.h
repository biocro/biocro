#ifndef STANDARDBML_FORMAT_TIME_H
#define STANDARDBML_FORMAT_TIME_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class format_time
 *
 * @brief Converts `time` (expressed as the number of hours since midnight on
 * January 1) to day of year (`doy`) and `hour` values.
 *
 */
class format_time : public direct_module
{
   public:
    format_time(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          time{get_input(input_quantities, "time")},

          // Get pointers to output quantities
          doy_op{get_op(output_quantities, "doy")},
          fractional_doy_op{get_op(output_quantities, "fractional_doy")},
          hour_op{get_op(output_quantities, "hour")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "format_time"; }

   private:
    // References to input quantities
    double const& time;

    // Pointers to output quantities
    double* doy_op;
    double* fractional_doy_op;
    double* hour_op;

    // Main operation
    void do_operation() const;
};

string_vector format_time::get_inputs()
{
    return {
        "time"  // hours
    };
}

string_vector format_time::get_outputs()
{
    return {
        "doy",             // days
        "fractional_doy",  // days
        "hour"             // hours
    };
}

void format_time::do_operation() const
{
    double constexpr hr_per_day = 24.0;

    int const d = std::floor(time / hr_per_day);
    double const doy = 1 + d;                   // days
    double const hour = time - hr_per_day * d;  // hours

    update(doy_op, doy);
    update(fractional_doy_op, doy + hour / hr_per_day);
    update(hour_op, hour);
}

}  // namespace standardBML
#endif
