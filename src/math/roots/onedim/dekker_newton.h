
#ifndef ROOT_DEKKER_NEWTON_H
#define ROOT_DEKKER_NEWTON_H

#include "roots.h"

namespace root_finding
{

/**
 * @brief The "Dekker-Newton" method. A contrapoint bracketing method
 * using Newton's update. Provide a valid bracket and function object
 * implementing the derivative.
 *
 * @details A hybrid method combining the newton method and the bisection method.
 * Near a root, the newton method converges quickly, but for poor initial guesses,
 * the newton method can be unstable. This method, adapeted from Dekker's method,
 * saves three points between  iterations. The `best` current estimate
 * for the root, the `last` best estimate, and contrapoint.
 * The `contrapoint` and the `best` estimate form the bracket.
 *
 * A new best estimate is proposed using the secant method, but only accepted if
 * the proposal lies between the `best` estimate and the midpoint between the
 * `best` estimate and the `contrapoint` (midpoint of the bracket).

 * A new contrapoint is selected from the new `best` estimate and the old `best`
 * estimate so that the contrapoint and best estimate have opposite signs.
 *
 * This implementation was designed by Scott Oswald and based on Dekker's method
 * description. See references for details of Dekker's or Brent's method.
 *
 * References:
 * - Brent, R. P. (1973), "Chapter 4: An Algorithm with Guaranteed Convergence
 *   for Finding a Zero of a Function", Algorithms for Minimization without
 *   Derivatives, Englewood Cliffs, NJ: Prentice-Hall, ISBN 0-13-022335-2
 * - Dekker, T. J. (1969), "Finding a zero by means of successive linear
 *   interpolation", in Dejon, B.; Henrici, P. (eds.), Constructive Aspects of
 *   the Fundamental Theorem of Algebra, London: Wiley-Interscience,
 *   ISBN 978-0-471-20300-1
 */
struct dekker_newton : public root_finding_method<dekker_newton> {
    using root_finding_method::root_finding_method;

    graph_t contrapoint;
    graph_t last;
    graph_t best;

    double midpoint;
    double proposal;

    template <typename F>
    bool initialize(F&& fun, double a, double b)
    {
        contrapoint.x = a;
        best.x = b;
        contrapoint.y = fun(a);
        best.y = fun(b);
        if (smaller(contrapoint.y, best.y)) {
            std::swap(best, contrapoint);
        }

        last = best;

        if (is_zero(best.y)) {
            flag = Flag::residual_zero;
            return false;
        }

        if (same_signs(best.y, contrapoint.y)) {
            flag = Flag::invalid_bracket;
            return false;
        }

        return true;
    }

    template <typename F>
    bool initialize(F&& fun, double a, double b, double c)
    {
        best.x = a;
        best.y = fun(a);

        last.x = b;
        last.y = fun(b);

        contrapoint.x = c;
        contrapoint.y = fun(c);

        if (same_signs(best.y, contrapoint.y)) {
            if (same_signs(best.y, last.y)) {
                flag = Flag::invalid_bracket;
                return false;
            }

            std::swap(last, contrapoint);
        }

        if (smaller(contrapoint.y, best.y)) {
            std::swap(best, contrapoint);
        }

        if (is_zero(best.y)) {
            flag = Flag::residual_zero;
            return false;
        }

        return true;
    }

    template <typename F>
    bool iterate(F&& fun)
    {
        // newton update
        proposal = best.x - best.y / fun.derivative(best.x);
        midpoint = get_midpoint(contrapoint, best);

        // s.last not needed now;
        std::swap(best, last);

        if (is_between(proposal, last.x, midpoint)) {
            best.x = proposal;
        } else {
            best.x = midpoint;
        }
        best.y = fun(best.x);

        if (opposite_signs(last.y, best.y)) {
            contrapoint = last;
        }

        if (smaller(contrapoint.y, best.y)) {
            std::swap(contrapoint, best);
        }

        return true;
    }

    bool has_converged()
    {
        if (is_zero(best.y)) {
            flag = Flag::residual_zero;
            return true;
        }

        if (is_close(contrapoint.x, best.x)) {
            flag = Flag::bracket_width_zero;
            return true;
        }

        return false;
    }

    inline double root() const
    {
        return best.x;
    }

    inline double residual() const
    {
        return best.y;
    };
};

}  // namespace root_finding
#endif
