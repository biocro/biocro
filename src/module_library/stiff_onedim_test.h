#ifndef STIFF_ONEDIM_TEST_H
#define STIFF_ONEDIM_TEST_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include <cmath>

namespace standardBML
{
/**
 * @class stiff_onedim_test
 *
 * @brief Put documentation here.
 *
 */
class stiff_onedim_test : public differential_module
{
   public:
    stiff_onedim_test(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          y{get_input(input_quantities, "y")},
          time{get_input(input_quantities, "time")},
          a{get_input(input_quantities, "a")},

          // Get pointers to output quantities
          y_op{get_op(output_quantities, "y")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "stiff_onedim_test"; }

   private:
    // References to input quantities
    double const& y;
    double const& time;
    double const& a;

    // Pointers to output quantities
    double* y_op;

    // Main operation
    void do_operation() const;
};

string_vector stiff_onedim_test::get_inputs()
{
    return {
        "y",     // Put y units here
        "time",  // Put time units here
        "a"      // Put a units here
    };
}

string_vector stiff_onedim_test::get_outputs()
{
    return {
        "y"  // Put y units here
    };
}

void stiff_onedim_test::do_operation() const
{
    double y_dot = a * (std::cos(time) - y) - std::sin(time);
    update(y_op, y_dot);
}

}  // namespace standardBML
#endif
