#ifndef ROOTS_ONEDIM_H
#define ROOTS_ONEDIM_H

#include <cmath>
#include <limits>
#include <string>

/**
 * A C++ library for solving 1D equations.
 */

namespace root_algorithm
{

/**
 * @brief Used to hold points `(x, f(x))` on the graph of the function.
 */
struct graph_t {
    double x;
    double y;
};

// Helper function declarations
inline bool is_close(
    double x, double y, double tol, double rtol); // true if x == y
inline bool is_zero(double x, double tol); // true if x == 0
inline bool same_signs(double x, double y);     // true if sign(x) == sign(y)
inline bool opposite_signs(double x, double y); // true if sign(x) != sign(y)
inline bool smaller(double x, double y);        // true if |x| < |y|
inline bool is_between(double x, double a, double b); // true if `x` is `[a,b]`
inline double get_midpoint(const graph_t& a, const graph_t& b);
inline double get_secant_update(const graph_t& a, const graph_t& b);

// For error handling. These flags indicate the reason for termination.
enum class Flag {
    valid,  // don't terminate
    residual_zero,
    delta_root_zero,
    bracket_width_zero,
    max_iterations,
    invalid_bracket,
    division_by_zero,
    halley_no_cross,
    discontinuity,
    bracket_fixed_point
};

template <typename T>
inline bool is_valid(T x)
{
    return x.flag == Flag::valid;
}
inline bool is_successful(Flag flag);
inline std::string flag_message(Flag flag);

/**
 * @class result_t
 *
 * @brief Result from a root finding algorithm.
 *
 * @param root The identified root of a function `f`.
 *
 * @param residual The value of `f` at `root`; `f(root) == 0` if the
 * algorithm was successful.
 *
 * @param iteration The number of iterations performed.
 *
 * @param flag Indicates the reason for termination.
 *
 */
struct result_t {
    double root;
    double residual;
    size_t iteration;
    Flag flag;
};

/**
 * @class root_finder
 *
 * @brief Function object for finding the zero of a function `f`.
 *
 * @param [in] max_iter The total number of iterations allowed.
 *
 * @param [in] abs_tol The absolute tolerance, the error threshold for
 * floating-point zero. E.g., abs_tol = 1e-12 means |x| < 1e-12, or x equals
 * zero to 12 digits. Used for convergence testing.
 *
 * @param [in] rel_tol The relative tolerance, the error threshold for
 * comparing floating point number equality. E.g., rel_tol = 1e-12 means
 * x equals y if the first 12 digits match. Used for convergernce testing.
 *
 * @details This class creates a function object whose `solve` or call method
 * is an interface for using any of the root finding methods. A class template
 * so an instance of this class must be declared with a method as the template
 * argument. Example usage:
 *
 * @code{.cpp}
 * // The zero of `f` equals sqrt(3)
 * double f(double x) { return x*x - 3; };
 *
 * //Declare the solver
 * root_algorithm::root_finder<root_algorithm::secant> solver;
 *
 * // Call the solve method to find a root.
 * root_algorithm::result_t result = solver.solve(f, 1., 2.);
 * @endcode
 *
 * The function `f` can be a c++ function, lambda function, or function object.
 * We will say function `f` in mathematical sense for all of these options.
 * For algorithms which explicitly evaluate derivatives, you must
 * pass a function object with a `double derivative(double x)` method. If
 * not possible or easy to evaluate derivatives, prefer methods which
 * approximate the derivatives rather than using finite differences to
 * approximate the derivatives.
 *
 * See the documentation comment for each method for details on how to
 * use each method.
 *
 * List of methods (using their name here).
 *
 * + newton (derivative)
 * + halley (derivative, second derivative)
 * + steffensen (no derivatives)
 * + fixed_point
 * + secant (no derivatives)
 * + bisection (bracketing)
 * + regula_falsi (bracketing)
 * + ridder (bracketing)
 * + illinois (bracketing)
 * + pegasus (bracketing)
 * + anderson_bjorck (bracketing)
 * + dekekr (contrapoint)
 *
 * Methods range in their typical robustness and speed. Speed and robustness
 * also depend on the problem. Root-bracketing methods are typically more
 * robust, since they are guaranteed to converge for a continuous function.
 * However, that safety comes at the cost of speed methods, as local approximation
 * methods usually require fewer iterations, and methods that evaluate derivatives
 * almost always take fewer iterations. The actual speed of computation
 * depends on how expensive evaluating the function (or its derivatives)
 * is. The function calls per iteration can be counted.
 *
 * + If the first derivative can be evaluated, try Newton's method.
 * + If the solution can be bracket, then try the Illinois method.
 * + If other methods fail to converge, try the bisection method.
 *
 * Each method contains a short description of how it works and why it might fail.
 * References are given for more complex or less famous methods.
 *
 * A new root-finding algorithm can be added by creating a class or struct
 * with the following methods:
 *
 * @code{.cpp}
 * struct state;
 * //holds whatever state saved between iterations.
 * //methods do not have to be instantiated as objects
 * //should have a `Flag flag` member
 * //Efforts should made to minimize copying / allocation between loop
 * //iterations.
 *
 * state initialize(F&& f, Args... args);
 * // takes the initial problem info (e.g., a bracket, an
 * // initial guess) and instantiates a value of type `state`. If the
 * //  provided info is invalid, then return a flag indicating the error.
 *
 * state& iterate(F&& f, state& s);
 * // maps state to state. The actual formula of most methods is
 * // implemented here. If the iteration encounters an error, return the
 * // flag indicating the error.
 * // Some methods handle errors that other methods do not.
 * // iterate is implemented like a compound assignment operator,
 * // updating in-place rather than using an immutable maping.
 * // It should be equivalent to state iterate(f, const state& s)
 * // Where it maps a state to state, and adds a new flag if an error is
 * // encountered. Ideally, convergence is not checked here.
 *
 * state& check_convergence(state& s, _abs_tol, _rel_tol);
 * // checks the state to see if the algorithm has found a root to within
 * // tolerance; set the state flag.
 *
 * double root(const state& s);
 * // Extract a single value for the root. Should be the best guess.
 *
 * double residual(const state& s);
 * // Extract the residual from the state. For user to evaluate if a root
 * // has been found.
 * @endcode
 *
 * Several methods have the same convergence checks and state; so a struct
 * provides a template for those methods. New methods can (but do not have
 * to) inherit from these templates if they are useful.
 *
 * `initialize` takes the initial problem info (like the two guesses in
 * the secant method) and creates an object that holds the state that is
 * kept from one iteration to the next. `iterate` contains the meat of each
 *  method; it refines the guess, etc. `check_convergence` checks if state has
 *  converged; this means the `state` object has to have whatever info is
 * used to make this decision. `root` and `residual` extract the last best
 *  guess and the function's residual at that root from the `state`.
 *
 */
template <typename Method>
struct root_finder : public Method {
    size_t max_iterations;
    double _abs_tol;
    double _rel_tol;

    root_finder() : max_iterations{100},
                    _abs_tol{1e-10},
                    _rel_tol{1e-8} {}

    root_finder(size_t max_iter) : max_iterations{max_iter},
                                   _abs_tol{1e-10},
                                   _rel_tol{1e-8} {}
    root_finder(size_t max_iter, double abs_tol, double rel_tol)
        : max_iterations{max_iter},
          _abs_tol{abs_tol},
          _rel_tol{rel_tol} {}

    using state = typename Method::state;

    template <typename F, typename... Args>
    result_t solve(F&& func, Args&&... args)
    {
        state s = Method::initialize(
            std::forward<F>(func), std::forward<Args>(args)...,
            _abs_tol, _rel_tol);

        for (size_t i = 0; i < (max_iterations + 1); ++i) {
            if (!is_valid(s)) {
                return make_result(s, i);
            }

            s = Method::iterate(std::forward<F>(func), s, _abs_tol, _rel_tol);

            s = Method::check_convergence(s, _abs_tol, _rel_tol);
        }

        s.flag = Flag::max_iterations;
        return make_result(s, max_iterations);
    }

    template <typename F, typename... Args>
    result_t operator()(F&& func, Args&&... args)
    {
        return solve(std::forward<F>(func), std::forward<Args>(args)...);
    }

   private:
    inline result_t make_result(const state& s, size_t iteration)
    {
        return result_t{Method::root(s), Method::residual(s), iteration, s.flag};
    }
};

// Householder and multistep methods

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
struct secant {
    struct state {
        Flag flag;
        graph_t last;
        graph_t best;
    };

    template <typename F>
    inline state initialize(F&& fun, double x0, double x1, double abs_tol, double rel_tol)
    {
        graph_t first = {x0, fun(x0)};
        graph_t second = {x1, fun(x1)};
        if (is_zero(first.y, abs_tol)) {
            return state{Flag::residual_zero, second, first};
        }
        if (is_zero(second.y, abs_tol)) {
            return state{Flag::residual_zero, first, second};
        }
        return state{Flag::valid, first, second};
    }

    template <typename F>
    inline state& iterate(F&& fun, state& s, double abs_tol, double rel_tol)
    {
        auto& best = s.best;
        auto& last = s.last;
        double r = best.y / last.y;
        double p = (best.x - last.x) * r;
        double q = 1 - r;
        if (is_zero(q, abs_tol)) {
            s.flag = Flag::division_by_zero;
            return s;
        }
        last = best;
        best.x += p / q;
        best.y = fun(best.x);
        return s;
    }

    inline state& check_convergence(state& s, double abs_tol, double rel_tol)
    {
        if (is_zero(s.best.y, abs_tol)) {
            s.flag = Flag::residual_zero;
            return s;
        }

        if (is_close(s.last.x, s.best.x, abs_tol, rel_tol)) {
            s.flag = Flag::delta_root_zero;
            return s;
        }

        return s;
    }

    inline double root(const state& s)
    {
        return s.best.x;
    }

    inline double residual(const state& s)
    {
        return s.best.y;
    }
};

/**
 * @brief Fixed Point Method. Provide one initial guesses for root.
 *
 * @details WARNING: Do not use unless you know what you're doing.
 *
 * The fixed point iteration assumes that `f` is defined as
 * `f(x) = g(x) - x` and that `g` has a fixed point which is the desired root.
 *
 * This method iterates the following function:
 * \f[x \mapsto x + f(x) = g(x) \f]
 *
 * If `g` has a fixed point and if `g` is a contraction mapping `|g'(x)| < 1` for
 * all `x` in a neighborhood of the fixed point, then the sequence of iterates will
 * converge to the fixed point.
 *
 * Because fixed point iteration was used in previous versions of BioCro to solve
 * for the CO2 concentrations, this method is provided for cross version comparisons.
 * However, its use is not recommended as all other methods are safer and faster!
 */
struct fixed_point {
    struct state {
        Flag flag;
        double x;
        double y;
    };

    template <typename F>
    inline state initialize(F&& fun, double x0, double abs_tol, double rel_tol)
    {
        state s{Flag::valid, x0, fun(x0)};
        if (is_zero(s.y, abs_tol)) {
            s.flag = Flag::residual_zero;
        }
        return s;
    }

    template <typename F>
    inline state& iterate(F&& fun, state& s, double abs_tol, double rel_tol)
    {
        s.x += s.y;  // x + f(x) <-> x = g(x) if f(x) = g(x) - x
        s.y = fun(s.x);
        return s;
    }

    inline state& check_convergence(state& s, double abs_tol, double rel_tol)
    {
        if (is_zero(s.y, abs_tol)) {
            s.flag = Flag::residual_zero;
            return s;
        }

        return s;
    }

    inline double root(const state& s)
    {
        return s.x;
    }

    inline double residual(const state& s)
    {
        return s.y;
    }
};

// Householder methods
// methods common to newton, halley, etc.
struct one_step_method {
    struct state {
        Flag flag;
        double x;
        double y;
    };

    template <typename F>
    state initialize(F&& fun, double x0, double abs_tol, double rel_tol)
    {
        double y0 = fun(x0);
        if (is_zero(y0, abs_tol)) {
            return state{Flag::residual_zero, x0, fun(x0)};
        }
        return state{Flag::valid, x0, fun(x0)};
    }

    inline state& check_convergence(state& s, double abs_tol, double rel_tol)
    {
        if (is_zero(s.y, abs_tol)) {
            s.flag = Flag::residual_zero;
        }
        return s;
    }

    inline double root(const state& s)
    {
        return s.x;
    }

    inline double residual(const state& s)
    {
        return s.y;
    }
};

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
struct newton : public one_step_method {
    template <typename F>
    inline state& iterate(F&& fun, state& s, double abs_tol, double rel_tol)
    {
        double& x = s.x;
        double& y = s.y;
        double slope = fun.derivative(x);
        if (is_zero(slope, abs_tol)) {
            s.flag = Flag::division_by_zero;
            return s;
        }
        x -= y / slope;
        y = fun(x);
        return s;
    }
};

/**
 * @brief Halley's Method. Provide a function object with methods
 * `double derivative(double x)` and `double second_derivative(double x)`
 * implementing the first and second derivatives. Provide an initual guesss.
 *
 * @details Halley's rational method uses a first and second order Taylor series
 * approximation to refine a guess for the root. Essentially Newton's method
 * is used to pick a direction, and the second derivative is used to pick a
 * good step size (reducing the step size when high curvature is present).
 *
 * Halley's method is even faster than Newton's method, but requires evaluating
 * the second derivative. Not usually feasible for problems where `f`
 * involves complicated expressions, but fast for polynomials or simple
 * transcendental equations.
 *
 * Only use if derivatives can be evaluated directly, prefer the `secant`
 * method if derivatives cannot be evaluated.
 *
 * This method is Halley's rational method. Halley's irrational method solves the
 * quadratic of the Taylor series, using the Newton step to select a root of the
 * quadratic.
 */
struct halley : public one_step_method {
    template <typename F>
    inline state& iterate(F&& fun, state& s, double abs_tol, double rel_tol)
    {
        double& x = s.x;
        double& y = s.y;
        double df = fun.derivative(x);
        if (is_zero(df, abs_tol)) {
            s.flag = Flag::division_by_zero;
            return s;
        }
        double df2 = fun.second_derivative(x);
        double a = y / df;
        double b = df2 / (2 * df);
        b *= a;
        if (is_close(b, 1, abs_tol, rel_tol)) {
            s.flag = Flag::halley_no_cross;
            return s;
        }
        x -= a / (1. - b);  // division by zero Only if no solution
        y = fun(x);
        return s;
    }
};

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
struct steffensen : public one_step_method {
    template <typename F>
    inline state& iterate(F&& fun, state& s, double abs_tol, double rel_tol)
    {
        double& x = s.x;
        double& y = s.y;
        double g = fun(x + y) / y - 1;
        if (is_zero(g, abs_tol)) {
            s.flag = Flag::division_by_zero;
            return s;
        }
        x -= y / g;
        y = fun(x);
        return s;
    }
};

/**
 * @brief Common to all bracketing methods defined here. See `bisection` method
 * or `regula_falsi` for an example.  In general, a bracket method maintains
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
struct bracket_method {
    struct state {
        Flag flag;
        graph_t left;
        graph_t right;
        graph_t proposal;
    };

    template <typename F>
    state initialize(F&& fun, double a, double b, double abs_tol, double rel_tol)
    {
        state s;
        s.left.x = a;
        s.right.x = b;
        s.left.y = fun(a);
        s.right.y = fun(b);
        s.proposal = s.left;
        s.flag = Flag::valid;

        if (is_zero(s.left.y, abs_tol)) {
            s.flag = Flag::residual_zero;
            return s;
        }

        if (is_zero(s.right.y, abs_tol)) {
            s.flag = Flag::residual_zero;
            s.proposal = s.right;
            return s;
        }

        if (same_signs(s.left.y, s.right.y)) {
            s.flag = Flag::invalid_bracket;
            return s;
        }

        return s;
    }

    inline state& check_convergence(state& s, double abs_tol, double rel_tol)
    {
        bool zero_found = is_zero(s.proposal.y, abs_tol);
        if (zero_found) {
            s.flag = Flag::residual_zero;
            return s;
        }

        if (is_close(s.left.x, s.right.x, abs_tol, abs_tol)) {
            double delta_y = std::abs(s.left.y - s.right.y);
            double delta_x = std::abs(s.left.x - s.right.x);
            if (delta_y < (delta_x / rel_tol))
                s.flag = Flag::bracket_width_zero;
            else
                s.flag = Flag::discontinuity;
            return s;
        }

        return s;
    }

    inline double root(const state& s)
    {
        return s.proposal.x;
    }

    inline double residual(const state& s)
    {
        return s.proposal.y;
    }

    inline state& update_bracket(state& s)
    {
        if (same_signs(s.left.y, s.proposal.y)) {
            s.left = s.proposal;
        } else {
            s.right = s.proposal;
        }
        return s;
    }

    template <typename F>
    inline state& midpoint_proposal(F&& fun, state& s)
    {
        // division is safe if bracket is valid
        s.proposal.x = get_midpoint(s.left, s.right);
        s.proposal.y = fun(s.proposal.x);
        return s;
    }

    template <typename F>
    inline state& secant_proposal(F&& fun, state& s)
    {
        // division is safe if bracket is valid
        s.proposal.x = get_secant_update(s.left, s.right);
        s.proposal.y = fun(s.proposal.x);
        return s;
    }
};

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
 */
struct bisection : public bracket_method {
    template <typename F>
    inline state& iterate(F&& fun, state& s, double abs_tol, double rel_tol)
    {
        s = midpoint_proposal(std::forward<F>(fun), s);
        s = update_bracket(s);
        return s;
    }
};

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
struct regula_falsi : public bracket_method {
    template <typename F>
    inline state& iterate(F&& fun, state& s, double abs_tol, double rel_tol)
    {
        s = secant_proposal(std::forward<F>(fun), s);
        s = update_bracket(s);
        return s;
    }
};

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
struct ridder : public bracket_method {
    template <typename F>
    inline state& iterate(F&& fun, state& s, double abs_tol, double rel_tol)
    {
        s = midpoint_proposal(std::forward<F>(fun), s);

        double d = s.proposal.x - s.left.x;
        double a = s.proposal.y / s.left.y;
        double b = s.right.y / s.left.y;
        double denom = a * a - b;
        if (is_zero(denom, abs_tol)) {
            // is this state possible ? fall back to bisection
            s.flag = Flag::division_by_zero;
            return s;
        }

        s.proposal.x += d * a / std::sqrt(denom);
        s.proposal.y = fun(s.proposal.x);
        s = update_bracket(s);
        return s;
    }
};

/**
 * @brief Not a method. Illinois-type methods use the update bracket defined here.
 * See the documentation of the `illinois` method for details.
 */
struct illinois_type : public bracket_method {
    inline state& update_bracket(state& s, double gamma)
    {
        if (opposite_signs(s.proposal.y, s.right.y)) {
            s.left = s.right;
        } else {
            s.left.y *= gamma;
        }
        s.right = s.proposal;
        return s;
    }
};

/**
 * @brief The "Illinois" method. Provide a function object and an initial
 * valid bracket. Bracket is valid if the sign of the function differs at
 * the end points.
 *
 * @details The "Illinois" method, so called because it was developed at
 * the University of Illinois in the 1950s, is effectively the same as
 * "regula falsi" except it catches the failure mode of the regula falsi.
 *
 * Regula falsi is slow if the same end point is retained twice in a row.
 * In the Illinois method, if the same end point is retained twice then
 * the value of the function is reduced by half for computing the next
 * iterate.
 *
 * Let \f$ [x_{n-1}, x_n]\f$ be a bracket, where the "right" side is most recent
 * endpoint. It is possible to have $x_n < x_{n-1}$ even though intervals are not
 * normally written that way. A new point is generated by the secant formula:
 *
 * \f[ x_{n+1} = \frac{f(x_n) x_{n-1} - f(x_{n-1}) x_{n}}{f(x_n) - f(x_{n-1})} \f]
 *
 * If the sign of \f$f(x_{n+1})\f$ is the same as \f$f(x_{n-1})\f$ then we form
 * a new bracket \f$ [x_n , x_{n+1} ] \f$ as normal in "regula falsi". Otherwise,
 * if the sign of \f$f(x_{n+1})\f$ is the same as \f$f(x_{n})\f$ then we build a
 * new bracket \f$[x_{n-1} , x_{n+1} ]\f$ but for the next iteration, we use the
 * value \f$f(x_{n-1})/2 \f$ instead of \f$f(x_{n-1})\f$. In practice, the saved
 * result `left.y = f(left.x)` is simply halved: ` left.y /= 2 `.
 *
 * The illinois method is the simplest of a family of methods, which
 * all rescale the value of the function `f` at the retained endpoint when
 * that endpoint is retained for a second iteration. `left.y *= gamma`.
 *
 * This method is basically always faster than "regula falsi" and has
 * robustness similar to the bisection method.
 *
 * References:
 * - Ford, J. A. (1995). "Improved Illinois-type methods for the solution
 *   of nonlinear equations." Technical Report, University of Essex Press.
 *
 * - Dowell, M.; Jarratt, P. (1971). "A modified regula falsi method for
 *   computing the root of an equation". BIT. 11 (2): 168–174.
 *   doi:10.1007/BF01934364
 */
struct illinois : public illinois_type {
    // does not preserve left and right. Treats right as best guess.
    template <typename F>
    inline state& iterate(F&& fun, state& s, double abs_tol, double rel_tol)
    {
        s = secant_proposal(std::forward<F>(fun), s);
        s = update_bracket(s, 0.5);
        return s;
    }
};

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
struct pegasus : public illinois_type {
    // does not preserve left and right. Treats right as best guess.
    template <typename F>
    inline state& iterate(F&& fun, state& s, double abs_tol, double rel_tol)
    {
        s = secant_proposal(std::forward<F>(fun), s);
        s = update_bracket(
            s,
            s.right.y / (s.right.y + s.proposal.y));
        return s;
    }
};

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
struct anderson_bjorck : public illinois_type {
    // does not preserve left and right. Treats right as best guess.
    template <typename F>
    inline state& iterate(F&& fun, state& s, double abs_tol, double rel_tol)
    {
        s = secant_proposal(std::forward<F>(fun), s);
        double m0 = (s.proposal.y - s.right.y) / (s.proposal.x - s.right.x);
        double m1 = (s.right.y - s.left.y) / (s.right.x - s.left.x);
        s = update_bracket(s, m0 / m1);
        return s;
    }
};

/**
 * @brief The "Dekker" method. A contrapoint bracketing method. Provide a
 * valid bracket.
 *
 * @details A hybrid method combining the secant method and the bisection method.
 * Near a root, the secant method converges quickly, but for poor initial guesses,
 * the secant method can be unstable. Dekker's method saves three points between
 * iterations. The `best` current estimate for the root, the `last` best estimate,
 * and contrapoint. The `contrapoint` and the `best` estimate form the bracket.
 *
 * A new best estimate is proposed using the secant method, but only accepted if
 * the proposal lies between the `best` estimate and the midpoint between the
 * `best` estimate and the `contrapoint` (midpoint of the bracket).

 * A new contrapoint is selected from the new `best` estimate and the old `best`
 * estimate so that the contrapoint and best estimate have opposite signs.
 *
 * Dekker's method has a similar best-case rate of convergence to `secant` and
 * a better rate of convergence than regula falsi. It should perform better
 * against the pathologies of those methods by defaulting to the bisection method.
 *
 * As described in Brent (1973), the method has a pathology where the secant method
 * is always accepted but arbitrarily small.
 *
 * This implementation was adapted from Brent's description. See reference for
 * details.
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
struct dekker {

    struct state {
        Flag flag;
        graph_t contrapoint;
        graph_t last;
        graph_t best;

        double midpoint;
        double secant_proposal;
    };

    template <typename F>
    state initialize(F&& fun, double a, double b, double abs_tol, double rel_tol)
    {
        state s;
        s.contrapoint.x = a;
        s.best.x = b;
        s.contrapoint.y = fun(a);
        s.best.y = fun(b);
        if (smaller(s.contrapoint.y, s.best.y)){
            std::swap(s.best, s.contrapoint);
        }

        s.last = s.best;
        s.flag = Flag::valid;

        if (is_zero(s.best.y, abs_tol)) {
            s.flag = Flag::residual_zero;
            return s;
        }

        if (same_signs(s.best.y, s.contrapoint.y)) {
            s.flag = Flag::invalid_bracket;
            return s;
        }

        return s;
    }

    template <typename F>
    state initialize(F&& fun, double best, double last, double contrapoint, double abs_tol, double rel_tol)
    {
        state s;

        s.best.x = best;
        s.best.y = fun(best);

        s.last.x = last;
        s.last.y = fun(last);

        s.contrapoint.x = contrapoint;
        s.contrapoint.y = fun(contrapoint);

        if (same_signs(s.best.y, s.contrapoint.y))
        {
            if (same_signs(s.best.y, s.last.y)){
                s.flag = Flag::invalid_bracket;
                return s;
            }

            std::swap(s.last, s.contrapoint);
        }

        if (smaller(s.contrapoint.y, s.best.y)){
            std::swap(s.best, s.contrapoint);
        }

        s.flag = Flag::valid;

        if (is_zero(s.best.y, abs_tol)) {
            s.flag = Flag::residual_zero;
            return s;
        }

        return s;
    }

    inline state& check_convergence(state& s, double abs_tol, double rel_tol)
    {
        bool zero_found = is_zero(s.best.y, abs_tol);
        if (zero_found) {
            s.flag = Flag::residual_zero;
            return s;
        }

        if (is_close(s.contrapoint.x, s.best.x, abs_tol, abs_tol)) {
            double delta_y = std::abs(s.contrapoint.y - s.best.y);
            double delta_x = std::abs(s.contrapoint.x - s.best.x);
            if (delta_y < (delta_x / rel_tol))
                s.flag = Flag::bracket_width_zero;
            else
                s.flag = Flag::discontinuity;
            return s;
        }

        return s;
    }

    inline double root(const state& s)
    {
        return s.best.x;
    }

    inline double residual(const state& s)
    {
        return s.best.y;
    }

    template <typename F>
    inline state& iterate(F&& fun, state& s, double abs_tol, double rel_tol)
    {
        s.secant_proposal = get_secant_update(s.last, s.best);
        s.midpoint = get_midpoint(s.contrapoint, s.best);

        // s.last not needed now;
        std::swap(s.best, s.last);

        if (is_between(s.secant_proposal, s.last.x, s.midpoint)){
            s.best.x = s.secant_proposal;
        } else {
            s.best.x = s.midpoint;
        }
        s.best.y = fun(s.best.x);

        if (opposite_signs(s.last.y, s.best.y)){
            s.contrapoint = s.last;
        }

        if (smaller(s.contrapoint.y, s.best.y)) {
            std::swap(s.contrapoint, s.best);
        }

        return s;
    }

};


// Helper function definitions.

inline bool is_close(double x, double y, double tol, double rtol)
{
    using std::abs;
    using std::max;
    double norm = max(abs(x), abs(y));
    return abs(x - y) <= max(tol, rtol * norm);
}

inline bool is_zero(double x, double tol)
{
    return std::abs(x) <= tol;
}

inline bool same_signs(double x, double y)
{
    return x * y > 0;
}

inline bool opposite_signs(double x, double y)
{
    return x * y < 0;
}

inline bool smaller(double x, double y)
{
    return std::abs(x) < std::abs(y);
}

inline bool is_between(double x, double a, double b)
{
    return ((x >= a) && (x <= b)) || ((x <= a) && (x >= b));
}

inline double get_midpoint(const graph_t& a, const graph_t& b){
    return 0.5 * (a.x + b.x);
}

inline double get_secant_update(const graph_t& a, const graph_t& b){
    return (b.y * a.x - a.y * b.x) / (b.y - a.y);
}

bool is_successful(Flag flag)
{
    switch (flag) {
        case Flag::residual_zero:
            return true;
        case Flag::bracket_width_zero:
            return true;
        default:
            return false;
    }
}

std::string flag_message(Flag flag)
{
    switch (flag) {
        case Flag::residual_zero:
            return "Residual is zero.";
        case Flag::delta_root_zero:
            return "Change in guess is zero.";
        case Flag::bracket_width_zero:
            return "Bracket width is zero.";
        case Flag::invalid_bracket:
            return "Bracket is invalid; Function has same signs at both endpoints.";
        case Flag::max_iterations:
            return "Reached the maximum number of iterations.";
        case Flag::division_by_zero:
            return "Division by zero occurred.";
        case Flag::halley_no_cross:
            return "Halley update failed; local quadratic does not cross zero.";
        case Flag::discontinuity:
            return "Found a probable discontinuity or singularity.";
        case Flag::bracket_fixed_point:
            return "Bracket stopped shrinking.";
        case Flag::valid:
            return "Valid state, but convergence not reached. Normally not a termination.";
        default:
            return "Flag not recognized.";
    }
}

}  // namespace root_algorithm
#endif
