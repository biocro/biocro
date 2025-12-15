#ifndef ROOT_PEGASUS_H
#define ROOT_PEGASUS_H

#include "roots.h"

namespace root_finding
{

/**
 * @brief The "pegasus" method. An Illinois-type bracketing method. Provide a
 * valid bracket.
 *
 * @details The `pegasus` has the same general idea as the `illinois` method but
 * uses a different update for when the same endpoint is retained twice. See the
 * `illinois` method for details.
 *
 * The scaling factor is the ratio:
 *
 * \f[ \gamma = \frac{f(x_n)}{f(x_n) + f(x_{n+1})} \f]
 *
 * which is always positive because the update only occurs when \f$f(x_n)\f$ and
 * \f$f(x_{n+1})\f$ have the same same sign.
 *
 * It is slightly faster than the `illinois` method in numerical tests.
 * See references for additional details.
 *
 * References:
 * - Ford, J. A. (1995). "Improved Illinois-type methods for the solution
 *   of nonlinear equations." Technical Report, University of Essex Press.
 *
 * - Dowell, M., Jarratt, P. The “Pegasus” method for computing the root of an
 *   equation. BIT 12, 503–508 (1972). https://doi.org/10.1007/BF01932959
 */
struct pegasus : public root_finding_method<pegasus> {
    using root_finding_method::root_finding_method;
    // does not preserve left and right. Treats right as best guess.
    template <typename F>
    bool iterate(F&& fun)
    {
        proposal.x = get_secant_update(left, right);
        proposal.y = fun(proposal.x);
        update_bracket(right.y / (right.y + proposal.y));
        return true;
    }

    void update_bracket(double gamma)
    {
        if (opposite_signs(proposal.y, right.y)) {
            left = right;
        } else {
            left.y *= gamma;
        }
        right = proposal;
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
};
}  // namespace root_finding
#endif
