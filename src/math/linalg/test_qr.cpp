#include "base.h"
#include <iostream>

/*
This source file is not intended for long term usage

It is intended to prototype the code in the linalg library
to permit faster modifications, rathe than recompiling biocro.

*/

template<typename Matrix>
void print(Matrix const& mat) {
    for (size_t i =0; i < mat.nrow(); ++i){
        for ( size_t j = 0; j < mat.ncol(); ++j) {
            std::cout << mat(i, j) << ' ';
        }
        std::cout << '\n';

    }
}



int main() {
    std::cout << "Running QR test program...\n";
    linalg::matrix<double, 3, 3> mat = {{12, -51, 4, 6, 167, -68, -4, 24, -41}};
    print(mat);

    double qraux = 1.8571429;
    linalg::vector<double, 3> v = {1.8571429,  0.4285714, -0.2857143};
    for (size_t col = 0; col < v.dim(); ++col) {
        double vtc = 0;
        for(size_t row = 0; row < mat.nrow(); ++row) {
            vtc += v[row] * mat(row, col);
        }
        std::cout << "col " << col << ' ' << vtc << '\n';
        double scal = vtc /qraux;
        for(size_t row = 0; row < mat.nrow(); ++row) {
            mat(row, col) -= scal * v[row] ;
        }

    }

    print(mat);





}
