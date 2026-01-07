#ifndef ROOT_BROYDEN_H
#define ROOT_BROYDEN_H

#include <array>
#include <cmath>    // std::sqrt, std::isnan
#include <numeric>  // std::inner_product
#include "zeros.h"

namespace root_multidim
{

/** @brief Broyden's method.
 *
 *  @details
 */
template <size_t dim>
struct broyden : public zero_finding_method<dim, broyden<dim>> {
    using zero_finding_method<dim, broyden<dim>>::zero_finding_method;
    using vec_t = typename zero_finding_method<dim, broyden<dim>>::vec_t;
    using mat_t = typename zero_finding_method<dim, broyden<dim>>::mat_t;

    vec_t _zero;
    vec_t _residual;
    vec_t delta_x;
    vec_t delta_y;
    mat_t inv_jac;

    vec_t _tmp_a;
    vec_t _tmp_b;
    double _tmp_c;

    template <typename F>
    bool initialize(F&& fun, const vec_t& guess)
    {
        _zero = guess;
        _residual = fun(guess);
        inv_jac = this->identity();
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
        delta_y = fun(_zero);
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

    vec_t residual() const
    {
        return _residual;
    }

    vec_t zero() const
    {
        return _zero;
    }

    inline void update_x()
    {
        // these formulae are computed in a single for-loop pass
        // delta_x = dot(inv_jac,  -residual)
        // zero += delta_x
        for (size_t i = 0; i < dim; ++i) {
            delta_x[i] = 0;
            for (size_t j = 0; j < dim; ++j) {
                delta_x[i] += inv_jac[i][j] * -_residual[j];
            }
            _zero[i] += delta_x[i];
        }
    }

    inline void update_y()
    {
        // this is computed in a single for-loop pass
        for (size_t i = 0; i < dim; ++i) {
            std::swap(delta_y[i], _residual[i]);     // swap values so that y[i] is new value, and delta_y[i] is old
            delta_y[i] = _residual[i] - delta_y[i];  //
        }
    }

    void update_inv_jac()
    {
        /*
        inv_jac ->  inv_jac + outer(a, b) / c
          where
            a = delta_x - dot(inv_jac , delta_y)
            b = dot(delta_x, inv_jac)
            c = dot(delta_x, dot(inv_jac, delta_y))


        */
        _tmp_c = 0;
        _tmp_b.fill(0);

        for (size_t i = 0; i < dim; ++i) {
            _tmp_a[i] = delta_x[i];
            for (size_t j = 0; j < dim; ++j) {
                _tmp_a[i] -= inv_jac[i][j] * delta_y[j];
                _tmp_b[j] += delta_x[i] * inv_jac[i][j];
                _tmp_c += delta_x[i] * inv_jac[i][j] * delta_y[j];
            }
        }

        for (size_t i = 0; i < dim; ++i) {
            for (size_t j = 0; j < dim; ++j) {
                inv_jac[i][j] += _tmp_a[i] * _tmp_b[j] / _tmp_c;
            }
        }
    }
};

}  // namespace root_multidim
#endif
