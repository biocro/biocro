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
 *
 * Zwillinger, Daniel (Ed.) CRC standard mathematical tables and formulae, 2012, 32. ed.
 *     pg 638-649 CRC Press: Boca Raton
 */
namespace quadrature
{

/**
 * @brief Rule traits for composite closed Newton-Cotes quadrature.
 *
 * Closed Newton-Cotes rules evaluate the integrand at `Order * n + 1`
 * equally-spaced nodes across `[a, b]`, **including** both endpoints.  Within
 * each subinterval of width `dx = (b-a) / (Order * n)` the interior nodes
 * cycle through `Order` distinct weight classes indexed by `i % Order`.
 * Shared interior nodes (where two adjacent panels meet) appear in both
 * panels and therefore carry double weight relative to a single-panel formula;
 * this doubling is pre-absorbed into `weights[0]`.
 *
 * All weights are pre-scaled by the rule's scale factor so that
 * `closed_newton_cotes` only multiplies the accumulated sum by `dx` once.
 * `endpoint_weight` is the pre-scaled weight applied to `f(a)` and `f(b)`.
 *
 * | Order | Common name      | Unscaled endpoint | Unscaled interior weights    |
 * |-------|------------------|-------------------|------------------------------|
 * |   1   | Trapezoid        | 1/2               | 1                            |
 * |   2   | Simpson's        | 1/3               | 2/3, 4/3                     |
 * |   3   | Three-eighths    | 3/8               | 6/8, 9/8, 9/8                |
 * |   4   | Boole's          | 14/45             | 28/45, 64/45, 24/45, 64/45  |
 *
 * To add a new rule, specialise `ClosedNewtonCotes<N>` with pre-scaled
 * `endpoint_weight` and `weights`, then call `closed_newton_cotes<N>`.
 */
template <int Order>
struct ClosedNewtonCotes;

// Trapezoid rule: linear interpolation between endpoints.
template <>
struct ClosedNewtonCotes<1> {
    static constexpr double endpoint_weight = 0.5;
    static constexpr std::array<double, 1> weights = {1.0};
};

// Simpson's rule: exact for polynomials up to degree 3.
template <>
struct ClosedNewtonCotes<2> {
    static constexpr double endpoint_weight = 1.0 / 3.0;
    static constexpr std::array<double, 2> weights = {2.0 / 3.0, 4.0 / 3.0};
};

// Three-eighths rule: exact for polynomials up to degree 3.
template <>
struct ClosedNewtonCotes<3> {
    static constexpr double endpoint_weight = 3.0 / 8.0;
    static constexpr std::array<double, 3> weights = {6.0 / 8.0, 9.0 / 8.0, 9.0 / 8.0};
};

// Boole's rule: exact for polynomials up to degree 5.
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

/**
 * @brief Rule traits for composite open Newton-Cotes quadrature.
 *
 * Open Newton-Cotes rules evaluate the integrand at `Order` equally-spaced
 * interior points of each subinterval, **excluding** both endpoints.  Within
 * a subinterval of width `dx`, the nodes are placed at
 *
 *   `x_j = a + i·dx + j·h`,  j = 1 … Order,  h = dx / (Order + 1)
 *
 * so the two phantom endpoints at j = 0 and j = Order+1 are never evaluated.
 * Each `OpenNewtonCotes<Order>` specialisation stores `Order` pre-scaled
 * weights (already divided by the rule's scale factor h) so that
 * `open_newton_cotes` multiplies the accumulated sum by `h` exactly once.
 *
 * | Order | Common name    | Unscaled weights    |
 * |-------|----------------|---------------------|
 * |   1   | Midpoint rule  | 2                   |
 * |   2   | Two-point open | 3/2, 3/2            |
 * |   3   | Milne's rule   | 8/3, −4/3, 8/3      |
 *
 * To add a new rule, specialise `OpenNewtonCotes<N>` with pre-scaled weights
 * and call `open_newton_cotes<N>(f, a, b, n)`.
 */
template <int Order>
struct OpenNewtonCotes;

// Midpoint rule: single node at the centre of each subinterval.
template <>
struct OpenNewtonCotes<1> {
    static constexpr std::array<double, 1> weights = {2.0};
};

// Two-point open rule: nodes at 1/3 and 2/3 of each subinterval.
template <>
struct OpenNewtonCotes<2> {
    static constexpr std::array<double, 2> weights = {3.0 / 2.0, 3.0 / 2.0};
};

// Milne's rule: nodes at 1/4, 1/2, and 3/4 of each subinterval.
// Note the negative centre weight — this rule is not positive-definite.
template <>
struct OpenNewtonCotes<3> {
    static constexpr std::array<double, 3> weights = {8.0 / 3.0, -4.0 / 3.0, 8.0 / 3.0};
};

template <int Order, typename T = double, typename Integrand>
T open_newton_cotes(Integrand&& f, double a, double b, int n)
{
    using Rule = OpenNewtonCotes<Order>;
    double const dx = (b - a) / n;
    double const h = dx / (Order + 1);
    T result{};
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < Order; ++j)
            result += f(a + i * dx + (j + 1) * h) * Rule::weights[j];
    }
    return result * h;
}

template <typename T = double, typename Integrand>
T midpoint_rule(Integrand&& f, double a, double b, int n)
{
    return open_newton_cotes<1, T>(std::forward<Integrand>(f), a, b, n);
}

/**
 * @brief Rule traits for composite Gauss-Legendre quadrature.
 *
 * Gauss-Legendre rules place `Order` nodes per subinterval at positions
 * chosen to maximise the degree of polynomial integrated exactly: an
 * `Order`-point rule is exact for polynomials up to degree `2*Order - 1`.
 * Unlike Newton-Cotes rules, the nodes are **not** equally spaced and never
 * coincide with the subinterval endpoints.
 *
 * Node positions are stored as `abscissa` values on the reference interval
 * `[-1, 1]`.  `gauss_legendre` maps them to each subinterval of width `dx`
 * via `mid + 0.5 * dx * abscissa[j]`, then multiplies the result by
 * `0.5 * dx` (the Jacobian of the change of variables).  Each node has a
 * corresponding `weights[j]` that represents the standard Gauss-Legendre
 * weight on `[-1, 1]`.
 *
 * | Order | Exact up to degree | Node positions (on [-1,1])          |
 * |-------|--------------------|--------------------------------------|
 * |   2   | 3                  | ±1/√3                               |
 * |   3   | 5                  | 0, ±√(3/5)                          |
 * |   4   | 7                  | ±0.339…, ±0.861…                    |
 *
 * To add a new order, specialise `GaussLegendreRule<N>` with the standard
 * Gauss-Legendre abscissae and weights on `[-1, 1]` (available in standard
 * references or computed via eigenvalue methods).
 */
template <int Order>
struct GaussLegendreRule;  // leave undefined; only specializations are valid

// 2-point rule: exact for polynomials up to degree 3.
template <>
struct GaussLegendreRule<2> {
    static constexpr std::array<double, 2> abscissa = {-0.5773502691896258, 0.5773502691896258};
    static constexpr std::array<double, 2> weights = {1.0, 1.0};
};

// 3-point rule: exact for polynomials up to degree 5.
template <>
struct GaussLegendreRule<3> {
    static constexpr std::array<double, 3> abscissa = {-0.7745966692414834, 0.0, 0.7745966692414834};
    static constexpr std::array<double, 3> weights = {5.0 / 9.0, 8.0 / 9.0, 5.0 / 9.0};
};

// 4-point rule: exact for polynomials up to degree 7.
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
