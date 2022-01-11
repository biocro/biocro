#ifndef ED_C4_LEAF_PHOTOSYNTHESIS3_H
#define ED_C4_LEAF_PHOTOSYNTHESIS3_H

#include <cmath>      // for pow, abs
#include <algorithm>  // for std::max
#include "se_module.h"
#include "../state_map.h"
#include "AuxBioCro.h"   // for saturation_water_vapor_pressure

#include "ed_rh_to_mole_fraction.h"
#include "ed_penman_monteith_leaf_temperature.h"
#include "ed_gas_concentrations.h"
#include "ed_apply_water_stress.h"
#include "ed_ball_berry.h"
#include "ed_collatz_c4_assimilation.h"
#include "ed_penman_monteith_transpiration.h"

#include <Rinternals.h>  // temporary for debugging
bool const eclp3_print = false;

namespace ed_c4_leaf_photosynthesis3_stuff
{
std::string const module_name = "ed_c4_leaf_photosynthesis3";

// Create module wrapper objects for the sub-modules
module_wrapper<ed_rh_to_mole_fraction> mole_fraction;
module_wrapper<ed_p_m_temperature_solve> leaf_temperature;
module_wrapper<ed_gas_concentrations> gas_concentrations;
module_wrapper<ed_apply_stomatal_water_stress_via_conductance> water_stress;
module_wrapper<ed_ball_berry> stomatal_conductance;
module_wrapper<ed_collatz_c4_assimilation> assimilation;
module_wrapper<ed_penman_monteith_transpiration> transpiration;

// Create pointers to the wrappers
mwp_vector const sub_mwps{
    &mole_fraction,
    &leaf_temperature,
    &gas_concentrations,
    &water_stress,
    &stomatal_conductance,
    &assimilation,
    &transpiration
};

std::string const solver_type = "newton_raphson_backtrack_boost";

int const max_iterations = 50;

bool const should_reorder_guesses = true;

bool const return_default_on_failure = false;

// Note: order must agree with std::sort applied to quantity name
std::vector<double> const lower_bounds = {
    -1.0,   // assimilation_net: this limit would correspond to absurdly high respiration
    1e-10,  // conductance_stomatal_h2o: conductance must be positive
};

// Note: order must agree with std::sort applied to quantity name
std::vector<double> const upper_bounds = {
    1.0,  // assimilation_net: this limit would correspond to absurdly high assimilation
    10,   // conductance_stomatal_h2o: this limit would correspond to absurdly high conductance
};

// Note: order must agree with std::sort applied to quantity name
double const abs_error_tol = 0.1;
std::vector<double> const absolute_error_tolerances = {
    1e-8,  // assimilation_net: chosen to be smaller than 0.001 * 50 micromol / m^2 / s
    1e-4,  // conductance_stomatal_h2o: chosen to be smaller than 0.001 * 0.5 mol / m^2 / s
};

// Note: order must agree with std::sort applied to quantity name
double const rel_error_tol = 1e-3;
std::vector<double> const relative_error_tolerances = {
    rel_error_tol,  // assimilation_net: use standard value
    rel_error_tol,  // conductance_stomatal_h2o: use standard value
};
}  // namespace ed_c4_leaf_photosynthesis3_stuff

/**
 * @class ed_c4_leaf_photosynthesis3
 *
 * @brief Solves a set of modules for the unknown quantities `assimilation_net` and
 * `conductance_boundary_h2o_free`. Also returns other quantities derived from these.
 * Represents photosynthesis at the leaf level for a c4 plant.
 */
class ed_c4_leaf_photosynthesis3 : public se_module::base
{
   public:
    ed_c4_leaf_photosynthesis3(
        state_map const& input_quantities,
        state_map* output_quantities)
        : se_module::base(ed_c4_leaf_photosynthesis3_stuff::module_name,
                          ed_c4_leaf_photosynthesis3_stuff::sub_mwps,
                          ed_c4_leaf_photosynthesis3_stuff::solver_type,
                          ed_c4_leaf_photosynthesis3_stuff::max_iterations,
                          ed_c4_leaf_photosynthesis3_stuff::lower_bounds,
                          ed_c4_leaf_photosynthesis3_stuff::upper_bounds,
                          ed_c4_leaf_photosynthesis3_stuff::absolute_error_tolerances,
                          ed_c4_leaf_photosynthesis3_stuff::relative_error_tolerances,
                          ed_c4_leaf_photosynthesis3_stuff::should_reorder_guesses,
                          ed_c4_leaf_photosynthesis3_stuff::return_default_on_failure,
                          input_quantities,
                          output_quantities),
          // Get references to input quantities
          collatz_PAR_flux(get_input(input_quantities, "collatz_PAR_flux")),
          StomataWS(get_input(input_quantities, "StomataWS")),
          collatz_q10(get_input(input_quantities, "collatz_q10")),
          temperature_air(get_input(input_quantities, "temp")),
          collatz_rd(get_input(input_quantities, "collatz_rd")),
          collatz_k(get_input(input_quantities, "collatz_k")),
          collatz_vmax(get_input(input_quantities, "collatz_vmax")),
          collatz_rubisco_temperature_lower(get_input(input_quantities, "collatz_rubisco_temperature_lower")),
          collatz_rubisco_temperature_upper(get_input(input_quantities, "collatz_rubisco_temperature_upper")),
          mole_fraction_co2_atmosphere(get_input(input_quantities, "mole_fraction_co2_atmosphere")),
          relative_humidity_atmosphere(get_input(input_quantities, "rh")),
          conductance_stomatal_h2o_min(get_input(input_quantities, "conductance_stomatal_h2o_min")),
          ball_berry_slope(get_input(input_quantities, "ball_berry_slope")),
          ball_berry_intercept(get_input(input_quantities, "ball_berry_intercept"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return ed_c4_leaf_photosynthesis3_stuff::module_name; }

   private:
    // References to input quantities
    double const& collatz_PAR_flux;
    double const& StomataWS;
    double const& collatz_q10;
    double const& temperature_air;
    double const& collatz_rd;
    double const& collatz_k;
    double const& collatz_vmax;
    double const& collatz_rubisco_temperature_lower;
    double const& collatz_rubisco_temperature_upper;
    double const& mole_fraction_co2_atmosphere;
    double const& relative_humidity_atmosphere;
    double const& conductance_stomatal_h2o_min;
    double const& ball_berry_slope;
    double const& ball_berry_intercept;
    // Main operation
    std::vector<std::vector<double>> get_initial_guesses() const override;
};

string_vector ed_c4_leaf_photosynthesis3::get_inputs()
{
    return se_module::get_se_inputs(ed_c4_leaf_photosynthesis3_stuff::sub_mwps);
}

string_vector ed_c4_leaf_photosynthesis3::get_outputs()
{
    string_vector outputs = se_module::get_se_outputs(ed_c4_leaf_photosynthesis3_stuff::sub_mwps);
    outputs.push_back(se_module::get_ncalls_output_name(ed_c4_leaf_photosynthesis3_stuff::module_name));
    outputs.push_back(se_module::get_nsteps_output_name(ed_c4_leaf_photosynthesis3_stuff::module_name));
    outputs.push_back(se_module::get_success_output_name(ed_c4_leaf_photosynthesis3_stuff::module_name));
    return outputs;
}

std::vector<std::vector<double>> ed_c4_leaf_photosynthesis3::get_initial_guesses() const
{
    // Here we try to guess a good starting point based on the availability
    // of light and water.

    // When light is scarce, gross CO2 assimilation will be close to zero
    // and net assimilation will be dominated by the respiration term. Here we
    // calculate it as in the Collatz model, assuming the leaf is at air temperature.
    double const temperature_factor = pow(collatz_q10, (temperature_air - 25.0) / 10.0);      // dimensionless
    double const respiration_base = collatz_rd * temperature_factor;                          // mol / m^2 / s
    double const respiration_high_temp_inhibition = 1 + exp(0.3 * (temperature_air - 55.0));  // dimensionless
    double const respiration = respiration_base / respiration_high_temp_inhibition;           // mol / m^2 / s

    // When light is plentiful, gross CO2 assimilation will be limited by CO2 or
    // Rubisco. Here we calculate these terms as in the Collatz model, assuming
    // the leaf is at air temperature and the CO2 concentration in the leaf is at the
    // atmospheric level.
    double const co2_rate_constant = collatz_k * temperature_factor;                              // mol / m^2 / s
    double const assimilation_carbon_limited = co2_rate_constant * mole_fraction_co2_atmosphere;  // mol / m^2 / s

    const double rubisco_capacity_base = collatz_vmax * temperature_factor;
    const double rubisco_low_temp_inhibition = 1 + exp(0.3 * (collatz_rubisco_temperature_lower - temperature_air));
    const double rubisco_high_temp_inhibition = 1 + exp(0.3 * (temperature_air - collatz_rubisco_temperature_upper));
    const double assimilation_rubisco_limited = rubisco_capacity_base / (rubisco_low_temp_inhibition * rubisco_high_temp_inhibition);

    // Estimate the gross assimilation using a rectangular hyperbola where the independent
    // variable is the PAR flux. The maximum value should be the carbon limited rate. We have
    // somewhat arbitrarily decided that the rate should be 60% of the maximum when the PAR
    // flux is 2000 micromol / m^2 / s. Essentially, we are attempting to replicate a typical
    // light response curve.
    double const reference_PAR = 2000 * 1e-6;                                                // mol / m^2 / s
    double const reference_relative_rate = 0.6;                                              // dimensionless
    double const PAR_at_half_max = reference_PAR / reference_relative_rate - reference_PAR;  // mol / m^2 / s
    double const assimilation_gross_guess = std::min(assimilation_carbon_limited, assimilation_rubisco_limited) *
                                            collatz_PAR_flux /
                                            (collatz_PAR_flux + PAR_at_half_max);  // mol / m^2 / s

    // Get a guess for net assimilation by subtracting respiration from the guess for gross
    // assimilation. Take water stress into account as a multiplicative factor.
    double const assimilation_net_guess = StomataWS * (assimilation_gross_guess - respiration);  // mol / m^2 / s

    // Make a guess for the Ball-Berry conductance using the guess for net assimilation determined
    // above. Here we assume the leaf is at air temperature and the concentrations of H2O and CO2 at
    // the leaf surface are at their atmospheric values.
    double const ball_berry_index = std::max(assimilation_net_guess, 0.0) * relative_humidity_atmosphere / mole_fraction_co2_atmosphere;
    double const ball_berry_conductance = ball_berry_slope * ball_berry_index + ball_berry_intercept;

    // Adjust the conductance according to water availability, treating the Ball-Berry value as a
    // maximum that occurs when water is plentiful.
    double const conductance_guess = conductance_stomatal_h2o_min +
                                     StomataWS * (ball_berry_conductance - conductance_stomatal_h2o_min);

    // We also want to try a few perturbations surrounding this guess.
    // For net assimilation, we'll try using 10% of the guess value as an additive perturbation, but also enforce a minumum.
    // For stomatal conductance, we'll try using a factor of 10 as a multiplicative perturbation
    double const perturbation_fraction_an = 0.1;                                                                            // dimensionless
    double const min_an_perturbation = 1e-9;                                                                                // mol / m^2 / s
    double const an_perturbation = std::max(perturbation_fraction_an * fabs(assimilation_net_guess), min_an_perturbation);  // mol / m^2 / s
    double const perturbation_fraction_gs = 0.1;                                                                            // dimensionless

    // We'll add and subtract the perturbation from each guess `num_pts` times, i.e.,
    // we'll try 2 * num_pts + 1 guesses for each parameter and test each pair for a
    // total of (2 * num_pts + 1)^2 guesses.
    int const num_pts = 2;

    std::vector<std::vector<double>> input_guesses;

    std::string message = "Guesses from `ed_c4_leaf_photosynthesis3`:\n";
    char buff[1024];
    sprintf(buff, " central an guess = %e, central gs guess = %e\n", assimilation_net_guess, conductance_guess);
    message += std::string(buff);

    for (int i = -num_pts; i <= num_pts; ++i) {
        double an_value = assimilation_net_guess + an_perturbation * i;
        for (int j = -num_pts; j <= num_pts; ++j) {
            double gs_value = conductance_guess * pow(perturbation_fraction_gs, j);
            input_guesses.push_back({an_value, gs_value});
            sprintf(buff, " an = %e, gs = %e\n", an_value, gs_value);
            message += std::string(buff);
        }
    }
    message += "\n";
    if (eclp3_print) {
        Rprintf(message.c_str());
    }

    return input_guesses;
}

#endif
