#ifndef ROOT_BROYDEN_H
#define ROOT_BROYDEN_H

#include "zeros.h"
#include "../../linalg/base.h"
#include "../../linalg/lu.h"

namespace root_multidim
{

/**
 * @brief Tag passed to `broyden::solve` to request finite-difference
 * Jacobian initialization instead of the default identity matrix.
 *
 * Pass an instance of this struct as the third argument to `solve`:
 * @code
 * broyden<2> solver(100, 1e-8, 1e-8);
 * auto result = solver.solve(f, guess, root_multidim::fd_jacobian_t{});
 * // or with a custom step size:
 * auto result = solver.solve(f, guess, root_multidim::fd_jacobian_t{1e-4});
 * @endcode
 *
 * The Jacobian is approximated by forward differences at the initial guess.
 * Its inverse is computed via LU decomposition. If the Jacobian is singular
 * at the initial guess the solver falls back to the identity matrix.
 */
struct fd_jacobian_t {
    double h = 1e-5;  //!< Forward-difference step size
};

/// Convenience default-constructed tag value.
inline constexpr fd_jacobian_t fd_jacobian{};

/**
 * @brief Broyden's method for finding vector-valued zeros of nonlinear systems.
 *
 * This struct implements Broyden's "good" method — a quasi-Newton root-finding
 * algorithm that approximates the Jacobian inverse iteratively, avoiding the
 * cost of computing or factoring a full Jacobian at every step.
 *
 * @tparam Dim The dimension of the system (number of equations = number of unknowns).
 *
 * @par Usage Example
 * @code
 * // Define the system: find x such that f(x) = 0
 * auto f = [](std::array<double, 2> x) -> std::array<double, 2> {
 *     return { x[0]*x[0] + x[1] - 1.0,
 *              x[0]      - x[1]*x[1] };
 * };
 * using namespace root_multidim;
 * // set solver with a max iterations, abs_tol, rel_rol
 * broyden<2> solver(100, 1e-5, 1e-5);
 * std::array<double, 2> guess = { 0.5, 0.5 };
 *
 * result_t result = solver.solve(f, guess);
 *
 * @endcode
 *
 * @note The inverse Jacobian is initialized to the identity matrix, so the
 *       first step is equivalent to a Newton step with @f$ J = I @f$.
 *       Convergence may be slow if the true Jacobian at the starting point
 *       differs greatly from the identity.
 */
template <size_t Dim>
struct broyden : public zero_finding_method<Dim, broyden<Dim>> {
    // zero_finding_method methods can call private methods defined here
    friend class zero_finding_method<Dim, broyden<Dim>>::zero_finding_method;

    // "import" parent class methods
    using zero_finding_method<Dim, broyden<Dim>>::zero_finding_method;

   private:
    // --- type aliases -------------------------------------------------------
    using vec_t = typename linalg::vector<double, Dim>;       ///< Dense vector type.
    using mat_t = typename linalg::matrix<double, Dim, Dim>;  ///< Dense matrix type.

    // --- state -----------------------------------------------------------------

    vec_t _zero;      ///< Current best estimate of the zero.
    vec_t _residual;  ///< Residual f(_zero) at the current estimate.
    vec_t delta_x;    ///< Last step taken in x-space.
    vec_t delta_y;    ///< Change in residual across the last step.
    mat_t inv_jac;    ///< Running approximation of the inverse Jacobian.

    // --- internal temporaries (avoid per-iteration allocation) ----------------

    vec_t _tmp_a;   ///< @private Numerator row  in the rank-1 update.
    vec_t _tmp_b;   ///< @private Numerator col  in the rank-1 update.
    double _tmp_c;  ///< @private Denominator scalar in the rank-1 update.

    template <typename F>
    Status initialize(F&& fun, std::array<double, Dim> const& guess)
    {
        _zero     = guess;
        _residual = fun(guess);
        inv_jac   = mat_t::identity();
        return Status::ok;
    }

    /**
     * @brief Initialise with a finite-difference inverse Jacobian.
     *
     * Builds the Jacobian by forward differences (one additional function
     * evaluation per dimension), then inverts it via LU decomposition.
     * Falls back to the identity matrix if the Jacobian is singular at
     * the initial guess.
     */
    template <typename F>
    Status initialize(F&& fun, std::array<double, Dim> const& guess, fd_jacobian_t fd)
    {
        _zero     = guess;
        _residual = fun(guess);
        inv_jac   = _fd_inv_jacobian(std::forward<F>(fun), fd.h);
        return Status::ok;
    }

    /**
     * @brief Performs one Broyden iteration.
     *
     * Advances the current estimate by one quasi-Newton step and updates the
     * inverse-Jacobian approximation via the rank-1 Broyden formula:
     *
     * @f[
     *   J^{-1}_{\text{new}} = J^{-1} +
     *       \frac{(\Delta x - J^{-1}\,\Delta y)\,\Delta x^T J^{-1}}
     *            {\Delta x^T J^{-1}\,\Delta y}
     * @f]
     *
     * Call has_converged() after each call to decide whether to continue.
     *
     * @tparam F  Callable with the same signature as in initialize().
     * @param fun The function whose zero is sought (same object as passed to initialize()).
     * @return Always `true` (reserved for future error reporting).
     */
    template <typename F>
    Status iterate(F&& fun)
    {
        /*
        UPDATE FORMULA
            x -> x + delta_x ;
            where
                delta_x = -inv_jac(y);
                y = fun(x);
                delta_y = fun(x + delta_x) - fun(x);
            inv_jac ->  inv_jac + outer(a, b) / c
            where
                a = delta_x - dot(inv_jac , delta_y)
                b = dot(delta_x, inv_jac)
                c = dot(delta_x, dot(inv_jac, delta_y))

            `update_x` does
                delta_x = dot(inv_jac, -y);
                x += delta_x;

            need to save current/old `residual` to compute new `delta_y`
            don't need `delta_y` so overwrite it with new `residual`
            delta_y = fun(_zero)
            `update_y`
                std::swap(_residual, delta_y);
                // _residual is now new `residual`
                // delta_y is now old `residual`
                delta_y = _residual - y_old

            Could check convergence now

            `update_inv_jac` computes
                a = delta_x - dot(inv_jac , delta_y)
                b = dot(delta_x, inv_jac)
                c = dot(delta_x, dot(inv_jac, delta_y))

        */
        update_x();
        delta_y = fun(_zero.asarray());
        update_y();
        update_inv_jac();
        return Status::ok;
    }

    Status has_converged()
    {
        // converged if f(x) == 0
        if (this->is_zero(_residual, _zero)) {
            this->flag = Flag::residual_zero;
            return Status::converged;
        }

        // converged if no improvement (maybe should be a failure condition?)
        if (this->is_zero(delta_x, _zero)) {
            this->flag = Flag::delta_x_zero;
            return Status::converged;
        }

        return Status::ok;
    }

    std::array<double, Dim> residual() const
    {
        return _residual.asarray();
    }

    std::array<double, Dim> zero() const
    {
        return _zero.asarray();
    }

    inline void update_x()
    {
        delta_x = inv_jac * (-1.0 * _residual);
        _zero += delta_x;
    }

    inline void update_y()
    {
        std::swap(delta_y, _residual);
        delta_y = _residual - delta_y;
    }

    void update_inv_jac()
    {
        _tmp_a = delta_x - inv_jac * delta_y;
        _tmp_b = delta_x * inv_jac;
        _tmp_c = linalg::quadratic_form(inv_jac, delta_x, delta_y);
        // above equivalent to  delta_x * inv_jac * delta_y
        inv_jac += linalg::outer(_tmp_a, _tmp_b) / _tmp_c;
    }

    /**
     * @brief Build J^{-1} from a forward-difference Jacobian approximation.
     *
     * For each column j, perturbs `_zero` by `h` in direction j and estimates
     * the j-th column of J as `(f(x + h*e_j) - f(x)) / h`.  The inverse is
     * then computed by solving `J * x = e_j` for each unit vector via LU
     * decomposition.
     *
     * @param fun Callable representing the function whose zero is sought.
     * @param h   Forward-difference step size.
     * @return J^{-1}, or the identity matrix if J is singular at `_zero`.
     */
    template <typename F>
    mat_t _fd_inv_jacobian(F&& fun, double h) const
    {
        // Build forward-difference Jacobian column by column
        mat_t J;
        for (size_t j = 0; j < Dim; ++j) {
            vec_t x_pert = _zero;
            x_pert[j] += h;
            std::array<double, Dim> f_pert = fun(x_pert.asarray());
            for (size_t i = 0; i < Dim; ++i) {
                J(i, j) = (f_pert[i] - _residual[i]) / h;
            }
        }

        // Invert J by solving J * e_j = col_j for each unit vector e_j
        linalg::LU<double, Dim> lu{J};
        mat_t inv_J;
        for (size_t j = 0; j < Dim; ++j) {
            vec_t e_j;
            for (size_t i = 0; i < Dim; ++i) e_j[i] = (i == j) ? 1.0 : 0.0;
            auto col = lu.solve(e_j);
            if (!col) return mat_t::identity();  // singular: fall back to identity
            for (size_t i = 0; i < Dim; ++i) inv_J(i, j) = (*col)[i];
        }
        return inv_J;
    }
};

}  // namespace root_multidim
#endif
