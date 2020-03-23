#ifndef GOLDEN_RATIO_HYPERBOLA_H
#define GOLDEN_RATIO_HYPERBOLA_H

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
        "x",  // unitless
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

#endif