#ifndef ROOT_BROYDEN_H
#define ROOT_BROYDEN_H

#include "zeros.h"
#include "../../linalg/base.h"

namespace root_multidim
{

/** @brief Broyden's method.
 *
 *  @details
 */
template <size_t Dim>
struct broyden : public zero_finding_method<Dim, broyden<Dim>> {
    using zero_finding_method<Dim, broyden<Dim>>::zero_finding_method;
    using vec_t = typename linalg::vector<double, Dim>;
    using mat_t = typename linalg::matrix<double, Dim, Dim>;

    vec_t _zero;
    vec_t _residual;
    vec_t delta_x;
    vec_t delta_y;
    mat_t inv_jac;

    vec_t _tmp_a;
    vec_t _tmp_b;
    double _tmp_c;

    template <typename F>
    bool initialize(F&& fun, std::array<double, Dim> const& guess)
    {
        _zero = guess;
        _residual = fun(guess);
        inv_jac = linalg::matrix<double, Dim, Dim>::identity();
        return true;
    }

    template <typename F>
    bool iterate(F&& fun)
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
        return true;
    }

    bool has_converged()
    {
        if (this->is_zero(_residual, _zero)) {
            this->flag = Flag::residual_zero;
            return true;
        }

        if (this->is_zero(delta_x, _zero)) {
            this->flag = Flag::delta_x_zero;
            return true;
        }

        return false;
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
};

}  // namespace root_multidim
#endif
