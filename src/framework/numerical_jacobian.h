#ifndef NUMERICAL_JACOBIAN_H
#define NUMERICAL_JACOBIAN_H

#include <memory>       // for std::shared_ptr
#include "constants.h"  // for calculation_constants::eps_deriv

/**
 *  @brief Numerically compute the Jacobian matrix of a vector valued function.
 *
 *  The Jacobian \f$ \mathbf{J} \f$ of a vector-valued function
 *  \f$ \vec{f} (\vec{x}, t) \f$ is a matrix whose elements are defined by
 *  \f$ J_{i,j} = \frac{\partial f_i}{\partial x_j} \f$, where \f$ f_i \f$ is
 *  the \f$ i^{th} \f$ component of \f$ \vec{f} \f$ and \f$ x_j \f$ is the
 *  \f$ j^{th} \f$ component of \f$ \vec{x} \f$. (Here, \f$ t \f$ is a scalar.)
 *  In other words, each element of \f$ \mathbf{J} \f$ is itself a function of
 *  \f$ \vec{x} \f$ and \f$ t \f$ defined by one of the possible partial
 *  derivatives of \f$ \vec{f} \f$ with respect to the elements of
 *  \f$ \vec{x} \f$. For more information, see the
 *  [Wikpedia page for the Jacobian matrix](https://en.wikipedia.org/wiki/Jacobian_matrix_and_determinant).
 *
 *  Here, we numerically calculate the value of the Jacobian matrix at
 *  particular values of \f$ \vec{x} \f$ and \f$ t \f$. To do this, we use the
 *  simplest (but not the most accurate) method for estimating the value of each
 *  partial derivative in the matrix. This can be classified as the "forward
 *  perturbation method" and is defined as follows:
 *
 *  \f[
 *    \frac{\partial f_i}{\partial x_j} \approx
 *      \frac{f_i(\vec{x} + h \cdot \vec{e}_j) - f_i(\vec{x})}{h},
 *  \f]
 *
 *  where \f$ h \f$ is a small number and \f$ \vec{e}_j \f$ is the unit vector
 *  in the \f$ j \f$ direction. \f$ h \f$ is calculated to be a small fraction
 *  of \f$ x_j \f$, or a minimal value when \f$ x_j = 0 \f$.
 *
 *  By using this method, we are avoiding more complex options that determine an
 *  optimal value for \f$ h \f$, that consider forward and backward
 *  perturbations, or that consider additional terms in the Taylor series for
 *  \f$ \vec{f} \f$. For our purposes, the additional computational costs
 *  inherent to these methods do not justify the marginal increases in
 *  accuracy. For example, see this discussion of step size from
 *  [Nadim Khalil's thesis](http://www.iue.tuwien.ac.at/phd/khalil/node14.html):
 *
 *  > It is also known that numerical differentiation is an unstable procedure
 *  > prone to truncation and subtractive cancellation errors. Decreasing the
 *  > step size [\f$ h \f$ in the equation above] will reduce the truncation
 *  > error. Unfortunately a smaller step has the opposite effect on the
 *  > cancellation error. Selecting the _optimal_ step size for a certain
 *  > problem is computationally expensive and the benefits achieved are not
 *  > justifiable as the effect of small errors in the values of the elements of
 *  > the Jacobian matrix is minor. For this reason, the sizing of the finite
 *  > difference step is not attempted and a constant increment size is used in
 *  > evaluating the gradient.
 *
 *  Also note that in general, if \f$ \vec{x} \f$ has \f$ N \f$ elements, then
 *  calculating the Jacobian using the forward pertubration scheme requires
 *  \f$ N + 1 \f$ function evaluations, while using forward and backward
 *  perturbations requires \f$ 2 N \f$. Likewise, higher-order alternatives also
 *  add a large number of function evaluations. Function evaluations are
 *  expensive in BioCro, so it is important to use them sparingly.
 *
 *  When calculating numerical derivatives, roundoff error in the value of the
 *  step size \f$ h \f$ can have a significant effect. For that reason, it is
 *  important to ensure that \f$ h \f$ is exactly representable in the computer
 *  architecture. This can be accomplished using a simple scheme like
 *
 *  > `double volatile temp = x + h;`
 *  >
 *  > `h = temp - x;`
 *
 *  The `volatile` keyword ensures that the compiler's optimizer does not
 *  eliminate this reassignment process. For more information about this issue,
 *  see Section 5.7 of _Numerical Recipes in C_,
 *  [2nd Edition](http://s3.amazonaws.com/nrbook.com/book_C210.html) or
 *  [3rd Edition](http://numerical.recipes/book/book.html).
 *
 *  In BioCro, the Jacobian is almost always calculated for the case where
 *  \f$ \vec{f}(\vec{x}, t) = d \vec{x} / dt \f$. In this case, \f$ \vec{x} \f$
 *  and \f$ \vec{y} = \vec{f}(\vec{x}, t) \f$ have the same dimension. Even in
 *  other BioCro applications where \f$ \vec{f}(\vec{x}, t) \f$ is not the time
 *  derivative of \f$ \vec{x} \f$, this restriction on dimensionality still
 *  holds. Because of this, here we assume that the output from \f$ \vec{f} \f$
 *  _always_ has the same length as \f$ \vec{x} \f$.
 *
 *  @param[in] equation_ptr a pointer to an object that can be used to represent
 *             a vector valued function `f(x, t)`. There must be an associated
 *             `evaluate_equations(*equation_ptr_type, x, t, y)` function
 *             defined for `equation_ptr_type`. It is assumed that the output
 *             vector produced by the equations has the same length as the input
 *             vector.
 *
 *  @param[in] x an input vector to be passed to `f`.
 *
 *  @param[in] t an input time to be passed to `f`.
 *
 *  @param[in] f_current the output of `f` evaluated at `x` and `t`
 *
 *  @param[out] jacobi the calculated Jacobian matrix (containing the partial
 *              derivatives \f$ \partial{f_i}/\partial{x_j} \f$ evaluated at `x`
 *              and `t`)
 */
template <typename equation_ptr_type,
          typename vector_type,
          typename time_type,
          typename matrix_type>
void calculate_jacobian(
    equation_ptr_type const& equation_ptr,
    vector_type const& x,
    time_type t,
    vector_type const& f_current,
    matrix_type& jacobi)
{
    size_t n = x.size();

    // Make a vector to store the perturbed f(x,t)
    vector_type f_perturbed(n);

    // Perturb each element x_i of the input vector to find df_j(x,t)/dx_i,
    // which is stored at jacobi(j,i)
    double h;
    vector_type x_perturbed = x;

    for (size_t i = 0; i < n; i++) {
        // Detemine the step size h by taking a fraction of x[i] defined by
        // h = x[i] * eps_deriv. Ensure that the step size h is close to this
        // value but is exactly representable in the computer architecture.
        h = x[i] * calculation_constants::eps_deriv;
        if (h == 0.0) {
            h = calculation_constants::eps_deriv * calculation_constants::eps_deriv;
        }
        double volatile temp = x[i] + h;
        h = temp - x[i];

        // Calculate the new function value by adding h to the ith element of x
        // and evaluating f, storing its value in f_perturbed.
        x_perturbed[i] = x[i] + h;
        evaluate_equations(equation_ptr, x_perturbed, t, f_perturbed);

        // Store the results in the Jacobian matrix
        for (size_t j = 0; j < n; j++) {
            jacobi(j, i) = (f_perturbed[j] - f_current[j]) / h;
        }

        // Reset the ith differential quantity
        x_perturbed[i] = x[i];
    }
}

/**
 *  @brief A wrapper for the five-argument version of `calculate_jacobian()`
 *  that automatically evaluates `f_current`.
 */
template <typename equation_ptr_type,
          typename vector_type,
          typename time_type,
          typename matrix_type>
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
 *  @brief A wrapper for the five-argument version of `calculate_jacobian()` for
 *  equations that have no time dependence.
 */
template <typename equation_ptr_type,
          typename vector_type,
          typename matrix_type>
void calculate_jacobian_nt(
    equation_ptr_type const& equation_ptr,
    vector_type const& x,
    vector_type const& f_current,
    matrix_type& jacobi)
{
    calculate_jacobian(equation_ptr, x, 0, f_current, jacobi);
}

/**
 *  @brief A wrapper for the five-argument version of `calculate_jacobian()`
 *  that automatically evaluates `f_current` for equations that have no time
 *  dependence.
 */
template <typename equation_ptr_type,
          typename vector_type,
          typename matrix_type>
void calculate_jacobian(
    equation_ptr_type const& equation_ptr,
    vector_type const& x,
    matrix_type& jacobi)
{
    calculate_jacobian(equation_ptr, x, 0, jacobi);
}

/**
 *  @brief Numerically compute the explicit time derivatives of a vector valued
 *  function.
 *
 *  Here, the explicit time derivative of a vector-valued function
 *  \f$ \vec{f}(\vec{x}, t) \f$ is calculated numerically using
 *
 *  \f[
 *    \frac{d f_i}{dt} \approx \frac{f_i(\vec{x}, t + h) - f_i(\vec{x}, t)}{h},
 *  \f]
 *
 *  where \f$ f_i \f$ is the \f$ i^{th} \f$ component of \f$ \vec{f} \f$. As is
 *  the case with the partial derivatives of `calculate_jacobian()`, this method
 *  is fast but not the most accurate, and requires that \f$ h \f$ be exactly
 *  representable in the computer architecture. With this definition, we ignore
 *  any possible implicit time dependence of \f$ \vec{x} \f$, which is why we
 *  refer to this derivative as the "explicit" time derivative.
 *
 *  One consideration unique to the time derivative is that the time domain is
 *  usually finite. If \f$ t \f$ is close to the maximum possible value, then
 *  this forward perturbation approach is not applicable. In that case, we
 *  instead calculate the derivative as
 *
 *  \f[
 *    \frac{d f_i}{dt} \approx \frac{f_i(\vec{x}, t) - f_i(\vec{x}, t - h)}{h}.
 *  \f]
 *
 *  This is a backwards perturbation.
 *
 *  In BioCro, the explicit time derivative is almost always calculated for the
 *  case where \f$ \vec{f}(\vec{x}, t) = d \vec{x} / dt \f$. In this case,
 *  \f$ \vec{x} \f$ and \f$ \vec{y} = \vec{f}(\vec{x}, t) \f$ have the same
 *  dimension. Even in other BioCro applications where
 *  \f$ \vec{f}(\vec{x}, t) \f$ is not the time derivative of \f$ \vec{x} \f$,
 *  this restriction on dimensionality still holds. Because of this, here we
 *  assume that the output from \f$ \vec{f} \f$ _always_ has the same length as
 *  \f$ \vec{x} \f$.
 *
 *  @param[in] equation_ptr a pointer to an object that can be used to represent
 *             a vector valued function `f(x, t)`. There must be an associated
 *             `evaluate_equations(*equation_ptr_type, x, t, y)` function
 *             defined for `equation_ptr_type`. It is assumed that the output
 *             vector produced by the equations has the same length as the input
 *             vector.
 *
 *  @param[in] max_time the largest time value that can be passed to `f`.
 *
 *  @param[in] x an input vector to be passed to `f`.
 *
 *  @param[in] t an input time to be passed to `f`.
 *
 *  @param[in] f_current the output of `f` evaluated at `x` and `t`.
 *
 *  @param[out] dfdt the calculated time derivative, i.e., the vector of
 *              derivatives \f$ df_i / dt \f$ evaluated at `x` and `t`.
 */
template <typename equation_ptr_type,
          typename time_type,
          typename vector_type>
void calculate_time_derivative(
    equation_ptr_type const& equation_ptr,
    time_type max_time,
    vector_type const& x,
    time_type t,
    vector_type const& f_current,
    vector_type& dfdt)
{
    size_t n = x.size();

    // Make a vector to store the perturbed f(x,t)
    vector_type f_perturbed(n);

    // Perturb the time to find df(x,t)/dt. Use a forward step whenever
    // possible. Detemine the step size h by taking a fraction of t:
    // h = t * eps_deriv. Ensure that the step size h is close to this value but
    // is exactly representable in the computer architecture.
    double h = t * calculation_constants::eps_deriv;
    if (h == 0.0) {
        h = calculation_constants::eps_deriv * calculation_constants::eps_deriv;
    }
    if (t + h <= max_time) {
        double volatile temp = t + h;
        h = temp - t;
        evaluate_equations(equation_ptr, x, t + h, f_perturbed);
        for (size_t j = 0; j < n; j++) {
            dfdt[j] = (f_perturbed[j] - f_current[j]) / h;
        }
    } else {
        double volatile temp = t - h;
        h = t - temp;
        evaluate_equations(equation_ptr, x, t - h, f_perturbed);
        for (size_t j = 0; j < n; j++) {
            dfdt[j] = (f_current[j] - f_perturbed[j]) / h;
        }
    }
}

/**
 *  @brief A wrapper for  the six-argument version of
 *  `calculate_time_derivative()` that automatically calculates `f_current`.
 */
template <typename equation_ptr_type,
          typename time_type,
          typename vector_type>
void calculate_time_derivative(
    equation_ptr_type const& equation_ptr,
    time_type max_time,
    vector_type const& x,
    time_type t,
    vector_type& dfdt)
{
    vector_type f_current(x.size());
    evaluate_equations(equation_ptr, x, t, f_current);
    calculate_time_derivative(equation_ptr, max_time, x, t, f_current, dfdt);
}

/**
 *  @brief Numerically compute the Jacobian matrix and the explicit time
 *  derivative of a vector valued function.
 *
 *  This function simply calls `calculate_jacobian()` and
 *  `calculate_time_derivative()`.
 *
 *  @param[in] equation_ptr a pointer to an object that can be used to represent
 *             a vector valued function `f(x, t)`. There must be an associated
 *             `evaluate_equations(*equation_ptr_type, x, t, y)` function
 *             defined for `equation_ptr_type`. It is assumed that the output
 *             vector produced by the equations has the same length as the input
 *             vector.
 *
 *  @param[in] max_time the largest time value that can be passed to `f`.
 *
 *  @param[in] x an input vector to be passed to `f`.
 *
 *  @param[in] t an input time to be passed to `f`.
 *
 *  @param[out] jacobi the calculated Jacobian matrix (containing the partial
 *              derivatives \f$ \partial{f_i}/\partial{x_j} \f$ evaluated at `x`
 *              and `t`)
 *
 *  @param[out] dfdt the calculated time derivative, i.e., the vector of
 *              derivatives \f$ df_i / dt \f$ evaluated at `x` and `t`.
 */
template <typename equation_ptr_type,
          typename time_type,
          typename vector_type,
          typename matrix_type>
void calculate_jacobian_and_time_derivative(
    equation_ptr_type const& equation_ptr,
    time_type max_time,
    vector_type const& x,
    time_type t,
    matrix_type& jacobi,
    vector_type& dfdt)
{
    vector_type f_current(x.size());
    evaluate_equations(equation_ptr, x, t, f_current);
    calculate_jacobian(equation_ptr, x, t, f_current, jacobi);
    calculate_time_derivative(equation_ptr, max_time, x, t, f_current, dfdt);
}

#endif
