#ifndef LINALG_VEC_EXPR_H
#define LINALG_VEC_EXPR_H
#include <cmath>
#include <type_traits>  // std::conditional
namespace linalg
{

template <typename Scalar, size_t Dim, typename E>
struct Vector {
    static constexpr bool IS_LEAF = false;

    [[nodiscard]]
    Scalar operator[](size_t i) const
    {
        // Delegation to the actual expression type.
        // This avoids dynamic polymorphism (a.k.a. virtual functions in C++)
        return static_cast<E const&>(*this)[i];
    }
};

// VECTOR OPERATIONS
template <typename Scalar, size_t Dim, typename E1, typename E2>
class VecAdd : public Vector<Scalar, Dim, VecAdd<Scalar, Dim, E1, E2>>
{
   private:
    // cref if leaf, copy otherwise
    typename std::conditional<E1::IS_LEAF, const E1&, const E1>::type u;
    typename std::conditional<E2::IS_LEAF, const E2&, const E2>::type v;

   public:
    static constexpr bool IS_LEAF = false;

    VecAdd(E1 const& u, E2 const& v) : u{u}, v{v} {}

    [[nodiscard]]
    Scalar operator[](size_t i) const noexcept
    {
        return u[i] + v[i];
    }
};

template <typename Scalar, size_t Dim, typename E1, typename E2>
VecAdd<Scalar, Dim, E1, E2> operator+(Vector<Scalar, Dim, E1> const& u, Vector<Scalar, Dim, E2> const& v)
{
    return VecAdd<Scalar, Dim, E1, E2>(*static_cast<const E1*>(&u), *static_cast<const E2*>(&v));
}

template <typename Scalar, size_t Dim, typename E1, typename E2>
class VecSub : public Vector<Scalar, Dim, VecSub<Scalar, Dim, E1, E2>>
{
   private:
    // cref if leaf, copy otherwise
    typename std::conditional<E1::IS_LEAF, const E1&, const E1>::type u;
    typename std::conditional<E2::IS_LEAF, const E2&, const E2>::type v;

   public:
    static constexpr bool IS_LEAF = false;

    VecSub(E1 const& u, E2 const& v) : u{u}, v{v} {}

    [[nodiscard]]
    Scalar operator[](size_t i) const noexcept
    {
        return u[i] - v[i];
    }
};

template <typename Scalar, size_t Dim, typename E1, typename E2>
VecSub<Scalar, Dim, E1, E2> operator-(Vector<Scalar, Dim, E1> const& u, Vector<Scalar, Dim, E2> const& v)
{
    return VecSub<Scalar, Dim, E1, E2>(*static_cast<const E1*>(&u), *static_cast<const E2*>(&v));
}

template <typename Scalar, size_t Dim, typename E>
class ScalarMul : public Vector<Scalar, Dim, ScalarMul<Scalar, Dim, E>>
{
   private:
    // cref if leaf, copy otherwise
    typename std::conditional<E::IS_LEAF, const E&, const E>::type v;
    Scalar scalar;

   public:
    static constexpr bool IS_LEAF = false;

    ScalarMul(E const& v, Scalar c) : v{v}, scalar{c} {}

    [[nodiscard]]
    Scalar operator[](size_t i) const noexcept
    {
        return scalar * v[i];
    }
};

template <typename Scalar, size_t Dim, typename E>
ScalarMul<Scalar, Dim, E> operator*(Vector<Scalar, Dim, E> const& v, Scalar c)
{
    return ScalarMul<Scalar, Dim, E>(*static_cast<const E*>(&v), c);
}

template <typename Scalar, size_t Dim, typename E>
ScalarMul<Scalar, Dim, E> operator*(Scalar c, Vector<Scalar, Dim, E> const& v)
{
    return ScalarMul<Scalar, Dim, E>(*static_cast<const E*>(&v), c);
}

template <typename Scalar, size_t Dim, typename E>
ScalarMul<Scalar, Dim, E> operator/(Vector<Scalar, Dim, E> const& v, Scalar c)
{
    return ScalarMul<Scalar, Dim, E>(*static_cast<const E*>(&v), 1 / c);
}

template <typename Scalar, size_t Dim, typename E1, typename E2>
Scalar dot(Vector<Scalar, Dim, E1> const& u, Vector<Scalar, Dim, E2> const& v)
{
    Scalar out = 0;
    for (size_t i = 0; i < Dim; ++i) {
        out += u[i] * v[i];
    }
    return out;
}

template <typename Scalar, size_t Dim, typename E>
Scalar norm(Vector<Scalar, Dim, E> const& u)
{
    Scalar out = dot(u, u);
    return std::sqrt(out);
}

}  // namespace linalg

#endif
