#ifndef LINALG_MATRIX_EXPR_H
#define LINALG_MATRIX_EXPR_H
#include <type_traits>  // std::conditional
#include "vector.h"
namespace linalg
{

template <typename Scalar, size_t Row, size_t Col, typename E>
struct Matrix {
    static constexpr bool IS_LEAF = false;

    [[nodiscard]]
    Scalar operator()(size_t i, size_t j) const
    {
        // Delegation to the actual expression type.
        // This avoids dynamic polymorphism (a.k.a. virtual functions in C++)
        return static_cast<E const&>(*this)(i, j);
    }
};

template <typename Scalar, size_t Row, size_t Col, typename E1, typename E2>
struct MatVecMul : Vector<Scalar, Row, MatVecMul<Scalar, Row, Col, E1, E2>> {
   private:
    // cref if leaf, copy otherwise
    typename std::conditional<E1::IS_LEAF, const E1&, const E1>::type mat;
    typename std::conditional<E2::IS_LEAF, const E2&, const E2>::type vec;

   public:
    static constexpr bool IS_LEAF = false;

    MatVecMul(E1 const& mat, E2 const& vec) : mat{mat}, vec{vec} {}

    [[nodiscard]]
    Scalar operator[](size_t i) const noexcept
    {
        Scalar out = 0;
        for (size_t j = 0; j < Col; ++j)
            out += mat(i, j) * vec[j];
        return out;
    }
};

template <typename Scalar, size_t Row, size_t Col, typename E1, typename E2>
MatVecMul<Scalar, Row, Col, E1, E2> operator*(Matrix<Scalar, Row, Col, E1> const& u, Vector<Scalar, Col, E2> const& v)
{
    return MatVecMul<Scalar, Row, Col, E1, E2>(*static_cast<const E1*>(&u), *static_cast<const E2*>(&v));
}

template <typename Scalar, size_t Row, size_t Col, typename E1, typename E2>
struct VecMatMul : Vector<Scalar, Col, VecMatMul<Scalar, Row, Col, E1, E2>> {
   private:
    // cref if leaf, copy otherwise
    typename std::conditional<E1::IS_LEAF, const E1&, const E1>::type mat;
    typename std::conditional<E2::IS_LEAF, const E2&, const E2>::type vec;

   public:
    static constexpr bool IS_LEAF = false;

    VecMatMul(E1 const& mat, E2 const& vec) : mat{mat}, vec{vec} {}

    [[nodiscard]]
    Scalar operator[](size_t i) const noexcept
    {
        Scalar out = 0;
        for (size_t j = 0; j < Row; ++j)
            out += vec[j] * mat(j, i);
        return out;
    }
};

template <typename Scalar, size_t Row, size_t Col, typename E1, typename E2>
VecMatMul<Scalar, Row, Col, E1, E2> operator*(Vector<Scalar, Row, E2> const& u, Matrix<Scalar, Row, Col, E1> const& v)
{
    return VecMatMul<Scalar, Row, Col, E1, E2>(*static_cast<const E1*>(&v), *static_cast<const E2*>(&u));
}

template <typename Scalar, size_t Row, size_t Col, typename E1, typename E2, typename E3>
Scalar quadratic_form(
    Matrix<Scalar, Row, Col, E1> const& mat,
    Vector<Scalar, Row, E2> const& u,
    Vector<Scalar, Col, E3> const& v)
{
    Scalar out = 0;
    for (size_t i = 0; i < Row; ++i) {
        for (size_t j = 0; j < Col; ++j) {
            out += u[i] * mat(i, j) * v[j];
        }
    }
    return out;
}

// VECTOR OPERATIONS
template <typename Scalar, size_t Row, size_t Col, typename E1, typename E2>
class MatAdd : public Matrix<Scalar, Row, Col, MatAdd<Scalar, Row, Col, E1, E2>>
{
   private:
    // cref if leaf, copy otherwise
    typename std::conditional<E1::IS_LEAF, const E1&, const E1>::type A;
    typename std::conditional<E2::IS_LEAF, const E2&, const E2>::type B;

   public:
    static constexpr bool IS_LEAF = false;

    MatAdd(E1 const& a, E2 const& b) : A{a}, B{b} {}

    [[nodiscard]]
    Scalar operator()(size_t i, size_t j) const noexcept
    {
        return A(i, j) + B(i, j);
    }
};

template <typename Scalar, size_t Row, size_t Col, typename E1, typename E2>
MatAdd<Scalar, Row, Col, E1, E2> operator+(Matrix<Scalar, Row, Col, E1> const& u, Matrix<Scalar, Row, Col, E2> const& v)
{
    return MatAdd<Scalar, Row, Col, E1, E2>(*static_cast<const E1*>(&u), *static_cast<const E2*>(&v));
}

// VECTOR OPERATIONS
template <typename Scalar, size_t Row, size_t Col, typename E1, typename E2>
class MatSub : public Matrix<Scalar, Row, Col, MatSub<Scalar, Row, Col, E1, E2>>
{
   private:
    // cref if leaf, copy otherwise
    typename std::conditional<E1::IS_LEAF, const E1&, const E1>::type A;
    typename std::conditional<E2::IS_LEAF, const E2&, const E2>::type B;

   public:
    static constexpr bool IS_LEAF = false;

    MatSub(E1 const& a, E2 const& b) : A{a}, B{b} {}

    [[nodiscard]]
    Scalar operator()(size_t i, size_t j) const noexcept
    {
        return A(i, j) - B(i, j);
    }
};

template <typename Scalar, size_t Row, size_t Col, typename E1, typename E2>
MatSub<Scalar, Row, Col, E1, E2> operator-(Matrix<Scalar, Row, Col, E1> const& u, Matrix<Scalar, Row, Col, E2> const& v)
{
    return MatSub<Scalar, Row, Col, E1, E2>(*static_cast<const E1*>(&u), *static_cast<const E2*>(&v));
}

template <typename Scalar, size_t Row, size_t Col, typename E>
class ScalarMatMul : public Matrix<Scalar, Row, Col, ScalarMatMul<Scalar, Row, Col, E>>
{
   private:
    // cref if leaf, copy otherwise
    typename std::conditional<E::IS_LEAF, const E&, const E>::type A;
    Scalar scalar;

   public:
    static constexpr bool IS_LEAF = false;

    ScalarMatMul(E const& a, Scalar c) : A{a}, scalar{c} {}

    [[nodiscard]]
    Scalar operator()(size_t i, size_t j) const noexcept
    {
        return scalar * A(i, j);
    }
};

template <typename Scalar, size_t Row, size_t Col, typename E>
ScalarMatMul<Scalar, Row, Col, E> operator*(Matrix<Scalar, Row, Col, E> const& v, Scalar c)
{
    return ScalarMatMul<Scalar, Row, Col, E>(*static_cast<const E*>(&v), c);
}

template <typename Scalar, size_t Row, size_t Col, typename E>
ScalarMatMul<Scalar, Row, Col, E> operator*(Scalar c, Matrix<Scalar, Row, Col, E> const& v)
{
    return ScalarMatMul<Scalar, Row, Col, E>(*static_cast<const E*>(&v), c);
}

template <typename Scalar, size_t Row, size_t Col, typename E>
ScalarMatMul<Scalar, Row, Col, E> operator/(Matrix<Scalar, Row, Col, E> const& v, Scalar c)
{
    return ScalarMatMul<Scalar, Row, Col, E>(*static_cast<const E*>(&v), 1 / c);
}

template <typename Scalar, size_t Row, size_t Col, typename E1, typename E2>
struct OuterProd : Matrix<Scalar, Row, Col, OuterProd<Scalar, Row, Col, E1, E2>> {
   private:
    // cref if leaf, copy otherwise
    typename std::conditional<E1::IS_LEAF, const E1&, const E1>::type u;
    typename std::conditional<E2::IS_LEAF, const E2&, const E2>::type v;

   public:
    OuterProd(E1 const& u, E2 const& v) : u{u}, v{v} {}

    [[nodiscard]]
    Scalar operator()(size_t i, size_t j) const noexcept
    {
        return u[i] * v[j];
    }
};

template <typename Scalar, size_t Row, size_t Col, typename E1, typename E2>
OuterProd<Scalar, Row, Col, E1, E2> outer(Vector<Scalar, Row, E1> const& u, Vector<Scalar, Col, E2> const& v)
{
    return OuterProd<Scalar, Row, Col, E1, E2>(*static_cast<const E1*>(&u), *static_cast<const E1*>(&v));
}

}  // namespace linalg
#endif
