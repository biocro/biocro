#ifndef BROYDEN_TEST_H
#define BROYDEN_TEST_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "../math/roots/multidim/broyden.h"
#include "../math/linalg/base.h"

#include <array>

namespace standardBML
{

struct test_function {

    std::array<double, 2> operator()(const std::array<double, 2>& x)
    {
        std::array<double, 2> y;
        y[0] = x[0] / (x[0] * x[0] + 1) - x[1] + 0.5;
        y[1] = 2 - x[0] - x[1];
        return y;
    }

    linalg::matrix<double, 2, 2> jacobian(const std::array<double, 2>& x) {
        linalg::matrix<double, 2, 2> jac;
        double v = x[0] * x[0] + 1;
        double dv = 2 * x[0];
        double u = x[0];
        double du = 1;
        jac(0, 0) = du/v - u*dv/(v*v);
        jac(0, 1) = -1;
        jac(1, 0) = -1;
        jac(1, 1) = -1;
        return jac;
    }
};

class broyden_test : public direct_module
{
    struct result {
        std::array<double*, 2> x;
        std::array<double*, 2> y;
        double* iter;
        
        result(state_map* output_quantities, std::string method) {
            for (size_t i = 0; i < 2; ++i){
                x[i] = get_op(output_quantities, method + "_x" + std::to_string(i));                
                y[i] = get_op(output_quantities, method + "_y" + std::to_string(i));
            }
            iter = get_op(output_quantities, method + "_iter");                
        }
        
    };
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
    std::vector<result> results;
    static string_vector methods() {
        return {"broyden", "newton"};
    }
    // Main operation
    void do_operation() const;

    void update_result(result const& r, root_multidim::result_t<2> out) const {
        for(size_t i = 0; i < 2; ++i){
            update(r.x[i], out.zero[i]);
            update(r.y[i], out.residual[i]);
        }
        update(r.iter, out.iteration);
              
    }
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
    string_vector outputs = {"x1", "x2", "y1", "y2", "iter"};
    string_vector out;
    string_vector _methods = broyden_test::methods();
    out.reserve(outputs.size() * _methods.size());
    for (auto method : _methods){
        for(auto output : outputs) {
           out.push_back(method + "_" + output); 
        }    
    }
    return out;
    
}

void broyden_test::do_operation() const
{
    using namespace root_multidim;

    std::array<double, 2> guess = {guess_1, guess_2};
    test_function func;
    
    broyden<2> bs(static_cast<size_t>(max_iterations), abs_tol, rel_tol);
    update_result(results[0], bs(func, guess));
    newton<2> ns(static_cast<size_t>(max_iterations), abs_tol, rel_tol);    
    update_result(results[1], ns(func, guess));
    
        
}

}  // namespace standardBML
#endif
