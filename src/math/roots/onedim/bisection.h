#ifndef ROOT_BISECTION_H
#define ROOT_BISECTION_H

#include "roots.h"

namespace root_finding
{
/**
 * @brief The Bisection Method. Provide a function object and an initial
 * valid bracket. Bracket is valid if the sign of the function differs at
 * the end points.
 *
 * @details The bisection method is a bracketing method: if `(a, b)` is
 * either side of a simple root then `f(a)` and `f(b)` will have different
 * signs. The bisection method finds roots by finding where `f` switches
 * sign between positive and negative. At each iteration, this method
 * divides the interval in half `c = (a + b) / 2` and then replace `a` with
 * `c` if the sign of `f(c)` is the same as `f(a)`, otherwise `c` replaces
 * `b`.
 *
 * For a continuous function, this method is guaranteed to converge to
 * a root, provided a valid bracket. However, its convergence is quite slow
 * as it only halves the interval at each step. Regardless of problem,
 * 50-150 iterations is common depending on the desired accuracy.
 *
 * The bisection method is not guaranteed to converge to a double root.
 * It does work for any number of roots provided that there is an odd number
 * inside the bracket. It can also converge to discontinuities and singularities
 * if the function switches sign at those points. For instance, the bisection
 * method will correctly identify `x = 0` as the sign switch point for
 * `f(x) = 1 / x` or for `f(x) = signum(x)`.
 *
 * Note `abs_tol` is used to determine if a residual is zero or if the
 * bracket width is effectively zero. `rel_tol` sets the tolerance for
 * deciding if the function is continuous at the estimated root.
 *
 * References:
 *
 * - Press et al. (2007). Numerical recipes, 3rd edition.
 *   Cambridge University Press. https://numerical.recipes/book.html
 *
 * Common to all bracketing methods defined here  In general, a bracket method maintains
 * an interval, a pair of points below and above the root.
 *
 * The `state` struct holds the `proposal` so that each new point can be checked
 * before it's used to update the bracket.
 *
 * These methods use `abs_tol` to test whether the residual is zero and whether
 * the bracket width is zero. For the latter, `abs_tol` is both the absolute and
 * relative tolerance. It is possible that the bracket zeroes in on a root without
 * reaching the absolute tolerance. The `rel_tol` is only used to determine
 * whether or not the bracket of zero-width contains a zero, by testing continuity.
 */

struct bisection : public root_finding_method<bisection> {
    using root_finding_method::root_finding_method;

    graph_t left;
    graph_t right;
    graph_t proposal;

    template <typename F>
    bool initialize(F&& fun, double a, double b)
    {
        left.x = a;
        right.x = b;
        left.y = fun(a);
        right.y = fun(b);
        proposal = left;

        if (is_zero(left.y)) {
            flag = Flag::residual_zero;
            return false;
        }

        if (is_zero(right.y)) {
            flag = Flag::residual_zero;
            proposal = right;
            return false;
        }

        if (same_signs(left.y, right.y)) {
            flag = Flag::invalid_bracket;
            return false;
        }

        return true;
    }

    template <typename F>
    bool iterate(F&& fun)
    {
        proposal.x = get_midpoint(left, right);
        proposal.y = fun(proposal.x);
        update_bracket();
        return true;
    }

    bool has_converged()
    {
        if (is_zero(proposal.y)) {
            flag = Flag::residual_zero;
            return true;
        }

        if (is_close(left.x, right.x)) {
            flag = Flag::bracket_width_zero;
            return true;
        }

        return false;
    }

    double root() const
    {
        return proposal.x;
    }

    double residual() const
    {
        return proposal.y;
    }

    void update_bracket()
    {
        if (same_signs(left.y, proposal.y)) {
            left = proposal;
        } else {
            right = proposal;
        }
    }
};

}  // namespace root_finding
#endif
