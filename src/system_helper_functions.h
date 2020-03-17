#ifndef SYSTEM_HELPER_FUNCTIONS_H
#define SYSTEM_HELPER_FUNCTIONS_H

#include <vector>
#include <set>
#include <unordered_map>
#include <string>
#include <memory>  // For unique_ptr
#include "validate_system.h"
#include "constants.h"

/**
 * @brief Numerically compute the Jacobian matrix and time derivatives of a vector valued function.
 * 
 * @param[in] system_ptr a pointer to an object with the following method:
 *            operator()(vector_type const& x, vector_type& y, time_type const& t).
 *            This function should calculate y (an out param) based on a vector x
 *            and time t (input params). The vectors x and y are assumed to have the same
 *            length.
 * 
 * @param[in] max_time the largest time value that can be passed to vector_valued_function.
 * 
 * @param[in] x an input vector, to be passed to vector_valued_function.
 * 
 * @param[in] t an input time, to be passed to vector_valued_function.
 * 
 * @param[out] jacobi the calculated Jacobian matrix (containing df_i/dx_j evaluated at x, t)
 * 
 * @param[out] dfdt the calculated explicit time dependence (i.e., df_i/dt evaluated at x, t)
 */
template <typename system_type, typename time_type, typename vector_type, typename matrix_type>
void calculate_jacobian_and_time_derivative(
    system_type* const& system_ptr,
    time_type const& max_time,
    vector_type const& x,
    time_type const& t,
    matrix_type& jacobi,
    vector_type& dfdt)
{
    calculate_jacobian(system_ptr, x, t, jacobi);
    calculate_time_derivative(system_ptr, max_time, x, t, dfdt);
}

/**
 * @brief Numerically compute the Jacobian matrix of a vector valued function.
 * 
 * @param[in] system_ptr a pointer to an object with the following method:
 *            operator()(vector_type const& x, vector_type& y, time_type const& t).
 *            This function should calculate y (an out param) based on a vector x
 *            and time t (input params). The vectors x and y are assumed to have the same
 *            length.
 * 
 * @param[in] x an input vector, to be passed to vector_valued_function.
 * 
 * @param[in] t an input time, to be passed to vector_valued_function.
 * 
 * @param[out] jacobi the calculated Jacobian matrix (containing df_i/dx_j evaluated at x, t)
 * 
 * * Discussion of step size from <a href="http://www.iue.tuwien.ac.at/phd/khalil/node14.html">Nadim Khalil's thesis</a>:
 * <BLOCKQUOTE>
 * It is known that numerical differentiation is an unstable procedure prone to truncation and subtractive cancellation errors.
 * Decreasing the step size will reduce the truncation error.
 * Unfortunately a smaller step has the opposite effect on the cancellation error.
 * Selecting the optimal step size for a certain problem is computationally expensive and the benefits achieved are not justifiable
 *  as the effect of small errors in the values of the elements of the Jacobian matrix is minor.
 * For this reason, the sizing of the finite difference step is not attempted and a constant increment size is used in evaluating the gradient.
 * </BLOCKQUOTE>
 * 
 * In BioCro, we fix a step size and only evaluate the forward perturbation to reduce calculation costs.
 *  In other words:
 *   - (1) We calculate f(x,t) using the input (x,t) (called f_current)
 *   - (2) We make a forward perturbation by adding h to one vector element and calculating the time derivatives (called f_perturbation)
 *   - (3) We calculate the rate of change for each vector element according to (f_perturbed[i] - f_current[i])/h
 *   - (4) We repeat steps (2) and (3) for each state variable
 * 
 *  The alternative method would be:
 *   - (1) We make a backward perturbation by substracting h from one state variable and calculating the time derivatives (called f_backward)
 *   - (2) We make a forward perturbation by adding h to the same state variable and calculating the time derivatives (called f_forward)
 *   - (3) We calculate the rate of change for each state variable according to (f_forward[i] - f_backward[i])/(2*h)
 *   - (4) We repeat steps (1) through (3) for each state variable
 * 
 *  In the simpler scheme, we make N + 1 derivative evaluations, where N is the number of state variables.
 *  In the other scheme, we make 2N derivative evaluations.
 *  The improvement in accuracy does not seem to outweigh the cost of additional calculations, since BioCro derivatives are expensive.
 *  Likewise, higher-order numerical derivative calculations are also not worthwhile.
 */
template <typename system_type, typename time_type, typename vector_type, typename matrix_type>
void calculate_jacobian(
    system_type* const& system_ptr,
    vector_type const& x,
    time_type const& t,
    matrix_type& jacobi)
{
    size_t n = x.size();

    // Make vectors to store the current and perturbed f(x,t)
    vector_type f_current(n);
    vector_type f_perturbed(n);

    // Get the current f(x,t)
    system_ptr->operator()(x, f_current, t);

    // Perturb each element x_i of the input vector to find df(x,t)/dx_i
    double h;
    vector_type x_perturbed = x;
    for (size_t i = 0; i < n; i++) {
        // Ensure that the step size h is close to eps_deriv but is exactly representable
        //  (see Numerical Recipes in C, 2nd ed., Section 5.7)
        h = calculation_constants::eps_deriv;
        double temp = x[i] + h;
        h = temp - x[i];

        // Calculate the new function value
        x_perturbed[i] = x[i] + h;                            // Add h to the ith state variable
        system_ptr->operator()(x_perturbed, f_perturbed, t);  // Calculate f_perturbed

        // Store the results in the Jacobian matrix
        for (size_t j = 0; j < n; j++) {
            jacobi(j, i) = (f_perturbed[j] - f_current[j]) / h;
        }

        // Reset the ith state variable
        x_perturbed[i] = x[i];
    }
}

/**
 * @brief Numerically compute the explicit time derivatives of a vector valued function.
 * 
 * @param[in] system_ptr a pointer to an object with the following method:
 *            operator()(vector_type const& x, vector_type& y, time_type const& t).
 *            This function should calculate y (an out param) based on a vector x
 *            and time t (input params). The vectors x and y are assumed to have the same
 *            length.
 * 
 * @param[in] max_time the largest time value that can be passed to vector_valued_function.
 * 
 * @param[in] x an input vector, to be passed to vector_valued_function.
 * 
 * @param[in] t an input time, to be passed to vector_valued_function.
 * 
 * @param[out] dfdt the calculated explicit time dependence (i.e., df_i/dt evaluated at x, t)
 */
template <typename system_type, typename time_type, typename vector_type>
void calculate_time_derivative(
    system_type* const& system_ptr,
    time_type const& max_time,
    vector_type const& x,
    time_type const& t,
    vector_type& dfdt)
{
    size_t n = x.size();

    // Make vectors to store the current and perturbed f(x,t)
    vector_type f_current(n);
    vector_type f_perturbed(n);

    // Get the current f(x,t)
    system_ptr->operator()(x, f_current, t);

    // Perturb the time to find df(x,t)/dt
    // Use a forward step whenever possible
    // Ensure that the step size h is close to eps_deriv but is exactly representable
    //  (see Numerical Recipes in C, 2nd ed., Section 5.7)
    double h = calculation_constants::eps_deriv;
    if (t + h <= max_time) {
        double temp = t + h;
        h = temp - t;
        system_ptr->operator()(x, f_perturbed, t + h);
        for (size_t j = 0; j < n; j++) {
            dfdt[j] = (f_perturbed[j] - f_current[j]) / h;
        }
    } else {
        double temp = t - h;
        h = temp + t;
        system_ptr->operator()(x, f_perturbed, t - h);
        for (size_t j = 0; j < n; j++) {
            dfdt[j] = (f_current[j] - f_perturbed[j]) / h;
        }
    }
}

/**
 * Returns pairs of pointers that each point to the same named quantity in each of the two input maps.
 * Here it is essential that map_1 and map_2 are passed by reference; otherwise the pointers will be
 * useless.
 */
template <typename name_list_type, typename value_type_1, typename value_type_2>
std::vector<std::pair<value_type_1*, const value_type_2*>> get_pointer_pairs(
    name_list_type quantity_names,
    std::unordered_map<std::string, value_type_1>& map_1,
    std::unordered_map<std::string, value_type_2> const& map_2)
{
    std::vector<std::pair<value_type_1*, const value_type_2*>> pointer_pairs;

    for (std::string const& name : quantity_names) {
        std::pair<value_type_1*, const value_type_2*> temporary(&map_1.at(name), &map_2.at(name));
        pointer_pairs.push_back(temporary);
    }

    return pointer_pairs;
}

/**
 * @brief Returns pairs of pointers that each point to the same named quantity in different places.
 * 
 * The resulting pairs are intended to be used for updating a central map of quantities (the `target_map`,
 * which is probably the `quantities` member of a Standalone_SS object) using values from another source
 * (the `source_map`, which probably points to variables outside the Standalone_SS object).
 * 
 * One pair will be created for each named quantity in the `source_map` input.
 * 
 * Here it is essential that the `target_map` is passed by reference; otherwise the pointer pairs will be useless.
 * 
 * @param[in] target_map An unordered map with string keys and elements that are variables of type `target_value_type` (often double).
 * 
 * @param[in] source_map An unordered map with string keys and elements that point to variables of type `source_value_type' (often double).
 * 
 * @returns A vector of pairs of pointers. The first pointer in each pair points to a "target" variable in the
 *          `target_map` input, while the second pointer points to the same named quantity in another location,
 *          as specified by the `source_map` input.
 */
template <typename target_value_type, typename source_value_type>
std::vector<std::pair<target_value_type*, const source_value_type*>> get_input_pointer_pairs(
    std::unordered_map<std::string, target_value_type>& target_map,
    std::unordered_map<std::string, const source_value_type*> const& source_map)
{
    std::vector<std::pair<target_value_type*, const source_value_type*>> pointer_pairs;

    for (auto const& it : source_map) {
        std::pair<target_value_type*, const source_value_type*> temporary(&target_map.at(it.first), it.second);
        pointer_pairs.push_back(temporary);
    }

    return pointer_pairs;
}

/**
 * @brief Returns pairs of pointers that each point to the same named quantity in different places.
 * 
 * The resulting pairs are intended to be used for updating quantities (specified by the keys and pointers
 * in the `target_map`) using values from the `source_map` (which is usually the `quantities` member of a
 * Standalone_SS object).
 * 
 * One pair will be created for each named quantity in the `target_map` input.
 * 
 * Here it is essential that the `source_map` is passed by reference; otherwise the pointer pairs will be useless.
 * 
 * @param[in] source_map An unordered map with string keys and elements that are variables of type `source_value_type' (often double).
 * 
 * @param[in] target_map An unordered map with string keys and elements that point to variables of type `target_value_type` (often double).
 * 
 * @returns A vector of pairs of pointers. The first pointer in each pair points to a "source" variable in the
 *          `source_map` input, while the second pointer points to the same named quantity in another location,
 *          as specified by the `target_map` input.
 */
template <typename source_value_type, typename target_value_type>
std::vector<std::pair<const source_value_type*, target_value_type*>> get_output_pointer_pairs(
    std::unordered_map<std::string, source_value_type> const& source_map,
    std::unordered_map<std::string, target_value_type*> const& target_map)
{
    std::vector<std::pair<const source_value_type*, target_value_type*>> pointer_pairs;

    for (auto const& it : target_map) {
        std::pair<const source_value_type*, target_value_type*> temporary(&source_map.at(it.first), it.second);
        pointer_pairs.push_back(temporary);
    }

    return pointer_pairs;
}

/**
 * Returns a vector of pointers that point to elements of map whose targets are specified by quantity_names.
 * Here it is essential that the map is passed by reference; otherwise the pointers will be useless
 */
template <typename name_list_type, typename value_type>
std::vector<value_type*> get_pointers(
    name_list_type quantity_names,
    std::unordered_map<std::string, value_type>& map)
{
    std::vector<value_type*> pointers;

    for (std::string const& name : quantity_names) {
        pointers.push_back(&map.at(name));
    }

    return pointers;
}

bool check_adaptive_compatible(const module_vector* ptr_to_module_vector);

#endif
