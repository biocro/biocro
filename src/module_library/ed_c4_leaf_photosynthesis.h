#ifndef ED_C4_LEAF_PHOTOSYNTHESIS_H
#define ED_C4_LEAF_PHOTOSYNTHESIS_H

namespace ed_c4_leaf_photosynthesis_stuff
{
std::string const module_name = "ed_c4_leaf_photosynthesis";

string_vector const sub_module_names = {
    "ed_rh_to_mole_fraction",                          // Convert relative humidity to H2O mole fraction
    "ed_nikolov_conductance_forced",                   // Calculate `forced` boundary layer conductance
    "ed_nikolov_conductance_free",                     // Calculate `free` boundary layer conductance
    "ed_boundary_conductance_max",                     // Determine overall boundary layer conductance from `forced` and `free` values
    "ed_gas_concentrations",                           // Calculate CO2 and H20 mole fractions at various locations
    "ed_apply_stomatal_water_stress_via_conductance",  // Determine how to account for water stress
    "ed_ball_berry",                                   // Calculate stomatal conductance
    "ed_collatz_c4_assimilation",                      // Calculate net assimilation
    "ed_long_wave_energy_loss",                        // Calculate long-wave energy loss from a leaf to its environment
    "ed_water_vapor_properties",                       // Calculate properties of water vapor from the air temperature and H20 mole fraction
    "ed_penman_monteith_leaf_temperature"              // Calculate leaf temperature
};

std::string const solver_type = "newton_raphson_boost";

int const max_iterations = 50;

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
class ed_c4_leaf_photosynthesis : public se_module::base
{
   public:
    ed_c4_leaf_photosynthesis(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        : se_module::base(ed_c4_leaf_photosynthesis_stuff::module_name,
                          ed_c4_leaf_photosynthesis_stuff::sub_module_names,
                          ed_c4_leaf_photosynthesis_stuff::solver_type,
                          ed_c4_leaf_photosynthesis_stuff::max_iterations,
                          ed_c4_leaf_photosynthesis_stuff::lower_bounds,
                          ed_c4_leaf_photosynthesis_stuff::upper_bounds,
                          ed_c4_leaf_photosynthesis_stuff::absolute_error_tolerances,
                          ed_c4_leaf_photosynthesis_stuff::relative_error_tolerances,
                          input_parameters,
                          output_parameters),
          // Get pointers to input parameters
          collatz_k(get_input(input_parameters, "collatz_k")),
          collatz_vmax(get_input(input_parameters, "collatz_vmax")),
          collatz_alpha(get_input(input_parameters, "collatz_alpha")),
          collatz_PAR_flux(get_input(input_parameters, "collatz_PAR_flux")),
          collatz_rd(get_input(input_parameters, "collatz_rd")),
          ball_berry_intercept(get_input(input_parameters, "ball_berry_intercept")),
          ball_berry_slope(get_input(input_parameters, "ball_berry_slope")),
          StomataWS(get_input(input_parameters, "StomataWS")),
          conductance_stomatal_h2o_min(get_input(input_parameters, "conductance_stomatal_h2o_min")),
          mole_fraction_co2_atmosphere(get_input(input_parameters, "mole_fraction_co2_atmosphere")),
          relative_humidity_atmosphere(get_input(input_parameters, "rh")),
          temperature_air(get_input(input_parameters, "temp"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // References to specific input parameters
    double const& collatz_k;
    double const& collatz_vmax;
    double const& collatz_alpha;
    double const& collatz_PAR_flux;
    double const& collatz_rd;
    double const& ball_berry_intercept;
    double const& ball_berry_slope;
    double const& StomataWS;
    double const& conductance_stomatal_h2o_min;
    double const& mole_fraction_co2_atmosphere;
    double const& relative_humidity_atmosphere;
    double const& temperature_air;
    // Main operation
    std::vector<std::vector<double>> get_initial_guesses() const override;
};

std::vector<std::string> ed_c4_leaf_photosynthesis::get_inputs()
{
    return se_module::get_se_inputs(ed_c4_leaf_photosynthesis_stuff::sub_module_names);
}

std::vector<std::string> ed_c4_leaf_photosynthesis::get_outputs()
{
    std::vector<std::string> outputs = se_module::get_se_outputs(ed_c4_leaf_photosynthesis_stuff::sub_module_names);
    outputs.push_back(se_module::get_ncalls_output_name(ed_c4_leaf_photosynthesis_stuff::module_name));
    outputs.push_back(se_module::get_nsteps_output_name(ed_c4_leaf_photosynthesis_stuff::module_name));
    return outputs;
}

std::vector<std::vector<double>> ed_c4_leaf_photosynthesis::get_initial_guesses() const
{
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
    const double assimilation_carbon_limited = collatz_k * mole_fraction_co2_atmosphere;
    const double assimilation_rubisco_limited = collatz_vmax;
    const double assimilation_light_limited = collatz_alpha * collatz_PAR_flux;
    const double assimilation_gross = std::min(assimilation_carbon_limited, std::min(assimilation_rubisco_limited, assimilation_light_limited));
    const double assimilation_net_estimate = assimilation_gross - collatz_rd;

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
    const double ball_berry_index = std::max(0.0, assimilation_net_estimate) * relative_humidity_atmosphere / mole_fraction_co2_atmosphere;
    const double ball_berry_conductance = ball_berry_intercept + ball_berry_slope * ball_berry_index;
    const double conductance_stomatal_h2o_estimate = conductance_stomatal_h2o_min + StomataWS * (ball_berry_conductance - conductance_stomatal_h2o_min);

    // Note: order must agree with std::sort applied to quantity name
    return std::vector<std::vector<double>>{
        {
            assimilation_net_estimate,          // assimilation_net
            conductance_stomatal_h2o_estimate,  // conductance_boundary_h2o_free (just use the stomatal conductance estimate)
            conductance_stomatal_h2o_estimate,  // conductance_stomatal_h2o
            temperature_air                     // temperature_leaf (assume leaf is at air temperature)
        }                                       // first choice for a guess
    };
}

#endif
