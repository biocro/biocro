#ifndef STIFF_TWODIM_TEST_H
#define STIFF_TWODIM_TEST_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include <cmath>

namespace standardBML
{
/**
 * @class stiff_twodim_test
 *
 * @brief Put documentation here.
 *
 */
class stiff_twodim_test : public differential_module
{
   public:
    stiff_twodim_test(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities

          x{get_input(input_quantities, "x")},
          y{get_input(input_quantities, "y")},
          time{get_input(input_quantities, "time")},
          a{get_input(input_quantities, "a")},
          kcat{get_input(input_quantities, "kcat")},
          k1{get_input(input_quantities, "k1")},
          k2{get_input(input_quantities, "k2")},

          // Get pointers to output quantities

          x_op{get_op(output_quantities, "x")},
          y_op{get_op(output_quantities, "y")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "stiff_twodim_test"; }

   private:
    // References to input quantities
    double const& x;
    double const& y;
    double const& time;
    double const& a;
    double const& kcat;
    double const& k1;
    double const& k2;

    // Pointers to output quantities
    double* x_op;
    double* y_op;

    // Main operation
    void do_operation() const;
};

string_vector stiff_twodim_test::get_inputs()
{
    return {
        "x",
        "y",     // Put y units here
        "time",  // Put time units here
        "a",     // Put a units here
        "kcat",
        "k1",
        "k2",
    };
}

string_vector stiff_twodim_test::get_outputs()
{
    return {
        "x",
        "y"  // Put y units here
    };
}

void stiff_twodim_test::do_operation() const
{
    double r = -k1 * x * y + k2 * (1 - y);
    double x_dot = a + r;
    double y_dot = r + kcat * (1 - y);
    update(x_op, x_dot);
    update(y_op, y_dot);
}

}  // namespace standardBML
#endif
