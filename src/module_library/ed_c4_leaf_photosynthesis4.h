#ifndef ED_C4_LEAF_PHOTOSYNTHESIS4_H
#define ED_C4_LEAF_PHOTOSYNTHESIS4_H

#include "se_module.h"
#include <cmath>                            // for fabs and sqrt
#include <algorithm>                        // for std::min and std::max
#include "../constants.h"                   // for physical_constants::celsius_to_kelvin and physical_constants::ideal_gas_constant
#include "ed_nikolov_conductance.h"         // for nikolov namespace
#include "AuxBioCro.h"                      // for TempToLHV and other similar functions
#include "../se_solver_helper_functions.h"  // for generate_guess_list
#include <Rinternals.h>                     // for debugging
const bool eclp4_print = false;             // for debugging

namespace ed_c4_leaf_photosynthesis4_stuff
{
std::string const module_name = "ed_c4_leaf_photosynthesis4";

string_vector const sub_module_names = {
    "ed_rh_to_mole_fraction",                          // Convert relative humidity to H2O mole fraction
    "ed_nikolov_conductance_forced",                   // Calculate `forced` boundary layer conductance
    "ed_nikolov_conductance_free_solve",               // Calculate `free` boundary layer conductance
    "ed_boundary_conductance_max",                     // Determine overall boundary layer conductance from `forced` and `free` values
    "ed_gas_concentrations",                           // Calculate CO2 and H20 mole fractions at various locations
    "ed_apply_stomatal_water_stress_via_conductance",  // Determine how to account for water stress
    "ed_ball_berry",                                   // Calculate stomatal conductance
    "ed_collatz_c4_assimilation",                      // Calculate net assimilation
    "ed_long_wave_energy_loss",                        // Calculate long-wave energy loss from a leaf to its environment
    "ed_water_vapor_properties",                       // Calculate properties of water vapor from the air temperature and H20 mole fraction
    "ed_leaf_temperature"                              // Calculate leaf temperature and transpiration
};

std::string const solver_type = "newton_raphson_backtrack_boost";

int const max_iterations = 50;

bool const should_reorder_guesses = true;

// Note: order must agree with std::sort applied to quantity name
std::vector<double> const lower_bounds = {
    -1.0,   // assimilation_net: this limit would correspond to absurdly high respiration
    1e-10,  // conductance_stomatal_h2o: conductance must be positive
    -100    // temperature_leaf: significantly colder than the coldest air temperature ever recorded on the Earth's surface
};

// Note: order must agree with std::sort applied to quantity name
std::vector<double> const upper_bounds = {
    1.0,  // assimilation_net: this limit would correspond to absurdly high assimilation
    10,   // conductance_stomatal_h2o: this limit would correspond to absurdly high conductance
    100   // temperature_leaf: significantly higher than the hottest air temperature ever recorded on the Earth's surface
};

// Note: order must agree with std::sort applied to quantity name
std::vector<double> const absolute_error_tolerances = {
    1e-8,  // assimilation_net: chosen to be smaller than 0.001 * 50 micromol / m^2 / s
    1e-4,  // conductance_stomatal_h2o: chosen to be smaller than 0.001 * 0.5 mol / m^2 / s
    1e-3   // temperature_leaf
};

// Note: order must agree with std::sort applied to quantity name
double const rel_error_tol = 1e-3;
std::vector<double> const relative_error_tolerances = {
    rel_error_tol,  // assimilation_net: use standard value
    rel_error_tol,  // conductance_stomatal_h2o: use standard value
    rel_error_tol   // temperature_leaf: use standard value
};
}  // namespace ed_c4_leaf_photosynthesis4_stuff

/**
 * @class ed_c4_leaf_photosynthesis4
 * 
 * @brief Solves a set of modules for the unknown quantities assimilation_net,
 * conductance_stomatal_h2o, and temperature_leaf. Also returns other quantities
 * derived from these. Represents photosynthesis at the leaf level for a c4 plant.
 */
class ed_c4_leaf_photosynthesis4 : public se_module::base
{
   public:
    ed_c4_leaf_photosynthesis4(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        : se_module::base(ed_c4_leaf_photosynthesis4_stuff::module_name,
                          ed_c4_leaf_photosynthesis4_stuff::sub_module_names,
                          ed_c4_leaf_photosynthesis4_stuff::solver_type,
                          ed_c4_leaf_photosynthesis4_stuff::max_iterations,
                          ed_c4_leaf_photosynthesis4_stuff::lower_bounds,
                          ed_c4_leaf_photosynthesis4_stuff::upper_bounds,
                          ed_c4_leaf_photosynthesis4_stuff::absolute_error_tolerances,
                          ed_c4_leaf_photosynthesis4_stuff::relative_error_tolerances,
                          ed_c4_leaf_photosynthesis4_stuff::should_reorder_guesses,
                          input_parameters,
                          output_parameters),
          // Get pointers to input parameters
          collatz_PAR_flux(get_input(input_parameters, "collatz_PAR_flux")),
          StomataWS(get_input(input_parameters, "StomataWS")),
          collatz_q10(get_input(input_parameters, "collatz_q10")),
          temperature_air(get_input(input_parameters, "temp")),
          collatz_rd(get_input(input_parameters, "collatz_rd")),
          collatz_k(get_input(input_parameters, "collatz_k")),
          collatz_vmax(get_input(input_parameters, "collatz_vmax")),
          collatz_rubisco_temperature_lower(get_input(input_parameters, "collatz_rubisco_temperature_lower")),
          collatz_rubisco_temperature_upper(get_input(input_parameters, "collatz_rubisco_temperature_upper")),
          mole_fraction_co2_atmosphere(get_input(input_parameters, "mole_fraction_co2_atmosphere")),
          relative_humidity_atmosphere(get_input(input_parameters, "rh")),
          conductance_stomatal_h2o_min(get_input(input_parameters, "conductance_stomatal_h2o_min")),
          ball_berry_slope(get_input(input_parameters, "ball_berry_slope")),
          ball_berry_intercept(get_input(input_parameters, "ball_berry_intercept"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // References to specific input parameters
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

std::vector<std::string> ed_c4_leaf_photosynthesis4::get_inputs()
{
    return se_module::get_se_inputs(ed_c4_leaf_photosynthesis4_stuff::sub_module_names);
}

std::vector<std::string> ed_c4_leaf_photosynthesis4::get_outputs()
{
    std::vector<std::string> outputs = se_module::get_se_outputs(ed_c4_leaf_photosynthesis4_stuff::sub_module_names);
    outputs.push_back(se_module::get_ncalls_output_name(ed_c4_leaf_photosynthesis4_stuff::module_name));
    outputs.push_back(se_module::get_nsteps_output_name(ed_c4_leaf_photosynthesis4_stuff::module_name));
    return outputs;
}

std::vector<std::vector<double>> ed_c4_leaf_photosynthesis4::get_initial_guesses() const
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
    double const conductance_stomatal_h2o_guess = conductance_stomatal_h2o_min +
                                                  StomataWS * (ball_berry_conductance - conductance_stomatal_h2o_min);

    // The guess for leaf temperature is less critical. Just set it equal to ambient temperature.
    double const temperature_leaf_guess = temperature_air;

    std::string message = "Initial guess calculated by ed_c4_leaf_photosynthesis4:\n";
    char buff[128];
    sprintf(buff, " assimilation_net_guess = %e\n", assimilation_net_guess);
    message += std::string(buff);
    sprintf(buff, " conductance_stomatal_h2o_guess = %e\n", conductance_stomatal_h2o_guess);
    message += std::string(buff);
    sprintf(buff, " temperature_leaf_guess = %e\n", temperature_leaf_guess);
    message += std::string(buff);

    std::vector<std::string> names = {
        "assimilation_net",
        "conductance_stomatal_h2o",
        "temperature_leaf"};

    message += std::string("\nOrder of unknown variable names as supplied:\n");
    message += std::string(" ") + names[0] + std::string("\n");
    message += std::string(" ") + names[1] + std::string("\n");
    message += std::string(" ") + names[2] + std::string("\n");

    std::sort(names.begin(), names.end());

    message += std::string("\nOrder of unknown variable names after applying std::sort:\n");
    message += std::string(" ") + names[0] + std::string("\n");
    message += std::string(" ") + names[1] + std::string("\n");
    message += std::string(" ") + names[2] + std::string("\n");

    // We also want to try a few perturbations surrounding this guess.
    // For net assimilation, we'll try using a percentage of the guess value as an additive perturbation, but also enforce a minumum.
    // For stomatal conductance, we'll try using a multiplicative factor for the perturbation
    // For leaf temperature, we'll just use a fixed step
    double const perturbation_fraction_an = 0.1;                                                                            // dimensionless
    double const min_an_perturbation = 1e-9;                                                                                // mol / m^2 / s
    double const an_perturbation = std::max(perturbation_fraction_an * fabs(assimilation_net_guess), min_an_perturbation);  // mol / m^2 / s
    double const perturbation_fraction_gs = 0.1;                                                                            // dimensionless
    double const tl_perturbation = 5.0;                                                                                     // degrees C

    // For An and Gs, we'll add and subtract the perturbation from each guess `num_pts`
    // times, i.e., we'll try 2 * num_pts + 1 guesses for each parameter. For Tl, we'll
    // fix num_pts to be 1. So in total, we will try 3 * (2 * num_pts + 1)^2 guesses.
    int const num_pts = 2;
    int const num_pts_tl = 1;

    std::vector<std::vector<double>> input_guesses;

    message += "\nGuesses from `ed_c4_leaf_photosynthesis4`:\n";

    for (int i = -num_pts; i <= num_pts; ++i) {
        double an_value = assimilation_net_guess + an_perturbation * i;
        for (int j = -num_pts; j <= num_pts; ++j) {
            double gs_value = conductance_stomatal_h2o_guess * pow(perturbation_fraction_gs, j);
            for (int k = -num_pts_tl; k <= num_pts_tl; ++k) {
                double tl_value = temperature_leaf_guess + tl_perturbation * k;
                input_guesses.push_back({an_value, gs_value, tl_value});
                sprintf(buff, " an = %e, gs = %e, tl = %e\n", an_value, gs_value, tl_value);
                message += std::string(buff);
            }
        }
    }

    message += "\n";
    if (eclp4_print) {
        Rprintf(message.c_str());
    }

    return input_guesses;
}

#endif
