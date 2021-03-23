#ifndef SOYBEAN_DEVELOPMENT_RATE_CALCULATOR_H
#define SOYBEAN_DEVELOPMENT_RATE_CALCULATOR_H

#include "../modules.h"
#include "../state_map.h"
#include <cmath>

/**
 * \brief This module uses photothermal functions to determine the hourly development rate for soybean growth. 
 *
 * References:
 *  Setiyono, T.D. et al. 2007. “Understanding and Modeling the Effect of Temperature and Daylength on Soybean Phenology under High-Yield Conditions.” Field Crops Research 100(2–3): 257–71. https://doi.org/10.1016/j.fcr.2006.07.011
 *
 *  Osborne, T. et al. 2015. “JULES-Crop: A Parametrisation of Crops in the Joint UK Land Environment Simulator.” Geoscientific Model Development 8(4): 1139–55. https://doi.org/10.5194/gmd-8-1139-2015
 */

double photoFunc(double P, double Popt, double Pcrit);
double tempFunc(double T, double Tmin, double Topt, double Tmax);

class soybean_development_rate_calculator : public SteadyModule
{
public:
    soybean_development_rate_calculator(
        const state_map* input_parameters,
        state_map* output_parameters
    )
    // Define basic module properties by passing its name to its parent class
    : SteadyModule{"soybean_development_rate_calculator"},
    
    // Get pointers to input parameters
    maturity_group{get_input(input_parameters, "maturity_group")},
    DVI{get_input(input_parameters, "DVI")},
    day_length{get_input(input_parameters, "day_length")},
    temp{get_input(input_parameters, "temp")},
    Tbase_emr{get_input(input_parameters, "Tbase_emr")},
    TTemr_threshold{get_input(input_parameters, "TTemr_threshold")},
    Rmax_emrV0{get_input(input_parameters, "Rmax_emrV0")},
    Tmin_emrV0{get_input(input_parameters, "Tmin_emrV0")},
    Topt_emrV0{get_input(input_parameters, "Topt_emrV0")},
    Tmax_emrV0{get_input(input_parameters, "Tmax_emrV0")},
    Tmin_R0R1{get_input(input_parameters, "Tmin_R0R1")},
    Topt_R0R1{get_input(input_parameters, "Topt_R0R1")},
    Tmax_R0R1{get_input(input_parameters, "Tmax_R0R1")},
    Tmin_R1R7{get_input(input_parameters, "Tmin_R1R7")},
    Topt_R1R7{get_input(input_parameters, "Topt_R1R7")},
    Tmax_R1R7{get_input(input_parameters, "Tmax_R1R7")},
    
    // Get pointers to output parameters
    development_rate_per_hour_op{get_op(output_parameters, "development_rate_per_hour")}
    {}
    static string_vector get_inputs();
    static string_vector get_outputs();
    
    private:
    // Pointers to input parameters
    const double& maturity_group;
    const double& DVI;
    const double& day_length;
    const double& temp;
    const double& Tbase_emr;
    const double& TTemr_threshold;
    const double& Rmax_emrV0;
    const double& Tmin_emrV0;
    const double& Topt_emrV0;
    const double& Tmax_emrV0;
    const double& Tmin_R0R1;
    const double& Topt_R0R1;
    const double& Tmax_R0R1;
    const double& Tmin_R1R7;
    const double& Topt_R1R7;
    const double& Tmax_R1R7;
    
    // Pointers to output parameters
    double* development_rate_per_hour_op;
    
    // Main operation
    void do_operation() const override final;
    
};

string_vector soybean_development_rate_calculator::get_inputs() {
    return {
        "maturity_group",  // dimensionless; maturity group of soybean cultivar
        "DVI",             // dimensionless; development index, see Osborne et al. 2015
        "day_length",      // hours
        "temp",            // degrees C
        "Tbase_emr",       // degrees C; base temp for thermal time from sowing to emergence
        "TTemr_threshold", // degrees C * days; accumulated thermal time for emergence
        "Rmax_emrV0",      // day^-1
        "Tmin_emrV0",      // degrees C
        "Topt_emrV0",      // degrees C
        "Tmax_emrV0",      // degrees C
        "Tmin_R0R1",       // degrees C
        "Topt_R0R1",       // degrees C
        "Tmax_R0R1",       // degrees C
        "Tmin_R1R7",       // degrees C
        "Topt_R1R7",       // degrees C
        "Tmax_R1R7",       // degrees C
    };
}

string_vector soybean_development_rate_calculator::get_outputs() {
    return {
        "development_rate_per_hour" // hour^-1
    };
}

void soybean_development_rate_calculator::do_operation() const {
    double soybean_development_rate; // day^-1
    
    if (DVI >= -1 && DVI < 0) { // 1. Sowing to emergence
        
        double temp_diff = temp - Tbase_emr; // degrees C
        double soybean_development_rate = temp_diff / TTemr_threshold; // day^-1
        
    } else if (DVI >= 0 && DVI < 1) { // 2. Vegetative stages
        
        if (DVI < 0.333) { // 2a. Emr - V0; r = Rmax * f(T)
            
            double soybean_development_rate = Rmax_emrV0 * tempFunc(temp, Tmin_emrV0, Topt_emrV0, Tmax_emrV0) / 3.0; // day^-1
            
        } else if (DVI >= 0.333 && DVI < 0.667) { // 2b. V0 - R0; r = Rmax * f(P)
            
            const double Rmax_V0R0 = 0.0294 + 0.1561 / maturity_group; // day^-1; Setiyono et al., 2007, Fig. 12a
            const double Popt_V0R0 = 12.759 - 0.388 * maturity_group - 0.058 * pow(maturity_group,2); // hours; Setiyono et al., 2007, Fig 4
            const double Pcrit_V0R0 = 27.275 - 0.493 * maturity_group - 0.066 * pow(maturity_group,2); // hours; Setiyono et al., 2007, Fig 4
            double day_length_civil = 1.072 * day_length; // hours; converts day length calculated based on solar elevation = -0.83 degrees to civil day length (based on solar elevation angle = -6 degrees)
            double soybean_development_rate = Rmax_V0R0 * photoFunc(day_length_civil, Popt_V0R0, Pcrit_V0R0) / 3.0; // day^-1
        } else { // 2c. R0 - R1; r = Rmax * f(T)
            
            const double Rmax_R0R1 = (0.2551 + 0.0965 * maturity_group) / (1 + 2.1107 * maturity_group); // day^-1; Setiyono et al., 2007, Fig 11a
            double soybean_development_rate = Rmax_R0R1 * tempFunc(temp, Tmin_R0R1, Topt_R0R1, Tmax_R0R1) / 3.0; // day^-1
        }
    } else if (DVI >= 1) { // 3. Reproductive stages, R1 - R7; r = Rmax * f(T) * f(P)
        
        const double Rmax_R1R7 = (0.0563 + 0.0228 * maturity_group) / (1 + 1.9683 * maturity_group); // day^-1; Setiyono et al., 2007, Fig 11d
        const double Popt_R1R7 = 10.6595 + 2.9706 / maturity_group; // hrs; Setiyono et al., 2007, Fig 12c
        const double Pcrit_R1R7 = 16.1257 + 4.3143 / maturity_group; // hrs; Setiyono et al., 2007, Fig 12c
        
        double soybean_development_rate = Rmax_R1R7 * tempFunc(temp, Tmin_R1R7, Topt_R1R7, Tmax_R1R7) * photoFunc(day_length, Popt_R1R7, Pcrit_R1R7); // day^-1
        
    } else {
        // error, DVI out of bounds, this should never occur, but prevents warning messages when compiling biocro
        double soybean_development_rate = 0; //
    }
    
    double development_rate_per_hour = soybean_development_rate / 24.0; // hr^-1
    
    // Update the output parameter list
    update(development_rate_per_hour_op, (development_rate_per_hour > eps) ? development_rate_per_hour : 0);
}

double tempFunc(double T, double Tmin, double Topt, double Tmax) {
    double fT; // dimensionless
    
    if (T > Tmin && T < Tmax) {
        double alpha = log(2.0) / log((Tmax - Tmin) / (Topt - Tmin)); // dimensionless
        double fT_num_pt1 = 2.0 * pow(T - Tmin, alpha) * pow(Topt - Tmin, alpha); // dimensionless
        double fT_num_pt2 = pow(T - Tmin, 2.0 * alpha); // dimensionless
        double fT_denom = pow(Topt - Tmin, 2.0 * alpha); // dimensionless
        
        fT = (fT_num_pt1 - fT_num_pt2) / fT_denom; // dimensionless
    } else {
        fT = 0.0; // dimensionless
    }
    
    return fT; // dimensionless
    
}

double photoFunc(double P, double Popt, double Pcrit) {
    double m = 3.0; // hrs; Setiyono et al., 2007 Equation 4, value in text below equation
    double fP; // dimensionless
    
    if (P >= Popt && P <= Pcrit) {
        double alpha = log(2.0) / log(1.0 + (Pcrit - Popt) / m); // dimensionless
        double fP_pt1 = 1.0 + (P - Popt) / m; // dimensionless
        double fP_pt2 = (Pcrit - P) / (Pcrit - Popt); // dimensionless
        double fP_pt3 = (Pcrit - Popt) / m; // dimensionless
        
        fP = pow(fP_pt1 * pow(fP_pt2, fP_pt3), alpha); // dimensionless
        
    } else if (P < Popt) {
        fP = 1.0; // dimensionless
    } else {
        fP = 0.0; // dimensionless
    }
    
    return fP; // dimensionless
    
}

#endif


