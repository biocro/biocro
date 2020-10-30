#ifndef SE_MODULE_H
#define SE_MODULE_H

#include "../modules.h"                              // for SteadyModule, update
#include "../module_helper_functions.h"              // for get_ip, get_op
#include "../validate_system.h"                      // for string_vector, find_strictly_required_inputs, all_are_in_list
#include "../simultaneous_equations.h"               // for get_unknown_quantities
#include "../state_map.h"                            // for state_map
#include "../se_solver_helper_functions.h"           // for f_scalar_from_f_vec
#include "../se_solver_library/se_solver_factory.h"  // for se_solver_factory::create
#include <map>                                       // for map
#include <Rinternals.h>                              // for debugging
const bool se_module_print = false;                  // for debugging

namespace se_module
{
/**
 * @brief Returns the name of an output quantity that describes the number of times
 * the simultaneous equations were evaluated while solving for the unknowns.
 */
std::string get_ncalls_output_name(std::string const& module_name)
{
    return module_name + std::string("_ncalls");
}

/**
 * @brief Returns the name of an output quantity that describes the number of steps
 * the solver took.
 */
std::string get_nsteps_output_name(std::string const& module_name)
{
    return module_name + std::string("_nsteps");
}

/**
 * @brief Returns the name of an output quantity that indicates whether the solver
 * was successful in finding a solution.
 */
std::string get_success_output_name(std::string const& module_name)
{
    return module_name + std::string("_success");
}

/**
 * @brief Returns a sorted list of inputs required by the modules, excluding any unknown quantities
 */
string_vector get_se_inputs(string_vector module_names)
{
    string_set all_inputs = find_strictly_required_inputs(std::vector<string_vector>{module_names});

    string_vector unknown_quantities = get_unknown_quantities(std::vector<string_vector>{module_names});

    string_vector result = string_vector_difference(
        string_set_to_string_vector(all_inputs),
        unknown_quantities);

    std::sort(result.begin(), result.end());

    return result;
}

/**
 * @brief Returns a sorted list of outputs produced by the modules
 */
string_vector get_se_outputs(string_vector module_names)
{
    return string_set_to_string_vector(find_unique_module_outputs(std::vector<string_vector>{module_names}));
}

/**
 * @brief Make a simultaneous_equations object from the modules
 */
std::unique_ptr<simultaneous_equations> make_se(string_vector module_names)
{
    string_vector unknown_quantities = get_unknown_quantities(std::vector<string_vector>{module_names});  // returns a sorted vector

    string_vector known_inputs = get_se_inputs(module_names);

    state_map known_quantities;
    for (std::string const& kq : known_inputs) {
        known_quantities[kq] = 0.0;  // Initialize a state_map for the known quantities with all values set to zero
    }

    return std::unique_ptr<simultaneous_equations>(new simultaneous_equations(known_quantities,
                                                                              unknown_quantities,
                                                                              module_names));
}

/**
 * @brief Reorders the initial guesses according to their corresponding
 * f_scalar_norm values.
 */
std::vector<std::vector<double>> reorder_initial_guesses(
    std::vector<std::vector<double>> initial_guesses,
    std::unique_ptr<simultaneous_equations> const& se)
{
    // Make a map that will automatically order the guesses according to their f_scalar_norm values
    std::map<double, std::vector<double>> guess_map;

    // Calculate f_scalar_norm for each guess and store the result in the map
    std::vector<double> F_vec(initial_guesses[0].size());
    double f_scalar_norm;
    for (std::vector<double> const& guess : initial_guesses) {
        (*se)(guess, F_vec);  // evaluates F_vec
        f_scalar_norm = f_scalar_norm_from_f_vec(F_vec, guess);
        guess_map[f_scalar_norm] = guess;
    }

    if (se_module_print) {
        std::string message = "Reordering the set of possible initial guesses:\n";
        char buff[1024];
        for (auto const& x : guess_map) {
            sprintf(buff, " %e, %e, %e, %e, %e\n",
                    x.second[0],
                    x.second[1],
                    x.second[2],
                    x.second[3],
                    x.first);
            message += std::string(buff);
        }
        message += "\n";
        Rprintf(message.c_str());
    }

    // Return the result
    std::vector<std::vector<double>> reordered_guesses;
    for (auto const& x : guess_map) {
        reordered_guesses.push_back(x.second);
    }
    return reordered_guesses;
}

/**
 * @class base
 * 
 * @brief A general module that solves a set of simultaneous equations.
 * Note that this module cannot be created by the module_wrapper_factory
 * since its constructor has a different signature than a typical module.
 * Instead, it must have concrete derived classes. This class has been
 * made abstract to indicate this.
 */
class base : public SteadyModule
{
   public:
    base(std::string module_name,
         string_vector sub_module_names,
         std::string solver_name,
         int max_iterations,
         std::vector<double> lower_bounds,
         std::vector<double> upper_bounds,
         std::vector<double> absolute_error_tolerances,
         std::vector<double> relative_error_tolerances,
         bool should_reorder_guesses,
         bool return_default_on_failure,
         const state_map* input_parameters,
         state_map* output_parameters)
        : SteadyModule(module_name),
          se(make_se(sub_module_names)),
          solver(se_solver_factory::create(solver_name, max_iterations)),
          lower_bounds(lower_bounds),
          upper_bounds(upper_bounds),
          absolute_error_tolerances(absolute_error_tolerances),
          relative_error_tolerances(relative_error_tolerances),
          should_reorder_guesses(should_reorder_guesses),
          return_default_on_failure(return_default_on_failure),
          input_ptrs(get_ip(input_parameters, get_se_inputs(sub_module_names))),
          output_ptrs(get_op(output_parameters, get_se_outputs(sub_module_names))),
          ncalls_op(get_op(output_parameters, get_ncalls_output_name(module_name))),
          nsteps_op(get_op(output_parameters, get_nsteps_output_name(module_name))),
          success_op(get_op(output_parameters, get_success_output_name(module_name)))
    {
        // Initialize vectors to the correct size
        outputs_from_modules.resize(output_ptrs.size());
    }
    virtual ~base() = 0;

   private:
    // Stuff for solving the simultaneous equations
    std::unique_ptr<simultaneous_equations> se;
    std::unique_ptr<se_solver> solver;
    std::vector<double> const lower_bounds;
    std::vector<double> const upper_bounds;
    std::vector<double> const absolute_error_tolerances;
    std::vector<double> const relative_error_tolerances;
    bool const should_reorder_guesses;
    std::vector<double> mutable best_guess;
    std::vector<double> mutable outputs_from_modules;
    bool const return_default_on_failure;
    virtual void get_default(std::vector<double>& /*guess_vec*/) const {}
    // Pointers to input parameters
    std::vector<const double*> input_ptrs;
    // Pointers to output parameters
    std::vector<double*> output_ptrs;
    double* ncalls_op;
    double* nsteps_op;
    double* success_op;
    // Main operation
    void do_operation() const override;
    virtual std::vector<std::vector<double>> get_initial_guesses() const = 0;
};

/** A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header */
inline base::~base() {}

/**
 * @brief Uses the solver to solve the simultaneous equations for the unknown parameters,
 * and then reports the values of all outputs calculated using the best guess for the
 * unknowns. Derived classes need to implement a way to provide a set of possible initial guesses.
 */
void base::do_operation() const
{
    se->update_known_quantities(input_ptrs);

    se->reset_ncalls();

    std::vector<std::vector<double>> initial_guesses = get_initial_guesses();  // must be implemented by derived class

    if (should_reorder_guesses) {
        initial_guesses = reorder_initial_guesses(initial_guesses, se);
    }

    bool success = false;
    int nsteps = 0;

    for (std::vector<double> const& initial_guess : initial_guesses) {
        success = solver->solve(
            se, initial_guess,
            lower_bounds, upper_bounds,
            absolute_error_tolerances, relative_error_tolerances,
            best_guess);  // modifies best_guess

        nsteps += solver->get_nsteps();

        if (success) {
            break;
        }
    }

    if (!success && !return_default_on_failure) {
        if (se_module_print) {
            Rprintf("The solver was unable to find a solution.\n");
        }
        throw std::runtime_error(std::string("Thrown by ") + get_name() + std::string(": the solver was unable to find a solution."));
    } else if (!success) {
        if (se_module_print) {
            Rprintf("The solver was unable to find a solution. Returning the default guess.\n");
        }
        get_default(best_guess);  // modifies best_guess
    }

    se->get_all_outputs(outputs_from_modules, best_guess);  // modifies outputs_from_modules

    for (size_t i = 0; i < output_ptrs.size(); ++i) {
        update(output_ptrs[i], outputs_from_modules[i]);
    }

    update(ncalls_op, se->get_ncalls());
    update(nsteps_op, nsteps);
    update(success_op, success);
}
}  // namespace se_module

#endif
