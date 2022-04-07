#ifndef ED_BOUNDARY_CONDUCTANCE_H
#define ED_BOUNDARY_CONDUCTANCE_H

#include <algorithm>  // for std::max
#include "../framework/module.h"
#include "../framework/state_map.h"

/**
 * @class ed_boundary_conductance_quadrature
 *
 * @brief Determines an overall value for the boundary layer conductance from
 * the free and forced conductances. Currently only intended for use by Ed.
 *
 * Nikolov et al. suggest using the larger of the forced and free conductances.
 * This approach may not be strictly valid when the two have similar magnitude;
 * see section 7.10 on page 105 of Campbell & Norman (1998).
 *
 * Here we add the two conductances in quadrature to produce a smoothly varying
 * output. If either the free or forced conductance is much larger than the other,
 * overall conductance determined this way will be approximately equal to the
 * larger conductance as in Nikolov.
 */
namespace standardBML
{
class ed_boundary_conductance_quadrature : public direct_module
{
   public:
    ed_boundary_conductance_quadrature(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get pointers to input quantities
          conductance_boundary_h2o_forced_ip(get_ip(input_quantities, "conductance_boundary_h2o_forced")),
          conductance_boundary_h2o_free_ip(get_ip(input_quantities, "conductance_boundary_h2o_free")),

          // Get pointers to output quantities
          conductance_boundary_h2o_op(get_op(output_quantities, "conductance_boundary_h2o"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ed_boundary_conductance_quadrature"; }

   private:
    // Pointers to input quantities
    const double* conductance_boundary_h2o_forced_ip;
    const double* conductance_boundary_h2o_free_ip;

    // Pointers to output quantities
    double* conductance_boundary_h2o_op;

    // Main operation
    void do_operation() const override;
};

string_vector ed_boundary_conductance_quadrature::get_inputs()
{
    return {
        "conductance_boundary_h2o_forced",  // mol / m^2 / s
        "conductance_boundary_h2o_free"     // mol / m^2 / s
    };
}

string_vector ed_boundary_conductance_quadrature::get_outputs()
{
    return {
        "conductance_boundary_h2o"  // mol / m^2 / s
    };
}

void ed_boundary_conductance_quadrature::do_operation() const
{
    const double gbv_free = *conductance_boundary_h2o_free_ip;
    const double gbv_forced = *conductance_boundary_h2o_forced_ip;
    const double overall_conductance = sqrt(gbv_free * gbv_free + gbv_forced * gbv_forced);

    update(conductance_boundary_h2o_op, overall_conductance);
}

/**
 * @class ed_boundary_conductance_max
 *
 * @brief Determines an overall value for the boundary layer conductance from
 * the free and forced conductances. Currently only intended for use by Ed.
 *
 * Here we follow Nikolov et al. who suggest using the larger of the forced
 * and free conductances.
 */
class ed_boundary_conductance_max : public direct_module
{
   public:
    ed_boundary_conductance_max(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get pointers to input quantities
          conductance_boundary_h2o_forced_ip(get_ip(input_quantities, "conductance_boundary_h2o_forced")),
          conductance_boundary_h2o_free_ip(get_ip(input_quantities, "conductance_boundary_h2o_free")),

          // Get pointers to output quantities
          conductance_boundary_h2o_op(get_op(output_quantities, "conductance_boundary_h2o"))

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ed_boundary_conductance_max"; }

   private:
    // Pointers to input quantities
    const double* conductance_boundary_h2o_forced_ip;
    const double* conductance_boundary_h2o_free_ip;

    // Pointers to output quantities
    double* conductance_boundary_h2o_op;

    // Main operation
    void do_operation() const override;
};

string_vector ed_boundary_conductance_max::get_inputs()
{
    return {
        "conductance_boundary_h2o_forced",  // mol / m^2 / s
        "conductance_boundary_h2o_free"     // mol / m^2 / s
    };
}

string_vector ed_boundary_conductance_max::get_outputs()
{
    return {
        "conductance_boundary_h2o"  // mol / m^2 / s
    };
}

void ed_boundary_conductance_max::do_operation() const
{
    const double gbv_free = *conductance_boundary_h2o_free_ip;
    const double gbv_forced = *conductance_boundary_h2o_forced_ip;
    const double overall_conductance = std::max(gbv_free, gbv_forced);

    update(conductance_boundary_h2o_op, overall_conductance);
}

}  // namespace standardBML
#endif
