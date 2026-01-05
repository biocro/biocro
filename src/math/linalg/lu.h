#ifndef LINALG_LU_H
#define LINALG_LU_H
#include <limits>
#include <optional>
#include "vector.h" //Vector
#include "matrix.h" //Matrix
#include "base.h"  // vector, matrix


namespace linalg {


template<typename Scalar, size_t Dim>
struct LU {
    
    
    LU(matrix<Scalar, Dim, Dim> const& A): 
        A{A}
    {
    }
    
    LU(matrix<Scalar, Dim, Dim> const& A, Scalar tol): 
        A{A}, tol{tol} 
    {
                    
    }    
    
    
    std::optional<vector<Scalar, Dim>> solve(vector<Scalar, Dim> const& b) {
        if (factor())
            return solve_lu_triangular(b);
        return {};
    }
    
    
    vector<Scalar, Dim> unsafe_solve(vector<Scalar, Dim> const& b) {
        factor();
        return solve_lu_triangular(b);
    }    
        
    void set_A(matrix<Scalar, Dim, Dim> const& A_new) {
        A = A_new;
        factored = false;
    };
    
    void set_A(matrix<Scalar, Dim, Dim> const&& A_new) {
        A = A_new;
        factored = false;
    };      
          
        
    matrix<Scalar, Dim, Dim> A;
    std::array<size_t, Dim + 1> P;
    bool factored = false;
    Scalar tol = 5 * std::numeric_limits<Scalar>::epsilon(); 
private:
         
    void reset_permutation() {
        for (size_t i = 0; i < Dim; ++i)
            P[i] = i;
        P[Dim] = 0;
    }
 
    bool factor() {
        if (factored) {
            return true;
        }
        Scalar maxA, absA;
        size_t imax;    
        reset_permutation();
    
        for (size_t i = 0; i < Dim; ++i){
            maxA = 0;
            imax = i;
            // find pivot
            for (size_t k = i; k < Dim; ++k) {
                if ((absA = std::abs(A(k, i))) > maxA) {
                    maxA = absA;
                    imax = k;
                }
            }   
                     
            if (maxA < tol) return factored = false; // singular matrix

            if (imax != i) {
                // pivoting P 
                std::swap(P[i], P[imax]);
                // count pivots
                ++P[Dim];

                // swap row i and imax;
                A.swap_row(i, imax);
            }        
            
            for (size_t j = i + 1; j < Dim; ++ j ){    
                A(j, i) /= A(i, i);
                for (size_t k = i + 1; k < Dim; ++k) {
                    A(j, k) -= A(j, i) * A(i, k);
                }
            }
        }
        
        return (factored = true);
    }

    
    vector<Scalar, Dim> solve_lu_triangular(vector<Scalar, Dim> const& b) {
        vector<Scalar, Dim> x;
        
        // solve L * x = b
        for (size_t i = 0; i < Dim; ++i ) {
            x[i] = b[P[i]];
            for (size_t j = 0; j < i; ++j)
                x[i] -= A(i, j) * x[j];
        }
        // solve U * x  = b
        for (size_t i = Dim - 1; i < Dim; --i) {
            for (size_t j = i + 1; j < Dim; ++j)
                x[i] -= A(i, j) * x[j];
            x[i] /= A(i, i); 
        }

        return x;
    }

    
};


}
#endif 
