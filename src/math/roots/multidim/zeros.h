#ifndef ZEROS_MULTIDIM_H
#define ZEROS_MULTIDIM_H

#include <array>
#include <algorithm>  // std::swap
#include <cmath>      // std::sqrt, std::isnan
#include <sstream>
#include "../../linalg/base.h"
#include "../../linalg/lu.h"
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
class Status
{
   public:
    enum Flag {
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

    Status() : flag{Flag::ok} {}
    Status(Flag f) : flag{f} {}

    void set(Flag f)
    {
        flag = f;
    }

    /// Returns `true` only for successful terminal states.
    constexpr bool is_success() const
    {
        return flag == Flag::residual_zero;
    }

    /// Returns `true` for any terminal Status (success or failure).
    constexpr bool is_ok() const
    {
        return flag == Flag::ok;
    }

    /// Returns `true` for any terminal Status (success or failure).
    constexpr bool is_terminal() const
    {
        return !is_ok();
    }

    std::string str() const
    {
        std::ostringstream oss;
        switch (flag) {
            case Flag::ok:
                oss << "ok: iteration continuing";
                break;
            case Flag::residual_zero:
                oss << "converged: ||f(x)|| < tolerance";
                break;
            case Flag::stagnated:
                oss << "stagnated: ||dx|| < tolerance";
                break;
            case Flag::boundary:
                oss << "no feasible step";
                break;
            case Flag::max_iterations:
                oss << "failed: maximum iterations reached";
                break;
            case Flag::zero_is_nonfinite:
                oss << "failed: x contains NaN or Inf";
                break;
            case Flag::function_is_nonfinite:
                oss << "failed: f(x) contains NaN or Inf";
                break;
            case Flag::singular_matrix:
                oss << "failed: Jacobian is singular";
                break;
            default:
                oss << "unknown status";
                break;
        }
        return oss.str();
    }

   private:
    Flag flag;
};

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
    double residual_norm = 0;

    result_t() = default;
    result_t(
        std::array<double, Dim> const& x,
        std::array<double, Dim> const& y,
        size_t i,
        Status s)
        : zero{x}, residual{y}, iteration{i}, status{s}
    {
        for (double r : residual)
            residual_norm += r * r;
        residual_norm = std::sqrt(residual_norm);
    }

    /// Returns a string describing the solver outcome and all result fields.
    std::string status_message(bool verbose = false) const
    {
        std::ostringstream oss;
        oss << status.str();
        if (verbose) {
            oss << "\n  iteration      = " << iteration;
            oss << "\n  success        = " << (is_success() ? "true" : "false");
            oss << "\n  residual_norm  = " << residual_norm;
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

    bool is_success() const
    {
        return status.is_success();
    }
};

// ---------------------------------------------------------------------------
// detail: shared implementation helpers
// ---------------------------------------------------------------------------
namespace detail
{
/// Fills `jac` column-by-column using forward finite differences.
template <size_t N, typename F>
void fd_jacobian(
    linalg::matrix<double, N, N>& jac,
    F& fun,
    linalg::vector<double, N> const& x,
    linalg::vector<double, N> const& y)
{
    using vec_t = linalg::vector<double, N>;
    vec_t f1;
    for (size_t i = 0; i < N; ++i) {
        vec_t x1 = x;
        double const eps = 1e-8 * std::max(1.0, std::abs(x[i]));
        x1[i] += eps;
        f1 = fun(x1.asarray());
        for (size_t j = 0; j < N; ++j) {
            jac(j, i) = (f1[j] - y[j]) / eps;
        }
    }
}
}  // namespace detail

// ---------------------------------------------------------------------------
// Jacobian strategies — satisfy the Jacobian concept:
//
//   void initialize(F& fun, vec_t const& x, vec_t const& y)
//   void update(F& fun, vec_t const& x, vec_t const& y,
//               vec_t const& dx, vec_t const& dy)
//   mat_t jac   (public member read by NewtonMethod)
// ---------------------------------------------------------------------------

/// Jacobian supplied analytically by the function object (`fun.jacobian(x)`).
template <size_t N>
struct Exact {
    using vec_t = linalg::vector<double, N>;
    using mat_t = linalg::matrix<double, N, N>;

    mat_t jac;

    template <typename F>
    void initialize(F& fun, vec_t const& x, vec_t const&)
    {
        jac = fun.jacobian(x.asarray());
    }

    template <typename F>
    void update(F& fun, vec_t const& x, vec_t const&, vec_t const&, vec_t const&)
    {
        jac = fun.jacobian(x.asarray());
    }
};

/// Jacobian approximated by forward finite differences at every iteration.
template <size_t N>
struct ForwardDiff {
    using vec_t = linalg::vector<double, N>;
    using mat_t = linalg::matrix<double, N, N>;

    mat_t jac;

    template <typename F>
    void initialize(F& fun, vec_t const& x, vec_t const& y)
    {
        detail::fd_jacobian(jac, fun, x, y);
    }

    template <typename F>
    void update(F& fun, vec_t const& x, vec_t const& y, vec_t const&, vec_t const&)
    {
        detail::fd_jacobian(jac, fun, x, y);
    }
};

/**
 * @brief Broyden rank-1 Jacobian update (Broyden's "good" method).
 *
 * Initialises with a forward-difference Jacobian, then maintains the
 * approximation via the secant condition:
 * @f[
 *   J_{\text{new}} = J + \frac{(\Delta y - J\,\Delta x)\,\Delta x^T}
 *                             {\|\Delta x\|^2}
 * @f]
 */
template <size_t N>
struct BroydenJacobian {
    using vec_t = linalg::vector<double, N>;
    using mat_t = linalg::matrix<double, N, N>;

    mat_t jac;

    template <typename F>
    void initialize(F& fun, vec_t const& x, vec_t const& y)
    {
        detail::fd_jacobian(jac, fun, x, y);
    }

    template <typename F>
    void update(F&, vec_t const&, vec_t const&, vec_t const& dx, vec_t const& dy)
    {
        double dx_sq = linalg::dot(dx, dx);
        if (dx_sq < 1e-30) return;
        vec_t r = dy - jac * dx;
        jac += linalg::outer(r, dx) / dx_sq;
    }
};

// ---------------------------------------------------------------------------
// NewtonMethod — solve loop parameterised by Jacobian strategy
// ---------------------------------------------------------------------------

/**
 * @brief Newton-type solver parameterised by a Jacobian strategy.
 *
 * At each iteration:
 *  1. Solves @f$ J\,\delta x = -f(x) @f$ via LU factorisation.
 *  2. Updates @f$ x \leftarrow x + \delta x @f$.
 *  3. Calls `Jacobian::update` with the new @f$(x, f(x), \delta x, \delta f)@f$.
 *
 * Three aliases are provided for common strategies:
 * @code
 *   Newton<N>   — exact Jacobian supplied by the function object
 *   NewtonFD<N> — Jacobian re-computed by forward differences each iteration
 *   Broyden<N>  — Jacobian maintained by the rank-1 Broyden update
 * @endcode
 *
 * @tparam N        Dimension of the system.
 * @tparam Jacobian A type satisfying the Jacobian concept (see above).
 */
template <size_t N, typename Jacobian>
struct NewtonMethod {
    using vec_t = linalg::vector<double, N>;
    using mat_t = linalg::matrix<double, N, N>;

    NewtonMethod(size_t max_iter, double abs_tol, double rel_tol)
        : max_iter{max_iter}, atol{abs_tol}, rtol{rel_tol}
    {
    }
    NewtonMethod() = default;

    size_t max_iter = 100;
    double atol = 1e-10;
    double rtol = 1e-10;

    vec_t x, y, dx, dy;
    Jacobian jacobian;
    double y_norm;
    double y_norm0;

    template <typename F>
    result_t<N> solve(F&& fun, std::array<double, N> const& x0)
    {
        x = x0;
        evaluate(fun);
        y_norm0 = y_norm;
        if (residual_is_zero())
            return make_result(0, Status::Flag::residual_zero);

        jacobian.initialize(fun, x, y);

        for (size_t iter = 1; iter <= max_iter; ++iter) {
            linalg::LU<double, N> lu(jacobian.jac);
            auto sol = lu.solve(-1.0 * y);
            if (!sol) {
                // Jacobian is singular: apply Tikhonov regularisation to a
                // temporary copy so the stored Jacobian is not corrupted.
                mat_t jac_reg = jacobian.jac;
                for (size_t i = 0; i < N; ++i)
                    jac_reg(i, i) += 1e-3;
                linalg::LU<double, N> lu2(jac_reg);
                sol = lu2.solve(-1.0 * y);
                if (!sol)
                    return make_result(iter, Status::Flag::singular_matrix);
            }

            dx = sol.value();
            x += dx;

            dy = y;
            evaluate(fun);
            dy = y - dy;

            jacobian.update(fun, x, y, dx, dy);

            if (residual_is_zero())
                return make_result(iter, Status::Flag::residual_zero);
        }

        return make_result(max_iter, Status::Flag::max_iterations);
    }

    template <typename F>
    inline result_t<N> operator()(F&& fun, std::array<double, N> const& x0)
    {
        return solve(std::forward<F>(fun), x0);
    }

   private:
    result_t<N> make_result(size_t i, Status s)
    {
        return {x.asarray(), y.asarray(), i, s};
    }

    template <typename F>
    void evaluate(F& fun)
    {
        y = fun(x.asarray());
        y_norm = linalg::norm(y);
    }

    bool residual_is_zero() const
    {
        return y_norm < atol + rtol * y_norm0;
    }
};

template <size_t N>
using Newton = NewtonMethod<N, Exact<N>>;

template <size_t N>
using NewtonFD = NewtonMethod<N, ForwardDiff<N>>;

template <size_t N>
using Broyden = NewtonMethod<N, BroydenJacobian<N>>;

}  // namespace root_multidim
#endif
