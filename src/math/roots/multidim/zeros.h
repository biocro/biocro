#ifndef ZEROS_MULTIDIM_H
#define ZEROS_MULTIDIM_H

#include <algorithm>  // std::min
#include <array>
#include <cmath>    // std::sqrt, std::isfinite
#include <sstream>  // std::ostringstream
#include <string>

#include "../../linalg/base.h"
#include "../../linalg/lu.h"
// #include "common.h"
// #include "stepper.h"

namespace root_multidim
{

/**
 * @brief Unified termination status for zero-finding solvers.
 *
 * `Status::ok` is the only non-terminal state. All other values are terminal
 * and encode both the fact that iteration has ended and the reason why. Use
 * `is_terminal()` to test for any terminal state, and `is_success()` to
 * distinguish convergence from failure.
 */
enum class Status {
    // --- continuing ---
    ok,
    // --- success (terminal) ---
    residual_zero,  ///< ||f(x)|| < tolerance
    // --- failure (terminal) ---
    stagnated,  ///< ||dx|| < tolerance; step stagnated
    boundary,
    max_iterations,         ///< iteration limit reached without convergence
    zero_is_nonfinite,      ///< x contains NaN or Inf
    function_is_nonfinite,  ///< f(x) contains NaN or Inf
    singular_matrix,        ///< Jacobian is singular; no valid step exists
};

/// Returns `true` for any terminal Status (success or failure).
constexpr bool is_terminal(Status s) { return s != Status::ok; }

/// Returns `true` only for successful terminal states.
constexpr bool is_success(Status s)
{
    return s == Status::residual_zero;
}

/**
 * @brief Holds the outcome of a zero-finding solve.
 *
 * @tparam Dim Dimension of the system (number of equations = unknowns).
 */
template <size_t Dim>
struct result_t {
    std::array<double, Dim> zero;      ///< Final estimate of the root.
    std::array<double, Dim> residual;  ///< f(zero) at termination.
    size_t iteration;                  ///< Number of iterations performed.
    Status status;                     ///< Reason iteration stopped.
    bool success;                      ///< True iff status is a success code.
    double residual_norm = 0;
    double residual_norm_inf = 0;

    result_t() = default;
    result_t(
        std::array<double, Dim> const& x,
        std::array<double, Dim> const& y,
        size_t i,
        Status s)
        : zero{x}, residual{y}, iteration{i}, status{s}, success{is_success(s)}
    {
        for (size_t i = 0; i < Dim; ++i) {
            residual_norm += residual[i] * residual[i];
            double a = std::abs(residual[i]);
            if (a > residual_norm_inf)
                residual_norm_inf = a;
        }
        residual_norm = std::sqrt(residual_norm);
    }

    /// Returns a string describing the solver outcome and all result fields.
    std::string status_message(bool verbose = false) const
    {
        std::ostringstream oss;
        switch (status) {
            case Status::ok:
                oss << "ok: iteration continuing";
                break;
            case Status::residual_zero:
                oss << "converged: ||f(x)|| < tolerance";
                break;
            case Status::stagnated:
                oss << "stagnated: ||dx|| < tolerance";
                break;
            case Status::boundary:
                oss << "no feasible step";
                break;
            case Status::max_iterations:
                oss << "failed: maximum iterations reached";
                break;
            case Status::zero_is_nonfinite:
                oss << "failed: x contains NaN or Inf";
                break;
            case Status::function_is_nonfinite:
                oss << "failed: f(x) contains NaN or Inf";
                break;
            case Status::singular_matrix:
                oss << "failed: Jacobian is singular";
                break;
            default:
                oss << "unknown status";
                break;
        }
        if (verbose) {
            oss << "\n  iteration      = " << iteration;
            oss << "\n  success        = " << (success ? "true" : "false");
            oss << "\n  residual_norm  = " << residual_norm;
            oss << "\n  residual_norm_inf = " << residual_norm_inf;
            oss << "\n  zero     = [";
            for (size_t i = 0; i < Dim; ++i) {
                if (i > 0) oss << ", ";
                oss << zero[i];
            }
            oss << "]\n  residual = [";
            for (size_t i = 0; i < Dim; ++i) {
                if (i > 0) oss << ", ";
                oss << residual[i];
            }
            oss << "]";
        }
        return oss.str();
    }
};

// ============================================================================
// Default policies
// ============================================================================

/**
 * @brief No-op constraint policy: imposes no feasibility requirements.
 *
 * `max_step_size` always returns 1.0; `project` is a no-op. Both are trivial
 * inlines that the compiler eliminates entirely when this is the active
 * `ConstraintPolicy`.
 */
struct NoConstraints {
    static constexpr bool is_active = false;
};

/**
 * @brief No-op line-search policy: always takes the full quasi-Newton step.
 *
 * `find_alpha` returns 1.0 without evaluating `fun`. The compiler eliminates
 * all calls when this is the active `LineSearchPolicy`.
 */
struct NoLineSearch {
    static constexpr bool is_active = false;
};

template <size_t Dim>
struct BoxConstraints {
    static constexpr bool is_active = true;

    std::array<double, Dim> lower;
    std::array<double, Dim> upper;

    double max_step_size(
        linalg::vector<double, Dim> const& x,
        linalg::vector<double, Dim> const& dx) const
    {
        double step_size = 1.0;
        for (size_t i = 0; i < Dim; ++i) {
            if (dx[i] < 0) {
                step_size = std::min(step_size, (x[i] - lower[i]) / -dx[i]);
            }

            if (dx[i] > 0) {
                step_size = std::min(step_size, (upper[i] - x[i]) / dx[i]);
            }
        }
        return step_size;
    }

    void project(linalg::vector<double, Dim>& x) const
    {
        for (size_t i = 0; i < Dim; ++i) {
            x[i] = std::clamp(x[i], lower[i], upper[i]);
        }
    }
};

/**
 * @brief Armijo backtracking line search.
 *
 * Shrinks the step length by factor `rho` until the sufficient-decrease
 * (Armijo) condition is satisfied:
 *
 *   φ(α) ≤ φ(0) · (1 − 2·c·α)
 *
 * where φ(α) = ‖f(x + α·p)‖². The gradient of φ at α = 0 along the
 * quasi-Newton direction p equals −‖f(x)‖², so the condition is exact for
 * Newton steps and approximate for Broyden steps.
 *
 * **Members** (configure before calling `solve`):
 * - `c` — sufficient-decrease constant, `c ∈ (0, 0.5)`. Default: 1e-4.
 * - `rho` — step reduction factor per backtrack. Default: 0.5.
 * - `max_backtracks` — iteration cap; returns the last `alpha` if never
 *   satisfied. Default: 50.
 */
struct BacktrackingLineSearch {
    static constexpr bool is_active = true;
    double c = 1e-4;
    double rho = 0.5;
    size_t max_backtracks = 5;

    template <typename F, size_t Dim>
    double find_alpha(
        F& fun,
        linalg::vector<double, Dim> const& x,
        linalg::vector<double, Dim> const& dx,
        linalg::vector<double, Dim> const& y,
        double max_step_size) const
    {
        double const phi0 = linalg::dot(y, y);
        double alpha = max_step_size;
        for (size_t k = 0; k < max_backtracks; ++k) {
            linalg::vector<double, Dim> x_trial = x + alpha * dx;
            linalg::vector<double, Dim> y_trial = fun(x_trial.asarray());
            double const phi = linalg::dot(y_trial, y_trial);
            if (phi <= phi0 * (1.0 - 2.0 * c * alpha)) {
                return alpha;
            }
            alpha *= rho;
        }

        // linalg::vector<double, Dim> x_trial = x + alpha * dx;
        // linalg::vector<double, Dim> y_trial = fun(x_trial.asarray());
        // double const phi = linalg::dot(y_trial, y_trial);
        // std::cout << " max depth in line search " << alpha << "  " << phi / phi0 << '\n';
        return alpha;
    }
};

// struct CubicLineSearch {
//     static constexpr bool is_active = true;
//     double c = 1e-4;
//     size_t max_backtracks = 10;

//     template <typename F, size_t Dim>
//     double find_alpha(
//         F& fun,
//         linalg::vector<double, Dim> const& x,
//         linalg::vector<double, Dim> const& dx,
//         linalg::vector<double, Dim> const& y,
//         double max_step_size) const
//     {
//         double const phi0 = linalg::dot(y, y);
//         // coefficients of a cubic
//         std::array<double, 4> coef;
//         coef[0] = phi0;
//         // step direction is chosen so that d_phi0 =
//         coef[1] = -2.0 * phi0;

//         // perform one first step
//         double alpha = max_step_size;

//         double step;

//         // perform cubic

//         for (size_t k = 0; k < max_backtracks; ++k) {
//             linalg::vector<double, Dim> x_trial = x + alpha * dx;
//             linalg::vector<double, Dim> y_trial = fun(x_trial.asarray());
//             double phi = linalg::dot(y_trial, y_trial);

//             if (armijo_rule(phi, phi0, alpha)) {
//                 return alpha;
//             }

//             if (k == 0) {
//                 // compute quadratic coef
//                 coef[2] = phi - coef[0] - coef[1];
//                 // minimizer of quadratic with guard to ensure alpha >= 0.1
//                 alpha = std::min(-coef[1] / (2.0 * coef[2]), 0.1 * alpha);

//             } else {
//             }
//         }
//         return alpha;
//     }

//    private:
//     inline bool armijo_rule(double phi, double phi0, double alpha)
//     {
//         return phi <= phi0 * (1.0 - 2.0 * c * alpha);
//     }
// };

// ============================================================================
// QuasiNewton
// ============================================================================

/**
 * @brief Quasi-Newton root-finding solver with pluggable step, constraint,
 * and line-search policies.
 *
 * The solver owns the iteration state (`x`, `dx`, `y`, `dy`) directly as
 * members. Policies are stateful member objects configured before calling
 * `solve`. Policy types default to `NoConstraints` and `NoLineSearch`; the
 * compiler eliminates their calls entirely for the default case.
 *
 * **Stepper interface** — `Stepper` must provide:
 * @code
 *   Status initialize(F& fun, vec_t const& x, vec_t const& y);
 *   Status propose   (F& fun, vec_t const& x, vec_t const& y, vec_t& dx);
 *   Status update    (vec_t const& dx, vec_t const& dy);
 * @endcode
 * `propose` writes the search direction into `dx` and returns `Status::ok`,
 * or returns a terminal failure code without writing `dx`. `update` returns
 * `Status::ok`; degenerate updates are skipped silently.
 *
 * @tparam Dim              Dimension of the system.
 * @tparam Stepper          Step-direction policy (`BroydenStep`, `NewtonStep`).
 * @tparam ConstraintPolicy Feasibility policy; default `NoConstraints`.
 * @tparam LineSearchPolicy Step-length policy; default `NoLineSearch`.
 */
template <size_t Dim,
          typename Stepper,
          typename Constraint = NoConstraints,
          typename LineSearch = NoLineSearch>
struct QuasiNewton {
    using vec_t = linalg::vector<double, Dim>;

    // --- policies ---
    Stepper stepper;
    Constraint constraints;
    LineSearch line_search;

    // --- configuration ---
    size_t max_iterations = 100;
    double abs_tol = 1e-8;
    double rel_tol = 1e-8;
    double xtol = 1e-12;

    size_t max_stagnant_iterations = 5;

    // --- iteration state (valid only during solve) ---
    vec_t x;   ///< Current estimate of the root.
    vec_t dx;  ///< Step taken in the last iteration.
    vec_t y;   ///< Residual f(x) at the current estimate.
    vec_t dy;  ///< Change in residual across the last step.

    double y_norm;
    double initial_y_norm;

    QuasiNewton() = default;
    QuasiNewton(size_t max_iter, double abs_tol_, double rel_tol_, double xtol_)
        : max_iterations{max_iter}, abs_tol{abs_tol_}, rel_tol{rel_tol_}, xtol{xtol_}
    {
    }

    /**
     * @brief Find a zero of `fun` starting from `x0`.
     *
     * @tparam F Callable: `std::array<double,Dim>(std::array<double,Dim>)`.
     *           `NewtonStep` additionally requires `fun.jacobian(x)`.
     * @param fun The function whose zero is sought.
     * @param x0  Initial guess; projected onto the feasible set on entry.
     */
    template <typename F>
    result_t<Dim> solve(F&& fun, std::array<double, Dim> const& x0)
    {
        size_t stagnant_iterations = 0;
        x = x0;
        if constexpr (Constraint::is_active)
            constraints.project(x);
        y = fun(x.asarray());

        if (is_nonfinite(x)) return make_result(0, Status::zero_is_nonfinite);
        if (is_nonfinite(y)) return make_result(0, Status::function_is_nonfinite);
        y_norm = norm_inf(y);
        initial_y_norm = y_norm;
        if (y_norm < abs_tol) return make_result(0, Status::residual_zero);

        Status s = stepper.initialize(fun, x, y);
        if (is_terminal(s)) return make_result(0, s);

        for (size_t i = 1; i <= max_iterations; ++i) {
            s = stepper.propose(fun, x, y, dx);
            if (is_terminal(s)) return make_result(i, s);

            double step_size = 1.0;
            if constexpr (Constraint::is_active) {
                step_size = constraints.max_step_size(x, dx);
                // The proposed direction points entirely outside the feasible
                // region. A Broyden reset at the same boundary point would face
                // the same constraint, so terminate rather than retry.
                // if (step_size == 0.0) {
                //     return make_result(i, Status::boundary);
                // }
            }

            if constexpr (LineSearch::is_active) {
                step_size = line_search.find_alpha(fun, x, dx, y, step_size);
            }

            dx *= step_size;
            x += dx;

            if constexpr (Constraint::is_active)
                constraints.project(x);

            if (is_nonfinite(x)) return make_result(i, Status::zero_is_nonfinite);

            vec_t y_new = fun(x.asarray());
            if (is_nonfinite(y_new)) return make_result(i, Status::function_is_nonfinite);

            dy = y_new - y;
            y = y_new;
            y_norm = norm_inf(y);
            stepper.update(dx, dy);
            // convergence check
            if (y_norm < abs_tol + rel_tol * initial_y_norm)
                return make_result(i, Status::residual_zero);

            // Stagnation check: if dx is zero for too many consecutive iterations,
            // then terminate early.
            if (scaled_norm_inf(dx, x) < xtol) {
                if (stagnant_iterations >= max_stagnant_iterations)
                    return make_result(i, Status::stagnated);
                ++stagnant_iterations;
            } else {
                stagnant_iterations = 0;
            }
        }
        return make_result(max_iterations, Status::max_iterations);
    }

    template <typename F>
    result_t<Dim> operator()(F&& fun, std::array<double, Dim> const& x0)
    {
        return solve(std::forward<F>(fun), x0);
    }

   private:
    result_t<Dim> make_result(size_t i, Status s)
    {
        return result_t<Dim>(x.asarray(), y.asarray(), i, s);
    }

    bool is_nonfinite(vec_t const& v) const
    {
        for (double d : v)
            if (!std::isfinite(d)) return true;
        return false;
    }

    double norm_inf(vec_t const& v) const
    {
        double a = 0.0;
        for (double vi : v) {
            double y = std::abs(vi);
            if (y > a)
                a = y;
        }
        return a;
    }

    double scaled_norm_inf(vec_t const& dx, vec_t const& x) const
    {
        double a = 0.0;
        for (size_t i = 0; i < Dim; ++i) {
            double y = std::abs(dx[i]) / std::max(1.0, std::abs(x[i]));
            if (y > a)
                a = y;
        }
        return a;
    }

    bool is_zero(vec_t const& v) const
    {
        return linalg::norm(v) < abs_tol;
    }

    bool is_zero(vec_t const& y_vec, vec_t const& x_vec) const
    {
        double yn = linalg::norm(y_vec);
        double xn = linalg::norm(x_vec);
        return yn < abs_tol + rel_tol * xn;
    }
};

// ============================================================================
// Steppers
// ============================================================================

/**
 * @brief Newton step policy.
 *
 * Requires `fun.jacobian(x)` returning a `linalg::matrix<double, Dim, Dim>`.
 * Recomputes the Jacobian and solves the linear system at every step.
 * Returns `Status::singular_matrix` when LU decomposition fails.
 * `initialize` and `update` are no-ops.
 */
template <size_t Dim>
struct NewtonStep {
    using vec_t = linalg::vector<double, Dim>;

    template <typename F>
    Status initialize(F&, vec_t const&, vec_t const&)
    {
        return Status::ok;
    }

    template <typename F>
    Status propose(F& fun, vec_t const& x, vec_t const& y, vec_t& dx)
    {
        linalg::LU<double, Dim> lu(fun.jacobian(x));
        auto sol = lu.solve(-1.0 * y);
        if (!sol) return Status::singular_matrix;
        dx = sol.value();
        return Status::ok;
    }

    Status update(vec_t const&, vec_t const&) { return Status::ok; }
};

/**
 * @brief Broyden "good" step policy.
 *
 * Maintains a running rank-1 approximation to the inverse Jacobian,
 * initialised via finite-difference probes along each coordinate axis.
 * Rank-1 updates whose denominator falls below `1e-14` are skipped silently
 * to prevent `inv_jac` from being corrupted near degenerate steps.
 */
template <size_t Dim>
struct BroydenStep {
    using vec_t = linalg::vector<double, Dim>;
    using mat_t = linalg::matrix<double, Dim, Dim>;

    mat_t inv_jac;
    size_t max_degenerate_updates = 3;
    size_t _degenerate_updates = 0;

    template <typename F>
    Status initialize(F& fun, vec_t const& x0, vec_t const& y0)
    {
        double constexpr eps = 1e-8;
        inv_jac = mat_t::identity();
        vec_t probe_dx(0.0);
        vec_t fwd = x0;

        for (size_t i = 0; i < Dim; ++i) {
            if (i > 0) {
                fwd[i - 1] = x0[i - 1];
                probe_dx[i - 1] = 0;
            }
            fwd[i] += eps;
            probe_dx[i] = eps;

            vec_t y_fwd = fun(fwd.asarray());
            vec_t dy = y_fwd - y0;
            _rank1_update(probe_dx, dy);
        }
        _degenerate_updates = 0;
        return Status::ok;
    }

    template <typename F>
    Status propose(F& fun, vec_t const& x, vec_t const& y, vec_t& dx)
    {
        if (_degenerate_updates >= max_degenerate_updates) {
            Status s = initialize(fun, x, y);  // FD reset
            if (is_terminal(s)) return s;
        }
        dx = inv_jac * (-1.0 * y);
        return Status::ok;
    }

    Status update(vec_t const& dx, vec_t const& dy)
    {
        _rank1_update(dx, dy);
        return Status::ok;
    }

   private:
    void _rank1_update(vec_t const& dx, vec_t const& dy)
    {
        double c = linalg::quadratic_form(inv_jac, dx, dy);
        if (std::abs(c) < 1e-14) {
            ++_degenerate_updates;
            return;
        }
        vec_t a = dx - inv_jac * dy;
        vec_t b = dx * inv_jac;
        inv_jac += linalg::outer(a, b) / c;
    }
};

// ============================================================================
// Type aliases
// ============================================================================

template <size_t Dim>
using Newton = QuasiNewton<Dim, NewtonStep<Dim>>;

template <size_t Dim>
using Broyden = QuasiNewton<Dim, BroydenStep<Dim>>;

template <size_t Dim>
using SafeBroyden = QuasiNewton<Dim, BroydenStep<Dim>, BoxConstraints<Dim>, BacktrackingLineSearch>;

}  // namespace root_multidim

#endif
