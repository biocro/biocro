#ifndef ED_BALL_BERRY_H
#define ED_BALL_BERRY_H

#include "../modules.h"

/**
 * @class ed_ball_berry
 * 
 * @brief Uses the Ball-Berry equation to calculate stomatal conductance to water vapor
 */
class ed_ball_berry : public SteadyModule
{
   public:
    ed_ball_berry(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters) :  // Define basic module properties by passing its name to its parent class
                                                                      SteadyModule("ed_ball_berry"),
                                                                      // Get pointers to input parameters
                                                                      ball_berry_slope_ip(get_ip(input_parameters, "ball_berry_slope")),
                                                                      ball_berry_intercept_ip(get_ip(input_parameters, "ball_berry_intercept")),
                                                                      assimilation_net_ip(get_ip(input_parameters, "assimilation_net")),
                                                                      relative_humidity_leaf_surface_ip(get_ip(input_parameters, "relative_humidity_leaf_surface")),
                                                                      co2_mole_fraction_leaf_surface_ip(get_ip(input_parameters, "co2_mole_fraction_leaf_surface")),
                                                                      // Get pointers to output parameters
                                                                      conductance_stomatal_h2o_op(get_op(output_parameters, "conductance_stomatal_h2o"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* ball_berry_slope_ip;
    const double* ball_berry_intercept_ip;
    const double* assimilation_net_ip;
    const double* relative_humidity_leaf_surface_ip;
    const double* co2_mole_fraction_leaf_surface_ip;
    // Pointers to output parameters
    double* conductance_stomatal_h2o_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_ball_berry::get_inputs()
{
    return {
        "ball_berry_slope",                // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
        "ball_berry_intercept",            // mol / m^2 / s
        "assimilation_net",                // mol / m^2 / s
        "relative_humidity_leaf_surface",  // Pa / Pa
        "co2_mole_fraction_leaf_surface"   // mol / mol
    };
}

std::vector<std::string> ed_ball_berry::get_outputs()
{
    return {
        "conductance_stomatal_h2o"  // mol / m^2 / s
    };
}

void ed_ball_berry::do_operation() const
{
    // Calculate the stomatal conductance for H2O
    const double conductance_stomatal_h2o =
        *ball_berry_slope_ip * *assimilation_net_ip * *relative_humidity_leaf_surface_ip / *co2_mole_fraction_leaf_surface_ip +
        *ball_berry_intercept_ip;

    // Update the output parameter list
    update(conductance_stomatal_h2o_op, conductance_stomatal_h2o);
}

#endif