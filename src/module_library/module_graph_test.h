#ifndef MODULE_GRAPH_TEST_H
#define MODULE_GRAPH_TEST_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
class Module_1 : public direct_module
{
   public:
    Module_1(state_map const& /*input_quantities*/, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          // None

          // Get pointers to output quantities
          A_op{get_op(output_quantities, "A")},
          B_op{get_op(output_quantities, "B")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "Module_1"; }

   private:
    // Pointers to input quantities
    // Nothing here

    // Pointers to output quantities
    double* A_op;
    double* B_op;

    // Main operation
    void do_operation() const;
};

string_vector Module_1::get_inputs()
{
    return {
        // None
    };
}

string_vector Module_1::get_outputs()
{
    return {
        "A",  //
        "B"   //
    };
}

void Module_1::do_operation() const
{
    // Collect inputs and make calculations
    // Nothing here

    // Update the output quantity list
    update(A_op, 0.0);
    update(B_op, 0.0);
}

class Module_2 : public direct_module
{
   public:
    Module_2(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          B_ip{get_ip(input_quantities, "B")},

          // Get pointers to output quantities
          C_op{get_op(output_quantities, "C")},
          D_op{get_op(output_quantities, "D")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "Module_2"; }

   private:
    // Pointers to input quantities
    const double* B_ip;

    // Pointers to output quantities
    double* C_op;
    double* D_op;

    // Main operation
    void do_operation() const;
};

string_vector Module_2::get_inputs()
{
    return {
        "B"  //
    };
}

string_vector Module_2::get_outputs()
{
    return {
        "C",  //
        "D"   //
    };
}

void Module_2::do_operation() const
{
    // Collect inputs and make calculations
    // Nothing here

    // Update the output quantity list
    update(C_op, 0.0);
    update(D_op, 0.0);
}

class Module_3 : public direct_module
{
   public:
    Module_3(state_map const& input_quantities, state_map* output_quantities)
        : direct_module{},

          // Get pointers to input quantities
          A_ip{get_ip(input_quantities, "A")},
          C_ip{get_ip(input_quantities, "C")},

          // Get pointers to output quantities
          E_op{get_op(output_quantities, "E")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "Module_3"; }

   private:
    // Pointers to input quantities
    const double* A_ip;
    const double* C_ip;

    // Pointers to output quantities
    double* E_op;

    // Main operation
    void do_operation() const;
};

string_vector Module_3::get_inputs()
{
    return {
        "A",  //
        "C"   //
    };
}

string_vector Module_3::get_outputs()
{
    return {
        "E"  //
    };
}

void Module_3::do_operation() const
{
    // Collect inputs and make calculations
    // Nothing here

    // Update the output quantity list
    update(E_op, 0.0);
}

}  // namespace standardBML
#endif
