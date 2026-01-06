#ifndef BROYDEN_TEST_H
#define BROYDEN_TEST_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "../math/roots/multidim/broyden.h"

#include <array>

namespace standardBML
{

std::array<double, 2> test_function(const std::array<double, 2>& x)
{
    std::array<double, 2> y;
    y[0] = x[0] / (x[0] * x[0] + 1) - x[1] + 0.5;
    y[1] = 2 - x[0] - x[1];
    return y;
}

class broyden_test : public direct_module
{
   public:
    broyden_test(
        state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          max_iterations{get_input(input_quantities, "max_iterations")},
          abs_tol{get_input(input_quantities, "abs_tol")},
          rel_tol{get_input(input_quantities, "rel_tol")},
          guess_1{get_input(input_quantities, "guess_1")},
          guess_2{get_input(input_quantities, "guess_2")},
          x1{get_op(output_quantities, "x1")},
          x2{get_op(output_quantities, "x2")},
          y1{get_op(output_quantities, "y1")},
          y2{get_op(output_quantities, "y2")},
          iter{get_op(output_quantities, "iter")}

    // Get pointers to output quantities

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "broyden_test"; }

   private:
    // Pointers to input quantities
    const double& max_iterations;
    const double& abs_tol;
    const double& rel_tol;
    const double& guess_1;
    const double& guess_2;

    // Pointers to output quantities
    double* x1;
    double* x2;
    double* y1;
    double* y2;
    double* iter;

    // Main operation
    void do_operation() const;
};

string_vector broyden_test::get_inputs()
{
    return {
        "max_iterations",
        "abs_tol",
        "rel_tol",
        "guess_1",
        "guess_2"};
}

string_vector broyden_test::get_outputs()
{
    return {
        "x1",
        "x2",
        "y1",
        "y2",
        "iter"};
}

void broyden_test::do_operation() const
{
    using namespace root_multidim;
    broyden<2> solve(static_cast<size_t>(max_iterations), abs_tol, rel_tol);
    std::array<double, 2> guess = {guess_1, guess_2};
    result_t<2> res = solve(test_function, guess);

    update(x1, res.zero[0]);
    update(x2, res.zero[1]);

    update(y1, res.residual[0]);
    update(y2, res.residual[1]);

    update(iter, res.iteration);
}

}  // namespace standardBML
#endif
