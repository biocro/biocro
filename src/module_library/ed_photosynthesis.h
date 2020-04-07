#ifndef ED_LEAF_PHOTOSYNTHESIS_H
#define ED_LEAF_PHOTOSYNTHESIS_H

#include <algorithm>                    // for std:sort
#include "../validate_system.h"         // for find_strictly_required_inputs and all_are_in_list
#include "../simultaneous_equations.h"  // for get_unknown_quantities
#include "../modules.h"
#include "../module_library/module_wrapper_factory.h"
#include "../se_solver_library/se_solver_factory.h"
#include "../state_map.h"

namespace ed_leaf_photosynthesis_stuff
{
// Define a list of reference module names
string_vector reference_module_names = {
    "ed_apply_stomatal_water_stress_via_conductance",
    "ed_gas_concentrations",
    "ed_ball_berry",
    "ed_collatz_c4_assimilation",
    "ed_long_wave_energy_loss",
    "water_vapor_properties_from_air_temperature",
    "ed_penman_monteith_leaf_temperature"};

// Define a list of the unknown quantities.
// Although these quantities can be automatically determined
// from the modules using `get_unknown_quantities', here we
// write them explicitly so the order is known.
string_vector unknown_quantities = {
    "assimilation_net",
    "conductance_stomatal_h2o",
    "temperature_leaf"};

// A function that finds all inputs required by the reference modules, excluding any unknowns.
// Here it is important to sort the results so the order is correct when using
// simultaneous_equations::update_known_quantities.
string_vector get_reference_inputs()
{
    string_set all_reference_inputs = find_strictly_required_inputs(std::vector<string_vector>{reference_module_names});
    string_vector unknown_quantities = get_unknown_quantities(std::vector<string_vector>{reference_module_names});
    string_vector result = string_vector_difference(
        string_set_to_string_vector(all_reference_inputs),
        unknown_quantities);
    std::sort(result.begin(), result.end());
    return result;
}

// A function that finds all outputs produced by the reference modules.
// Note that the result is automatically sorted since the results are originally
// a string_set.
string_vector get_reference_outputs()
{
    return string_set_to_string_vector(find_unique_module_outputs(std::vector<string_vector>{reference_module_names}));
}

// Check the the test module to make sure it has the same inputs and outputs as the reference module
void check_module_io_equivalence(std::string reference_module, std::string module_to_test)
{
    auto w_ref = module_wrapper_factory::create(reference_module);
    string_vector ref_inputs = w_ref->get_inputs();
    string_vector ref_outputs = w_ref->get_outputs();

    auto w_test = module_wrapper_factory::create(module_to_test);
    string_vector test_inputs = w_test->get_inputs();
    string_vector test_outputs = w_test->get_outputs();

    if (ref_inputs.size() != test_inputs.size() || !all_are_in_list(ref_inputs, test_inputs)) {
        throw std::logic_error(
            std::string("The '") +
            module_to_test +
            std::string("' module does not have the same inputs as the reference model ('") +
            reference_module +
            std::string("'"));
    }

    if (ref_outputs.size() != test_outputs.size() || !all_are_in_list(ref_outputs, test_outputs)) {
        throw std::logic_error(
            std::string("The '") +
            module_to_test +
            std::string("' module does not have the same outputs as the reference model ('") +
            reference_module +
            std::string("'"));
    }
}

// Make a simultaneous_equations object after checking the modules for consistency with respect to the
// reference set
std::unique_ptr<simultaneous_equations> make_se(
    std::string stomatal_water_stress_application_module,
    std::string gas_concentration_module,
    std::string stomatal_conductance_module,
    std::string assimilation_module,
    std::string long_wave_energy_loss_module,
    std::string water_vapor_properties_module,
    std::string leaf_temperature_module)
{
    string_vector input_module_names = {
        stomatal_water_stress_application_module,
        gas_concentration_module,
        stomatal_conductance_module,
        assimilation_module,
        long_wave_energy_loss_module,
        water_vapor_properties_module,
        leaf_temperature_module};

    // Check to make sure each module has the correct inputs and outputs
    for (size_t i = 0; i < input_module_names.size(); ++i) {
        try {
            check_module_io_equivalence(reference_module_names[i], input_module_names[i]);
        } catch (std::exception const& e) {
            throw std::logic_error(std::string("Thrown by ed_leaf_photosynthesis: ") + e.what());
        }
    }

    // Initialize a state_map for the known quantities
    state_map known_quantities;
    string_vector known_reference_inputs = get_reference_inputs();
    for (std::string const& kq : known_reference_inputs) {
        known_quantities[kq] = 0.0;
    }

    return std::unique_ptr<simultaneous_equations>(new simultaneous_equations(known_quantities,
                                                                              unknown_quantities,
                                                                              input_module_names));
}

}  // namespace ed_leaf_photosynthesis_stuff

/**
 * @class ed_leaf_photosynthesis
 * 
 * @brief A general module representing photosynthesis calculations.
 * Note that this module cannot be created by the module_wrapper_factory
 * since its constructor has a different signature than a typical module.
 * Instead, it must have concrete derived classes. This class has been
 * made abstract to indicate this.
 * 
 * @param[in] stomatal_water_stress_application_module
 *            the name of a module with the same input and output quantities
 *            as `ed_apply_stomatal_water_stress_via_conductance`
 * 
 * @param[in] gas_concentration_module
 *            the name of a module with the same input and output quantities
 *            as `ed_gas_concentrations`
 * 
 * @param[in] stomatal_conductance_module
 *            the name of a module with the same input and output quantities
 *            as `ed_ball_berry`
 * 
 * @param[in] assimilation_module
 *            the name of a module with the same input and output quantities
 *            as `ed_collatz_c4_assimilation`
 * 
 * @param[in] long_wave_energy_loss_module
 *            the name of a module with the same input and output quantities
 *            as `ed_long_wave_energy_loss`
 * 
 * @param[in] water_vapor_properties_module
 *            the name of a module with the same input and output quantities
 *            as `water_vapor_properties_from_air_temperature`
 * 
 * @param[in] leaf_temperature_module
 *            the name of a module with the same input and output quantities
 *            as `ed_penman_monteith_leaf_temperature`
 */
class ed_leaf_photosynthesis : public SteadyModule
{
   public:
    ed_leaf_photosynthesis(
        std::string module_name,
        std::string stomatal_water_stress_application_module,
        std::string gas_concentration_module,
        std::string stomatal_conductance_module,
        std::string assimilation_module,
        std::string long_wave_energy_loss_module,
        std::string water_vapor_properties_module,
        std::string leaf_temperature_module,
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        : SteadyModule(module_name),
          solver(se_solver_factory::create("newton_raphson_boost", 1e-3, 1e-6, 10)),
          se(ed_leaf_photosynthesis_stuff::make_se(stomatal_water_stress_application_module,
                                                   gas_concentration_module,
                                                   stomatal_conductance_module,
                                                   assimilation_module,
                                                   long_wave_energy_loss_module,
                                                   water_vapor_properties_module,
                                                   leaf_temperature_module)),
          // Get pointers to input parameters
          input_ptrs(get_ip(input_parameters, ed_leaf_photosynthesis_stuff::get_reference_inputs())),
          temperature_air_ip(get_ip(input_parameters, "temp")),
          ball_berry_intercept_ip(get_ip(input_parameters, "ball_berry_intercept")),
          // Get pointers to output parameters
          output_ptrs(get_op(output_parameters, ed_leaf_photosynthesis_stuff::get_reference_outputs()))

    {
        // Make sure the initial_guess and best_guess are the correct size
        initial_guess.resize(ed_leaf_photosynthesis_stuff::unknown_quantities.size());
        best_guess = initial_guess;

        // Make sure outputs_from_modules has the correct size
        outputs_from_modules.resize(ed_leaf_photosynthesis_stuff::get_reference_outputs().size());

        // Define the lower bounds
        lower_bounds = {
            -1.0,   // assimilation_net: this limit would correspond to absurdly high respiration
            1e-10,  // conductance_stomatal_h2o: this conductance must be positive
            -110    // temperature_leaf: significantly colder than the coldest temperature ever recorded on the Earth's surface
        };

        // Define the upper bounds
        upper_bounds = {
            1.0,  // assimilation_net: this limit would correspond to absurdly high assimilation
            10,   // conductance_stomatal_h2o: this limit would correspond to absurdly high conductance
            80    // temperature_leaf: significantly higher than the hottest temperature ever recorded on the Earth's surface
        };
    }
    virtual ~ed_leaf_photosynthesis() = 0;

   private:
    // Stuff for solving the simultaneous equations
    std::unique_ptr<se_solver> solver;
    std::unique_ptr<simultaneous_equations> se;
    std::vector<double> mutable initial_guess;
    std::vector<double> mutable best_guess;
    std::vector<double> mutable outputs_from_modules;
    std::vector<double> lower_bounds;
    std::vector<double> upper_bounds;
    // Pointers to input parameters
    std::vector<const double*> input_ptrs;
    const double* temperature_air_ip;
    const double* ball_berry_intercept_ip;
    // Pointers to output parameters
    std::vector<double*> output_ptrs;
    // Main operation
    void do_operation() const override;
};

// A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header
inline ed_leaf_photosynthesis::~ed_leaf_photosynthesis() {}

void ed_leaf_photosynthesis::do_operation() const
{
    // Make the initial guess
    initial_guess[0] = 0.0;                       // assimilation_net
    initial_guess[1] = *ball_berry_intercept_ip;  // conductance_stomatal_h2o
    initial_guess[2] = *temperature_air_ip;       // temperature_leaf

    // Update the known parameters using the input pointers
    se->update_known_quantities(input_ptrs);

    // Solve the equations without using an observer
    bool success = solver->solve(se, initial_guess, lower_bounds, upper_bounds, best_guess);

    if (!success) {
        throw std::runtime_error("Thrown by ed_leaf_photosynthesis::do_operation: the solver was unable to find a solution.");
    }

    // Get the outputs from the modules (evaluated using the best guess)
    se->get_all_outputs(outputs_from_modules);

    // Update the output pointers
    for (size_t i = 0; i < output_ptrs.size(); ++i) {
        update(output_ptrs[i], outputs_from_modules[i]);
    }
}

/**
 * @class ed_c4_leaf_photosynthesis
 * 
 * @brief Determines stomatal conductance, net assimilation, and leaf temperature.
 * Stomatal water stress is included by modifying stomatal conductance,
 * the Ball-Berry model is used to calculate stomatal conductance,
 * the Collatz model for C4 photosynthesis is used to calculate assimilation,
 * and the Penman-Monteith model is used to calculate leaf temperature.
 * The Newton-Raphson method is used to find self-consistent values for the
 * unknown quantities.
 */
class ed_c4_leaf_photosynthesis : public ed_leaf_photosynthesis
{
   public:
    ed_c4_leaf_photosynthesis(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        : ed_leaf_photosynthesis("ed_c4_leaf_photosynthesis",  // the name of this module
                                 "ed_apply_stomatal_water_stress_via_conductance",
                                 "ed_gas_concentrations",
                                 "ed_ball_berry",
                                 "ed_collatz_c4_assimilation",
                                 "ed_long_wave_energy_loss",
                                 "water_vapor_properties_from_air_temperature",
                                 "ed_penman_monteith_leaf_temperature",
                                 input_parameters,
                                 output_parameters)
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();
};

// Get inputs from the typical modules
std::vector<std::string> ed_c4_leaf_photosynthesis::get_inputs()
{
    return ed_leaf_photosynthesis_stuff::get_reference_inputs();
}

// Get outputs from the typical modules
std::vector<std::string> ed_c4_leaf_photosynthesis::get_outputs()
{
    return ed_leaf_photosynthesis_stuff::get_reference_outputs();
}

#endif
