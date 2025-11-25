#ifndef LEAF_PHOTO_H
#define LEAF_PHOTO_H

#include <array>


struct Leaf {
  
    // solve for internal states
    void solve();
    
    // system of equations to solve
    std::array<double, 3> defining_function(std::array<double, 3> x){
        std::array<double, 3> y;
        y[0] = ci_balance_equation();
        y[1] = 
        y[2] =
    }     

    double ci_balance_equation(double ci, double gs) {
        double Gt = sequential_conductance(gbw / dr_boundary, gs / dr_stomata);  // mol / m^2 / s
        double assim = FvCB;
        return Gt * (Ca - Ci) - assim;    
    }

    double stomatal_equation(double gs) {
        
        double g0 = bb_intercept + bb_slope * bb_index; 
        return gs - g0;   
    }

    
      
    
};

#endif
