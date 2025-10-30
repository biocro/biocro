#ifndef ROOT_SECANT_H
#define ROOT_SECANT_H

#include "roots.h"

namespace root_finding
{

/**
 * @brief Secant Method. Provide two initial guesses for root.
 *
 * @details The secant method uses two points \f$x_{n-1}\f$ and \f$x_n\f$ to
 * locally estimate the first derivative to approximate the function as a linear
 * polynomial. The zero of the secant is used as a new guess for a new root.
 *
 * \f[ x_{n+1} = x_n - f(x_n)\left(\frac{ x_n - x_{n-1}}{f(x_n) - f(x_{n-1})}\right \f]
 *
 * Or written this way.
 *
 * \f[ x_{n+1} = \frac{f(x_n) x_{n-1} - f(x_{n-1}) x_{n}}{f(x_n) - f(x_{n-1})} \f]
 *
 * The speed of convergence is about 1.618 per iteration and per evaluation.
 * Although slower than the top speed of Newton's method, the secant method can
 * be more stable.
 *
 * Functions with flat regions (slope is almost zero) or high curvature can cause
 * the secant method to diverge or converge slowly. The second method also converges
 * more slowly for non-simple roots (roots of multiplicity greater than 1).
 */
struct secant : public root_finding_method<secant> {
    using root_finding_method::root_finding_method;
    graph_t last;
    graph_t best;

    template <typename F>
    bool initialize(F&& fun, double x0, double x1)
    {
        last = {x0, fun(x0)};
        best = {x1, fun(x1)};
        if (is_zero(best.y)) {
            flag = Flag::residual_zero;
            return false;
        }
        if (is_zero(last.y)) {
            flag = Flag::residual_zero;
            std::swap(best, last);
            return false;
        }

        return true;
    }

    template <typename F>
    bool iterate(F&& fun)
    {
        double x = get_secant_update(best, last);
        if (!std::isfinite(x)) {
            flag = Flag::division_by_zero;
            return false;
        }
        last = best;
        best.x = x;
        best.y = fun(x);
        return true;
    }

    bool has_converged()
    {
        if (is_zero(best.y)) {
            flag = Flag::residual_zero;
            return true;
        }

        if (is_close(last.x, best.x)) {
            flag = Flag::delta_root_zero;
            return true;
        }

        return false;
    }

    double root() const
    {
        return best.x;
    }

    double residual() const
    {
        return best.y;
    }
};

}  // namespace root_finding
#endif
