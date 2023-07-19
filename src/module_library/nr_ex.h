#ifndef NR_EX_H
#define NR_EX_H

#include "../framework/module.h"
#include "../framework/state_map.h"

// This module is an example in Chapter 16 of Numerical Recipes in C
// The analytical solution is:
//  u = 2 * exp(-x) - exp(-1000x)
//  v = -exp(-x) + exp(-1000x)
// For u(0) = 1, v(0) = 0

namespace standardBML
{
class nr_ex : public differential_module
{
   public:
    nr_ex(state_map const& input_quantities, state_map* output_quantities)
        : differential_module{},

          // Get input pointers
          u_ip{get_ip(input_quantities, "u")},
          v_ip{get_ip(input_quantities, "v")},

          // Get output pointers for time derivatives
          u_op{get_op(output_quantities, "u")},
          v_op{get_op(output_quantities, "v")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "nr_ex"; }

   private:
    // Input pointers
    const double* u_ip;
    const double* v_ip;

    // Output pointers for time derivatives
    double* u_op;
    double* v_op;

    // Main operation
    void do_operation() const;
};

string_vector nr_ex::get_inputs()
{
    return {
        "u",  //
        "v"   //
    };
}

string_vector nr_ex::get_outputs()
{
    return {
        "u",  //
        "v"   //
    };
}

void nr_ex::do_operation() const
{
    // Calculate the time derivatives and modify the module output map
    update(u_op, +998.0 * (*u_ip) + 1998.0 * (*v_ip));
    update(v_op, -999.0 * (*u_ip) - 1999.0 * (*v_ip));
}

}  // namespace standardBML
#endif
