#ifndef ROOT_ONEDIM_COMMON_H
#define ROOT_ONEDIM_COMMON_H

#include <cmath>
#include <limits>
#include <string>

/**
 * BioCro's library for solving 1D equations.
 */

namespace root_finding
{

/**
 * @brief Used to hold points `(x, f(x))` on the graph of the function.
 */
struct graph_t {
    double x;
    double y;
};

// For error handling. These flags indicate the reason for termination.
enum class Flag {
    residual_zero,
    delta_root_zero,
    bracket_width_zero,
    max_iterations,
    invalid_bracket,
    division_by_zero,
    halley_no_cross,
    bracket_fixed_point,
    function_is_nonfinite
};

// Helper function declarations

inline bool same_signs(double x, double y);                           // true if sign(x) == sign(y)
inline bool opposite_signs(double x, double y);                       // true if sign(x) != sign(y)
inline bool smaller(double x, double y);                              // true if |x| < |y|
inline bool is_between(double x, double a, double b);                 // true if `x` is `[a,b]`
inline double get_midpoint(const graph_t& a, const graph_t& b);       // return (a.x + b.x)/2
inline double get_secant_update(const graph_t& a, const graph_t& b);  // computes secant formula; may return NaN
inline bool is_successful(Flag flag);                                 // assuming continuous function.
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
 * @class root_finding_method
 *
 * @brief Base class for root_finding methods.
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
 * @details This base class for all root finding methods. It defines the logic
 * common to all root_finding_methods and defines the interface to external users.
 * This base class uses the "curiously recurring template pattern" so that the methods
 * in this base class are statically casted to derived classes. A root finding method
 * (or algorithm) is a derived class `X` of this class where `X` is the template
 * (see example of new method below). Effectively, this is a template version of
 * an abstract base class.
 *
 * This documentation describes the usage of the methods since this class implements
 * the common interface to all methods. Here's an example of using this library:
 *
 * @code{.cpp}
 * // include the appropriate header files
 * // including a specific method's header file will include this file.
 * #include "src/math/roots/onedim/secant.h"  // for secant
 *
 *  // The zero of `f` equals sqrt(3)
 * double f(double x) { return x*x - 3; };
 *
 * // Each method is a class.
 * using namespace root_finding::secant;
 * secant solve(100, 1e-12, 1e-14);
 *
 * // Call the `operator()` method or the `solve` method to find a root.
 * root_finding::result_t result = solve(f, 1., 2.); // initial guesses are passed with the function
 * result = solve.solve(f, 3., 2.); // different guesses
 *
 * // solve a different problem with the same solver
 * // any callable may be passed.
 * auto g = [](double x) {return x/(x + 1) - 0.99; } ;
 * result = solve(g, 0, 1);
 *
 * struct FuncObj {
 *      double operator()(double x) {
 *          return (1 - x*x) * x   - 1;
 *      }
 * };
 *
 * result = solve(FuncObj{}, 0, 1);
 * // solve the same problem with a different method;
 * using namespace root_finding::bisection;
 * bisection find_zero(50); // 50 iterations is enough
 * result = find_zero(f, 0, 3); // bracketing methods need to span a root
 * result = find_zero(g, 0, 100);
 *
 * @endcode
 *
 *
 * The function `f` can be a regular function, lambda function, or function object.
 * We will say function `f` in mathematical sense for all of these options.
 *
 * The exception is that for a algorithm which explicitly evaluate derivatives,
 * you must pass a function object with a `double derivative(double x)` method.
 * If not possible or easy to evaluate derivatives, prefer methods which
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
 * is. The function calls per iteration can be counted.
 *
 * + If the first derivative can be evaluated, try Newton's method.
 * + If the solution can be bracket, then try the Illinois method.
 * + If other methods fail to converge, try the bisection method.
 *
 * Each method contains a short description of how it works and why it might fail.
 * References are given for more complex or less famous methods.
 *
 * A new root-finding algorithm can be added by creating a child class that inherits from
 * `root_finding_method` using the curriously-recurring-template-pattern:
 *
 * @code{.cpp}
 * struct a_new_method : public root_finding_method<a_new_method> {
 *
 * // inherit constructors
 * using root_finding_method::root_finding_method;
 *
 * //member fields hold whatever state saved between iterations.
 * State state;
 * // Persistent state means few memory allocations are needed between loop
 * // iterations.
 *
 * template<typename F>
 * bool initialize(F&& f, double x0, double x1);
 * // takes the initial problem info (e.g., a bracket, an
 * // initial guess) and prepares internal state for iteration.
 * // return `true` if input is valid for iteration loop
 * // return `false` if input is invalid or input is already a root
 * // set termination flags
 *
 * template<typename F>
 * bool iterate(F&& f);
 * // Update internal state to refine estimate of a root
 * // The iteration formula / logic of most methods is implemented here.
 * // If the iteration encounters an error, then return `false` else
 * // return `true`.  Ideally, convergence is not checked here.
 *
 * bool has_converged();
 * // checks the state to see if the algorithm has found a root to within
 * // tolerance
 *
 * double root() const ;
 * // Extract a single value for the root. Should be the best guess.
 *
 * double residual() const;
 * // Extract the residual from the state. For user to evaluate if a root
 * // has been found.
 * };
 * @endcode
 *
 */
template <typename Method>
struct root_finding_method {
    // member fields common to all root_finding_methods
    size_t max_iterations;
    double _abs_tol;
    double _rel_tol;
    bool is_valid;
    Flag flag;  // termination_flag

    root_finding_method() : max_iterations{100},
                            _abs_tol{1e-10},
                            _rel_tol{1e-8},
                            is_valid{false},
                            flag{} {}

    root_finding_method(size_t max_iter) : max_iterations{max_iter},
                                           _abs_tol{1e-10},
                                           _rel_tol{1e-8},
                                           is_valid{false},
                                           flag{} {}
    root_finding_method(size_t max_iter, double abs_tol, double rel_tol)
        : max_iterations{max_iter},
          _abs_tol{abs_tol},
          _rel_tol{rel_tol},
          is_valid{false},
          flag{} {}

    // iteration loop logic is here
    template <typename F, typename... Args>
    result_t solve(F&& func, Args&&... args)
    {
        // each method has different states
        // `initialize` initializes the internal state.
        // is_valid==false indicates a termination state
        is_valid = static_cast<Method*>(this)->initialize(std::forward<F>(func), std::forward<Args>(args)...);
        for (size_t i = 0; i < (max_iterations + 1); ++i) {
            // terminate if `initialize`, `
            if (!is_valid) {
                return make_result(i);
            }

            // `iterate` computes the next iteration of an iterative method
            // is_valid == true indicates successful iteration
            is_valid = static_cast<Method*>(this)->iterate(std::forward<F>(func));

            // check if residual is NaN
            is_valid = std::isfinite(static_cast<Method*>(this)->residual());
            if (!is_valid)
                flag = Flag::function_is_nonfinite;

            if (is_valid)
                // check for convergence; method specific implementation.
                is_valid = !(static_cast<Method*>(this)->has_converged());
        }

        flag = Flag::max_iterations;  // reason for termination
        return make_result(max_iterations);
    }

    template <typename F, typename... Args>
    result_t operator()(F&& func, Args&&... args)
    {
        return solve(std::forward<F>(func), std::forward<Args>(args)...);
    }

    // All methods require tolerance-based floating point number equality tests.
    // Are two floating point numbers equal?
    // Equality is lax if both `x` and `y` are big.
    inline bool is_close(double x, double y)
    {
        double norm = std::min(std::abs(x), std::abs(y));
        return std::abs(x - y) <= std::max(_abs_tol, _rel_tol * norm);
    }

    // Is a floating point number zero?
    inline bool is_zero(double x)
    {
        return std::abs(x) <= _abs_tol;
    }

   protected:
    // extract info
    result_t make_result(size_t iteration)
    {
        return result_t{static_cast<Method*>(this)->root(), static_cast<Method*>(this)->residual(), iteration, flag};
    }
};

// FUNCTION DEFINITIONS
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

inline double get_midpoint(const graph_t& a, const graph_t& b)
{
    return 0.5 * (a.x + b.x);
}

inline double get_secant_update(const graph_t& a, const graph_t& b)
{
    return (b.y * a.x - a.y * b.x) / (b.y - a.y);
}

// If the function is continuous, then bracket_width_zero always indicates
// success. Bracketing methods identify where a sign-change occurs, and functions
// with discontinuities can have sign-changes which are not true roots.
inline bool is_successful(Flag flag)
{
    return (flag == Flag::residual_zero || flag == Flag::bracket_width_zero);
}

std::string flag_message(Flag flag)
{
    switch (flag) {
        case Flag::residual_zero:
            return "Residual is zero.";
        case Flag::delta_root_zero:
            return "Change in guess is zero. Slow improvement";
        case Flag::bracket_width_zero:
            return "Bracket width is zero. Could be singularity";
        case Flag::invalid_bracket:
            return "Bracket is invalid; Function has same signs at both endpoints.";
        case Flag::max_iterations:
            return "Reached the maximum number of iterations.";
        case Flag::division_by_zero:
            return "Division by zero occurred.";
        case Flag::halley_no_cross:
            return "Halley update failed; local quadratic does not cross zero.";
        case Flag::bracket_fixed_point:
            return "Bracket stopped shrinking.";
        case Flag::function_is_nonfinite:
            return "Function returned a nonfinite value: NaN, Inf, -Inf.";
        default:
            return "Flag not recognized.";
    }
}

}  // namespace root_finding
#endif
