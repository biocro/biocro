#ifndef SE_MODULE_H
#define SE_MODULE_H

#include "../modules.h"
#include "../validate_system.h"         // for string_vector, find_strictly_required_inputs, and all_are_in_list
#include "../simultaneous_equations.h"  // for get_unknown_quantities
#include "../state_map.h"
#include "../se_solver_library/se_solver_factory.h"

namespace se_module
{
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
         const state_map* input_parameters,
         state_map* output_parameters)
        : SteadyModule(module_name),
          se(make_se(sub_module_names)),
          solver(se_solver_factory::create(solver_name, max_iterations)),
          lower_bounds(lower_bounds),
          upper_bounds(upper_bounds),
          absolute_error_tolerances(absolute_error_tolerances),
          relative_error_tolerances(relative_error_tolerances),
          input_ptrs(get_ip(input_parameters, get_se_inputs(sub_module_names))),
          output_ptrs(get_op(output_parameters, get_se_outputs(sub_module_names)))
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
    std::vector<double> mutable initial_guess;
    std::vector<double> mutable best_guess;
    std::vector<double> mutable outputs_from_modules;
    // Main operation
    void do_operation() const override;
    virtual std::vector<double> get_initial_guess() const = 0;

   protected:
    // Pointers to input parameters
    std::vector<const double*> input_ptrs;
    // Pointers to output parameters
    std::vector<double*> output_ptrs;
};

/** A destructor must be defined, and since the default is overwritten when defining it as pure virtual, add an inline one in the header */
inline base::~base() {}

/**
 * @brief Uses the solver to solve the simultaneous equations for the unknown parameters,
 * and then reports the values of all outputs calculated using the best guess for the
 * unknowns. Derived classes need to implement a way to get an initial guess.
 */
void base::do_operation() const
{
    initial_guess = get_initial_guess();  // must be implemented by derived class

    se->update_known_quantities(input_ptrs);

    bool success = solver->solve(se, initial_guess,
                                 lower_bounds, upper_bounds,
                                 absolute_error_tolerances, relative_error_tolerances,
                                 best_guess);  // modifies best_guess

    if (!success) {
        throw std::runtime_error(std::string("Thrown by ") + get_name() + std::string(": the solver was unable to find a solution."));
    }

    se->get_all_outputs(outputs_from_modules, best_guess);  // modifies outputs_from_modules

    for (size_t i = 0; i < output_ptrs.size(); ++i) {
        update(output_ptrs[i], outputs_from_modules[i]);
    }
}
}  // namespace se_module

#endif
