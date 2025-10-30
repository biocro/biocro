#ifndef BROYDEN_METHOD_H
#define BROYDEN_METHOD_H

#include <array>
#include <cmath>    // std::sqrt, std::isnan
#include <numeric>  // std::inner_product

// #include <iostream>

namespace root_multidim
{

template <size_t dim>
using vec_t = std::array<double, dim>;

template <size_t dim>

using mat_t = std::array<vec_t<dim>, dim>;

template <size_t dim>
struct broyden {
    // declare types for solving
    using vec_t = vec_t<dim>;
    using mat_t = mat_t<dim>;

    struct result_t {
        vec_t zero;
        vec_t residual;
        size_t iteration;
    };

    broyden(size_t max_iter, double abs_tol, double rel_tol) : max_iterations{max_iter}, _abs_tol{abs_tol}, _rel_tol{rel_tol} {}

    size_t max_iterations = 100;
    double _abs_tol = 1e-12;
    double _rel_tol = 1e-12;

    template <typename F>
    result_t solve(F&& fun, const vec_t& guess)
    {
        result_t result;
        result.zero = guess;
        result.residual = fun(result.zero);
        // mat_t jac = approx_jac(fun, result.zero);
        // mat_t inv_jac = invert(jac);
        mat_t inv_jac = identity();
        vec_t delta_x;
        vec_t delta_y;

        for (size_t i = 0; i <= max_iterations; ++i) {
            /*
            UPDATE FORMULA
                x -> x + delta_x ;
                  where
                    delta_x = -inv_jac(y);
                    y = fun(x);
                    delta_y = fun(x + delta_x) - fun(x);
                inv_jac -> inv_jac + outer(a, b)/c;
                  where
                    a = delta_x - inv_jac(delta_y)
                    b = inv_jac.tranpose(delta_x)
            */
            update_x(result.zero, delta_x, result.residual, inv_jac);
            // print(std::cout, result.zero);
            delta_y = fun(result.zero);
            update_y(result.residual, delta_y);
            if (is_zero(result.residual, result.zero) || is_nan(result.zero)) {
                result.iteration = i;
                return result;
            }
            inv_jac = update_inv_jac(inv_jac, delta_y, delta_x);
        }
        result.iteration = max_iterations;
        return result;
    }

    void print(std::ostream& os, const vec_t& v)
    {
        os << "[ ";
        for (size_t i = 0; i < dim; ++i) {
            os << v[i];
            if (i < dim - 1) {
                os << ", ";
            }
        }
        os << " ]\n";
    }

    void print(std::ostream& os, const mat_t& v)
    {
        os << "[ ";
        for (size_t i = 0; i < dim; ++i) {
            os << v[i];
            if (i < dim - 1) {
                os << ", ";
            }
        }
        os << " ]\n";
    }

    template <typename F>
    mat_t approx_jac(F&& fun, const vec_t& x)
    {
        vec_t x0 = x;
        mat_t jac;
        vec_t y0 = fun(x);
        vec_t y1;
        double constexpr eps = 1e-10;
        for (size_t i = 0; i < dim; ++i) {
            x0[i] += eps;
            if (i > 0) {
                x0[i - 1] -= eps;
            }
            y1 = fun(x0);
            for (size_t j = 0; j < dim; ++j) {
                jac[j][i] = (y1[j] - y0[j]) / eps;
            }
        }

        return jac;
    }

    inline mat_t invert(const mat_t& A)
    {
        mat_t out = identity();
        double b;
        // gauss seidel method
        for (size_t iteration = 0; iteration < 100; ++iteration) {
            for (size_t i = 0; i < dim; ++i) {
                for (size_t k = 0; k < dim; ++k) {
                    b = i == k ? 1 : 0;
                    out[i][k] = b;
                    for (size_t j = 0; j < dim; ++j) {
                        if (j != i)
                            out[i][k] -= A[i][j] * out[j][k];
                    }
                    out[i][k] /= A[i][i];

                    if (std::isnan(out[i][k])) {
                        return identity();
                    }
                }
            }
        }
        return out;
    }

   private:
    inline mat_t identity()
    {
        mat_t out;
        for (size_t i = 0; i < dim; ++i) {
            for (size_t j = 0; j < dim; ++j) {
                out[i][j] = 0;
                if (i == j)
                    out[i][j] = 1;
            }
        }
        return out;
    }

    inline void update_x(vec_t& x, vec_t& delta_x, const vec_t& y, const mat_t& inv_jac)
    {
        // these formulae are computed in a single for-loop pass
        // delta_x = dot(inv_jac,  -y)
        // x += delta_x
        for (size_t i = 0; i < dim; ++i) {
            delta_x[i] = 0;
            for (size_t j = 0; j < dim; ++j) {
                delta_x[i] += inv_jac[i][j] * -y[j];
            }
            x[i] += delta_x[i];
        }
    }

    inline void update_y(vec_t& y, vec_t& delta_y)
    {
        // this is  computed in a single for-loop pass
        for (size_t i = 0; i < dim; ++i) {
            std::swap(delta_y[i], y[i]);     // swap values so that y[i] is new value, and delta_y[i] is old
            delta_y[i] = y[i] - delta_y[i];  //
        }
    }

    inline mat_t& update_inv_jac(mat_t& inv_jac, const vec_t& delta_y, const vec_t& delta_x)
    {
        /*
        inv_jac ->  inv_jac + outer(a, b) / c
          where
            a = delta_x - dot(inv_jac , delta_y)
            b = dot(delta_x, inv_jac)
            c = dot(delta_x, dot(inv_jac, delta_y))


        */
        vec_t a;
        vec_t b;
        double c = 0;
        for (size_t i = 0; i < dim; ++i) {
            a[i] = delta_x[i];
            for (size_t j = 0; j < dim; ++j) {
                a[i] -= inv_jac[i][j] * delta_y[j];
                b[j] += delta_x[i] * inv_jac[i][j];
                c += delta_x[i] * inv_jac[i][j] * delta_y[j];
            }
        }

        for (size_t i = 0; i < dim; ++i) {
            for (size_t j = 0; j < dim; ++j) {
                inv_jac[i][j] += a[i] * b[j] / c;
            }
        }

        return inv_jac;
    }

    bool inline is_zero(const vec_t& y, const vec_t& x)
    {
        double ysq = std::inner_product(y.cbegin(), y.cend(), y.cbegin(), 0.0);
        double xsq = std::inner_product(x.cbegin(), x.cend(), x.cbegin(), 0.0);
        return std::sqrt(ysq) < _abs_tol + _rel_tol * std::sqrt(xsq);
    }

    bool inline is_nan(const vec_t& x)
    {
        for (const double& v : x) {
            if (std::isnan(v)) return true;
        }
        return false;
    }
};

// template <size_t dim>
// vec_t<dim>& operator+=(vec_t<dim>& x, const vec_t<dim>& y)
// {
//     for (size_t i = 0; i < dim; ++i) {
//         x[i] += y[i];
//     }
//     return x;
// }

// template <size_t dim>
// vec_t<dim>& operator-=(vec_t<dim>& x, const vec_t<dim>& y)
// {
//     for (size_t i = 0; i < dim; ++i) {
//         x[i] -= y[i];
//     }
//     return x;
// }

// template <size_t dim>
// vec_t<dim> operator+(const vec_t<dim>& x, const vec_t<dim>& y)
// {
//     vec_t<dim> out;
//     for (size_t i = 0; i < dim; ++i) {
//         out[i] = x[i] + y[i];
//     }
//     return x;
// }

// template <size_t dim>
// vec_t<dim> operator-(const vec_t<dim>& x, const vec_t<dim>& y)
// {
//     vec_t<dim> out;
//     for (size_t i = 0; i < dim; ++i) {
//         out[i] = x[i] - y[i];
//     }
//     return out;
// }

// template <size_t dim>
// vec_t<dim> dot(const mat_t<dim>& A, const vec_t<dim>& v)
// {
//     vec_t<dim> out;
//     for (size_t i = 0; i < dim; ++i) {
//         out[i] = dot(A[i], v);
//     }
//     return out;
// }

// // transpose

// template <size_t dim>
// vec_t<dim> dot(const vec_t<dim>& v, const std::array<vec_t<dim>, dim>& A)
// {
//     vec_t<dim> out;
//     out.fill(0);
//     for (size_t i = 0; i < dim; ++i) {
//         for (size_t j = 0; j < dim; ++j) {
//             out[j] += v[i] * A[i][j];
//         }
//     }
//     return out;
// }

// template <size_t dim>
// inline double dot(const vec_t<dim>& u, const vec_t<dim>& v)
// {
//     double out = 0;
//     for (size_t i = 0; i < dim; ++i) {
//         out += u[i] * v[i];
//     }
//     return out;
// }

}  // namespace root_multidim
#endif
