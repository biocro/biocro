#ifndef ED_NIKOLOV_CONDUCTANCE_H
#define ED_NIKOLOV_CONDUCTANCE_H

#include <cmath>           // for fabs and sqrt
#include "../constants.h"  // for physical_constants::celsius_to_kelvin and physical_constants::ideal_gas_constant
#include "../modules.h"
#include "AuxBioCro.h"

namespace nikolov
{
/** Define some constants used in the Nikolov model */
constexpr double temperature_exponent = 0.56;
constexpr double temperature_offset = 120.0;
constexpr double Tvdiff_factor = 0.378;

}  // namespace nikolov

/**
 * @class ed_nikolov_conductance_forced
 * 
 * @brief Calculates forced boundary layer conductance for water according to the model
 * in Nikolov et al. Ecological Modelling 80, 205–235 (1995). Currently only intended
 * for use by Ed.
 * 
 * Discussion of conductance units and assumptions about temperature:
 * 
 * In the Nikolov paper, conductances are expressed in units of m / s. This is appropriate
 * when calculating an energy flux (units: J / m^2 / s) driven by a gas pressure gradient
 * (units: Pa = N / m^2 = J / m^3). However, in BioCro we think of molecular fluxes being
 * driven by a gradient in the mole fraction of a gas.
 * 
 * To shift between these two conventions, note that the internal energy of a gas is
 * proportional to its temperature and the number of molecules according to E = n * R * T,
 * where R is the ideal gas constant, n is the number of moles, and T is the temperature.
 * So if molecular conductance is defined according to
 * 
 *   molecular_flux = molecular_conductance * mole_fraction_difference, (eq. 1)
 * 
 * then we can multiply both sides by R * T to see that
 * 
 *   energy_flux = R * T * molecular_conductance * mole_fraction_difference, (eq. 2)
 * 
 * where we have noted that
 * 
 *   molecular_flux * R * T = n / area / time * R * T
 *                          = energy / area / time
 *                          = energy_flux
 * 
 * For an ideal gas, P * V = n * R * T, so we can solve for R * T = P * V / n and substitute
 * into eq. 2 to see that
 * 
 *   energy_flux = (molecular_conductance * V / n) * (mole_fraction_difference * P)
 *               = energy_conductance * pressure_difference
 * 
 * where it is now clear that energy_conductance = molecular_conductance * V / n. So to
 * convert the Nikolov conductances to BioCro units, we must include the volume per mole
 * of an ideal gas: molecular_conductance = energy_conductance / volume_per_mole.
 * 
 * Strictly speaking, the analysis above only holds when the gas temperature remains constant
 * during its transport. This is apparent in two places. First, energy_flux = molecular_flux *
 * R * T only makes sense if there is a single temperature during the gas transport. Second,
 * using the ideal gas law, we can write pressure_difference = P_2 - P_1 = (R * T_1 * (n / V)_1
 * - R * T_2 * (n / V)_2). It is only possible to factor out R * T if T_1 = T_2 = T.
 * 
 * However, it is important to note that in both cases temperature only appears via the
 * multiplicative factor R * T and that temperature is expressed in absolute units (i.e.,
 * in Kelvin rather than Celsius). In this case the errors introduced by assuming a constant
 * temperature are small. For example, if leaf_temperature = 34 deg. C and air_temperature = 30
 * deg. C, the difference in R * T between gas at leaf temperature vs air temperature is
 * (4 K) / (293.15 K) = 0.013, just over 1%. Compared to other sources of error in 
 * determining the boundary layer conductance, this is small, and so it is acceptable to
 * convert between molecular_conductance and energy_conductance using volume_per_mol
 * determined at air temperature.
 */
class ed_nikolov_conductance_forced : public SteadyModule
{
   public:
    ed_nikolov_conductance_forced(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_nikolov_conductance_forced"),
          // Get pointers to input parameters
          temperature_air_ip(get_ip(input_parameters, "temp")),
          atmospheric_pressure_ip(get_ip(input_parameters, "atmospheric_pressure")),
          windspeed_ip(get_ip(input_parameters, "windspeed")),
          leafwidth_ip(get_ip(input_parameters, "leafwidth")),
          nikolov_cf_ip(get_ip(input_parameters, "nikolov_cf")),
          // Get pointers to output parameters
          conductance_boundary_h2o_forced_op(get_op(output_parameters, "conductance_boundary_h2o_forced"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* temperature_air_ip;
    const double* atmospheric_pressure_ip;
    const double* windspeed_ip;
    const double* leafwidth_ip;
    const double* nikolov_cf_ip;
    // Pointers to output parameters
    double* conductance_boundary_h2o_forced_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_nikolov_conductance_forced::get_inputs()
{
    return {
        "temp",                  // deg. C
        "atmospheric_pressure",  // Pa
        "windspeed",             // m / s
        "leafwidth",             // m
        "nikolov_cf"             // dimensionless, usually 4.322e-3
    };
}

std::vector<std::string> ed_nikolov_conductance_forced::get_outputs()
{
    return {
        "conductance_boundary_h2o_forced"  // mol / m^2 / s
    };
}

void ed_nikolov_conductance_forced::do_operation() const
{
    // Convert temperatures to Kelvin
    const double Tak = *temperature_air_ip + physical_constants::celsius_to_kelvin;  // Kelvin

    // Calculate the volume of one mole of a gas at air temperature and pressure
    // using the ideal gas law
    const double volume_per_mol = physical_constants::ideal_gas_constant * Tak / *atmospheric_pressure_ip;  // m^3 / mol

    // Calculate the forced boundary layer conductance for water vapor
    // using Equation 29 in Nikolov et al. Note that we have used
    // volume_per_mol to convert from m / s to mol / m^2 / s (see
    // class description above for more details).
    const double gbv_forced = *nikolov_cf_ip *
                              pow(Tak, nikolov::temperature_exponent) *
                              pow((Tak + nikolov::temperature_offset) * *windspeed_ip / (*leafwidth_ip * *atmospheric_pressure_ip), 0.5) /
                              volume_per_mol;  // mol / m^2 / s

    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"atmospheric_pressure cannot be zero",                 *atmospheric_pressure_ip == 0},     // divide by zero
        {"volume_per_mol cannot be zero",                       volume_per_mol == 0},               // divide by zero
        {"leaf_width cannot be zero",                           *leafwidth_ip == 0}                 // divide by zero
    };

    check_error_conditions(errors_to_check, get_name());

    // Update the output parameter list
    update(conductance_boundary_h2o_forced_op, gbv_forced);
}

/**
 * @class ed_nikolov_conductance_free
 * 
 * @brief Calculates free boundary layer conductance for water according to the model
 * in Nikolov et al. Ecological Modelling 80, 205–235 (1995). Note that this module has
 * `conductance_boundary_h2o_free` as both an input and an output. Therefore, it can
 * be used by a simultanous_equations object but not a System object. Currently only
 * intended for use by Ed.
 * 
 * See the "ed_nikolov_conductance_forced" module for a discussion of conductance units
 * and assumptions about temperature.
 */
class ed_nikolov_conductance_free : public SteadyModule
{
   public:
    ed_nikolov_conductance_free(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("ed_nikolov_conductance_free"),
          // Get pointers to input parameters
          temperature_air_ip(get_ip(input_parameters, "temp")),
          temperature_leaf_ip(get_ip(input_parameters, "temperature_leaf")),
          atmospheric_pressure_ip(get_ip(input_parameters, "atmospheric_pressure")),
          leafwidth_ip(get_ip(input_parameters, "leafwidth")),
          mole_fraction_h2o_atmosphere_ip(get_ip(input_parameters, "mole_fraction_h2o_atmosphere")),
          conductance_boundary_h2o_free_ip(get_ip(input_parameters, "conductance_boundary_h2o_free")),
          conductance_stomatal_h2o_ip(get_ip(input_parameters, "conductance_stomatal_h2o")),
          nikolov_ce_ip(get_ip(input_parameters, "nikolov_ce")),
          // Get pointers to output parameters
          conductance_boundary_h2o_free_op(get_op(output_parameters, "conductance_boundary_h2o_free"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* temperature_air_ip;
    const double* temperature_leaf_ip;
    const double* atmospheric_pressure_ip;
    const double* leafwidth_ip;
    const double* mole_fraction_h2o_atmosphere_ip;
    const double* conductance_boundary_h2o_free_ip;
    const double* conductance_stomatal_h2o_ip;
    const double* nikolov_ce_ip;
    // Pointers to output parameters
    double* conductance_boundary_h2o_free_op;
    // Main operation
    void do_operation() const override;
};

std::vector<std::string> ed_nikolov_conductance_free::get_inputs()
{
    return {
        "temp",                           // deg. C
        "temperature_leaf",               // deg. C
        "atmospheric_pressure",           // Pa
        "leafwidth",                      // m
        "mole_fraction_h2o_atmosphere",   // dimensionless from mol / mol
        "conductance_boundary_h2o_free",  // mol / m^2 / s
        "conductance_stomatal_h2o",       // mol / m^2 / s
        "nikolov_ce"                      // dimensionless, usually 1.6361e-3
    };
}

std::vector<std::string> ed_nikolov_conductance_free::get_outputs()
{
    return {
        "conductance_boundary_h2o_free"  // mol / m^2 / s
    };
}

void ed_nikolov_conductance_free::do_operation() const
{
    // Convert temperatures to Kelvin
    const double Tak = *temperature_air_ip + physical_constants::celsius_to_kelvin;   // Kelvin
    const double Tlk = *temperature_leaf_ip + physical_constants::celsius_to_kelvin;  // Kelvin

    // Calculate the volume of one mole of a gas at air temperature and pressure
    // using the ideal gas law
    const double volume_per_mol = physical_constants::ideal_gas_constant * Tak / *atmospheric_pressure_ip;  // m^3 / mol

    // Calculate the water vapor mole fraction at the leaf surface that would occur
    // if boundary_layer_conductance = free_boundary_layer_conductance. This calculation
    // is analogous to Equation 35 in Nikolov et al., which was originally derived by
    // requiring equal energy fluxes in the boundary layer and across the stomata. Here
    // we use molecular fluxes instead. Also note that our concentration at the leaf
    // surface is equivalent to Nikolov's concentration in the boundary layer.
    const double mole_fraction_h2o_intercellular = saturation_vapor_pressure(*temperature_leaf_ip) / *atmospheric_pressure_ip;  // dimensionless
    const double h2o_mfs_numer = *conductance_boundary_h2o_free_ip * *mole_fraction_h2o_atmosphere_ip +
                                 *conductance_stomatal_h2o_ip * mole_fraction_h2o_intercellular;
    const double h2o_mfs_denom = *conductance_boundary_h2o_free_ip + *conductance_stomatal_h2o_ip;
    const double mole_fraction_h2o_leaf_surface = h2o_mfs_numer / h2o_mfs_denom;  // dimensionless

    // Calculate the virtual temperature difference using Equation 34 of Nikolov et al.
    // Note that the original equation contains terms of partial_pressure/atmospheric_pressure;
    // We have replaced those terms with equivalent mole fractions.
    const double Tvdiff = Tlk / (1.0 - nikolov::Tvdiff_factor * mole_fraction_h2o_leaf_surface) -
                          Tak / (1.0 - nikolov::Tvdiff_factor * *mole_fraction_h2o_atmosphere_ip);  // deg. C or K

    // Calculate the free boundary layer conductance using Equation 33 from Nikolov et al.
    // Note that we have used volume_per_mol to convert from m / s to mol / m^2 / s (see
    // class description above for more details).
    const double gbv_free = *nikolov_ce_ip *
                            pow(Tlk, nikolov::temperature_exponent) *
                            pow((Tlk + nikolov::temperature_offset) / *atmospheric_pressure_ip, 0.5) *
                            pow(fabs(Tvdiff) / *leafwidth_ip, 0.25) /
                            volume_per_mol;  // mol / m^2 / s

    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"atmospheric_pressure cannot be zero",                 *atmospheric_pressure_ip == 0},     // divide by zero
        {"volume_per_mol cannot be zero",                       volume_per_mol == 0},               // divide by zero
        {"leaf_width cannot be zero",                           *leafwidth_ip == 0},                // divide by zero
        {"the sum of water vapor conductances cannot be zero",  h2o_mfs_denom == 0}                 // divide by zero
    };

    check_error_conditions(errors_to_check, get_name());

    // Update the output parameter list
    update(conductance_boundary_h2o_free_op, gbv_free);
}

#endif
