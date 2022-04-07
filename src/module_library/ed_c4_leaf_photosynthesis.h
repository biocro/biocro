#ifndef ED_C4_LEAF_PHOTOSYNTHESIS_H
#define ED_C4_LEAF_PHOTOSYNTHESIS_H

#include "se_module.h"
#include "../framework/state_map.h"
#include <cmath>                                      // for fabs and sqrt
#include <algorithm>                                  // for std::min and std::max
#include "../framework/constants.h"                   // for conversion_constants::celsius_to_kelvin and physical_constants::ideal_gas_constant
#include "ed_nikolov_conductance.h"                   // for nikolov namespace
#include "AuxBioCro.h"                                // for TempToLHV and other similar functions
#include "../framework/se_solver_helper_functions.h"  // for generate_guess_list

#include "ed_rh_to_mole_fraction.h"
#include "ed_nikolov_conductance.h"
#include "ed_boundary_conductance.h"
#include "ed_gas_concentrations.h"
#include "ed_apply_water_stress.h"
#include "ed_ball_berry.h"
#include "ed_collatz_c4_assimilation.h"
#include "ed_long_wave_energy_loss.h"
#include "ed_water_vapor_properties.h"
#include "ed_penman_monteith_leaf_temperature.h"
#include "ed_penman_monteith_transpiration.h"

#include <Rinternals.h>         // for debugging
const bool eclp_print = false;  // for debugging

namespace ed_c4_leaf_photosynthesis_stuff
{
std::string const module_name = "ed_c4_leaf_photosynthesis";

// Create module wrapper objects for the sub-modules
module_creator_impl<standardBML::ed_rh_to_mole_fraction> mole_fraction;
module_creator_impl<standardBML::ed_nikolov_conductance_forced> forced_conductance;
module_creator_impl<standardBML::ed_nikolov_conductance_free> free_conductance;
module_creator_impl<standardBML::ed_boundary_conductance_max> boundary_conductance;
module_creator_impl<standardBML::ed_gas_concentrations> gas_concentrations;
module_creator_impl<standardBML::ed_apply_stomatal_water_stress_via_conductance> water_stress;
module_creator_impl<standardBML::ed_ball_berry> stomatal_conductance;
module_creator_impl<standardBML::ed_collatz_c4_assimilation> assimilation;
module_creator_impl<standardBML::ed_long_wave_energy_loss> longwave;
module_creator_impl<standardBML::ed_water_vapor_properties> water_properties;
module_creator_impl<standardBML::ed_penman_monteith_leaf_temperature> leaf_temperature;
module_creator_impl<standardBML::ed_penman_monteith_transpiration> transpiration;

// Create pointers to the wrappers
mc_vector const sub_mcs{
    &mole_fraction,
    &forced_conductance,
    &free_conductance,
    &boundary_conductance,
    &gas_concentrations,
    &water_stress,
    &stomatal_conductance,
    &assimilation,
    &longwave,
    &water_properties,
    &leaf_temperature,
    &transpiration};

std::string const solver_type = "newton_raphson_backtrack_boost";

int const max_iterations = 30;

int const num_guesses_to_try_educated = 100;

int const num_guesses_to_try_other = 100;

bool const should_reorder_guesses = true;

bool const return_default_on_failure = false;

// Note: order must agree with std::sort applied to quantity name
std::vector<double> const lower_bounds = {
    -1.0,   // assimilation_net: this limit would correspond to absurdly high respiration
    1e-10,  // conductance_boundary_h2o_free: chosen to be the same as conductance_stomatal_h2o limits
    1e-10,  // conductance_stomatal_h2o: conductance must be positive
    -110    // temperature_leaf: significantly colder than the coldest air temperature ever recorded on the Earth's surface
};

// Note: order must agree with std::sort applied to quantity name
std::vector<double> const upper_bounds = {
    1.0,  // assimilation_net: this limit would correspond to absurdly high assimilation
    10,   // conductance_boundary_h2o_free: chosen to be the same as conductance_stomatal_h2o limits
    10,   // conductance_stomatal_h2o: this limit would correspond to absurdly high conductance
    80    // temperature_leaf: significantly higher than the hottest air temperature ever recorded on the Earth's surface
};

// Note: order must agree with std::sort applied to quantity name
double const abs_error_tol = 0.1;
std::vector<double> const absolute_error_tolerances = {
    abs_error_tol,  // assimilation_net: use standard value
    abs_error_tol,  // conductance_boundary_h2o_free: use standard value
    abs_error_tol,  // conductance_stomatal_h2o: use standard value
    abs_error_tol   // temperature_leaf: use standard value
};

// Note: order must agree with std::sort applied to quantity name
double const rel_error_tol = 1e-3;
std::vector<double> const relative_error_tolerances = {
    rel_error_tol,  // assimilation_net: use standard value
    rel_error_tol,  // conductance_boundary_h2o_free: use standard value
    rel_error_tol,  // conductance_stomatal_h2o: use standard value
    rel_error_tol   // temperature_leaf: use standard value
};
}  // namespace ed_c4_leaf_photosynthesis_stuff

/**
 * @class ed_c4_leaf_photosynthesis
 *
 * @brief Solves a set of modules for the unknown quantities assimilation_net,
 * conductance_boundary_h2o_free, conductance_stomatal_h2o, and temperature_leaf.
 * Also returns other quantities derived from these. Represents photosynthesis at
 * the leaf level for a c4 plant.
 */
namespace standardBML
{
class ed_c4_leaf_photosynthesis : public se_module::base
{
   public:
    ed_c4_leaf_photosynthesis(
        state_map const& input_quantities,
        state_map* output_quantities)
        : se_module::base(ed_c4_leaf_photosynthesis_stuff::module_name,
                          ed_c4_leaf_photosynthesis_stuff::sub_mcs,
                          ed_c4_leaf_photosynthesis_stuff::solver_type,
                          ed_c4_leaf_photosynthesis_stuff::max_iterations,
                          ed_c4_leaf_photosynthesis_stuff::lower_bounds,
                          ed_c4_leaf_photosynthesis_stuff::upper_bounds,
                          ed_c4_leaf_photosynthesis_stuff::absolute_error_tolerances,
                          ed_c4_leaf_photosynthesis_stuff::relative_error_tolerances,
                          ed_c4_leaf_photosynthesis_stuff::should_reorder_guesses,
                          ed_c4_leaf_photosynthesis_stuff::return_default_on_failure,
                          input_quantities,
                          output_quantities),
          // Get pointers to input quantities
          collatz_k(get_input(input_quantities, "collatz_k")),
          collatz_vmax(get_input(input_quantities, "collatz_vmax")),
          collatz_alpha(get_input(input_quantities, "collatz_alpha")),
          collatz_PAR_flux(get_input(input_quantities, "collatz_PAR_flux")),
          collatz_rd(get_input(input_quantities, "collatz_rd")),
          ball_berry_intercept(get_input(input_quantities, "ball_berry_intercept")),
          ball_berry_slope(get_input(input_quantities, "ball_berry_slope")),
          nikolov_ce(get_input(input_quantities, "nikolov_ce")),
          leafwidth(get_input(input_quantities, "leafwidth")),
          StomataWS(get_input(input_quantities, "StomataWS")),
          conductance_stomatal_h2o_min(get_input(input_quantities, "conductance_stomatal_h2o_min")),
          mole_fraction_co2_atmosphere(get_input(input_quantities, "mole_fraction_co2_atmosphere")),
          relative_humidity_atmosphere(get_input(input_quantities, "rh")),
          temperature_air(get_input(input_quantities, "temp")),
          solar_energy_absorbed_leaf(get_input(input_quantities, "solar_energy_absorbed_leaf")),
          atmospheric_pressure(get_input(input_quantities, "atmospheric_pressure")),
          specific_heat_of_air(get_input(input_quantities, "specific_heat_of_air"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return ed_c4_leaf_photosynthesis_stuff::module_name; }

   private:
    // References to specific input quantities
    double const& collatz_k;
    double const& collatz_vmax;
    double const& collatz_alpha;
    double const& collatz_PAR_flux;
    double const& collatz_rd;
    double const& ball_berry_intercept;
    double const& ball_berry_slope;
    double const& nikolov_ce;
    double const& leafwidth;
    double const& StomataWS;
    double const& conductance_stomatal_h2o_min;
    double const& mole_fraction_co2_atmosphere;
    double const& relative_humidity_atmosphere;
    double const& temperature_air;
    double const& solar_energy_absorbed_leaf;
    double const& atmospheric_pressure;
    double const& specific_heat_of_air;
    // Main operation
    std::vector<std::vector<double>> get_initial_guesses() const override;
};

string_vector ed_c4_leaf_photosynthesis::get_inputs()
{
    return se_module::get_se_inputs(ed_c4_leaf_photosynthesis_stuff::sub_mcs);
}

string_vector ed_c4_leaf_photosynthesis::get_outputs()
{
    string_vector outputs = se_module::get_se_outputs(ed_c4_leaf_photosynthesis_stuff::sub_mcs);
    outputs.push_back(se_module::get_ncalls_output_name(ed_c4_leaf_photosynthesis_stuff::module_name));
    outputs.push_back(se_module::get_nsteps_output_name(ed_c4_leaf_photosynthesis_stuff::module_name));
    outputs.push_back(se_module::get_success_output_name(ed_c4_leaf_photosynthesis_stuff::module_name));
    return outputs;
}

std::vector<std::vector<double>> ed_c4_leaf_photosynthesis::get_initial_guesses() const
{
    // TODO: find a way to reduce duplicated code here. Maybe using module set objects?

    // Get a crappy estimate for an assimilation rate using
    // a simplified version of the collatz model.
    //
    // Here we make the following assumptions, which are
    // unlikely to be true but may nevertheless help choose
    // a good starting guess:
    //  (1) the leaf is at 25 C
    //      (i.e., we don't apply any temperature factors)
    //  (2) the CO2 concentration inside the leaf is equal to the atmospheric value
    //      (i.e., Ci = Ca)
    //  (3) gross assimilation is simply the smallest of the CO2, rubisco, and light limited rates
    //      (i.e., no quadratic mixing)
    const double assimilation_carbon_limited = collatz_k * mole_fraction_co2_atmosphere;                                                          // mol / m^2 / s
    const double assimilation_rubisco_limited = collatz_vmax;                                                                                     // mol / m^2 / s
    const double assimilation_light_limited = collatz_alpha * collatz_PAR_flux;                                                                   // mol / m^2 / s
    const double assimilation_gross = std::min(assimilation_carbon_limited, std::min(assimilation_rubisco_limited, assimilation_light_limited));  // mol / m^2 / s
    const double assimilation_net_estimate = assimilation_gross - collatz_rd;                                                                     // mol / m^2 / s

    // Get a crappy estimation for stomatal conductance using the
    // Ball-Berry model.
    //
    // Here we make the following assumptions, which are
    // unlikely to be true but may nevertheless help choose
    // a good starting guess:
    //  (1) the CO2 concentration at the leaf surface is equal to the atmospheric value
    //      (i.e., Cs = Ca)
    //  (2) the relative humidity at the leaf surface is equal to the atmospheric value
    //      (i.e., RHs = RHa)
    //  (3) net assimilation is given by the estimate calculated above
    const double ball_berry_index = std::max(0.0, assimilation_net_estimate) * relative_humidity_atmosphere / mole_fraction_co2_atmosphere;               // mol / m^2 / s
    const double ball_berry_conductance = ball_berry_intercept + ball_berry_slope * ball_berry_index;                                                     // mol / m^2 / s
    const double conductance_stomatal_h2o_estimate = conductance_stomatal_h2o_min + StomataWS * (ball_berry_conductance - conductance_stomatal_h2o_min);  // mol / m^2 / s

    // Get a crappy estimate for leaf temperature using the Penman-Monteith model.
    //
    // Here we make the following assumptions, which are
    // unlikely to be true but may nevertheless help choose
    // a good starting guess:
    //  (1) long wave energy loss from the leaf to its environment is zero
    //  (2) boundary layer conductance is 1.2 mol / m^2 / s, an arbitrary value that
    //      used to be hard-coded into the Ball-Berry model
    const double latent_heat_vaporization_of_water = TempToLHV(temperature_air);                // J / kg
    const double slope_water_vapor = TempToSFS(temperature_air);                                // kg / m^3 / K
    const double saturation_water_vapor_pressure = saturation_vapor_pressure(temperature_air);  // Pa
    const double density_of_dry_air = TempToDdryA(temperature_air);                             // kg / m^3
    const double saturation_water_vapor_content = saturation_water_vapor_pressure /
                                                  physical_constants::ideal_gas_constant /
                                                  (temperature_air + conversion_constants::celsius_to_kelvin) *
                                                  physical_constants::molar_mass_of_water;                     // kg / m^3
    const double vapor_density_deficit = saturation_water_vapor_content * (1 - relative_humidity_atmosphere);  // Pa
    const double psychrometric_parameter = density_of_dry_air *
                                           specific_heat_of_air /
                                           latent_heat_vaporization_of_water;                           // kg / m^3 / K
    const double Tak = temperature_air + conversion_constants::celsius_to_kelvin;                       // Kelvin
    const double volume_per_mol = physical_constants::ideal_gas_constant * Tak / atmospheric_pressure;  // m^3 / mol

    const double total_available_energy = solar_energy_absorbed_leaf;      // J / m^2 / s
    const double gc = conductance_stomatal_h2o_estimate * volume_per_mol;  // m / s
    const double ga = 1.2 * volume_per_mol;                                // m / s
    const double delta_t_numerator = total_available_energy * (1 / ga + 1 / gc) - latent_heat_vaporization_of_water * vapor_density_deficit;
    const double delta_t_denomenator = latent_heat_vaporization_of_water * (slope_water_vapor + psychrometric_parameter * (1 + ga / gc));
    const double delta_t = delta_t_numerator / delta_t_denomenator;      // deg. C
    const double temperature_leaf_estimate = temperature_air + delta_t;  // deg. C

    // Get a crappy estimate for free boundary layer conductance using the Nikolov model.
    //
    // Here we make the following assumptions, which are
    // unlikely to be true but may nevertheless help choose
    // a good starting guess:
    //  (1) the leaf temperature is the value estimated above
    //  (2) the relative humidity at the leaf surface is equal to the atmospheric value
    const double Tlk = temperature_leaf_estimate + conversion_constants::celsius_to_kelvin;                                             // Kelvin
    const double mole_fraction_h2o_atmosphere = relative_humidity_atmosphere * saturation_water_vapor_pressure / atmospheric_pressure;  // dimensionless
    const double mole_fraction_h2o_leaf_surface = mole_fraction_h2o_atmosphere;
    const double Tvdiff = Tlk / (1.0 - nikolov::Tvdiff_factor * mole_fraction_h2o_leaf_surface) -
                          Tak / (1.0 - nikolov::Tvdiff_factor * mole_fraction_h2o_atmosphere);  // deg. C or K
    const double gbv_free_estimate = nikolov_ce *
                                     pow(Tlk, nikolov::temperature_exponent) *
                                     pow((Tlk + nikolov::temperature_offset) / atmospheric_pressure, 0.5) *
                                     pow(fabs(Tvdiff) / leafwidth, 0.25) /
                                     volume_per_mol;  // mol / m^2 / s

    std::string message = "Initial guess calculated by ed_c4_leaf_photosynthesis:\n";
    char buff[128];
    sprintf(buff, " assimilation_net_estimate = %e\n", assimilation_net_estimate);
    message += std::string(buff);
    sprintf(buff, " gbv_free_estimate = %e\n", gbv_free_estimate);
    message += std::string(buff);
    sprintf(buff, " conductance_stomatal_h2o_estimate = %e\n", conductance_stomatal_h2o_estimate);
    message += std::string(buff);
    sprintf(buff, " temperature_leaf_estimate = %e\n", temperature_leaf_estimate);
    message += std::string(buff);

    string_vector names = {
        "assimilation_net",
        "conductance_boundary_h2o_free",
        "conductance_stomatal_h2o",
        "temperature_leaf"};

    message += std::string("\nOrder of unknown variable names as supplied:\n");
    message += std::string(" ") + names[0] + std::string("\n");
    message += std::string(" ") + names[1] + std::string("\n");
    message += std::string(" ") + names[2] + std::string("\n");
    message += std::string(" ") + names[3] + std::string("\n");

    std::sort(names.begin(), names.end());

    message += std::string("\nOrder of unknown variable names after applying std::sort:\n");
    message += std::string(" ") + names[0] + std::string("\n");
    message += std::string(" ") + names[1] + std::string("\n");
    message += std::string(" ") + names[2] + std::string("\n");
    message += std::string(" ") + names[3] + std::string("\n");

    message += "\n";
    if (eclp_print) {
        Rprintf(message.c_str());
    }

    // Get some guesses distributed around the educated guess
    std::vector<double> educated_central_guess = {
        assimilation_net_estimate,          // assimilation_net
        gbv_free_estimate,                  // conductance_boundary_h2o_free
        conductance_stomatal_h2o_estimate,  // conductance_stomatal_h2o
        temperature_leaf_estimate           // temperature_leaf
    };

    double const just_less_than_one = 0.999;
    std::vector<double> educated_scale_factors = {
        2.0 * fabs(assimilation_net_estimate),                         // assimilation_net: allow it to vary within [-a_estimate, 3 * a_estimate]
        just_less_than_one * fabs(gbv_free_estimate),                  // conductance_boundary_h2o_free: allow it to vary within (0, 2 * g_estimate)
        just_less_than_one * fabs(conductance_stomatal_h2o_estimate),  // conductance_stomatal_h2o: allow it to vary within (0, 2 * g_estimate)
        5.0                                                            // temperature_leaf: allow it to vary by +/- 5 deg. C
    };

    std::vector<std::vector<double>> full_guess_list = generate_guess_list(
        educated_central_guess,
        ed_c4_leaf_photosynthesis_stuff::lower_bounds,
        ed_c4_leaf_photosynthesis_stuff::upper_bounds,
        educated_scale_factors,
        ed_c4_leaf_photosynthesis_stuff::num_guesses_to_try_educated);

    // Get some other guesses from a wider region of parameter space
    double const conductance_factor = 10.0;
    std::vector<double> other_central_guess = {
        0.0,                                        // assimilation_net
        conductance_factor * ball_berry_intercept,  // conductance_boundary_h2o_free
        conductance_factor * ball_berry_intercept,  // conductance_stomatal_h2o
        temperature_air                             // temperature_leaf
    };

    std::vector<double> other_scale_factors = {
        5e-4,                                                            // assimilation_net
        just_less_than_one * conductance_factor * ball_berry_intercept,  // conductance_boundary_h2o_free: ensure we can't reach zero
        just_less_than_one * conductance_factor * ball_berry_intercept,  // conductance_stomatal_h2o: ensure we can't reach zero
        40.0                                                             // temperature_leaf
    };

    std::vector<std::vector<double>> temp_guess_list = generate_guess_list(
        other_central_guess,
        ed_c4_leaf_photosynthesis_stuff::lower_bounds,
        ed_c4_leaf_photosynthesis_stuff::upper_bounds,
        other_scale_factors,
        ed_c4_leaf_photosynthesis_stuff::num_guesses_to_try_other);

    // Combine the lists
    for (std::vector<double> const& guess : temp_guess_list) {
        full_guess_list.push_back(guess);
    }
    return full_guess_list;
}

}  // namespace standardBML
#endif
