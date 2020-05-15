#ifndef ED_BOUNDARY_CONDUCTANCE_H
#define ED_BOUNDARY_CONDUCTANCE_H

#include <algorithm>  // for std::max
#include "../modules.h"

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
class ed_boundary_conductance_quadrature : public SteadyModule
{
   public:
    ed_boundary_conductance_quadrature(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_boundary_conductance_quadrature"),
          // Get pointers to input parameters
          conductance_boundary_h2o_forced_ip(get_ip(input_parameters, "conductance_boundary_h2o_forced")),
          conductance_boundary_h2o_free_ip(get_ip(input_parameters, "conductance_boundary_h2o_free")),
          // Get pointers to output parameters
          conductance_boundary_h2o_op(get_op(output_parameters, "conductance_boundary_h2o"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* conductance_boundary_h2o_forced_ip;
    const double* conductance_boundary_h2o_free_ip;
    // Pointers to output parameters
    double* conductance_boundary_h2o_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_boundary_conductance_quadrature::get_inputs()
{
    return {
        "conductance_boundary_h2o_forced",  // mol / m^2 / s
        "conductance_boundary_h2o_free"     // mol / m^2 / s
    };
}

std::vector<std::string> ed_boundary_conductance_quadrature::get_outputs()
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
class ed_boundary_conductance_max : public SteadyModule
{
   public:
    ed_boundary_conductance_max(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_boundary_conductance_max"),
          // Get pointers to input parameters
          conductance_boundary_h2o_forced_ip(get_ip(input_parameters, "conductance_boundary_h2o_forced")),
          conductance_boundary_h2o_free_ip(get_ip(input_parameters, "conductance_boundary_h2o_free")),
          // Get pointers to output parameters
          conductance_boundary_h2o_op(get_op(output_parameters, "conductance_boundary_h2o"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* conductance_boundary_h2o_forced_ip;
    const double* conductance_boundary_h2o_free_ip;
    // Pointers to output parameters
    double* conductance_boundary_h2o_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_boundary_conductance_max::get_inputs()
{
    return {
        "conductance_boundary_h2o_forced",  // mol / m^2 / s
        "conductance_boundary_h2o_free"     // mol / m^2 / s
    };
}

std::vector<std::string> ed_boundary_conductance_max::get_outputs()
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

#endif
