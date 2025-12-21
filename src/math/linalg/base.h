#ifndef LINALG_BASE_H
#define LINALG_BASE_H

#include <vector>

namespace linalg {

struct vector {
    using container = std::vector<double>;
    
    size_t dim() const;  
private:
    container data;
    
};

struct matrix {
    using container = std::vector<double>;
    
    static matrix zeros(size_t row, size_t col) {
        return matrix(row, col);
    }

    const double& operator()(size_t i, size_t j) const {
        return data[col_dim * i + j];
    }
    
    double& operator()(size_t i, size_t j) {
        return data[col_dim * i + j];
    }

    
    size_t ncol() const {return row_dim;};
    size_t nrow() const {return col_dim;};
private:
    matrix(size_t row, size_t col) : row_dim{row}, col_dim{col}, data(row * col, 0) {}
    size_t row_dim;
    size_t col_dim;
    
    container data;
    
};



}

#endif 
