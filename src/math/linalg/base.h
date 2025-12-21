#ifndef LINALG_BASE_H
#define LINALG_BASE_H

#include <vector>
#include <array>

namespace linalg {

// Define common Vector and Matrix Expression intefaces
template<E>
struct Vector {
    static constexpr bool IS_LEAF = false;

    [[nodiscard]]
    double operator[](size_t i) const {
        // Delegation to the actual expression type.
        // This avoids dynamic polymorphism (a.k.a. virtual functions in C++)
        return static_cast<E const&>(*this)[i];
    }

    [[nodiscard]]
    size_t dim() const { 
        return static_cast<E const&>(*this).dim(); 
    }

};

template<E>
struct Matrix {
    
    static constexpr bool IS_LEAF = false;

    [[nodiscard]]
    double operator()(size_t i, size_t j) const {
        // Delegation to the actual expression type.
        // This avoids dynamic polymorphism (a.k.a. virtual functions in C++)
        return static_cast<E const&>(*this)(i, j);
    }

    [[nodiscard]]
    size_t row() const { 
        return static_cast<E const&>(*this).row(); 
    }    

    [[nodiscard]]
    size_t col() const { 
        return static_cast<E const&>(*this).col(); 
    }
}

// Declare Concrete Types for storage.

//struct vector {
//    using container = std::vector<double>;
//    
//    size_t dim() const;  
//private:
//    container data;
//    
//};
//
//struct dynamic_matrix {
//    using container = std::vector<double>;
//    
//    static matrix zeros(size_t row, size_t col) {
//        return matrix(row, col);
//    }
//
//    const double& operator()(size_t i, size_t j) const {
//        return data[col_dim * i + j];
//    }
//    
//    double& operator()(size_t i, size_t j) {
//        return data[col_dim * i + j];
//    }
//
//    
//    size_t col() const {return Row;};
//    size_t row() const {return Col;};
//private:
//    matrix(size_t row, size_t col) : Row{row}, Col{col}, data(Row * Col, 0) {}
//    size_t Row;
//    size_t Col;
//    
//    container data;
//    
//};
//
//
//template<size_t Row, size_t Col>
//struct matrix {
//    using container = std::array<double, Row * Col>;
//    
//    
//    
//    const double& operator()(size_t i, size_t j) const {
//        return data[Col * i + j];
//    }
//    
//    double& operator()(size_t i, size_t j) {
//        return data[Col * i + j];
//    }
//
//    
//    size_t col() const {return Row;};
//    size_t row() const {return Col;};
//private:
//    
//    container data;
//    
//};
//
//


}

#endif 
