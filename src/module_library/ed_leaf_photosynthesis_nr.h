#ifndef ED_LEAF_PHOTOSYNTHESIS_NR_H
#define ED_LEAF_PHOTOSYNTHESIS_NR_H

#include "ed_leaf_photosynthesis.h"

namespace ed_leaf_photosynthesis_nr
{
/**
 * Define a list of the unknown quantities.
 * Although these quantities can be automatically determined
 * from the modules using 'get_unknown_quantities', here we
 * write them explicitly for clarity and so the order is known.
 */
const string_vector unknown_quantities = {
    "assimilation_net",
    "conductance_boundary_h2o_free",
    "conductance_stomatal_h2o",
    "temperature_leaf"};

/** Set the lower bounds for solving the equations */
const std::vector<double> lower_bounds = {
    -1.0,   // assimilation_net: this limit would correspond to absurdly high respiration
    1e-10,  // conductance_boundary_h2o_free: chosen to be the same as conductance_stomatal_h2o limits
    1e-10,  // conductance_stomatal_h2o: conductance must be positive
    -110    // temperature_leaf: significantly colder than the coldest air temperature ever recorded on the Earth's surface
};

/** Set the upper bounds for solving the equations */
const std::vector<double> upper_bounds = {
    1.0,  // assimilation_net: this limit would correspond to absurdly high assimilation
    10,   // conductance_boundary_h2o_free: chosen to be the same as conductance_stomatal_h2o limits
    10,   // conductance_stomatal_h2o: this limit would correspond to absurdly high conductance
    80    // temperature_leaf: significantly higher than the hottest air temperature ever recorded on the Earth's surface
};

/** Set some basic properties for the Newton-Raphson solver */
constexpr double rel_error_tol = 1e-3;
constexpr double abs_error_tol = 10;
constexpr int max_iterations = 50;

/**
 * @brief Make a simultaneous_equations object from the modules
 */
std::unique_ptr<simultaneous_equations> make_se(string_vector input_module_names)
{
    state_map known_quantities;

    string_vector known_reference_inputs = ed_leaf_photosynthesis::get_reference_inputs();

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
 * @brief A general module representing photosynthesis calculations where
 * the simultaneous equations are solved using the Newton-Raphson method.
 * Note that this module cannot be created by the module_wrapper_factory
 * since its constructor has a different signature than a typical module.
 * Instead, it must have concrete derived classes. This class has been
 * made abstract to indicate this. See the base class for more information.
 * Note also that this class does have a do_operation method defined, so
 * base classes (mostly) just need to specify which modules to use.
 */
class module_base : public ed_leaf_photosynthesis::module_base
{
   public:
    module_base(std::string module_name,
                string_vector input_module_names,
                const state_map* input_parameters,
                state_map* output_parameters)
        : ed_leaf_photosynthesis::module_base(module_name, input_module_names, input_parameters, output_parameters),
          se(make_se(input_module_names)),
          solver(se_solver_factory::create("newton_raphson_boost", rel_error_tol, abs_error_tol, max_iterations)),
          temperature_air_ip(get_ip(input_parameters, "temp")),
          ball_berry_intercept_ip(get_ip(input_parameters, "ball_berry_intercept"))
    {
        // Initialize vectors to the correct size
        initial_guess.resize(unknown_quantities.size());
        best_guess.resize(unknown_quantities.size());
        outputs_from_modules.resize(ed_leaf_photosynthesis::get_reference_outputs().size());
    }
    virtual ~module_base() = 0;

   private:
    // Stuff for solving the simultaneous equations
    std::unique_ptr<simultaneous_equations> se;
    std::unique_ptr<se_solver> solver;
    std::vector<double> mutable initial_guess;
    std::vector<double> mutable best_guess;
    std::vector<double> mutable outputs_from_modules;
    // Pointers to specific input parameters
    const double* temperature_air_ip;
    const double* ball_berry_intercept_ip;
    // Main operation
    void do_operation() const override;
};

/** A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header */
inline module_base::~module_base() {}

/**
 * @brief Uses Newton-Raphson solver to solve the simultaneous equations for the unknown parameters,
 * and then reports the values of all outputs calculated using the best guess for the
 * unknowns. For the initial guess, we assume zero net assimilation (which corresponds to
 * stomatal conductance = ball_berry_intercept in the Ball-Berry model) and a leaf in
 * thermal equilibrium with the air.
 */
void module_base::do_operation() const
{
    initial_guess[0] = 0.0;                       // assimilation_net
    initial_guess[1] = 0.0;                       // conductance_boundary_h20_free
    initial_guess[2] = *ball_berry_intercept_ip;  // conductance_stomatal_h2o
    initial_guess[3] = *temperature_air_ip;       // temperature_leaf

    se->update_known_quantities(input_ptrs);

    bool success = solver->solve(se, initial_guess, lower_bounds, upper_bounds, best_guess);

    if (!success) {
        throw std::runtime_error("Thrown by ed_leaf_photosynthesis::module_base::do_operation: the solver was unable to find a solution.");
    }

    se->get_all_outputs(outputs_from_modules, best_guess);
    
    for (size_t i = 0; i < output_ptrs.size(); ++i) {
        update(output_ptrs[i], outputs_from_modules[i]);
    }
}

}  // namespace ed_leaf_photosynthesis_nr

#endif
