#ifndef ROOT_RIDDER_H
#define ROOT_RIDDER_H

#include "roots.h"

namespace root_finding
{

/**
 * @brief Ridder's Method. Provide a function object and an initial
 * valid bracket. Bracket is valid if the sign of the function differs at
 * the end points.
 *
 * @details Ridder's method is a bracketing method: if `(a, b)` is either
 * side of a simple root then `f(a)` and `f(b)` will have different signs.
 * The method finds roots by finding where `f` switches sign between
 * positive and negative.
 *
 * In essence, Ridder's method fits an exponential to the end points and
 * the mid point to estimate `m` in:
 *
 * \f[ h(x) = f(x) \exp(m x) \f]
 *
 * The exponential fit corrects high curvature, especially for exponential
 * like functions. A secant `h(x)` is then used to generate a new bracket
 * endpoint.
 *
 * Ridder's method can perform well on problems that frustrate regula falsi,
 * but it is slower on quadratics.
 *
 * References:
 * - Ridders, C. (1979). "A new algorithm for computing a single root of
 *   a real continuous function". IEEE Transactions on Circuits and Systems.
 *   26 (11): 979–980. doi:10.1109/TCS.1979.1084580
 *
 * - Press et al. (2007). Numerical recipes, 3rd edition.
 *   Cambridge University Press. https://numerical.recipes/book.html
 *
 */
struct ridder : public root_finding_method<ridder> {
    using root_finding_method::root_finding_method;

    template <typename F>
    bool iterate(F&& fun)
    {
        proposal.x = get_midpoint(left, right);
        proposal.y = fun(proposal.x);

        double d = proposal.x - left.x;
        double a = proposal.y / left.y;
        double b = right.y / left.y;
        double denom = a * a - b;
        if (!std::isfinite(denom)) {
            flag = Flag::division_by_zero;
            return false;
        }

        proposal.x += d * a / std::sqrt(denom);
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
