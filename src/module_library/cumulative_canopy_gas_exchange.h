#ifndef STANDARDBML_CUMULATIVE_CANOPY_GAS_EXCHANGE_H
#define STANDARDBML_CUMULATIVE_CANOPY_GAS_EXCHANGE_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 * @class cumulative_canopy_gas_exchange
 *
 * @brief Enables calculations of cumulative canopy gas exchange.
 *
 * Cumulative gas exchange will be included in the simulation output as
 * differential quantities called ``'canopy_assimilation'``,
 * ``'canopy_transpiration'``, ``'canopy_gross_assimilation'``, and
 * ``'canopy_photorespiration'``.
 *
 */
class cumulative_canopy_gas_exchange : public differential_module
{
   public:
    cumulative_canopy_gas_exchange(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module{},

          // Get references to input quantities
          canopy_assimilation_rate{get_input(input_quantities, "canopy_assimilation_rate")},
          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")},
          canopy_gross_assimilation_rate{get_input(input_quantities, "canopy_gross_assimilation_rate")},
          canopy_photorespiration_rate{get_input(input_quantities, "canopy_photorespiration_rate")},

          // Get pointers to output quantities
          canopy_assimilation_op{get_op(output_quantities, "canopy_assimilation")},
          canopy_transpiration_op{get_op(output_quantities, "canopy_transpiration")},
          canopy_gross_assimilation_op{get_op(output_quantities, "canopy_gross_assimilation")},
          canopy_photorespiration_op{get_op(output_quantities, "canopy_photorespiration")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "cumulative_canopy_gas_exchange"; }

   private:
    // References to input quantities
    double const& canopy_assimilation_rate;
    double const& canopy_transpiration_rate;
    double const& canopy_gross_assimilation_rate;
    double const& canopy_photorespiration_rate;

    // Pointers to output quantities
    double* canopy_assimilation_op;
    double* canopy_transpiration_op;
    double* canopy_gross_assimilation_op;
    double* canopy_photorespiration_op;

    // Main operation
    void do_operation() const;
};

string_vector cumulative_canopy_gas_exchange::get_inputs()
{
    return {
        "canopy_assimilation_rate",        // Mg / ha / hr
        "canopy_transpiration_rate",       // Mg / ha / hr
        "canopy_gross_assimilation_rate",  // Mg / ha / hr
        "canopy_photorespiration_rate"     // Mg / ha / hr
    };
}

string_vector cumulative_canopy_gas_exchange::get_outputs()
{
    return {
        "canopy_assimilation",        // Mg / ha / hr
        "canopy_transpiration",       // Mg / ha / hr
        "canopy_gross_assimilation",  // Mg / ha / hr
        "canopy_photorespiration"     // Mg / ha / hr
    };
}

void cumulative_canopy_gas_exchange::do_operation() const
{
    update(canopy_assimilation_op, canopy_assimilation_rate);              // Mg / ha / hr
    update(canopy_transpiration_op, canopy_transpiration_rate);            // Mg / ha / hr
    update(canopy_gross_assimilation_op, canopy_gross_assimilation_rate);  // Mg / ha / hr
    update(canopy_photorespiration_op, canopy_photorespiration_rate);      // Mg / ha / hr
}

}  // namespace standardBML
#endif
