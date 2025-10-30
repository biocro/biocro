#ifndef ROOT_ANDERSON_BJORCK_H
#define ROOT_ANDERSON_BJORCK_H

#include "roots.h"

namespace root_finding
{

/**
 * @brief The "Anderson-Björck" method. An Illinois-type bracketing method. Provide a
 * valid bracket.
 *
 * @details The same general idea as the `illinois` method but using a different
 * update for when the same endpoint is retained twice. See the `illinois` method
 * for details.
 *
 *  The scaling factor is the ratio of the divided differences:
 * \f[ \gamma = \frac{f[x_{n+1}, x_n]}{f[x_n, x_{n-1}]} \f]
 * Whether the divided difference is:
 *
 * \f[ f[x_{n+1}, x_n] = \frac{f(x_{n+1}) - f(x_n)}{x_{n+1} - x_n}\f]
 *
 * See reference for details.
 *
 * References:
 * - Ford, J. A. (1995). "Improved Illinois-type methods for the solution
 *   of nonlinear equations." Technical Report, University of Essex Press.
 *
 */
struct anderson_bjorck : public root_finding_method<anderson_bjorck> {
    using root_finding_method::root_finding_method;
    // does not preserve left and right. Treats right as best guess.
    template <typename F>
    bool iterate(F&& fun)
    {
        proposal.x = get_secant_update(left, right);
        proposal.y = fun(proposal.x);
        double m0 = (proposal.y - right.y) / (proposal.x - right.x);
        double m1 = (right.y - left.y) / (right.x - left.x);
        update_bracket(m0 / m1);
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
