#ifndef ROOT_NEWTON_H
#define ROOT_NEWTON_H

#include "roots.h"

namespace root_finding
{
/**
 * @brief Newton's Method. Provide a function object with `double derivative(double x)`
 * method implementing the derivative. Provide an initual guesss.
 *
 * @details Newton's method uses the zero of a first-order Taylor series
 * approximation of `f`to refine guesses for the root.
 *
 * Newton's method is quite fast, especially if the derivative and
 * function can be evaluated simultaneously. Convergence under optimal
 * conditions is quadratic.
 *
 * Regions where the first derivative is zero can cause divergent behavior.
 * Chaotic behavior can occur at the boundary between basins of attraction.
 * Non-simple roots tend to slow convergence.
 *
 * Use Newton's method preferentially if the derivatives can be evaluated. If not,
 * use the secant method, rather than approximate the first derivative.
 *
 */
struct newton : public root_finding_method<newton> {
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
        double slope = fun.derivative(current.x);
        if (!std::isfinite(slope)) {
            flag = Flag::division_by_zero;
            return false;
        }
        current.x -= current.y / slope;
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
