#ifndef ROOT_FIXED_POINT_H
#define ROOT_FIXED_POINT_H

#include "roots.h"

namespace root_finding
{
/**
 * @brief Fixed Point Method.
 *
 * @details WARNING: Do not use unless you know what you're doing.
 *
 * The fixed point iteration assumes that `f` defines a set of equations so that
 * whose solution is a fixed point `f(x) = x`. This method will simply iterate the
 * function.
 *
 * If `f` has a fixed point and if `f` is a contraction mapping `|df(x)| < 1` for
 * all `x` in a neighborhood of the fixed point, then the sequence of iterates will
 * converge to the fixed point.
 *
 * Because fixed point iteration was used in previous versions of BioCro to solve
 * for the CO2 concentrations, this method is provided for cross version comparisons.
 * However, its use is not recommended as all other methods are safer and faster!
 *
 * Use `g(x) = f(x) - x = 0` with any other root finding method.
 */
struct fixed_point : root_finding_method<fixed_point> {
    using root_finding_method::root_finding_method;

    double x;
    double y;

    template <typename F>
    bool initialize(F&& fun, double x0)
    {
        y = fun(x0);
        x = x0;
        if (is_close(x, y)) {
            flag = Flag::residual_zero;
            return false;
        }
        return true;
    }

    template <typename F>
    bool iterate(F&& fun)
    {
        x = y;
        y = fun(x);
        return true;
    }

    bool has_converged()
    {
        if (is_close(x, y)) {
            flag = Flag::residual_zero;
            return true;
        }

        return false;
    }

    double root() const
    {
        // In case the function `fun` has side-effects, it is necessary that it
        // was most recently called at the root. When a root has been found, the
        // most recent function call was `fun(x)`, so we consider `x` to be the
        // root rather than `y`
        return x;
    }

    double residual() const
    {
        return y - x;
    }
};

}  // namespace root_finding
#endif
