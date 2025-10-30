#ifndef ROOT_REGULA_FALSI_H
#define ROOT_REGULA_FALSI_H

#include "roots.h"

namespace root_finding
{

/**
 * @brief Regula falsi, or the false position method.
 * Provide a function object and an initial valid bracket.
 * Bracket is valid if the sign of the function differs at the end points.
 *
 * @details Regula falsi, or the false position method, is a bracketing
 * method: if `(a, b)` is either side of a simple root then `f(a)` and
 * `f(b)` will have different signs. The method finds roots by finding
 * where `f` switches sign between positive and negative.
 * This method computes a new bracket using the secant of the end points,
 * replacing the end points based on the signs.
 *
 * Regula falsi has the same safety as the bisection method but with better
 * speed. However, it usually is not as fast as the secant method. The secant
 * method uses the best guesses found so far to achieve a better convergence
 * rate, while regula falsi sacrifices some speed for robustness.
 *
 * Regula falsi can converge much slower than the bisection method for a
 * function with large curvature within the bracket. Regula falsi doesn't
 * necessarily produce a sequence of brackets that shrinks to zero.
 *
 * Thus, try regula falsi and use bisection if regula falsi fails to converge.
 *
 * References:
 *
 * - Press et al. (2007). Numerical recipes, 3rd edition.
 *   Cambridge University Press. https://numerical.recipes/book.html
 *
 */
struct regula_falsi : public root_finding_method<regula_falsi> {
    using root_finding_method::root_finding_method;

    template <typename F>
    bool iterate(F&& fun)
    {
        proposal.x = get_secant_update(left, right);
        proposal.y = fun(proposal.x);
        update_bracket();
        return true;
    }

    // same as bisection

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
