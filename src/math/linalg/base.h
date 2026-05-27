#ifndef LINALG_BASE_H
#define LINALG_BASE_H

#include <vector>
#include <array>
#include <algorithm>  // std::min;
#include <stdexcept>
#include "vector.h"  // Vector
#include "matrix.h"  // Matrix

namespace linalg
{
// Declare Concrete Types for storage.
template <typename Scalar, size_t Dim>
struct vector : Vector<Scalar, Dim, vector<Scalar, Dim>> {
    static constexpr bool IS_LEAF = true;
    using container = typename std::array<Scalar, Dim>;
    using iterator = typename container::iterator;
    using const_iterator = typename container::const_iterator;

    [[nodiscard]]
    double operator[](size_t i) const noexcept
    {
        return data[i];
    }

    double& operator[](size_t i) noexcept
    {
        return data[i];
    }

    template <typename E>
    vector(Vector<Scalar, Dim, E> const& expr)
    {
        for (size_t i = 0; i < Dim; ++i) {
            data[i] = expr[i];
        }
    }

    vector() : data{} {}
    vector(std::initializer_list<Scalar> ilist) : data{}
    {
        if (ilist.size() != Dim)
            throw std::logic_error(
                "Initializer list does not contain the correct number of elements for vector dimension.");
        std::copy(ilist.begin(), ilist.end(), data.begin());
    }

    vector(container const& arr) : data{arr} {}
    //   vector(const& vector v) : data{} {std::copy(data.begin(), data.end(), v.data.begin());}

    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    const_iterator cbegin() const { return data.cbegin(); }
    const_iterator cend() const { return data.cend(); }

    std::array<Scalar, Dim> asarray() const { return data; }

    //    vector operator-() const;

    template <typename E>
    vector& operator+=(Vector<Scalar, Dim, E> const& expr)
    {
        for (size_t i = 0; i < Dim; ++i)
            data[i] += expr[i];
        return *this;
    }

    template <typename E>
    vector& operator-=(Vector<Scalar, Dim, E> const& expr)
    {
        for (size_t i = 0; i < Dim; ++i)
            data[i] -= expr[i];
        return *this;
    }

    vector& operator*=(Scalar expr)
    {
        for (size_t i = 0; i < Dim; ++i)
            data[i] *= expr;
        return *this;
    }

    vector& operator/=(Scalar expr)
    {
        for (size_t i = 0; i < Dim; ++i)
            data[i] /= expr;
        return *this;
    }

   private:
    container data;
};

template <typename Scalar, size_t Row, size_t Col>
struct matrix : Matrix<Scalar, Row, Col, matrix<Scalar, Row, Col>> {
    using container = std::array<Scalar, Col * Row>;
    static constexpr bool IS_LEAF = true;

    inline size_t offset(size_t i, size_t j) const {
        if (i >= Row || j >= Col) {
            throw std::out_of_range("matrix index out of bounds");
        }
        return i * Col + j;
    }

    const Scalar& operator()(size_t i, size_t j) const
    {
        return data[ offset(i , j)];
    }

    Scalar& operator()(size_t i, size_t j)
    {
        return data[offset(i , j)];
    }


    matrix() = default;

    matrix(std::initializer_list<Scalar> ilist) : data{}
    {
        if (ilist.size() != Row * Col)
            throw std::logic_error(
                "Initializer list does not contain the correct number of elements for matrix dimension.");
        auto it = ilist.begin();
        for (size_t i = 0; i < Row; ++i) {
            for (size_t j = 0; j < Col; ++j) {
                data[offset(i,j)] = *it;
                ++it;
            }
        }
    }

    matrix(container const& arr) : data{arr} {}

    static matrix identity()
    {
        matrix out;
        for (size_t i = 0; i < Row; ++i) {
            for (size_t j = 0; j < Col; ++j) {
                out(i, j) = i == j ? 1 : 0;
            }
        }
        return out;
    }

    template <typename E>
    matrix& operator+=(Matrix<Scalar, Row, Col, E> const& expr)
    {
        for (size_t i = 0; i < Row; ++i) {
            for (size_t j = 0; j < Col; ++j)
                operator()(i, j) += expr(i, j);
        }
        return *this;
    }

    template <typename E>
    matrix& operator-=(Matrix<Scalar, Row, Col, E> const& expr)
    {
        for (size_t i = 0; i < Row; ++i) {
            for (size_t j = 0; j < Col; ++j)
                operator()(i, j) -= expr(i, j);
        }
        return *this;
    }

    matrix& operator*=(Scalar expr)
    {
        for (size_t i = 0; i < Row; ++i) {
            for (size_t j = 0; j < Col; ++j)
                operator()(i, j) *= expr;
        }
        return *this;
    }

    matrix& operator/=(Scalar expr)
    {
        for (size_t i = 0; i < Row; ++i) {
            for (size_t j = 0; j < Col; ++j)
                operator()(i, j) /= expr;
        }

        return *this;
    }

    matrix& swap_row(size_t i, size_t j)
    {
        for (size_t k = 0; k < Col; ++k)
            std::swap(data[offset(i,k)], data[offset(j,k)]);

        return *this;
    }

    matrix& swap_col(size_t i, size_t j)
    {
        for (size_t k = 0; k < Row; ++k)
            std::swap(data[offset(k,i)], data[offset(k,j)]);
        return *this;
    }

   private:
    container data;
};
}  // namespace linalg

#endif
