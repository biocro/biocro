#ifndef ED_PENMAN_MONTEITH_LEAF_TEMPERATURE_H
#define ED_PENMAN_MONTEITH_LEAF_TEMPERATURE_H

#include <cmath>           // For pow
#include "../framework/constants.h"  // for ideal_gas_constant and celsius_to_kelvin
#include "../framework/module.h"
#include "../framework/state_map.h"

#include "ed_nikolov_conductance.h"
#include "ed_boundary_conductance.h"
#include "ed_long_wave_energy_loss.h"
#include "ed_water_vapor_properties.h"

/**
 * @class ed_penman_monteith_leaf_temperature
 *
 * @brief Uses the Penman-Monteith equation to determine leaf temperature (see eq. 14.11e from Thornley (1990), p. 418).
 * Currently only intended for use by Ed.
 *
 * See the "ed_nikolov_conductance_forced" module for a discussion about converting molar conductances (with units
 * of mol / m^2 / s) to energy conductances (with units of m / s). In short, we replace all conductances g in the
 * Thornley formula with g * volume_per_mol_of_an_ideal_gas.
 */
class ed_penman_monteith_leaf_temperature : public direct_module
{
   public:
    ed_penman_monteith_leaf_temperature(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get pointers to input quantities
          long_wave_energy_loss_leaf_ip(get_ip(input_quantities, "long_wave_energy_loss_leaf")),
          solar_energy_absorbed_leaf_ip(get_ip(input_quantities, "solar_energy_absorbed_leaf")),
          slope_water_vapor_ip(get_ip(input_quantities, "slope_water_vapor")),
          psychrometric_parameter_ip(get_ip(input_quantities, "psychrometric_parameter")),
          latent_heat_vaporization_of_water_ip(get_ip(input_quantities, "latent_heat_vaporization_of_water")),
          vapor_density_deficit_ip(get_ip(input_quantities, "vapor_density_deficit")),
          conductance_boundary_h2o_ip(get_ip(input_quantities, "conductance_boundary_h2o")),
          conductance_stomatal_h2o_ip(get_ip(input_quantities, "conductance_stomatal_h2o")),
          temperature_air_ip(get_ip(input_quantities, "temp")),
          atmospheric_pressure_ip(get_ip(input_quantities, "atmospheric_pressure")),

          // Get pointers to output quantities
          temperature_leaf_op(get_op(output_quantities, "temperature_leaf"))

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ed_penman_monteith_leaf_temperature"; }

   private:
    // Pointers to input quantities
    const double* long_wave_energy_loss_leaf_ip;
    const double* solar_energy_absorbed_leaf_ip;
    const double* slope_water_vapor_ip;
    const double* psychrometric_parameter_ip;
    const double* latent_heat_vaporization_of_water_ip;
    const double* vapor_density_deficit_ip;
    const double* conductance_boundary_h2o_ip;
    const double* conductance_stomatal_h2o_ip;
    const double* temperature_air_ip;
    const double* atmospheric_pressure_ip;

    // Pointers to output quantities
    double* temperature_leaf_op;

    // Main operation
    void do_operation() const override;
};

string_vector ed_penman_monteith_leaf_temperature::get_inputs()
{
    return {
        "long_wave_energy_loss_leaf",         // W / m^2
        "solar_energy_absorbed_leaf",         // W / m^2 [should include losses to soil / lower canopy layers]
        "slope_water_vapor",                  // kg / m^3 / deg. K
        "psychrometric_parameter",            // kg / m^3 / deg. K
        "latent_heat_vaporization_of_water",  // J / kg
        "vapor_density_deficit",              // kg / m^3
        "conductance_boundary_h2o",           // mol / m^2 / s
        "conductance_stomatal_h2o",           // mol / m^2 / s
        "temp",                               // deg. C
        "atmospheric_pressure"                // Pa
    };
}

string_vector ed_penman_monteith_leaf_temperature::get_outputs()
{
    return {
        "temperature_leaf"  // deg. C
    };
}

void ed_penman_monteith_leaf_temperature::do_operation() const
{
    // Determine the total energy available to the leaf
    const double total_available_energy = *solar_energy_absorbed_leaf_ip - *long_wave_energy_loss_leaf_ip;  // W / m^2

    // Convert temperatures to Kelvin
    const double Tak = *temperature_air_ip + conversion_constants::celsius_to_kelvin;  // Kelvin

    // Calculate the volume of one mole of a gas at air temperature and pressure
    // using the ideal gas law
    const double volume_per_mol = physical_constants::ideal_gas_constant * Tak / *atmospheric_pressure_ip;  // m^3 / mol

    // Convert conductances to m / s
    const double gc = *conductance_stomatal_h2o_ip * volume_per_mol;  // m / s
    const double ga = *conductance_boundary_h2o_ip * volume_per_mol;  // m / s

    const double delta_t_numerator = total_available_energy * (1 / ga + 1 / gc) - *latent_heat_vaporization_of_water_ip * *vapor_density_deficit_ip;
    const double delta_t_denomenator = *latent_heat_vaporization_of_water_ip * (*slope_water_vapor_ip + *psychrometric_parameter_ip * (1 + ga / gc));
    const double delta_t = delta_t_numerator / delta_t_denomenator;

    const double temperature_leaf = *temperature_air_ip + delta_t;

    // Check for error conditions
    std::map<std::string, bool> errors_to_check = {
        {"ga cannot be zero",                   fabs(ga) < calculation_constants::eps_zero},                   // divide by zero
        {"gc cannot be zero",                   fabs(gc) < calculation_constants::eps_zero},                   // divide by zero
        {"delta_t_denomenator cannot be zero",  fabs(delta_t_denomenator) < calculation_constants::eps_zero}   // divide by zero
    };

    check_error_conditions(errors_to_check, get_name());

    update(temperature_leaf_op, temperature_leaf);
}

namespace ed_p_m_temperature_solve_stuff
{
std::string const module_name = "ed_p_m_temperature_solve";

// Create module wrapper objects for the sub-modules
module_creator_impl<ed_nikolov_conductance_forced> forced_conductance;
module_creator_impl<ed_nikolov_conductance_free_solve> free_conductance;
module_creator_impl<ed_boundary_conductance_max> boundary_conductance;
module_creator_impl<ed_long_wave_energy_loss> longwave;
module_creator_impl<ed_water_vapor_properties> water_properties;
module_creator_impl<ed_penman_monteith_leaf_temperature> leaf_temperature;

// Create pointers to the wrappers
mc_vector const sub_mcs{
    &forced_conductance,
    &free_conductance,
    &boundary_conductance,
    &longwave,
    &water_properties,
    &leaf_temperature
};

std::string const solver_type = "newton_raphson_backtrack_boost";

int const max_iterations = 50;

bool const should_reorder_guesses = false;

bool const return_default_on_failure = false;

std::vector<double> const lower_bounds = {-100};

std::vector<double> const upper_bounds = {100};

std::vector<double> const absolute_error_tolerances = {0.001};

std::vector<double> const relative_error_tolerances = {1e-3};
}  // namespace ed_p_m_temperature_solve_stuff

/**
 * @class ed_p_m_temperature_solve
 *
 * @brief Calculates leaf temperature using the Penman-Monteith equation, where boundary
 * layer conductance is determined using the model from Nikolov et al. Currently only
 * intended for use by Ed.
 */
class ed_p_m_temperature_solve : public se_module::base
{
   public:
    ed_p_m_temperature_solve(
        state_map const& input_quantities,
        state_map* output_quantities)
        : se_module::base(ed_p_m_temperature_solve_stuff::module_name,
                          ed_p_m_temperature_solve_stuff::sub_mcs,
                          ed_p_m_temperature_solve_stuff::solver_type,
                          ed_p_m_temperature_solve_stuff::max_iterations,
                          ed_p_m_temperature_solve_stuff::lower_bounds,
                          ed_p_m_temperature_solve_stuff::upper_bounds,
                          ed_p_m_temperature_solve_stuff::absolute_error_tolerances,
                          ed_p_m_temperature_solve_stuff::relative_error_tolerances,
                          ed_p_m_temperature_solve_stuff::should_reorder_guesses,
                          ed_p_m_temperature_solve_stuff::return_default_on_failure,
                          input_quantities,
                          output_quantities),

          // Get references to input quantities
          temperature_air(get_input(input_quantities, "temp"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return ed_p_m_temperature_solve_stuff::module_name; }

   private:
    // References to input quantities
    double const& temperature_air;

    // Main operation
    std::vector<std::vector<double>> get_initial_guesses() const override;
};

string_vector ed_p_m_temperature_solve::get_inputs()
{
    string_vector inputs = se_module::get_se_inputs(ed_p_m_temperature_solve_stuff::sub_mcs);
    inputs.push_back("temp");  // degrees C
    return inputs;
}

string_vector ed_p_m_temperature_solve::get_outputs()
{
    string_vector outputs = se_module::get_se_outputs(ed_p_m_temperature_solve_stuff::sub_mcs);
    outputs.push_back(se_module::get_ncalls_output_name(ed_p_m_temperature_solve_stuff::module_name));
    outputs.push_back(se_module::get_nsteps_output_name(ed_p_m_temperature_solve_stuff::module_name));
    outputs.push_back(se_module::get_success_output_name(ed_p_m_temperature_solve_stuff::module_name));
    return outputs;
}

std::vector<std::vector<double>> ed_p_m_temperature_solve::get_initial_guesses() const
{
    // We want to avoid a spurious high temperature root that may be present when
    // windspeed is low, so we try a low temperature first. If that fails, it is
    // likely because the cusp is lurking at low temperatures. In that case, a
    // higher starting temperature should work.
    double const temperature_offset = 10.0;  // degrees C
    return std::vector<std::vector<double>>{
        {temperature_air - temperature_offset},
        {temperature_air + temperature_offset}};
}

#endif
