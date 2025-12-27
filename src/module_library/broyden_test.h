#ifndef BROYDEN_TEST_H
#define BROYDEN_TEST_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include "../math/roots/multidim/broyden.h"
#include "../math/roots/multidim/newton.h"
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

    linalg::matrix<double, 2, 2> jacobian(linalg::vector<double, 2>const& x) {
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
//    struct result {
//        std::array<double*, 2> x;
//        std::array<double*, 2> y;
//        double* iter;
//        
//        result(state_map* output_quantities, std::string method) {
//            for (size_t i = 1; i <= 2; ++i){
//                x[i] = get_op(output_quantities, method + "_x" + std::to_string(i));                
//                y[i] = get_op(output_quantities, method + "_y" + std::to_string(i));
//            }
//            iter = get_op(output_quantities, method + "_iter");                
//        }
//        
//    };
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
          broyden_success{get_op(output_quantities, "broyden_success")},
          newton_success{get_op(output_quantities, "newton_success")}
    // Get pointers to output quantities

    {
//        string_vector _methods = broyden_test::methods();
//        results.reserve(_methods.size());
//        for (auto method : _methods){
//            results.emplace_back(result(output_quantities, method));
//        }    
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
    double* broyden_success;
    double* newton_success;
    // Main operation
    void do_operation() const;

//    void update_result(result const& r, root_multidim::result_t<2> out) const {
//        for(size_t i = 0; i < 2; ++i){
//            update(r.x[i], out.zero[i]);
//            update(r.y[i], out.residual[i]);
//        }
//        update(r.iter, out.iteration);
//              
//    }
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
    return {"broyden_success", "newton_success"};
    
}

void broyden_test::do_operation() const
{
    using namespace root_multidim;

    std::array<double, 2> guess = {guess_1, guess_2};
    test_function func;
    
    broyden<2> bs(static_cast<size_t>(max_iterations), abs_tol, rel_tol);
    update(broyden_success, static_cast<double>(bs(func, guess).flag));
    
    newton<2> ns(static_cast<size_t>(max_iterations), abs_tol, rel_tol);        
    update(newton_success, static_cast<double>(ns(func, guess).flag));
        
}

}  // namespace standardBML
#endif
