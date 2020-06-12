#ifndef ED_C4_LEAF_PHOTOSYNTHESIS_H
#define ED_C4_LEAF_PHOTOSYNTHESIS_H

namespace ed_c4_leaf_photosynthesis_stuff
{
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

std::string const solver_type = "newton_raphson_backtrack_boost";

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
        : se_module::base("ed_c4_leaf_photosynthesis",
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
          temperature_air_ip(get_ip(input_parameters, "temp")),
          ball_berry_intercept_ip(get_ip(input_parameters, "ball_berry_intercept"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* temperature_air_ip;
    const double* ball_berry_intercept_ip;
    // Main operation
    std::vector<std::vector<double>> get_initial_guesses() const override;
};

std::vector<std::string> ed_c4_leaf_photosynthesis::get_inputs()
{
    std::vector<std::string> inputs = se_module::get_se_inputs(ed_c4_leaf_photosynthesis_stuff::sub_module_names);
    inputs.push_back("temp");
    inputs.push_back("ball_berry_intercept");
    return inputs;
}

std::vector<std::string> ed_c4_leaf_photosynthesis::get_outputs()
{
    return se_module::get_se_outputs(ed_c4_leaf_photosynthesis_stuff::sub_module_names);
}

std::vector<std::vector<double>> ed_c4_leaf_photosynthesis::get_initial_guesses() const
{
    // Note: order must agree with std::sort applied to quantity name
    double const conductance_factor = 15.0;
    return std::vector<std::vector<double>>{
        {
            0.0,                       // assimilation_net
            *ball_berry_intercept_ip,  // conductance_boundary_h2o_free
            *ball_berry_intercept_ip,  // conductance_stomatal_h2o
            *temperature_air_ip        // temperature_leaf
        },                             // first choice for a guess
        {
            0.0,                                            // assimilation_net
            *ball_berry_intercept_ip * conductance_factor,  // conductance_boundary_h2o_free
            *ball_berry_intercept_ip * conductance_factor,  // conductance_stomatal_h2o
            *temperature_air_ip                             // temperature_leaf
        }                                                   // second choice for a guess
    };
}

#endif
