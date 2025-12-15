#ifndef ROOT_STEFFENSEN_H
#define ROOT_STEFFENSEN_H
#include "roots.h"

namespace root_finding
{

/**
 * @brief Steffensen's Method. Provide a function object and
 * an initial guesss.
 *
 * @details Steffensen's method approximates the function `f` with
 * a first order Taylor series, using the formula:
 *
 * \f[ f'(x) \approx \frac{f(x + h) - h}{h} \qquad h = f(x) \f]
 *
 * to approximate the first derivative. This expression requires two function
 * evaluations per iteration. This experession achieves quadratic convergence
 * rates under optimal conditions. I.e., it can be as fast as Newton's method
 * however, it is far less stable than either Newton's method or the secant
 * method if the initial guesss is not close to a root. Moreover, the rate
 * of convergence per function call is higher for the secant method.
 *
 */
struct steffensen : public root_finding_method<steffensen> {
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
        double g = fun(current.x + current.y) / current.y - 1;
        if (!std::isfinite(g)) {
            flag = Flag::division_by_zero;
            return false;
        }
        current.x -= current.y / g;
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
    };
};

}  // namespace root_finding
#endif
