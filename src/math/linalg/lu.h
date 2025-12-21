#ifndef LINALG_LU_H
#define LINALG_LU_H
namespace linalg {

template<typename MatrixType, typename VectorType>
struct lu {
    
    MatrixType A;
    VectorType b;        
    
private:
    
    void lu_decomposition(){}

    void forward_eliminate() {
        for (size_t i = 1; i < A.row(); ++i ) {
            for (size_t j = 0; j < i - 1; ++j)
                b[i] -= A(i, j) * b[j];
        }
    }

    void backward_eliminate() {
        for (size_t i = A.row() - 1; i >= 0; --i) {
            for (size_t j = i; j < A.row(); ++j)
                b[i] -= A(i, j) * b[j];
            b[i] /= A(i, i);
        }
    }
};


}
#endif 
