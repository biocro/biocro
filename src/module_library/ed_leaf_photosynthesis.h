#ifndef ED_LEAF_PHOTOSYNTHESIS_H
#define ED_LEAF_PHOTOSYNTHESIS_H

#include "../validate_system.h"         // for find_strictly_required_inputs and all_are_in_list
#include "../simultaneous_equations.h"  // for get_unknown_quantities
#include "../state_map.h"               // for keys
#include "../modules.h"
#include "../module_library/module_wrapper_factory.h"
#include "../se_solver_library/se_solver_factory.h"

namespace ed_leaf_photosynthesis
{
/** Define a list of reference module names */
constexpr string_vector reference_module_names = {
    "ed_apply_stomatal_water_stress_via_conductance",
    "ed_gas_concentrations",
    "ed_ball_berry",
    "ed_collatz_c4_assimilation",
    "ed_long_wave_energy_loss",
    "water_vapor_properties_from_air_temperature",
    "ed_penman_monteith_leaf_temperature"};

/**
 * Define a list of the unknown quantities.
 * Although these quantities can be automatically determined
 * from the modules using 'get_unknown_quantities', here we
 * write them explicitly for clarity and so the order is known.
 */
constexpr string_vector unknown_quantities = {
    "assimilation_net",
    "conductance_stomatal_h2o",
    "temperature_leaf"};

/** Set the lower bounds for solving the equations */
constexpr std::vector<double> lower_bounds = {
    -1.0,   // assimilation_net: this limit would correspond to absurdly high respiration
    1e-10,  // conductance_stomatal_h2o: conductance must be positive
    -110    // temperature_leaf: significantly colder than the coldest air temperature ever recorded on the Earth's surface
};

/** Set the upper bounds for solving the equations */
constexpr std::vector<double> upper_bounds = {
    1.0,  // assimilation_net: this limit would correspond to absurdly high assimilation
    10,   // conductance_stomatal_h2o: this limit would correspond to absurdly high conductance
    80    // temperature_leaf: significantly higher than the hottest air temperature ever recorded on the Earth's surface
};

/** Set some basic se_solver properties */
constexpr std::string se_solver_name = std::string("newton_raphson_boost");
constexpr double rel_error_tol = 1e-3;
constexpr double abs_error_tol = 1e-6;
constexpr int max_iterations = 10;

/**
 * @brief A function that finds all inputs required by the reference modules, excluding any unknowns.
 * Here it is important to sort the results so the order is correct when using
 * simultaneous_equations::update_known_quantities.
 */
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

/**
 * @brief A function that finds all outputs produced by the reference modules.
 * Here it is important to sort the results so the order is correct when using
 * simultaneous_equations::update_known_quantities. This is accomplished automatically
 * because the `find_unique_module_outputs` returns a string_set (which is sorted).
 */
string_vector get_reference_outputs()
{
    return string_set_to_string_vector(find_unique_module_outputs(std::vector<string_vector>{reference_module_names}));
}

/**
 * @brief Check the the test module to make sure it has the same inputs and outputs as the reference module.
 * This function throws an error if there is a problem.
 */
void check_module_io_equivalence(std::string reference_module, std::string module_to_test)
{
    // Make a lambda to simplify the checks. check_type should be "inputs" or "outputs".
    auto check_equiv =
        [&reference_module, &module_to_test](string_vector const& ref_sv, string_vector const& test_sv, std::string check_type) -> void {
        if (ref_sv.size() != test_sv.size() || !all_are_in_list(ref_sv, test_sv)) {
            throw std::logic_error(
                std::string("The '") +
                module_to_test +
                std::string("' module does not have the same ") +
                check_type +
                std::string(" as the reference model ('") +
                reference_module +
                std::string("'"));
        }
    };

    // Get the module wrappers
    auto w_ref = module_wrapper_factory::create(reference_module);
    auto w_test = module_wrapper_factory::create(module_to_test);

    // Perform the tests
    check_equiv(w_ref->get_inputs(), w_test->get_inputs(), "inputs");
    check_equiv(w_ref->get_outputs(), w_test->get_outputs(), "outputs");
}

/**
 * @brief Checks to make sure the input modules are valid.
 * The modules are valid if and only if each module has the same inputs and outputs as the
 * corresponding reference module. This function throws an error if there is a problem.
 */
void validate_modules(string_vector input_module_names)
{
    for (size_t i = 0; i < input_module_names.size(); ++i) {
        try {
            check_module_io_equivalence(reference_module_names[i], input_module_names[i]);
        } catch (std::exception const& e) {
            throw std::logic_error(std::string("Thrown by ed_leaf_photosynthesis: ") + e.what());
        }
    }
}

/**
 * @brief Make a simultaneous_equations object if the modules are valid
 */
std::unique_ptr<simultaneous_equations> make_se(string_vector input_module_names)
{
    validate_modules(input_module_names);

    state_map known_quantities;

    string_vector known_reference_inputs = get_reference_inputs();

    for (std::string const& kq : known_reference_inputs) {
        known_quantities[kq] = 0.0;  // Initialize a state_map for the known quantities with all values set to zero
    }

    return std::unique_ptr<simultaneous_equations>(new simultaneous_equations(known_quantities,
                                                                              unknown_quantities,
                                                                              input_module_names));
}

/**
 * @class module_base
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
class module_base : public SteadyModule
{
   public:
    module_base(
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
          solver(se_solver_factory::create(se_solver_name, rel_error_tol, abs_error_tol, max_iterations)),
          se(make_se(string_vector{stomatal_water_stress_application_module,
                                   gas_concentration_module,
                                   stomatal_conductance_module,
                                   assimilation_module,
                                   long_wave_energy_loss_module,
                                   water_vapor_properties_module,
                                   leaf_temperature_module})),
          // Get pointers to input parameters
          input_ptrs(get_ip(input_parameters, get_reference_inputs())),
          temperature_air_ip(get_ip(input_parameters, "temp")),
          ball_berry_intercept_ip(get_ip(input_parameters, "ball_berry_intercept")),
          // Get pointers to output parameters
          output_ptrs(get_op(output_parameters, get_reference_outputs()))

    {
        // Initialize vectors to the correct size
        initial_guess.resize(unknown_quantities.size());
        best_guess.resize(unknown_quantities.size());
        outputs_from_modules.resize(get_reference_outputs().size());
    }
    virtual ~module_base() = 0;

   private:
    // Stuff for solving the simultaneous equations
    std::unique_ptr<se_solver> solver;
    std::unique_ptr<simultaneous_equations> se;
    std::vector<double> mutable initial_guess;
    std::vector<double> mutable best_guess;
    std::vector<double> mutable outputs_from_modules;
    // Pointers to input parameters
    std::vector<const double*> input_ptrs;
    const double* temperature_air_ip;
    const double* ball_berry_intercept_ip;
    // Pointers to output parameters
    std::vector<double*> output_ptrs;
    // Main operation
    void do_operation() const override;
};

/** A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header */
inline module_base::~module_base() {}

/**
 * @brief Uses an se_solver to solve the simultaneous equations for the unknown parameters,
 * and then reports the values of all outputs calculated using the best guess for the
 * unknowns. For the initial guess, we assume zero net assimilation (which corresponds to
 * stomatal conductance = ball_berry_intercept in the Ball-Berry model) and a leaf at
 * thermal equilibrium with the air.
 */
void module_base::do_operation() const
{
    initial_guess[0] = 0.0;                       // assimilation_net
    initial_guess[1] = *ball_berry_intercept_ip;  // conductance_stomatal_h2o
    initial_guess[2] = *temperature_air_ip;       // temperature_leaf

    se->update_known_quantities(input_ptrs);

    bool success = solver->solve(se, initial_guess, lower_bounds, upper_bounds, best_guess);

    if (!success) {
        throw std::runtime_error("Thrown by ed_leaf_photosynthesis::module_base::do_operation: the solver was unable to find a solution.");
    }

    se->get_all_outputs(outputs_from_modules);

    for (size_t i = 0; i < output_ptrs.size(); ++i) {
        update(output_ptrs[i], outputs_from_modules[i]);
    }
}

}  // namespace ed_leaf_photosynthesis

#endif
