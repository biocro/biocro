#ifndef ZEROS_MULTIDIM_H
#define ZEROS_MULTIDIM_H

#include <array>
#include <algorithm>  // std::swap
#include <cmath>      // std::sqrt, std::isnan

#include "../../linalg/base.h"
#include "../../linalg/lu.h"

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
    result_t() = default;
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


template<size_t Dim>
struct QuasiNewtonState {
    using vec_t = typename linalg::vector<double, Dim>;
    vec_t x;  //  x
    vec_t dx; // dx
    vec_t y;  //  y
    vec_t dy; // dy
    Flag flag;
};

template<size_t Dim, typename Stepper>
struct QuasiNewton : Stepper {

    using vec_t = typename linalg::vector<double, Dim>;
    using mat_t = typename linalg::matrix<double, Dim, Dim>;
    QuasiNewton(size_t max_iter, double abs_tol, double rel_tol)
        : max_iterations{max_iter},
          _abs_tol{abs_tol},
          _rel_tol{rel_tol}
    {
    }

    QuasiNewton() = default;

    // --- configuration --------------------------------------------------------

    size_t max_iterations = 100;  ///< Maximum iterations before `Flag::max_iterations` is set.
    double _abs_tol = 1e-12;      ///< Absolute tolerance used to test for `f(x) == 0`.
    double _rel_tol = 1e-12;      ///< Relative tolerance used to test if `x == y`.

    // --- state ----------------------------------------------------------------

    Status status = Status::ok;  ///< Iteration status

    Stepper stepper;
    // LineSearch line_search;
    // Constraints constraints;

    QuasiNewtonState<Dim> state;

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
    template <typename F>
    result_t<Dim> solve(F&& fun, std::array<double, Dim> const& x0)
    {
        // `initialize` internal state; forward method-specific arguments
        // `initialize` checks if inputs satisfy requirements
        state.x = x0;
        state.y = fun(x0);
        if (this->is_zero(linalg::norm(state.y))) {
            state.flag = Flag::residual_zero;
            status = Status::converged;
        }
        if (status == Status::ok) {
            status = stepper.initialize(std::forward<F>(fun), state);
        }
        // iteration loop;
        // i counts the number of times `iterate` has been called
        for (size_t i = 0; i <= max_iterations; ++i) {
            if (status != Status::ok) {
                return make_result(i);
            }

            status = stepper.propose(std::forward<F>(fun), state);
            state.x += state.dx;
            vec_t y = fun(state.x.asarray());
            state.dy = y - state.y;
            state.y = y;
            status = stepper.update(state);

            // converged if f(x) == 0
            if (this->is_zero(linalg::norm(state.y))) {
                state.flag = Flag::residual_zero;
                status = Status::converged;
            }

            // converged if no improvement (maybe should be a failure condition?)
            if (this->is_zero(linalg::norm(state.dx))) {
                state.flag = Flag::delta_x_zero;
                status = Status::converged;
            }

        }
        state.flag = Flag::max_iterations;
        return make_result(max_iterations);
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
        result_t<Dim> out;
        out.zero = state.x.asarray();
        out.residual = state.y.asarray();
        out.iteration = i;
        out.flag = state.flag;
        out.success = status == Status::converged;
        return out;
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
    inline bool is_zero(vec_t const& y, vec_t const& x) const
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
    inline bool is_nan(vec_t const& x) const
    {
        for (const double& v : x) {
            if (std::isnan(v)) return true;
        }
        return false;
    }
};



template<size_t Dim>
struct NewtonStep {

    template<typename F>
    Status initialize(F&& fun, QuasiNewtonState<Dim>& state)
    {
        return Status::ok;
    }

    template <typename F>
    Status propose(F&& fun, QuasiNewtonState<Dim>& state)
    {
        linalg::LU<double, Dim> lu(fun.jacobian(state.x));
        auto sol = lu.solve(-1.0 * state.y);

        if (!sol) {
            state.flag = Flag::singular_matrix;
            return Status::failed;
        }
        state.dx = sol.value();
        return Status::ok;
    }

    Status update(QuasiNewtonState<Dim>& state)
    {
        return Status::ok;
    }

};


template<size_t Dim>
using Newton= typename QuasiNewton<Dim, NewtonStep<Dim>>;

template<size_t Dim>
struct BroydenStep {

    using vec_t = typename linalg::vector<double, Dim>;  ///< Dense matrix type.
    using mat_t = typename linalg::matrix<double, Dim, Dim>;  ///< Dense matrix type.
    mat_t inv_jac;

    template<typename F>
    Status initialize(F&& fun, QuasiNewtonState<Dim>& state) {
        vec_t dx(0);
        vec_t dy;
        vec_t y;
        vec_t fwd = state.x;
        double constexpr eps = 1e-8;
        inv_jac = linalg::matrix<double, Dim, Dim>::identity();
        for (size_t i = 0; i < Dim; ++i) {
            if (i > 0) {
                dx[i - 1] = 0.0;
                fwd[i - 1] = state.x[i - 1];
            }
            dx[i] = eps;
            fwd[i] += eps;
            y =  fun(fwd.asarray());
            dy = y - state.y;
            vec_t a = dx - inv_jac * dy;
            vec_t b = dx * inv_jac;
            double c = linalg::quadratic_form(inv_jac, dx, dy);
            inv_jac += linalg::outer(a, b) / c;
        }
        return Status::ok;
    }

    template <typename F>
    Status propose(F&& fun, QuasiNewtonState<Dim>& state)
    {
        state.dx = inv_jac * (-1.0 * state.y);
        return Status::ok;
    }

    Status update(QuasiNewtonState<Dim>& state)
    {
        vec_t a = state.dx - inv_jac * state.dy;
        vec_t b = state.dx * inv_jac;
        double c = linalg::quadratic_form(inv_jac, state.dx, state.dy);
        inv_jac += linalg::outer(a, b) / c;
        return Status::ok;
    }

};

template<size_t Dim>
using Broyden = QuasiNewton<Dim, BroydenStep<Dim>>;

}  // namespace root_multidim

/*
 * =============================================================================
 * DESIGN NOTES: Replacing CRTP with Policy-Based Design
 * =============================================================================
 *
 * The CRTP base class (`zero_finding_method`) forces step logic (Broyden,
 * Newton) and cross-cutting behaviours (constraints, line search, convergence)
 * into the same class hierarchy, making it awkward to mix them independently.
 * A policy-based design separates those axes cleanly.
 *
 * -----------------------------------------------------------------------------
 * POLICY AXES
 * -----------------------------------------------------------------------------
 *
 * Three natural independent dimensions:
 *
 *   template <size_t Dim,
 *             typename StepPolicy,                        // BroydenStep, NewtonStep
 *             typename ConstraintPolicy = NoConstraints,  // BoxConstraints, ...
 *             typename LineSearchPolicy = NoLineSearch>   // Backtracking, ...
 *   struct solver { ... };
 *
 * Each policy is a member object, not a base class. The `solver` owns the
 * solve loop directly — no CRTP base is needed.
 *
 * -----------------------------------------------------------------------------
 * WHAT EACH POLICY OWNS
 * -----------------------------------------------------------------------------
 *
 * StepPolicy — owns the step-direction computation and any associated state
 *   (e.g. `inv_jac` for Broyden):
 *
 *   void initialize(F& fun, vec_t& x, vec_t& residual);
 *   vec_t propose(vec_t const& residual);                  // returns proposed p
 *   void update(vec_t const& delta_x, vec_t const& delta_y); // post-step Jacobian update
 *
 * ConstraintPolicy — owns feasibility:
 *
 *   void project(vec_t& x) const;                         // clamp x into the feasible set
 *   double max_alpha(vec_t const& x, vec_t const& p) const; // largest α keeping x+α·p feasible
 *
 * LineSearchPolicy — owns step-length selection:
 *
 *   double find_alpha(F& fun, vec_t const& x, vec_t const& p,
 *                     vec_t const& residual) const;
 *
 * The solver loop sequences these:
 *
 *   p       = step.propose(residual)
 *   α_max   = constraints.max_alpha(x, p)
 *   α       = line_search.find_alpha(f, x, p, residual, α_max)
 *   delta_x = α * p
 *   x      += delta_x
 *   delta_y = f(x) - residual    (via evaluate + swap)
 *   step.update(delta_x, delta_y)   // Broyden uses actual step, not proposed
 *
 * The critical point: step.update receives delta_x after line search and
 * constraint capping, keeping the Broyden secant condition consistent with
 * the true step taken.
 *
 * -----------------------------------------------------------------------------
 * POLICY INTERACTION WITH NEWTON
 * -----------------------------------------------------------------------------
 *
 * Newton is the awkward case: it needs f.jacobian(x), while Broyden only
 * needs f(x). Options:
 *
 * 1. Newton takes a combined callable FJ with both operator() and jacobian().
 *    The solver loop calls fun(x) for all policies; the Newton step policy
 *    calls fun.jacobian(x) internally. Broyden ignores the jacobian method.
 *
 * 2. Newton is kept separate from this framework, since its interface
 *    requirement differs fundamentally. The policy framework covers only
 *    quasi-Newton and derivative-free methods.
 *
 * Option 2 is simpler and honest: Newton does not benefit from pluggable line
 * search or constraints in the same way since it recomputes the Jacobian every
 * iteration anyway.
 *
 * -----------------------------------------------------------------------------
 * STATELESS VS. STATEFUL POLICIES
 * -----------------------------------------------------------------------------
 *
 *   Policy                    Stateless (tag + static)   Stateful (member object)
 *   ------------------------  -------------------------  ------------------------
 *   NoConstraints             natural                    also works
 *   BoxConstraints            needs Dim-sized arrays     natural
 *   BacktrackingLineSearch    possible (template args)   natural (c, rho as members)
 *   BroydenStep               not possible (needs inv_jac) natural
 *
 * Stateful member objects are the uniform choice; stateless policies just
 * have trivial members.
 *
 * -----------------------------------------------------------------------------
 * CONFIGURATION INTERFACE
 * -----------------------------------------------------------------------------
 *
 * Preferred: member assignment (aggregate-style), consistent with how
 * tolerances are currently set:
 *
 *   solver<2, BroydenStep, BoxConstraints<2>> s;
 *   s.max_iterations = 100;
 *   s.constraints.lo = {0.0, -10.0};
 *   s.constraints.hi = {Ca,   80.0};
 *
 * Requires no constructor proliferation and matches the existing style
 * (_abs_tol, _rel_tol as public members).
 *
 * -----------------------------------------------------------------------------
 * CONVERGENCE
 * -----------------------------------------------------------------------------
 *
 * Currently `has_converged` is inside the derived class. With policies,
 * convergence checking is simple enough to live directly in the solver loop
 * using `is_zero` helpers — no separate policy is needed unless custom criteria
 * are anticipated (e.g. per-component tolerances).
 *
 * Three distinct outcomes to distinguish when constraints are active:
 *
 *   Situation                                    Meaning
 *   -------------------------------------------- ---------------------------
 *   ||f(x)|| ≈ 0, x interior                    True root found
 *   ||f(x)|| ≈ 0, x on boundary                 Root coincidentally on boundary
 *   ||f(x)|| > tol, x on boundary, step rejected Boundary stagnation
 *
 * The third case is not currently representable. A new Flag value
 * (boundary_stagnation) and a `feasible` field in result_t would make it
 * diagnosable.
 *
 * -----------------------------------------------------------------------------
 * LINE SEARCH
 * -----------------------------------------------------------------------------
 *
 * Merit function for root-finding: φ(α) = ½||f(x + α·p)||².
 *
 * Backtracking Armijo (sufficient-decrease condition):
 *   φ(α) ≤ φ(0) + c·α·∇φ(0)
 * where ∇φ(0) = f(x)ᵀ·J·p = -f(x)ᵀ·f(x)  (with Newton/Broyden direction p).
 * Start at α = 1, multiply by ρ ∈ (0,1) until satisfied.
 * One extra f evaluation per backtrack step.
 *
 * -----------------------------------------------------------------------------
 * BACKWARD COMPATIBILITY
 * -----------------------------------------------------------------------------
 *
 * Type aliases preserve existing call sites:
 *
 *   template <size_t Dim>
 *   using broyden = solver<Dim, BroydenStep>;
 *
 * result_t, Flag, and Status remain in zeros.h unchanged.
 * Flag::boundary_stagnation can be added here when constraints are implemented.
 *
 * -----------------------------------------------------------------------------
 * SUMMARY: WHAT GETS DELETED VS. ADDED
 * -----------------------------------------------------------------------------
 *
 *   Component               CRTP design              Policy design
 *   ----------------------  -----------------------  --------------------------
 *   zero_finding_method     CRTP base with solve()   Deleted; loop → solver
 *   broyden                 Derives from base        BroydenStep + alias
 *   newton                  Derives from base        Kept separate
 *   Constraints             Not present              NoConstraints, BoxConstraints<Dim>
 *   Line search             Not present              NoLineSearch, BacktrackingLineSearch
 *   zeros.h                 result_t, Flag, Status,  result_t, Flag, Status only
 *                           base class
 * =============================================================================
 */

#endif
