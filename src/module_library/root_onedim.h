#ifndef ROOTS_ONEDIM_H
#define ROOTS_ONEDIM_H

#include <cmath>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>

/**
 * A C++ library for solving 1D equations.
 */

namespace root_algorithm
{

// For error handling. These flags indicate the reason for termination.
enum class Flag {
    valid,  // don't terminate
    residual_zero,
    delta_root_zero,
    max_iterations,
    invalid_bracket,
    bracket_width_zero,
    singularity,
    non_finite_root,
    repeated_guess
};

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
    bool success;
};

inline std::string error_message(const result_t& r, std::string prefix);

/**
 * @class root_finder
 *
 * @brief Function object for finding the zero of a function `f`.
 *
 * @param [in] max_iter The total number of iterations allowed.
 *
 * @param [in] abs_tol The absolute tolerance, the error threshold for
 * floating-point zero. This parameter sets the accuracy for deciding if zero has
 * been found. `x` is a root or zero if `f(x)` evaluates to zero.
 * E.g., abs_tol = 1e-12 means \f$|f(x)| < 10^{-12}\f$, or `f(x)` equals
 * zero to 12 digits. Note the error in `x` may be quite large if
 *  \f$ |f'(x)| \ll 1 \f$`.
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
 * size_t max_iter = 100; // usually more than 100 iterations indicates problems
 * double abs_tol = 1e-12;
 * root_algorithm::root_finder<root_algorithm::secant> solver{max_iter, abs_tol};
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
 * + dekker (contrapoint)
 * + dekker-newton (contrapoint + derivative)
 *
 * Methods range in their typical robustness and speed. Speed and robustness
 * also depend on the problem. Root-bracketing methods are typically more
 * robust, since they are guaranteed to converge for a continuous function.
 * However, that safety comes at the cost of speed methods, as local approximation
 * methods usually require fewer iterations, and methods that evaluate derivatives
 * almost always take fewer iterations. The actual speed of computation
 * depends on how expensive evaluating the function (or its derivatives)
 * is. The function calls per iteration can be counted. For most methods, the function
 * is called once per iteration.
 *
 * Choosing a method: For well-behaved functions, try the Secant method or Newton's
 * method (if you can provide a formula for the derivative). These are faster but
 * less robust. The `bisection` method is useful for testing on poorly behaved
 * functions as it is insensitive to flat regions (which cause problems for the section method)
 *  and it can identify multiple simple roots, roots of odd multiplicity (which is rare in BioCro's uses),
 * discontinuities or singularities. Dekker's method (`dekker`) is a hybrid between
 * the secant method and the bisection method.
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
struct root_finder {
    size_t max_iterations = 100;
    Method method;

    root_finder(size_t max_iter, double abs_tol) : max_iterations{max_iter}, method{abs_tol} {}

    template <typename F, typename... Args>
    result_t solve(F&& func, Args&&... args)
    {
        method.initialize(std::forward<F>(func), std::forward<Args>(args)...);

        std::cout << std::setprecision(20);

        for (size_t i = 0; i < (max_iterations + 1); ++i) {
            if (method.flag() != Flag::valid) {
                return make_result(i);
            }

            // std::cout << "Iteration: " << i << "\n  ";
            // method.print();
            method.iterate(std::forward<F>(func));
            method.check_convergence();
        }

        method.set_flag(Flag::max_iterations);
        return make_result(max_iterations);
    }

    template <typename F, typename... Args>
    result_t operator()(F&& func, Args&&... args)
    {
        return solve(std::forward<F>(func), std::forward<Args>(args)...);
    }

   private:
    inline result_t make_result(size_t iteration)
    {
        return result_t{method.root(), method.residual(), iteration, method.flag(), is_successful(method.flag())};
    }

    inline bool is_successful(Flag flag)
    {
        return flag == Flag::residual_zero || flag == Flag::bracket_width_zero;
    }
};

struct method_base {
    struct graph_t {
        double x;
        double y;
    };

    double atol = 1e-12;
    double dbl_eps = 2 * std::numeric_limits<double>::epsilon();
    Flag _flag;

    method_base(double a) : atol{a}, dbl_eps{}, _flag{} {}

    inline bool is_zero(double x)
    {
        return std::abs(x) <= atol;
    }

    inline bool is_zero(const graph_t& a)
    {
        return std::abs(a.y) <= atol;
    }

    inline bool is_close(double x, double y)
    {
        double norm = std::min(std::abs(x), std::abs(y));
        return std::abs(x - y) <= 0.125 * atol + 2 * dbl_eps * (norm + 1);
    }

    inline bool is_same_sign(double x, double y)
    {
        return x * y > 0;
    }

    inline bool is_same_sign(const graph_t& a, const graph_t& b)
    {
        return is_same_sign(a.y, b.y);
    }

    inline bool is_opposite_sign(double x, double y)
    {
        return x * y < 0;
    }

    inline bool is_opposite_sign(const graph_t& a, const graph_t& b)
    {
        return is_opposite_sign(a.y, b.y);
    }

    inline bool smaller(double x, double y)
    {
        return std::abs(x) < std::abs(y);
    }

    inline bool smaller(const graph_t& a, const graph_t& b)
    {
        return std::abs(a.y) < std::abs(b.y);
    }

    inline bool is_between(double x, double a, double b)
    {
        return ((x >= a) && (x <= b)) || ((x <= a) && (x >= b));
    }

    inline double get_midpoint(const graph_t& a, const graph_t& b)
    {
        return 0.5 * (a.x + b.x);
    }

    inline double get_secant_update(const graph_t& a, const graph_t& b)
    {
        return (a.x * b.y - b.x * a.y) / (b.y - a.y);
    }

    Flag flag()
    {
        return _flag;
    }

    void set_flag(Flag f)
    {
        _flag = f;
    }

    inline double get_tol(double x)
    {
        return dbl_eps * std::abs(x) + atol;
    }
};

// Householder methods
struct two_point_method : method_base {
    two_point_method(double atol) : method_base{atol}, last{}, best{} {}

    graph_t last;
    graph_t best;

    template <typename F>
    inline two_point_method& initialize(F&& fun, double x0, double x1)
    {
        if (is_close(x0, x1)) {
            set_flag(Flag::repeated_guess);
            return *this;
        }

        last = {x0, fun(x0)};
        best = {x1, fun(x1)};

        if (is_zero(last)) {
            std::swap(last, best);
            set_flag(Flag::residual_zero);
            return *this;
        }

        if (is_zero(best)) {
            set_flag(Flag::residual_zero);
            return *this;
        }

        set_flag(Flag::valid);
        return *this;
    }

    inline two_point_method& check_convergence()
    {
        if (flag() != Flag::valid) {
            return *this;
        }

        if (is_zero(best.y)) {
            set_flag(Flag::residual_zero);
            return *this;
        }

        if (!std::isfinite(best.x)) {
            set_flag(Flag::non_finite_root);
            return *this;
        }

        if (is_close(best.x, last.x)) {
            set_flag(Flag::delta_root_zero);
            return *this;
        }

        return *this;
    }

    inline double root()
    {
        return best.x;
    }

    inline double residual()
    {
        return best.y;
    }
};

struct one_point_method : two_point_method {
    one_point_method(double atol) : two_point_method{atol} {}

    template <typename F>
    inline one_point_method& initialize(F&& fun, double x0)
    {
        best = {x0, fun(x0)};

        if (is_zero(best)) {
            set_flag(Flag::residual_zero);
            return *this;
        }

        set_flag(Flag::valid);
        return *this;
    }
};

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
struct secant : two_point_method {
    secant(double atol) : two_point_method{atol} {}

    template <typename F>
    inline secant& iterate(F&& fun)
    {
        std::swap(last, best);
        best.x = get_secant_update(last, best);
        best.y = fun(best.x);
        return *this;
    }
};

/**
 * @brief Fixed Point Method. Provide one initial guesses for root.
 *
 * @details WARNING: Do not use unless you know what you're doing.
 *
 * The fixed point iteration assumes that repeated application of `f` converges
 * to a fixed point such that `f(x) = x`.
 *
 * This method tests for convergence by checking `|f(x) - x| < tol`.
 *
 * If `f` has a fixed point and if `f` is a contraction mapping `|f'(x)| < 1` for
 * all `x` in a neighborhood of the fixed point, then the sequence of iterates will
 * converge to the fixed point.
 *
 * Because fixed point iteration was used in previous versions of BioCro to solve
 * for the CO2 concentrations, this method is provided for cross version comparisons.
 * However, its use is not recommended as all other methods are safer and faster!
 */
struct fixed_point : one_point_method {
    fixed_point(double atol) : one_point_method{atol} {}

    template <typename F>
    inline fixed_point& initialize(F&& fun, double x0)
    {
        last.x = x0;
        best.x = fun(x0);
        best.y = best.x - last.x;
        if (is_zero(best.y)) {
            set_flag(Flag::residual_zero);
            return *this;
        }
        set_flag(Flag::valid);
        return *this;
    }

    template <typename F>
    inline fixed_point& iterate(F&& fun)
    {
        last = best;
        best.x = fun(best.x);
        best.y = best.x - last.x;
        return *this;
    }

    inline fixed_point& check_convergence()
    {
        if (is_zero(best.y)) {
            set_flag(Flag::residual_zero);
            return *this;
        }

        if (!std::isfinite(best.x)) {
            set_flag(Flag::non_finite_root);
            return *this;
        }

        return *this;
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
struct newton : one_point_method {
    newton(double atol) : one_point_method(atol) {}

    template <typename F>
    inline newton& iterate(F&& fun)
    {
        double slope = fun.derivative(best.x);
        // if (is_divide_by_zero(slope)) {
        //     return *this;
        // }

        last = best;
        best.x -= best.y / slope;
        best.y = fun(best.x);
        return *this;
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
struct halley : one_point_method {
    halley(double a) : one_point_method{a} {}

    template <typename F>
    inline halley& iterate(F&& fun)
    {
        double& x = best.x;
        double& y = best.y;
        double df = fun.derivative(x);
        // if (is_divide_by_zero(df)) {
        //     return *this;
        // }

        last = best;
        double df2 = fun.second_derivative(x);
        double a = y / df;
        double b = df2 / (2 * df);
        b *= a;
        x -= a / (1. - b);  // division by zero Only if no solution
        y = fun(x);
        return *this;
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
struct steffensen : one_point_method {
    steffensen(double a) : one_point_method{a} {}

    template <typename F>
    inline steffensen& iterate(F&& fun)
    {
        double g = fun(best.x + best.y) / best.y - 1;
        // if (is_divide_by_zero(g)) {
        //     return *this;
        // }

        last = best;
        best.x -= best.y / g;
        best.y = fun(best.x);
        return *this;
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
struct bracket_method : method_base {
    graph_t left;
    graph_t right;
    graph_t proposal;

    bracket_method(double a) : method_base{a} {}

    template <typename F>
    bracket_method& initialize(F&& fun, double a, double b)
    {
        left = {a, fun(a)};
        right = {b, fun(b)};
        proposal = left;

        if (is_zero(left)) {
            set_flag(Flag::residual_zero);
            return *this;
        }

        if (is_zero(right)) {
            set_flag(Flag::residual_zero);
            proposal = right;
            return *this;
        }

        if (is_same_sign(left, right)) {
            set_flag(Flag::invalid_bracket);
            return *this;
        }

        set_flag(Flag::valid);
        return *this;
    }

    inline bracket_method& check_convergence()
    {
        if (is_zero(proposal)) {
            set_flag(Flag::residual_zero);
            return *this;
        }

        if (!std::isfinite(root())) {
            set_flag(Flag::non_finite_root);
            return *this;
        }

        if (is_close(left.x, right.x)) {
            double s0, s1;

            s0 = (proposal.y - left.y) / (proposal.x - left.x);
            s1 = (right.y - proposal.y) / (right.x - proposal.x);

            if (is_close(s0, s1)) {
                set_flag(Flag::bracket_width_zero);
            } else {
                set_flag(Flag::singularity);
            }
            return *this;
        }

        return *this;
    }

    inline double root()
    {
        return proposal.x;
    }

    inline double residual()
    {
        return proposal.y;
    }

    inline bracket_method& update_bracket()
    {
        if (is_same_sign(left, proposal)) {
            std::swap(left, proposal);
        } else {
            std::swap(right, proposal);
        }
        return *this;
    }

    template <typename F>
    inline bracket_method& midpoint_proposal(F&& fun)
    {
        proposal.x = get_midpoint(left, right);
        proposal.y = fun(proposal.x);
        return *this;
    }

    template <typename F>
    inline bracket_method& secant_proposal(F&& fun)
    {
        // division is safe if bracket is valid
        proposal.x = get_secant_update(left, right);
        proposal.y = fun(proposal.x);
        return *this;
    }

    inline void print()
    {
        std::cout << left.x << ", " << left.y << "\n  ";
        std::cout << right.x << ", " << right.y << "\n  ";
        std::cout << proposal.x << ", " << proposal.y << "\n  ";
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
struct bisection : bracket_method {
    bisection(double a) : bracket_method{a} {}

    template <typename F>
    inline bisection& iterate(F&& fun)
    {
        midpoint_proposal(std::forward<F>(fun));
        update_bracket();
        return *this;
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
struct regula_falsi : bracket_method {
    regula_falsi(double a) : bracket_method{a} {}

    template <typename F>
    inline regula_falsi& iterate(F&& fun)
    {
        secant_proposal(std::forward<F>(fun));
        update_bracket();
        return *this;
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
struct ridder : bracket_method {
    ridder(double a) : bracket_method{a} {}

    template <typename F>
    inline ridder& iterate(F&& fun)
    {
        midpoint_proposal(std::forward<F>(fun));

        double d = proposal.x - left.x;
        double a = proposal.y / left.y;
        double b = right.y / left.y;
        double denom = a * a - b;
        // if (is_divide_by_zero(denom)) {
        //     return *this;
        // }

        proposal.x += d * a / std::sqrt(denom);
        proposal.y = fun(proposal.x);
        update_bracket();
        return *this;
    }
};

/**
 * @brief Not a method. Illinois-type methods use the update bracket defined here.
 * See the documentation of the `illinois` method for details.
 */
struct illinois_type : bracket_method {
    illinois_type(double a) : bracket_method{a} {}

    inline illinois_type& update_bracket(double gamma)
    {
        if (is_opposite_sign(proposal, right)) {
            left = right;
        } else {
            left.y *= gamma;
        }
        right = proposal;
        return *this;
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
struct illinois : illinois_type {
    illinois(double a) : illinois_type{a} {}
    // does not preserve left and right. Treats right as best guess.
    template <typename F>
    inline illinois& iterate(F&& fun)
    {
        secant_proposal(std::forward<F>(fun));
        update_bracket(0.5);
        return *this;
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
struct pegasus : illinois_type {
    pegasus(double a) : illinois_type{a} {}

    // does not preserve left and right. Treats right as best guess.
    template <typename F>
    inline pegasus& iterate(F&& fun)
    {
        secant_proposal(std::forward<F>(fun));
        update_bracket(right.y / (right.y + proposal.y));
        return *this;
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
struct anderson_bjorck : illinois_type {
    anderson_bjorck(double a) : illinois_type{a} {}

    // does not preserve left and right. Treats right as best guess.
    template <typename F>
    inline anderson_bjorck& iterate(F&& fun)
    {
        secant_proposal(std::forward<F>(fun));
        double m0 = (proposal.y - right.y) / (proposal.x - right.x);
        double m1 = (right.y - left.y) / (right.x - left.x);
        update_bracket(m0 / m1);
        return *this;
    }
};

struct contrapoint_method : method_base {
    graph_t contrapoint;
    graph_t last;
    graph_t best;

    double midpoint;
    double proposal;

    contrapoint_method(double a) : method_base(a) {}

    template <typename F>
    inline contrapoint_method& initialize(F&& fun, double a, double b)
    {
        contrapoint.x = a;
        best.x = b;
        contrapoint.y = fun(a);
        best.y = fun(b);

        // check for zeros
        if (is_zero(best)) {
            set_flag(Flag::residual_zero);
            return *this;
        }

        if (is_zero(contrapoint)) {
            set_flag(Flag::residual_zero);
            return *this;
        }

        if (is_same_sign(best, contrapoint)) {
            set_flag(Flag::invalid_bracket);
            return *this;
        }

        if (smaller(contrapoint.y, best.y)) {
            std::swap(best, contrapoint);
        }

        // contrapoint so that secant method is well defined on first iteration.
        last = contrapoint;
        set_flag(Flag::valid);
        return *this;
    }

    template <typename F>
    inline contrapoint_method& initialize(F&& fun, double x0, double a, double b)
    {
        best.x = x0;
        best.y = fun(x0);

        last.x = a;
        last.y = fun(a);

        contrapoint.x = b;
        contrapoint.y = fun(b);

        if (is_same_sign(best, contrapoint)) {
            if (is_same_sign(best, last)) {
                set_flag(Flag::invalid_bracket);
                return *this;
            }

            std::swap(last, contrapoint);
        }

        if (smaller(contrapoint.y, best.y)) {
            std::swap(best, contrapoint);
        }

        if (is_zero(best)) {
            set_flag(Flag::residual_zero);
            return *this;
        }

        set_flag(Flag::valid);

        return *this;
    }

    inline contrapoint_method& check_convergence()
    {
        if (is_zero(best)) {
            set_flag(Flag::residual_zero);
            return *this;
        }

        if (is_close(best.x, contrapoint.x)) {
            double s0, s1;

            s0 = (best.y - last.y) / (best.x - last.x);
            s1 = (best.y - contrapoint.y) / (best.x - contrapoint.x);

            if (is_close(s0, s1)) {
                set_flag(Flag::bracket_width_zero);
            } else {
                set_flag(Flag::singularity);
            }

            return *this;
        }

        return *this;
    }

    inline double root()
    {
        return best.x;
    }

    inline double residual()
    {
        return best.y;
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
struct dekker : contrapoint_method {
    dekker(double a) : contrapoint_method{a} {}

    inline void print()
    {
        std::cout << best.x << ", " << best.y << "\n  ";
        std::cout << contrapoint.x << ", " << contrapoint.y << "\n  ";
        std::cout << last.x << ", " << last.y << "\n  ";
    }

    template <typename F>
    inline dekker& iterate(F&& fun)
    {
        proposal = get_secant_update(last, best);
        midpoint = get_midpoint(contrapoint, best);

        last = best;

        if (is_between(proposal, best.x, midpoint)) {
            best.x = proposal;
        } else {
            std::cout << " Bisection\n";
            best.x = midpoint;
        }
        best.y = fun(best.x);

        if (is_same_sign(best, contrapoint)) {
            contrapoint = last;
        }

        if (smaller(contrapoint, best)) {
            std::swap(contrapoint, best);
        }

        return *this;
    }
};

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
struct dekker_newton : contrapoint_method {
    dekker_newton(double a) : contrapoint_method{a} {}
    template <typename F>
    inline dekker_newton& iterate(F&& fun)
    {
        // newton update
        proposal = best.x - best.y / fun.derivative(best.x);
        midpoint = get_midpoint(contrapoint, best);

        // last not needed now;
        std::swap(best, last);

        if (is_between(proposal, last.x, midpoint)) {
            best.x = proposal;
        } else {
            best.x = midpoint;
        }
        best.y = fun(best.x);

        if (is_opposite_sign(last, best)) {
            contrapoint = last;
        }

        if (smaller(contrapoint, best)) {
            std::swap(contrapoint, best);
        }

        return *this;
    }
};

// Helper function definitions.

std::string flag_message(Flag flag)
{
    switch (flag) {
        case Flag::residual_zero:
            return "Residual is zero.";
        case Flag::delta_root_zero:
            return "No change in the sequence of iterates but `residual` is not zero. "
                   "Either method's iteration is stuck at a fixed point, or improvement is very small slow. "
                   "If `residual` is small, try a small increase in the tolerance. Otherwise, try a different method.";
        case Flag::repeated_guess:
            return "Every guess must be distinct for this method. If guesses are chosen automatically, "
                   "then guesses might be equal for some inputs; try adding a small constant to one of them.";
        case Flag::invalid_bracket:
            return "Bracket is invalid; Function has same signs at both endpoints. "
                   "Warning: automatically chosen endpoints might beequal for certain inputs, "
                   "try adding a small constant to an endpoint.";
        case Flag::max_iterations:
            return "Reached the maximum number of iterations."
                   "If `root` is reasonably and `residual` is small, try increasing `max_iterations` "
                   "or increase tolerance by 2x or 10x. Otherwise, try improving the starting guesses "
                   "or try a different method.";
        case Flag::valid:
            return "Valid state, but convergence not reached. "
                   "Indicates a programming error in the `root_algorithm` library.";
        case Flag::bracket_width_zero:
            return "Bracket width is zero, and ";
        case Flag::singularity:
            return "Singularity or jump discontinuity detected. "
                   "Bracket's width shrank below tolerance `2 * eps` without getting `residual = 0`. "
                   "If `residual` is small, then failure is a false alarm. Otherwise, ensure no "
                   "programming errors in the function (possible cause of singularities) and that "
                   "the function possesses a root to find.";
        case Flag::non_finite_root:
            return "Root is non-finite: inf, -inf, NaN."
                   "Either function returned non-finite value or `division by zero` in method, "
                   "possibly due to encountering a flat region of the function's domain.";
        default:
            return "Flag not recognized. Indicates a programming error in the `root_algorithm` library.";
    }
}
std::string error_message(const result_t& r, std::string prefix = "")
{
    std::stringstream out;
    out << prefix << ":\n  ";
    out << flag_message(r.flag) << "\n    ";
    out << "Flag = " << static_cast<int>(r.flag) << "\n    ";
    out << std::setprecision(20);
    out << "Root = " << r.root << "\n    ";
    out << "Residual = " << r.residual << "\n    ";
    out << "Iteration = " << r.iteration << '\n';
    return out.str();
}

}  // namespace root_algorithm
#endif
