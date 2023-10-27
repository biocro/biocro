#ifndef HYPERBOLAS_H
#define HYPERBOLAS_H

#include <cmath>  // For pow
#include "../framework/module.h"
#include "../framework/state_map.h"
#include "../framework/constants.h"

namespace standardBML
{
/**
 * @class golden_ratio_hyperbola
 *
 * @brief Represents a simple hyperbola defined by f(x) = 1 + 1 / x. Intended to
 * be used as a simple test case for simultaneous eqation solvers. The solution
 * to f(x) = x is x = (1 + sqrt(5))/2, the "golden ratio."
 */
class golden_ratio_hyperbola : public direct_module
{
   public:
    golden_ratio_hyperbola(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          x_ip{get_ip(input_quantities, "x")},

          // Get pointers to output quantities
          x_op{get_op(output_quantities, "x")}

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "golden_ratio_hyperbola"; }

   private:
    // Pointers to input quantities
    const double* x_ip;

    // Pointers to output quantities
    double* x_op;

    // Main operation
    void do_operation() const override;
};

string_vector golden_ratio_hyperbola::get_inputs()
{
    return {
        "x"  // unitless
    };
}

string_vector golden_ratio_hyperbola::get_outputs()
{
    return {
        "x"  // unitless
    };
}

void golden_ratio_hyperbola::do_operation() const
{
    update(x_op, 1 + 1 / *x_ip);
}

/**
 * @class hyperbola_2d
 *
 * @brief Represents two 2D hyperbolas defined by f(x,y) = 1 + 1 / (x + y) and g(x,y) = 1 + 1 / (2x - y).
 * Intended to be used as a simple test case for simultaneous equation solvers. One solution to f(x,y) = x,
 * g(x,y) = y exists at x = -1.126529, y = 0.656279 (determined numerically).
 */
class hyperbola_2d : public direct_module
{
   public:
    hyperbola_2d(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          x_ip{get_ip(input_quantities, "x")},
          y_ip{get_ip(input_quantities, "y")},

          // Get pointers to output quantities
          x_op{get_op(output_quantities, "x")},
          y_op{get_op(output_quantities, "y")}

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "hyperbola_2d"; }

   private:
    // Pointers to input quantities
    const double* x_ip;
    const double* y_ip;

    // Pointers to output quantities
    double* x_op;
    double* y_op;

    // Main operation
    void do_operation() const override;
};

string_vector hyperbola_2d::get_inputs()
{
    return {
        "x",  // unitless
        "y"   // unitless
    };
}

string_vector hyperbola_2d::get_outputs()
{
    return {
        "x",  // unitless
        "y"   // unitless
    };
}

void hyperbola_2d::do_operation() const
{
    update(x_op, 1 + 1 / (*x_ip + *y_ip));
    update(y_op, 1 + 1 / (2 * *x_ip - *y_ip));
}

}  // namespace standardBML
#endif
