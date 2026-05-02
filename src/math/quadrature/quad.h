#ifndef QUADRATURE_H
#define QUADRATURE_H
#include <cmath>
#include <array>

/**
 * @file
 * @brief Numerical quadrature routines for integrating a function over a
 * closed interval `[a, b]` using `n` equal subintervals.
 *
 * All algorithms are templated on the return type `T` of the integrand, so
 * they work with `double` as well as any vector-space type that supports
 * `operator+=` and scalar `operator*` — in particular `PhotoCore::LeafAssim`,
 * which is used to compute canopy-integrated photosynthesis in `c3CanAC` and
 * `CanAC`.
 *
 * **Closed Newton-Cotes rules** evaluate the integrand at `Order * n + 1`
 * equally-spaced nodes including both endpoints.  All weights are absorbed
 * into `ClosedNewtonCotes<Order>` so the integration loop in
 * `closed_newton_cotes` is identical for every rule.  Named convenience
 * wrappers (`trapezoid`, `simpsons_rule`, `three_eights_rule`, `booles_rule`)
 * forward to `closed_newton_cotes<1..4>`.  To add a new rule, specialise
 * `ClosedNewtonCotes<N>` with the pre-scaled `endpoint_weight` and `weights`
 * array (all weights already divided by the rule's scale factor).
 *
 * **Gauss-Legendre rules** (`gauss_legendre<Order>`) place `Order` nodes per
 * subinterval at positions that maximise the polynomial degree integrated
 * exactly.  Node positions and weights for each order are stored in
 * `GaussLegendreRule<Order>` specialisations (orders 2, 3, and 4 are
 * provided).  The 2-point rule is the primary quadrature method used for the
 * canopy photosynthesis integral.
 *
 * **Usage** (canopy integral returning `PhotoCore::LeafAssim`):
 * @code{.cpp}
 * PhotoCore::LeafAssim canopy =
 *     quadrature::gauss_legendre<2, PhotoCore::LeafAssim>(integrand, 0.0, LAI, nlayers);
 * @endcode
 */
namespace quadrature
{

template <int Order>
struct ClosedNewtonCotes;

// trapezoid rule
template <>
struct ClosedNewtonCotes<1> {
    static constexpr double endpoint_weight = 0.5;
    static constexpr std::array<double, 1> weights = {1.0};
};

// Simpson's rule
template <>
struct ClosedNewtonCotes<2> {
    static constexpr double endpoint_weight = 1.0 / 3.0;
    static constexpr std::array<double, 2> weights = {2.0 / 3.0, 4.0 / 3.0};
};

// three-eighths rule
template <>
struct ClosedNewtonCotes<3> {
    static constexpr double endpoint_weight = 3.0 / 8.0;
    static constexpr std::array<double, 3> weights = {6.0 / 8.0, 9.0 / 8.0, 9.0 / 8.0};
};

// Boole's rule
template <>
struct ClosedNewtonCotes<4> {
    static constexpr double endpoint_weight = 14.0 / 45.;
    static constexpr std::array<double, 4> weights = {28.0 / 45.0, 64.0 / 45.0, 24.0 / 45, 64.0 / 45.0};
};

template <int Order, typename T = double, typename Integrand>
T closed_newton_cotes(Integrand&& f, double a, double b, int n)
{
    using Rule = ClosedNewtonCotes<Order>;
    int num = Order * n;
    double const dx = (b - a) / num;
    T result = Rule::endpoint_weight * (f(a) + f(b));
    for (int i = 1; i < num; ++i) {
        result += f(a + i * dx) * Rule::weights[i % Order];
    }
    return result * dx;
}

template <typename T = double, typename Integrand>
T trapezoid(Integrand&& f, double a, double b, int n)
{
    return closed_newton_cotes<1, T>(std::forward<Integrand>(f), a, b, n);
}

template <typename T = double, typename Integrand>
T simpsons_rule(Integrand&& f, double a, double b, int n)
{
    return closed_newton_cotes<2, T>(std::forward<Integrand>(f), a, b, n);
}

template <typename T = double, typename Integrand>
T three_eights_rule(Integrand&& f, double a, double b, int n)
{
    return closed_newton_cotes<3, T>(std::forward<Integrand>(f), a, b, n);
}

template <typename T = double, typename Integrand>
T booles_rule(Integrand&& f, double a, double b, int n)
{
    return closed_newton_cotes<4, T>(std::forward<Integrand>(f), a, b, n);
}

template <int Order>
struct OpenNewtonCotes;

// midpoint rule
template <>
struct OpenNewtonCotes<1> {
    static constexpr double endpoint_weight = 0.5;
    static constexpr std::array<double, 1> weights = {1.0};
};

template <typename T = double, typename Integrand>
T midpoint_rule(Integrand&& f, double a, double b, int n)
{
    double const dx = (b - a) / n;
    T result{};
    for (int i = 0; i < n; ++i) {
        result += f(a + (i + 0.5) * dx);
    }
    return result * dx;
}

template <int Order>
struct GaussLegendreRule;  // leave undefined; only specializations are valid

template <>
struct GaussLegendreRule<2> {
    static constexpr std::array<double, 2> abscissa = {-0.5773502691896258, 0.5773502691896258};
    static constexpr std::array<double, 2> weights = {1.0, 1.0};
};

template <>
struct GaussLegendreRule<3> {
    static constexpr std::array<double, 3> abscissa = {-0.7745966692414834, 0.0, 0.7745966692414834};
    static constexpr std::array<double, 3> weights = {5.0 / 9.0, 8.0 / 9.0, 5.0 / 9.0};
};

template <>
struct GaussLegendreRule<4> {
    static constexpr std::array<double, 4> abscissa = {
        -0.8611363115940526,
        -0.3399810435848563,
        0.3399810435848563,
        0.8611363115940526};
    static constexpr std::array<double, 4> weights = {
        0.34785484513745385,
        0.6521451548625461,
        0.6521451548625461,
        0.34785484513745385,
    };
};

template <int Order, typename T = double, typename Integrand>
T gauss_legendre(Integrand&& f, double a, double b, int n)
{
    using Rule = GaussLegendreRule<Order>;
    double const dx = (b - a) / n;
    T result{};
    for (int i = 0; i < n; ++i) {
        double const mid = a + (i + 0.5) * dx;
        for (int j = 0; j < Order; ++j) {
            result += f(mid + 0.5 * dx * Rule::abscissa[j]) * Rule::weights[j];
        }
    }
    return result * (dx * 0.5);
}

}  // namespace quadrature

#endif
