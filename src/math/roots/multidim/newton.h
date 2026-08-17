#ifndef ZEROS_NEWTON_H
#define ZEROS_NEWTON_H

#include "zeros.h"
#include "../../linalg/base.h"
#include "../../linalg/lu.h"

namespace root_multidim
{
/**
 * @brief Newton's method for finding vector-valued zeros of nonlinear systems.
 *
 * This struct implements Newton's method given a function that provides the
 * avoiding the full, dense Jacobian at every step. The linear system is
 * solved using LU decomposition.
 *
 * @tparam Dim The dimension of the system (number of equations = number of unknowns).
 *
 * @par Usage Example
 * @code
 * // Define the system: find x such that f(x) = 0
 * struct f {
 * operator()(std::array<double, 2> x) -> std::array<double, 2> {
 *     return { x[0]*x[0] + x[1] - 1.0,
 *              x[0]      - x[1]*x[1] };
 * }
 *
 * };
 * using namespace root_multidim;
 * // set solver with a max iterations, abs_tol, rel_rol
 * newton<2> solver(100, 1e-5, 1e-5);
 * std::array<double, 2> guess = { 0.5, 0.5 };
 *
 * result_t result = solver.solve(f, guess);
 *
 * @endcode
 *
 * @note Convergence may occur if the Jacobian is approximate, but
 *       may be slow if the Jacobian is wrong. Use `broyden` or a Quasi-
 *       Newton method if the Jacobian must be approximated numerically.
 */
template <size_t Dim>
struct newton : public zero_finding_method<Dim, newton<Dim>> {
    using zero_finding_method<Dim, newton<Dim>>::zero_finding_method;

    friend struct zero_finding_method<Dim, newton<Dim>>;

   private:
    using vec_t = typename linalg::vector<double, Dim>;
    using mat_t = typename linalg::matrix<double, Dim, Dim>;

    vec_t x;
    vec_t delta_x;
    vec_t y;

    template <typename F>
    Status initialize(F&& fun, std::array<double, Dim> const& guess)
    {
        x = guess;
        y = fun(guess);
        return Status::ok;
    }

    template <typename F>
    Status iterate(F&& fun)
    {
        linalg::LU<double, Dim> lu(fun.jacobian(x));

        auto sol = lu.solve(-1.0 * y);
        if (!sol) {
            this->flag = Flag::singular_matrix;
            return Status::failed;
        }
        delta_x = sol.value();
        x += delta_x;
        y = fun(x.asarray());
        return Status::ok;
    }

    Status has_converged()
    {
        if (this->is_zero(y, x)) {
            this->flag = Flag::residual_zero;
            return Status::converged;
        }
        return Status::ok;
    }

    std::array<double, Dim> residual() const
    {
        return y.asarray();
    }

    std::array<double, Dim> zero() const
    {
        return x.asarray();
    }
};

}  // namespace root_multidim

#endif
