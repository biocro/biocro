#ifndef MULTIDIM_TEST_H
#define MULTIDIM_TEST_H

// std lib
#include <array>

// framework
#include "../framework/module.h"
#include "../framework/state_map.h"

// biocro
#include "../math/roots/multidim/broyden.h"
#include "../math/roots/multidim/newton.h"
#include "../math/linalg/base.h"

namespace standardBML
{

struct test_function {
    // solution is x1 = x2 = 1
    std::array<double, 2> operator()(const std::array<double, 2>& x)
    {
        std::array<double, 2> y;
        y[0] = x[0] / (x[0] * x[0] + 1) - x[1] + 0.5;
        y[1] = 2 - x[0] - x[1];
        return y;
    }

    linalg::matrix<double, 2, 2> jacobian(linalg::vector<double, 2> const& x)
    {
        linalg::matrix<double, 2, 2> jac;
        double v = x[0] * x[0] + 1;
        double dv = 2 * x[0];
        double u = x[0];
        double du = 1;
        jac(0, 0) = du / v - u * dv / (v * v);
        jac(0, 1) = -1;
        jac(1, 0) = -1;
        jac(1, 1) = -1;
        return jac;
    }
};

class root_multidim_test : public direct_module
{
   struct test_result {
       double* x1;
       double* x2;
       double* y1;
       double* y2;
       double* iteration;
       double* flag; 

        test_result(state_map* output_quantities, std::string method_name) :
        x1{get_op(output_quantities, method_name + "_x1" )},
        x2{get_op(output_quantities, method_name + "_x2" )},
        y1{get_op(output_quantities, method_name + "_y1" )},
        y2{get_op(output_quantities, method_name + "_y2" )},
        iteration{get_op(output_quantities, method_name + "_iteration" )},
        flag {get_op(output_quantities, method_name + "_flag" )}{}

        void set(root_multidim::result_t<2> result) const {
            *x1 = result.zero[0];
            *x2 = result.zero[1];
            *y1 = result.residual[0];
            *y2 = result.residual[1];
            *iteration = static_cast<double>(result.iteration);
            *flag =static_cast<double>(result.flag);
        }        
   };

   public:
    root_multidim_test(
        state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          max_iterations{get_input(input_quantities, "max_iterations")},
          abs_tol{get_input(input_quantities, "abs_tol")},
          rel_tol{get_input(input_quantities, "rel_tol")},
          guess_1{get_input(input_quantities, "guess_1")},
          guess_2{get_input(input_quantities, "guess_2")},
          broyden_result(output_quantities, "broyden"),
          newton_result(output_quantities, "newton")
    // Get pointers to output quantities

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "root_multidim_test"; }

   private:
    // Pointers to input quantities
    const double& max_iterations;
    const double& abs_tol;
    const double& rel_tol;
    const double& guess_1;
    const double& guess_2;

    // Pointers to output quantities
    test_result broyden_result;
    test_result newton_result;

    // Main operation
    void do_operation() const;
};

string_vector root_multidim_test::get_inputs()
{
    return {
        "max_iterations",
        "abs_tol",
        "rel_tol",
        "guess_1",
        "guess_2"};
}

string_vector root_multidim_test::get_outputs()
{
    string_vector methods = {
        "broyden",
        "newton"};
    string_vector test_outputs = {
        "_x1", "_x2", "_y1", "_y2", "_iteration", "_flag" 
    };
    string_vector outputs;
    outputs.reserve(methods.size() * test_outputs.size());
    for (auto method : methods) {
        for (auto test_output : test_outputs) {
            outputs.push_back(method + test_output);
        }
    }
    return outputs;
    
}

void root_multidim_test::do_operation() const
{
    using namespace root_multidim;
    std::array<double, 2> guess = {guess_1, guess_2};
    test_function func;

    
    size_t max_iter = static_cast<size_t>(max_iterations);

    broyden<2> bs(max_iter, abs_tol, rel_tol);
    auto result = bs(func, guess);    
    broyden_result.set(result);

    newton<2> ns(max_iter, abs_tol, rel_tol);
    result = ns(func, guess);
    newton_result.set(result);    
}

}  // namespace standardBML
#endif
