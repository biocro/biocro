#ifndef STANDARDBML_TESTMOD_H
#define STANDARDBML_TESTMOD_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class testmod
 *
 * @brief Put documentation here.
 *
 */
class testmod : public direct_module
{
   public:
    testmod(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          x1{get_input(input_quantities, "x1")},
          x2{get_input(input_quantities, "x2")},
          x3{get_input(input_quantities, "x3")},

          // Get pointers to output quantities
          y1_op{get_op(output_quantities, "y1")},
          y2_op{get_op(output_quantities, "y2")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "testmod"; }

   private:
    // References to input quantities
    double const& x1;
    double const& x2;
    double const& x3;

    // Pointers to output quantities
    double* y1_op;
    double* y2_op;

    // Main operation
    void do_operation() const;
};

string_vector testmod::get_inputs()
{
    return {
        "x1",          // Mg / ha
        "x2",          // Mg / ha / hr
        "x3"           // dimensionless
    };
}

string_vector testmod::get_outputs()
{
    return {
        "y1",          // Put y1 units here
        "y2"           // Put y2 units here
    };
}

void testmod::do_operation() const
{
    // Make calculations here

    // Use `update` to set outputs
    update(y1_op, 0);
    update(y2_op, 0);
}

}  // namespace standardBML
#endif

