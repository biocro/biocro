#ifndef HYPERBOLAS_H
#define HYPERBOLAS_H

#include <cmath>  // For pow
#include "../modules.h"
#include "../constants.h"

/**
 * @class golden_ratio_hyperbola
 * 
 * @brief Represents a simple hyperbola defined by f(x) = 1 + 1 / x. Intended to be used as a simple test case
 * for simultaneous eqation solvers. The solution to f(x) = x is x = (1 + sqrt(5))/2, the "golden ratio."
 */
class golden_ratio_hyperbola : public SteadyModule
{
   public:
    golden_ratio_hyperbola(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters) :  // Define basic module properties by passing its name to its parent class
                                                                      SteadyModule("golden_ratio_hyperbola"),
                                                                      // Get pointers to input parameters
                                                                      x_ip(get_ip(input_parameters, "x")),
                                                                      // Get pointers to output parameters
                                                                      x_op(get_op(output_parameters, "x"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* x_ip;
    // Pointers to output parameters
    double* x_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> golden_ratio_hyperbola::get_inputs()
{
    return {
        "x"  // unitless
    };
}

std::vector<std::string> golden_ratio_hyperbola::get_outputs()
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
class hyperbola_2d : public SteadyModule
{
   public:
    hyperbola_2d(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters) :  // Define basic module properties by passing its name to its parent class
                                                                      SteadyModule("hyperbola_2d"),
                                                                      // Get pointers to input parameters
                                                                      x_ip(get_ip(input_parameters, "x")),
                                                                      y_ip(get_ip(input_parameters, "y")),
                                                                      // Get pointers to output parameters
                                                                      x_op(get_op(output_parameters, "x")),
                                                                      y_op(get_op(output_parameters, "y"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* x_ip;
    const double* y_ip;
    // Pointers to output parameters
    double* x_op;
    double* y_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> hyperbola_2d::get_inputs()
{
    return {
        "x",  // unitless
        "y"   // unitless
    };
}

std::vector<std::string> hyperbola_2d::get_outputs()
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

#endif