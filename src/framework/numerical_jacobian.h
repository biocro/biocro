#ifndef NUMERICAL_JACOBIAN_H
#define NUMERICAL_JACOBIAN_H

#include <memory>       // for std::shared_ptr
#include "constants.h"  // for calculation_constants::eps_deriv

/**
 * @brief Numerically compute the Jacobian matrix of a vector valued function.
 *
 * @param[in] equation_ptr a pointer to an object that can be used to represent a vector valued function f.
 *                         There must be an associated `evaluate_equations(*equation_ptr_type, x, t, y)` function
 *                         defined for equation_ptr_type. It is assumed that the output vector produced by the
 *                         equations has the same length as the input vector.
 *
 * @param[in] x an input vector to be passed to the vector valued function.
 *
 * @param[in] t an input time to be passed to the vector valued function.
 *
 * @param[in] f_current the output of the vector valued function evaluated at x, t
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
 *   - (4) We repeat steps (2) and (3) for each differential quantity
 *
 *  The alternative method would be:
 *   - (1) We make a backward perturbation by substracting h from one differential quantity and calculating the time derivatives (called f_backward)
 *   - (2) We make a forward perturbation by adding h to the same differential quantity and calculating the time derivatives (called f_forward)
 *   - (3) We calculate the rate of change for each differential quantity according to (f_forward[i] - f_backward[i])/(2*h)
 *   - (4) We repeat steps (1) through (3) for each differential quantity
 *
 *  In the simpler scheme, we make N + 1 derivative evaluations, where N is the number of differential quantities.
 *  In the other scheme, we make 2N derivative evaluations.
 *  The improvement in accuracy does not seem to outweigh the cost of additional calculations, since BioCro evaluations are expensive.
 *  Likewise, higher-order numerical derivative calculations are also not worthwhile.
 */
template <typename equation_ptr_type, typename x_vector_type, typename time_type, typename f_vector_type, typename matrix_type>
void calculate_jacobian(
    equation_ptr_type const& equation_ptr,
    x_vector_type const& x,
    time_type t,
    f_vector_type const& f_current,
    matrix_type& jacobi)
{
    size_t n = x.size();

    // Make a vector to store the perturbed f(x,t)
    f_vector_type f_perturbed(n);

    // Perturb each element x_i of the input vector to find df_j(x,t)/dx_i,
    // which is stored at jacobi(j,i)
    double h;
    x_vector_type x_perturbed = x;

    for (size_t i = 0; i < n; i++) {
        // Detemine the step size h by taking a fraction of x[i]: h = x[i] * eps_deriv.
        // Ensure that the step size h is close to this value but is exactly representable
        //  (see Numerical Recipes in C, 2nd ed., Section 5.7)
        h = x[i] * calculation_constants::eps_deriv;
        if (h == 0.0) {
            h = calculation_constants::eps_deriv * calculation_constants::eps_deriv;
        }
        double temp = x[i] + h;
        h = temp - x[i];

        // Calculate the new function value
        x_perturbed[i] = x[i] + h;                                      // Add h to the ith differential quantity
        evaluate_equations(equation_ptr, x_perturbed, t, f_perturbed);  // Calculate f_perturbed

        // Store the results in the Jacobian matrix
        for (size_t j = 0; j < n; j++) {
            jacobi(j, i) = (f_perturbed[j] - f_current[j]) / h;
        }

        // Reset the ith differential quantity
        x_perturbed[i] = x[i];
    }
}

/**
 * @brief A wrapper for calculate_jacobian(equation_ptr, x, t, f_current, jacobi) that
 * automatically evaluates f_current.
 */
template <typename equation_ptr_type, typename vector_type, typename time_type, typename matrix_type>
void calculate_jacobian(
    equation_ptr_type const& equation_ptr,
    vector_type const& x,
    time_type t,
    matrix_type& jacobi)
{
    vector_type f_current(x.size());
    evaluate_equations(equation_ptr, x, t, f_current);
    calculate_jacobian(equation_ptr, x, t, f_current, jacobi);
}

/**
 * @brief A wrapper for calculate_jacobian(equation_ptr, x, t, f_current, jacobi) for equations that
 * have no time dependence.
 */
template <typename equation_ptr_type, typename x_vector_type, typename f_vector_type, typename matrix_type>
void calculate_jacobian_nt(
    equation_ptr_type const& equation_ptr,
    x_vector_type const& x,
    f_vector_type const& f_current,
    matrix_type& jacobi)
{
    calculate_jacobian(equation_ptr, x, 0, f_current, jacobi);
}

/**
 * @brief A wrapper for calculate_jacobian(equation_ptr, x, t, f_current, jacobi) that
 * automatically evaluates f_current for equations that have no time dependence.
 */
template <typename equation_ptr_type, typename vector_type, typename time_type, typename matrix_type>
void calculate_jacobian(
    equation_ptr_type const& equation_ptr,
    vector_type const& x,
    matrix_type& jacobi)
{
    calculate_jacobian(equation_ptr, x, 0, jacobi);
}

/**
 * @brief Numerically compute the explicit time derivatives of a vector valued function.
 *
 * @param[in] equation_ptr a pointer to an object that can be used to represent a vector valued function f.
 *                         There must be an associated `evaluate_equations(*equation_ptr_type, x, t, y)` function
 *                         defined for equation_ptr_type. It is assumed that the output vector produced by the
 *                         equations has the same length as the input vector.
 *
 * @param[in] max_time the largest time value that can be passed to the vector valued function.
 *
 * @param[in] x an input vector to be passed to the vector valued function.
 *
 * @param[in] t an input time to be passed to the vector valued function.
 *
 * @param[in] f_current the output of the vector valued function evaluated at x, t
 *
 * @param[out] dfdt the calculated time derivative, i.e., df_i/dt evaluated at (x, t)
 *
 * For discussion of numerical derivative calculation, see description of
 * jacobian::calculate_jacobian(equation_ptr, x, t, jacobi).
 */
template <typename equation_ptr_type, typename time_type, typename x_vector_type, typename f_vector_type>
void calculate_time_derivative(
    equation_ptr_type const& equation_ptr,
    time_type max_time,
    x_vector_type const& x,
    time_type t,
    f_vector_type const& f_current,
    f_vector_type& dfdt)
{
    size_t n = x.size();

    // Make a vector to store the perturbed f(x,t)
    f_vector_type f_perturbed(n);

    // Perturb the time to find df(x,t)/dt
    // Use a forward step whenever possible
    // Detemine the step size h by taking a fraction of t: h = t * eps_deriv.
    // Ensure that the step size h is close to this value but is exactly representable
    //  (see Numerical Recipes in C, 2nd ed., Section 5.7)
    double h = t * calculation_constants::eps_deriv;
    if (h == 0.0) {
        h = calculation_constants::eps_deriv * calculation_constants::eps_deriv;
    }
    if (t + h <= max_time) {
        double temp = t + h;
        h = temp - t;
        evaluate_equations(equation_ptr, x, t + h, f_perturbed);
        for (size_t j = 0; j < n; j++) {
            dfdt[j] = (f_perturbed[j] - f_current[j]) / h;
        }
    } else {
        double temp = t - h;
        h = temp + t;
        evaluate_equations(equation_ptr, x, t - h, f_perturbed);
        for (size_t j = 0; j < n; j++) {
            dfdt[j] = (f_current[j] - f_perturbed[j]) / h;
        }
    }
}

/**
 * @brief A wrapper for calculate_time_derivative(equation_ptr, max_time, x, t, f_current, dfdt)
 * that automatically calculates f_current
 */
template <typename equation_ptr_type, typename time_type, typename x_vector_type, typename f_vector_type>
void calculate_time_derivative(
    equation_ptr_type const& equation_ptr,
    time_type max_time,
    x_vector_type const& x,
    time_type t,
    f_vector_type& dfdt)
{
    f_vector_type f_current(x.size());
    evaluate_equations(equation_ptr, x, t, f_current);
    calculate_time_derivative(equation_ptr, max_time, x, t, f_current, dfdt);
}

/**
 * @brief Numerically compute the Jacobian matrix and time derivatives of a vector valued function.
 *
 * @param[in] equation_ptr a pointer to an object that can be used to represent a vector valued function f.
 *                         There must be an associated `evaluate_equations(*equation_ptr_type, x, t, y)` function
 *                         defined for equation_ptr_type. It is assumed that the output vector produced by the
 *                         equations has the same length as the input vector.
 *
 * @param[in] max_time the largest time value that can be passed to the vector valued function.
 *
 * @param[in] x an input vector to be passed to the vector valued function.
 *
 * @param[in] t an input time to be passed to the vector valued function.
 *
 * @param[out] jacobi the calculated Jacobian matrix (containing df_i/dx_j evaluated at x, t)
 *
 * @param[out] dfdt the calculated explicit time dependence (i.e., df_i/dt evaluated at x, t)
 */
template <typename equation_ptr_type, typename time_type, typename vector_type, typename matrix_type>
void calculate_jacobian_and_time_derivative(
    equation_ptr_type const& equation_ptr,
    time_type max_time,
    vector_type const& x,
    time_type t,
    matrix_type& jacobi,
    vector_type& dfdt)
{
    calculate_jacobian(equation_ptr, x, t, jacobi);
    calculate_time_derivative(equation_ptr, max_time, x, t, dfdt);
}

#endif
