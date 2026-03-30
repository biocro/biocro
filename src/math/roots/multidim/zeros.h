#ifndef ZEROS_MULTIDIM_H
#define ZEROS_MULTIDIM_H

#include <array>
#include <algorithm>  // std::swap
#include <cmath>      // std::sqrt, std::isnan

#include "../../linalg/base.h"
namespace root_multidim
{
// termination states
enum class Flag {
    residual_zero,
    delta_x_zero,
    zero_is_nonfinite,
    max_iterations,
    function_is_nonfinite,
    singular_matrix
};

template <size_t Dim>
struct result_t {
    std::array<double, Dim> zero;
    std::array<double, Dim> residual;
    size_t iteration;
    Flag flag;

    result_t(
        std::array<double, Dim> const& x,
        std::array<double, Dim> const& y,
        size_t i,
        Flag f) : zero{x}, residual{y}, iteration{i}, flag{f} {}
};

template <size_t Dim, typename Method>
struct zero_finding_method {
    zero_finding_method(size_t max_iter, double abs_tol, double rel_tol)
        : max_iterations{max_iter},
          _abs_tol{abs_tol},
          _rel_tol{rel_tol}
    {
    }
    zero_finding_method() = default;

    size_t max_iterations = 100;
    double _abs_tol = 1e-12;
    double _rel_tol = 1e-12;
    bool is_valid;
    Flag flag;

    template <typename F, typename... Args>
    result_t<Dim> solve(F&& fun, Args&&... args)
    {
        is_valid = static_cast<Method*>(this)->initialize(std::forward<F>(fun), std::forward<Args>(args)...);
        for (size_t i = 0; i <= max_iterations; ++i) {
            if (!is_valid) {
                return make_result(i);
            }

            is_valid = static_cast<Method*>(this)->iterate(std::forward<F>(fun));

            if (is_valid) {
                is_valid = !(static_cast<Method*>(this)->has_converged());
            }
        }
        flag = Flag::max_iterations;
        return make_result(max_iterations);
    }

    template <typename F, typename... Args>
    inline result_t<Dim> operator()(F&& fun, Args&&... args)
    {
        return solve(std::forward<F>(fun), std::forward<Args>(args)...);
    }

   protected:
    result_t<Dim> make_result(size_t i)
    {
        return result_t<Dim>(
            static_cast<Method*>(this)->zero(),
            static_cast<Method*>(this)->residual(),
            i, flag);
    }
    // All methods require tolerance-based floating point number equality tests.
    // Are two floating point numbers equal?
    // Equality is lax if both `x` and `y` are big.
    inline bool is_close(double x, double y) const
    {
        double norm = std::min(std::abs(x), std::abs(y));
        return std::abs(x - y) <= std::max(_abs_tol, _rel_tol * norm);
    }

    // Is a floating point number zero?
    inline bool is_zero(double x) const
    {
        return std::abs(x) <= _abs_tol;
    }

    // floating point errors can increase with dimension
    inline bool is_zero(
        linalg::vector<double, Dim> const& y,
        linalg::vector<double, Dim> const& x) const
    {
        double ysq = linalg::dot(y, y);
        double xsq = linalg::dot(x, x);
        return std::sqrt(ysq) < _abs_tol + _rel_tol * std::sqrt(xsq);
    }

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
