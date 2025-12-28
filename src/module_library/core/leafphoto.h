#ifndef LEAF_PHOTO_H
#define LEAF_PHOTO_H

#include <array>


struct Leaf {
  
    // solve for internal states
    
    // system of equations to solve
    std::array<double, 3> defining_function(std::array<double, 3> x){
        std::array<double, 3> y;
        y[0] = ci_balance_equation(ci, gs);
        y[1] = 
        y[2] =
    }     

    double ci_balance_equation(double Ci, double gs, double assim) {
        double Gs = gs / dr_stomata;  // mol / m^2 / s
        return Gs * (Cs - Ci) - assim;    
    }

    double cs_balance_equation(double Cs, double Ci, double gs) {
        double Gs = gs / dr_stomata;  // mol / m^2 / s
        double Gb = gbw / dr_stomata;
        return Gb * (Ca - Cs) - Gs * (Cs - Ci);    
    }

    double assim_equation(double assim, double Ci, double Tleaf){
    }

    double stomatal_equation(double Cs, double gs) {
            
        double g0 = bb_intercept + bb_slope * bb_index; 
        return g0 - gs;   
    }

    
      
    
};

#endif
