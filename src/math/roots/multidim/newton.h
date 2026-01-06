#ifndef ZEROS_NEWTON_H
#define ZEROS_NEWTON_H

#include "zeros.h"
#include "../../linalg/base.h"
#include "../../linalg/lu.h"

namespace root_multidim
{

/** @brief Newton's method.
 *
 *  @details
 */
template <size_t Dim>
struct newton : public zero_finding_method<Dim, newton<Dim>> {
    using zero_finding_method<Dim, newton<Dim>>::zero_finding_method;
    using vec_t = typename linalg::vector<double, Dim>;
    using mat_t = typename linalg::matrix<double, Dim, Dim>;

    vec_t x;
    vec_t delta_x;
    vec_t y;

    template <typename F>
    bool initialize(F&& fun, std::array<double, Dim> const& guess)
    {
        x = guess;
        y = fun(guess);
        return true;
    }

    template <typename F>
    bool iterate(F&& fun)
    {
        linalg::LU<double, Dim> lu(fun.jacobian(x));

        auto sol = lu.solve(-1.0 * y);
        if (!sol) {
            this->flag = Flag::singular_matrix;
            return false;
        }
        delta_x = sol.value();
        x += delta_x;
        y = fun(x.asarray());
        return true;
    }

    bool has_converged()
    {
        if (this->is_zero(y, x)) {
            this->flag = Flag::residual_zero;
            return true;
        }
        return false;
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
