#ifndef ROOT_HALLEY_H
#define ROOT_HALLEY_H

#include "roots.h"

namespace root_finding
{
/**
 * @brief Halley's Method. Provide a function object with methods
 * `double derivative(double x)` and `double second_derivative(double x)`
 * implementing the first and second derivatives. Provide an initual guesss.
 *
 * @details Halley's rational method uses a first and second order Taylor series
 * approximation to refine a guess for the root. Essentially Newton's method
 * is used to pick a direction, and the second derivative is used to pick a
 * good step size (reducing the step size when high curvature is present).
 *
 * Halley's method is even faster than Newton's method, but requires evaluating
 * the second derivative. Not usually feasible for problems where `f`
 * involves complicated expressions, but fast for polynomials or simple
 * transcendental equations.
 *
 * Only use if derivatives can be evaluated directly, prefer the `secant`
 * method if derivatives cannot be evaluated.
 *
 * This method is Halley's rational method. Halley's irrational method solves the
 * quadratic of the Taylor series, using the Newton step to select a root of the
 * quadratic.
 */
struct halley : public root_finding_method<halley> {
    using root_finding_method::root_finding_method;
    graph_t current;

    template <typename F>
    bool initialize(F&& fun, double x0)
    {
        current.x = x0;
        current.y = fun(x0);
        if (is_zero(current.y)) {
            flag = Flag::residual_zero;

            return false;
        }
        return true;
    }

    template <typename F>
    bool iterate(F&& fun)
    {
        double df = fun.derivative(current.x);
        if (!std::isfinite(df)) {
            flag = Flag::division_by_zero;
            return false;
        }
        double df2 = fun.second_derivative(current.x);
        double a = current.y / df;
        double b = df2 / (2 * df);
        b *= a;
        if (is_close(b, 1)) {
            flag = Flag::halley_no_cross;
            return false;
        }
        current.x -= a / (1 - b);  // division by zero Only if no solution
        current.y = fun(current.x);
        return true;
    }

    bool has_converged()
    {
        if (is_zero(current.y)) {
            flag = Flag::residual_zero;
            return true;
        }
        return false;
    }

    double root() const
    {
        return current.x;
    }

    double residual() const
    {
        return current.y;
    }
};

}  // namespace root_finding

#endif
