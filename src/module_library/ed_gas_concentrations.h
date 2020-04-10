#ifndef ED_GAS_CONCENTRATIONS_H
#define ED_GAS_CONCENTRATIONS_H

#include "../modules.h"
#include "AuxBioCro.h"  // for saturation_vapor_pressure

/**
 * @class ed_gas_concentrations
 * 
 * @brief Determines mole fractions of CO2 and H2O at several important locations.
 * Currently only intended for use by Ed.
 * 
 * CO2 conductances for transport through the boundary layer and stomata are
 * calculated from the corresponding water vapor conductances using proportionality factors.
 * These factors depend on the ratio of molecular diffusivity (H20 / CO2) and the primary 
 * transport mechanism the relevant area. See e.g. Table 7.4 on page 106 of Campbell & Norman (1998).
 * 
 * CO2 mole fractions at the leaf surface and in the intercellular spaces are calculated
 * from the atmospheric value using the relevant conductances and the net carbon assimilation,
 * assuming steady state conditions. Here we just use a simple 1D diffusion equation:
 * assimilation = conductance * mole_fraction_difference (see e.g. Equation 6.7 on page 79 of
 * Campbell & Norman (1998)).
 * 
 * The H2O mole fraction in the intercellular spaces is calculated by assuming relative humidity
 * in the leaf is 1, i.e., water vapor pressure = saturation water vapor pressure.
 * 
 * The H2O mole fraction at the leaf surface is calculated by assuming equal flux across the
 * stomata and the boundary layer, i.e. g_s * (h_s - h_i) = F = g_b * (h_a - h_s), where 'h'
 * is the H2O mole fraction, 'g' is water vapor conductance, and the subscripts 's', 'i',
 * and 'a' refer to surface, intercellular, and atmospheric concentrations, respectively.
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
          conductance_ratio_boundary_ip(get_ip(input_parameters, "conductance_ratio_boundary")),
          conductance_ratio_stomata_ip(get_ip(input_parameters, "conductance_ratio_stomata")),
          assimilation_net_ip(get_ip(input_parameters, "assimilation_net")),
          mole_fraction_co2_atmosphere_ip(get_ip(input_parameters, "mole_fraction_co2_atmosphere")),
          mole_fraction_h2o_atmosphere_ip(get_ip(input_parameters, "mole_fraction_h2o_atmosphere")),
          temperature_leaf_ip(get_ip(input_parameters, "temperature_leaf")),
          atmospheric_pressure_ip(get_ip(input_parameters, "atmospheric_pressure")),
          // Get pointers to output parameters
          conductance_boundary_co2_op(get_op(output_parameters, "conductance_boundary_co2")),
          conductance_stomatal_co2_op(get_op(output_parameters, "conductance_stomatal_co2")),
          mole_fraction_co2_leaf_surface_op(get_op(output_parameters, "mole_fraction_co2_leaf_surface")),
          mole_fraction_co2_intercellular_op(get_op(output_parameters, "mole_fraction_co2_intercellular")),
          mole_fraction_h2o_leaf_surface_op(get_op(output_parameters, "mole_fraction_h2o_leaf_surface")),
          mole_fraction_h2o_intercellular_op(get_op(output_parameters, "mole_fraction_h2o_intercellular"))

    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* conductance_boundary_h2o_ip;
    const double* conductance_stomatal_h2o_ip;
    const double* conductance_ratio_boundary_ip;
    const double* conductance_ratio_stomata_ip;
    const double* assimilation_net_ip;
    const double* mole_fraction_co2_atmosphere_ip;
    const double* mole_fraction_h2o_atmosphere_ip;
    const double* temperature_leaf_ip;
    const double* atmospheric_pressure_ip;
    // Pointers to output parameters
    double* conductance_boundary_co2_op;
    double* conductance_stomatal_co2_op;
    double* mole_fraction_co2_leaf_surface_op;
    double* mole_fraction_co2_intercellular_op;
    double* mole_fraction_h2o_leaf_surface_op;
    double* mole_fraction_h2o_intercellular_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_gas_concentrations::get_inputs()
{
    return {
        "conductance_boundary_h2o",      // mol / m^2 / s
        "conductance_stomatal_h2o",      // mol / m^2 / s
        "conductance_ratio_boundary",    // dimensionless
        "conductance_ratio_stomata",     // dimensionless
        "assimilation_net",              // mol / m^2 / s
        "mole_fraction_co2_atmosphere",  // dimensionless from mol / mol
        "mole_fraction_h2o_atmosphere",  // dimensionless from mol / mol
        "temperature_leaf",              // deg. C
        "atmospheric_pressure"           // Pa
    };
}

std::vector<std::string> ed_gas_concentrations::get_outputs()
{
    return {
        "conductance_boundary_co2",         // mol / m^2 / s
        "conductance_stomatal_co2",         // mol / m^2 / s
        "mole_fraction_co2_leaf_surface",   // dimensionless from mol / mol
        "mole_fraction_co2_intercellular",  // dimensionless from mol / mol
        "mole_fraction_h2o_leaf_surface",   // dimensionless from mol / mol
        "mole_fraction_h2o_intercellular"   // dimensionless from mol / mol
    };
}

void ed_gas_concentrations::do_operation() const
{
    // Calculate values for the output quantities
    const double conductance_boundary_co2 = *conductance_boundary_h2o_ip / *conductance_ratio_boundary_ip;  // mol / m^2 / s

    const double conductance_stomatal_co2 = *conductance_stomatal_h2o_ip / *conductance_ratio_stomata_ip;  // mol / m^2 / s

    const double mole_fraction_co2_leaf_surface =
        *mole_fraction_co2_atmosphere_ip - *assimilation_net_ip / conductance_boundary_co2;  // dimensionless

    const double mole_fraction_co2_intercellular =
        mole_fraction_co2_leaf_surface - *assimilation_net_ip / conductance_stomatal_co2;  // dimensionless

    const double mole_fraction_h2o_intercellular = saturation_vapor_pressure(*temperature_leaf_ip) / *atmospheric_pressure_ip;  // dimensionless

    const double h2o_mfs_numer = *conductance_boundary_h2o_ip * *mole_fraction_h2o_atmosphere_ip +
                                 *conductance_stomatal_h2o_ip * mole_fraction_h2o_intercellular;
    const double h2o_mfs_denom = *conductance_boundary_h2o_ip + *conductance_stomatal_h2o_ip;
    const double mole_fraction_h2o_leaf_surface = h2o_mfs_numer / h2o_mfs_denom;  // dimensionless

    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"conductance_ratio_boundary cannot be zero",               *conductance_ratio_boundary_ip == 0},   // divide by zero
        {"conductance_ratio_stomata cannot be zero",                *conductance_ratio_stomata_ip == 0},    // divide by zero
        {"conductance_boundary_co2 cannot be zero",                 conductance_boundary_co2 == 0},         // divide by zero
        {"conductance_stomatal_co2 cannot be zero",                 conductance_stomatal_co2 == 0},         // divide by zero
        {"atmospheric_pressure cannot be zero",                     *atmospheric_pressure_ip == 0},         // divide by zero
        {"the sum of water vapor conductances cannot be zero",      h2o_mfs_denom == 0}                     // divide by zero
    };

    check_error_conditions(errors_to_check, get_name());

    // Update the output parameter list
    update(conductance_boundary_co2_op, conductance_boundary_co2);
    update(conductance_stomatal_co2_op, conductance_stomatal_co2);
    update(mole_fraction_co2_leaf_surface_op, mole_fraction_co2_leaf_surface);
    update(mole_fraction_co2_intercellular_op, mole_fraction_co2_intercellular);
    update(mole_fraction_h2o_leaf_surface_op, mole_fraction_h2o_leaf_surface);
    update(mole_fraction_h2o_intercellular_op, mole_fraction_h2o_intercellular);
}

#endif
