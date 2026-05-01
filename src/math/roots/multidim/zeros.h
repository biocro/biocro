#ifndef ZEROS_MULTIDIM_H
#define ZEROS_MULTIDIM_H

#include <array>
#include <algorithm>  // std::swap
#include <cmath>      // std::sqrt, std::isnan

#include "../../linalg/base.h"
namespace root_multidim
{
/**
 * @brief Termination status codes for zero-finding solvers.
 *
 * Returned inside result_t to indicate why iteration stopped.
 * Successful termination is indicated by `residual_zero` or `delta_x_zero`;
 * all other values indicate failure or a limit was reached.
 */
enum class Flag {
    residual_zero,          // f(x) == 0
    delta_x_zero,           // current_x == last_x
    zero_is_nonfinite,      // x = NaN, Inf, -Inf,
    max_iterations,         //
    function_is_nonfinite,  // f(x) = NaN, Inf, -Inf,
    singular_matrix         // Esimtate
};

/**
 * @brief Holds the outcome of a zero-finding solve.
 *
 * Aggregates the final estimate, residual, iteration count, and the
 * reason iteration stopped.
 *
 * @tparam Dim Dimension of the system (number of equations = unknowns).
 */
template <size_t Dim>
struct result_t {
    std::array<double, Dim> zero;
    std::array<double, Dim> residual;
    size_t iteration;
    Flag flag;
    bool success;

    result_t(
        std::array<double, Dim> const& x,
        std::array<double, Dim> const& y,
        size_t i,
        Flag f,
        bool success) : zero{x},
                        residual{y},
                        iteration{i},
                        flag{f},
                        success{success} {}
};

enum class Status {
    ok,         // valid state, ok to continue iteration
    invalid,    // invalid inputs, do not iterate
    converged,  // successful termination
    failed      // failed to converge
};
/**
 * @brief CRTP base class for iterative zero-finding methods.
 *
 * Provides the outer solve loop, convergence tolerance helpers, and
 * result packaging. Concrete methods (e.g. `broyden<Dim>`) derive from
 * this class via the Curiously Recurring Template Pattern and must
 * implement three member functions:
 *
 * | Function        | Signature                                    | Purpose                                                   |
 * |-----------------|----------------------------------------------|-----------------------------------------------------------|
 * | `initialize`    | `Status initialize(F&& fun, Args&&... args)` | Set up state from the initial guess                       |
 * | `iterate`       | `Status iterate(F&& fun)`                    | Perform one iteration of the method (e.g., Newton update) |
 * | `has_converged` | `Status has_converged()`                     | Check if stopping criteria is met.                        |
 *
 * Each function returns a `Status` enum class; these are status codes, that allow the method
 * to communicate success or failure to this interface class.
 *
 * | `Status`            | Meaning                                                                            |
 * |---------------------|------------------------------------------------------------------------------------|
 * | `Status::ok`        | Iteration state is valid but has not converged. Ok to continue iteration           |
 * | `Status::invalid`   | Iteration state is invalid; initial guess does not satisfy requirements of method  |
 * | `Status::converged` | Iteration state meets convergence or tolerance criteria.                           |
 * | `Status::failed`    | Iteration state has failed to converge (e.g., exceeded maximum iterations)         |
 *
 *
 * The derived class must also expose `zero()` and `residual()` accessors
 * returning `std::array<double, Dim>`.
 *
 * @tparam Dim    Dimension of the system.
 * @tparam Method Concrete derived type (CRTP parameter).
 *
 * @par Typical usage (via a concrete method such as broyden)
 * @code
 * broyden<2> solver(200, 1e-10, 1e-10); // max_iter, abs_tol, rel_tol
 *
 * auto f = [](std::array<double, 2> x) -> std::array<double, 2> {
 *     return { x[0]*x[0] + x[1] - 1.0,
 *              x[0]      - x[1]*x[1] };
 * };
 *
 * result_t<2> res = solver(f, std::array<double,2>{0.5, 0.5});
 *
 * if (res.success) {
 *     // success — use res.zero
 * }
 * @endcode
 *
 * @note Tolerances apply dimension-aware norms: the vector overload of
 *       `is_zero()` tests @f$ \|y\| < \varepsilon_\text{abs} +
 *       \varepsilon_\text{rel}\|x\| @f$, so convergence criteria scale
 *       consistently with problem size.
 */
template <size_t Dim, typename Method>
struct zero_finding_method {
    zero_finding_method(size_t max_iter, double abs_tol, double rel_tol)
        : max_iterations{max_iter},
          _abs_tol{abs_tol},
          _rel_tol{rel_tol}
    {
    }
    zero_finding_method() = default;

    // --- configuration --------------------------------------------------------

    size_t max_iterations = 100;  ///< Maximum iterations before `Flag::max_iterations` is set.
    double _abs_tol = 1e-12;      ///< Absolute tolerance used to test for `f(x) == 0`.
    double _rel_tol = 1e-12;      ///< Relative tolerance used to test if `x == y`.

    // --- state ----------------------------------------------------------------

    Flag flag;                   ///< Reason for termination
    Status status = Status::ok;  ///< Iteration status

    // --- primary interface ----------------------------------------------------

    /**
     * @brief Runs the full solve loop.
     *
     * Calls `initialize`, then repeatedly calls `iterate` and
     * `has_converged` until convergence, a failure signal, or
     * `max_iterations` is reached.
     *
     * @tparam F    Callable representing the function whose zero is sought.
     * @tparam Args Types of any additional arguments forwarded to `initialize`
     *              (typically the initial guess).
     * @param fun  The function f : R^Dim → R^Dim.
     * @param args Additional arguments forwarded to `Method::initialize`.
     * @return A `result_t<Dim>` describing the outcome.
     */
    template <typename F, typename... Args>
    result_t<Dim> solve(F&& fun, Args&&... args)
    {
        // `initialize` internal state; forward method-specific arguments
        // `initialize` checks if inputs satisfy requirements
        status = static_cast<Method*>(this)->initialize(std::forward<F>(fun), std::forward<Args>(args)...);

        // iteration loop;
        // i counts the number of times `iterate` has been called
        for (size_t i = 0; i <= max_iterations; ++i) {
            if (status != Status::ok) {
                return make_result(i);
            }

            status = static_cast<Method*>(this)->iterate(std::forward<F>(fun));

            if (status == Status::ok) {
                status = static_cast<Method*>(this)->has_converged();
            }
        }
        flag = Flag::max_iterations;
        return make_result(max_iterations);
    }

    /**
     * @brief Convenience operator — equivalent to calling solve().
     *
     * Allows a solver object to be used as a callable:
     * @code
     *   result_t<N> res = solver(f, guess);
     * @endcode
     */
    template <typename F, typename... Args>
    inline result_t<Dim> operator()(F&& fun, Args&&... args)
    {
        return solve(std::forward<F>(fun), std::forward<Args>(args)...);
    }

   protected:
    // --- helpers available to derived classes ---------------------------------

    /**
     * @brief Packages the current solver state into a result_t.
     * @param i Iteration index at the time of termination.
     * @return  A `result_t` populated from the derived class's `zero()`,
     *          `residual()`, and `this->flag`.
     */
    result_t<Dim> make_result(size_t i)
    {
        return result_t<Dim>(
            static_cast<Method*>(this)->zero(),
            static_cast<Method*>(this)->residual(),
            i,
            flag,
            status == Status::converged);
    }

    /**
     * @brief Scalar approximate-equality test with mixed absolute/relative tolerance.
     *
     * Returns `true` when
     * @f$ |x - y| \le \max(\varepsilon_\text{abs},\, \varepsilon_\text{rel} \cdot \min(|x|,|y|)) @f$.
     *
     * The tolerance is anchored to the *smaller* magnitude, so equality is
     * easier to satisfy when both values are large (lax near infinity) and
     * harder when both are near zero (tight near the origin).
     *
     * @param x First value.
     * @param y Second value.
     * @return `true` if x and y are considered equal under the configured tolerances.
     */
    inline bool is_close(double x, double y) const
    {
        double norm = std::min(std::abs(x), std::abs(y));
        return std::abs(x - y) <= std::max(_abs_tol, _rel_tol * norm);
    }

    /**
     * @brief Scalar zero test.
     * @param x Value to test.
     * @return `true` if @f$ |x| \le \varepsilon_\text{abs} @f$.
     */
    inline bool is_zero(double x) const
    {
        return std::abs(x) <= _abs_tol;
    }

    /**
     * @brief Vector zero test with dimension-aware mixed tolerance.
     *
     * Returns `true` when
     * @f$ \|y\| < \varepsilon_\text{abs} + \varepsilon_\text{rel}\|x\| @f$.
     *
     * Using the norm of the current iterate `x` as the relative scale means
     * the effective tolerance grows with the solution magnitude and does not
     * tighten spuriously for large-valued problems.
     *
     * @param y Residual vector (the quantity being tested for smallness).
     * @param x Current zero estimate (provides the relative scale).
     * @return `true` if `y` is considered zero relative to `x`.
     */
    inline bool is_zero(
        linalg::vector<double, Dim> const& y,
        linalg::vector<double, Dim> const& x) const
    {
        double ysq = linalg::dot(y, y);
        double xsq = linalg::dot(x, x);
        return std::sqrt(ysq) < _abs_tol + _rel_tol * std::sqrt(xsq);
    }

    /**
     * @brief Checks whether any component of a vector is NaN.
     * @param x Vector to inspect.
     * @return `true` if at least one component satisfies `std::isnan`.
     */
    inline bool is_nan(linalg::vector<double, Dim> const& x) const
    {
        for (const double& v : x) {
            if (std::isnan(v)) return true;
        }
        return false;
    }
};

}  // namespace root_multidim
#endif
