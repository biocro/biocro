#ifndef ED_GAS_CONCENTRATIONS_H
#define ED_GAS_CONCENTRATIONS_H

#include "../modules.h"

/**
 * @class ed_gas_concentrations
 * 
 * @brief Determines CO2 mole fraction and relative humidity at several important locations.
 * Currently only intended for use by Ed.
 * 
 * First, CO2 conductances for transport through the boundary layer and stomata are
 * calculated from the corresponding water vapor conductances using proportionality factors.
 * 
 * Then CO2 mole fractions at the leaf surface and in the intercellular spaces are calculated
 * from the atmospheric value using the relevant conductances and the net carbon assimilation,
 * assuming steady state conditions. Here we just use a simple 1D diffusion equation:
 * assimilation = conductance * concentration_difference
 * 
 * Finally, relative humidity at the leaf surface is calculated by assuming equal water vapor
 * flow through the stomata and the boundary layer and maximal humidity inside the leaf. The
 * Ball-Berry model is used to express stomatal conductance in terms of the net assimilation.
 * 
 * In more detail, relative humiduty is calculated using the quadratic equation
 *  a * hs^2 + b * hs + c = 0
 * 
 * This can be derived as follows:
 * At steady-state
 *  E = gs * (hs - hi)  -- Equation 1
 *  and
 *  E = gb * (ha - hs)  -- Equation 2
 *
 * Substitute gs in equation 1 using the Ball-Berry model:
 *  gs = b1 * A * hs / cs + b0
 *
 *  Where A is assimilation rate, hs is relative humidity at the surface of the leaf, and cs is the CO2 mole fraction at the surface of the leaf.
 *
 * Assume hi = 1 based on saturation of water vapor in the interal airspace of a leaf.
 * Use the equality of equations 1 and 2 to solve for hs, and it's a quadratic with the coefficients given in the code. 
 */
class ed_gas_concentrations : public SteadyModule
{
   public:
    ed_gas_concentrations(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_gas_concentrations"),
          // Get pointers to input parameters
          conductance_boundary_h2o_ip(get_ip(input_parameters, "conductance_boundary_h2o")),
          conductance_stomatal_h2o_ip(get_ip(input_parameters, "conductance_stomatal_h2o")),
          diffusivity_ratio_boundary_ip(get_ip(input_parameters, "diffusivity_ratio_boundary")),
          diffusivity_ratio_stomata_ip(get_ip(input_parameters, "diffusivity_ratio_stomata")),
          co2_mole_fraction_atmosphere_ip(get_ip(input_parameters, "co2_mole_fraction_atmosphere")),
          relative_humidity_atmosphere_ip(get_ip(input_parameters, "rh")),
          ball_berry_slope_ip(get_ip(input_parameters, "ball_berry_slope")),
          ball_berry_intercept_ip(get_ip(input_parameters, "ball_berry_intercept")),
          assimilation_net_ip(get_ip(input_parameters, "assimilation_net")),
          // Get pointers to output parameters
          conductance_boundary_co2_op(get_op(output_parameters, "conductance_boundary_co2")),
          conductance_stomatal_co2_op(get_op(output_parameters, "conductance_stomatal_co2")),
          co2_mole_fraction_leaf_surface_op(get_op(output_parameters, "co2_mole_fraction_leaf_surface")),
          co2_mole_fraction_intercellular_op(get_op(output_parameters, "co2_mole_fraction_intercellular")),
          relative_humidity_leaf_surface_op(get_op(output_parameters, "relative_humidity_leaf_surface"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* conductance_boundary_h2o_ip;
    const double* conductance_stomatal_h2o_ip;
    const double* diffusivity_ratio_boundary_ip;
    const double* diffusivity_ratio_stomata_ip;
    const double* co2_mole_fraction_atmosphere_ip;
    const double* relative_humidity_atmosphere_ip;
    const double* ball_berry_slope_ip;
    const double* ball_berry_intercept_ip;
    const double* assimilation_net_ip;
    // Pointers to output parameters
    double* conductance_boundary_co2_op;
    double* conductance_stomatal_co2_op;
    double* co2_mole_fraction_leaf_surface_op;
    double* co2_mole_fraction_intercellular_op;
    double* relative_humidity_leaf_surface_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_gas_concentrations::get_inputs()
{
    return {
        "conductance_boundary_h2o",      // mol / m^2 / s
        "conductance_stomatal_h2o",      // mol / m^2 / s
        "diffusivity_ratio_boundary",    // dimensionless
        "diffusivity_ratio_stomata",     // dimensionless
        "co2_mole_fraction_atmosphere",  // dimensionless from mol / mol
        "rh",                            // dimensionless from Pa / Pa
        "ball_berry_slope",              // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
        "ball_berry_intercept",          // mol / m^2 / s
        "assimilation_net"               // mol / m^2 / s
    };
}

std::vector<std::string> ed_gas_concentrations::get_outputs()
{
    return {
        "conductance_boundary_co2",         // mol / m^2 / s
        "conductance_stomatal_co2",         // mol / m^2 / s
        "co2_mole_fraction_leaf_surface",   // mol / mol
        "co2_mole_fraction_intercellular",  // mol / mol
        "relative_humidity_leaf_surface"    // Pa / Pa
    };
}

void ed_gas_concentrations::do_operation() const
{
    // Calculate the boundary layer conductance for CO2
    const double conductance_boundary_co2 = *conductance_boundary_h2o_ip / *diffusivity_ratio_boundary_ip;

    // Calculate the stomatal conductance for CO2
    const double conductance_stomatal_co2 = *conductance_stomatal_h2o_ip / *diffusivity_ratio_stomata_ip;

    // Calculate the mole fraction of CO2 at the leaf surface
    const double co2_mole_fraction_leaf_surface =
        *co2_mole_fraction_atmosphere_ip - *assimilation_net_ip / conductance_boundary_co2;

    // Calculate the mole fraction of CO2 in the intercellular spaces
    const double co2_mole_fraction_intercellular =
        co2_mole_fraction_leaf_surface - *assimilation_net_ip / conductance_stomatal_co2;

    // Calculate the relative humidity at the leaf surface
    const double a = *ball_berry_slope_ip * *assimilation_net_ip / co2_mole_fraction_leaf_surface;
    const double b = *ball_berry_intercept_ip + *conductance_boundary_h2o_ip - a;
    const double c = -*ball_berry_intercept_ip - *conductance_boundary_h2o_ip * *relative_humidity_atmosphere_ip;
    const double root_term = b * b - 4 * a * c;

    double relative_humidity_leaf_surface;
    if (a == 0) {
        relative_humidity_leaf_surface = -c / b;
    } else {
        relative_humidity_leaf_surface = (-b + sqrt(root_term)) / (2 * a);
    }

    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"diffusivity_ratio_boundary cannot be zero",               *diffusivity_ratio_boundary_ip == 0},   // divide by zero
        {"diffusivity_ratio_stomata cannot be zero",                *diffusivity_ratio_stomata_ip == 0},    // divide by zero
        {"conductance_boundary_co2 cannot be zero",                 conductance_boundary_co2 == 0},         // divide by zero
        {"conductance_stomatal_co2 cannot be zero",                 conductance_stomatal_co2 == 0},         // divide by zero
        {"co2_mole_fraction_leaf_surface cannot be zero",           co2_mole_fraction_leaf_surface == 0},   // divide by zero
        {"the quadratic a and b terms cannot both be zero",         a == 0 && b == 0},                      // divide by zero
        {"the quadratic root_term cannot be negative for a != 0",   a != 0 && root_term < 0}                // imaginary sqrt
    };

    check_error_conditions(errors_to_check, get_name());

    // Update the output parameter list
    update(conductance_boundary_co2_op, conductance_boundary_co2);
    update(conductance_stomatal_co2_op, conductance_stomatal_co2);
    update(co2_mole_fraction_leaf_surface_op, co2_mole_fraction_leaf_surface);
    update(co2_mole_fraction_intercellular_op, co2_mole_fraction_intercellular);
    update(relative_humidity_leaf_surface_op, relative_humidity_leaf_surface);
}

#endif
