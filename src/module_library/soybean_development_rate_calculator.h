#ifndef SOYBEAN_DEVELOPMENT_RATE_CALCULATOR_h
#define SOYBEAN_DEVELOPMENT_RATE_CALCULATOR_h

#include "../modules.h"
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
public: soybean_development_rate_calculator( const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
    
    // Define basic module properties by passing its name to its parent class
    SteadyModule("soybean_development_rate_calculator"),
    
    // Get pointers to input parameters
    maturity_group_ip(get_ip(input_parameters,"maturity_group")),
    DVI_ip(get_ip(input_parameters,"DVI")),
    day_length_ip(get_ip(input_parameters,"day_length")),
    temp_ip(get_ip(input_parameters,"temp")),
    Tbase_emr_ip(get_ip(input_parameters,"Tbase_emr")),
    TTemr_threshold_ip(get_ip(input_parameters,"TTemr_threshold")),
    Rmax_emrV0_ip(get_ip(input_parameters,"Rmax_emrV0")),
    Tmin_emrV0_ip(get_ip(input_parameters,"Tmin_emrV0")),
    Topt_emrV0_ip(get_ip(input_parameters,"Topt_emrV0")),
    Tmax_emrV0_ip(get_ip(input_parameters,"Tmax_emrV0")),
    Tmin_R0R1_ip(get_ip(input_parameters,"Tmin_R0R1")),
    Topt_R0R1_ip(get_ip(input_parameters,"Topt_R0R1")),
    Tmax_R0R1_ip(get_ip(input_parameters,"Tmax_R0R1")),
    Tmin_R1R7_ip(get_ip(input_parameters,"Tmin_R1R7")),
    Topt_R1R7_ip(get_ip(input_parameters,"Topt_R1R7")),
    Tmax_R1R7_ip(get_ip(input_parameters,"Tmax_R1R7")),
    
    // Get pointers to output parameters
  development_rate_per_hour_op(get_op(output_parameters,"development_rate_per_hour"))
    
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();
    
    private:
    // Pointers to input parameters
    const double* maturity_group_ip;
    const double* DVI_ip;
    const double* day_length_ip;
    const double* temp_ip;
    const double* Tbase_emr_ip;
    const double* TTemr_threshold_ip;
    const double* Rmax_emrV0_ip;
    const double* Tmin_emrV0_ip;
    const double* Topt_emrV0_ip;
    const double* Tmax_emrV0_ip;
    const double* Tmin_R0R1_ip;
    const double* Topt_R0R1_ip;
    const double* Tmax_R0R1_ip;
    const double* Tmin_R1R7_ip;
    const double* Topt_R1R7_ip;
    const double* Tmax_R1R7_ip;
    
    // Pointers to output parameters
    double* development_rate_per_hour_op;
    
    // Main operation
    void do_operation() const;
    
};

std::vector<std::string> soybean_development_rate_calculator::get_inputs()
{
    return {
        "maturity_group",
        "DVI",
        "day_length",
        "temp",
        "Tbase_emr",
        "TTemr_threshold",
        "Rmax_emrV0",
        "Tmin_emrV0",
        "Topt_emrV0",
        "Tmax_emrV0",
        "Tmin_R0R1",
        "Topt_R0R1",
        "Tmax_R0R1",
        "Tmin_R1R7",
        "Topt_R1R7",
        "Tmax_R1R7",
    };
}

std::vector<std::string> soybean_development_rate_calculator::get_outputs()
{
    return {
        "development_rate_per_hour"
    };
}

void soybean_development_rate_calculator::do_operation() const
{
    // Gather parameters not specific to growth stages
    const double maturity_group = *maturity_group_ip; // dimensionless; maturity group of soybean cultivar
    const double DVI = *DVI_ip; // dimensionless; development index, see Osborne et al., 2015, and notes above.
    const double day_length = *day_length_ip; // hrs
    const double temp = *temp_ip; // degrees C
    
    // Gather and calculate parameters for the different growth stages
    // 1. Sowing to Emergence
    const double Tbase_emr = *Tbase_emr_ip; // degrees C; base temperature for thermal time from sowing to emergence
    const double TTemr_threshold = *TTemr_threshold_ip; // degrees C * days; accumulated thermal time for emergence
    
    // 2. Emergence to R1 (flowering)
    // 2a. Emergence to V0 (cotyledon stage)
    const double Rmax_emrV0 = *Rmax_emrV0_ip; // day^-1
    const double Tmin_emrV0 = *Tmin_emrV0_ip; // degrees C
    const double Topt_emrV0 = *Topt_emrV0_ip; // degrees C
    const double Tmax_emrV0 = *Tmax_emrV0_ip; // degrees C
    
    // 2b. V0 (cotyledon stage) to R0 (end of floral induction)
    const double Rmax_V0R0 = 0.0294 + 0.1561 / maturity_group; // day^-1; Setiyono et al., 2007, Fig. 12a
    const double Popt_V0R0 = 12.759 - 0.388 * maturity_group - 0.058 * pow(maturity_group,2); // hrs; Setiyono et al., 2007, Fig 4
    const double Pcrit_V0R0 = 27.275 - 0.493 * maturity_group - 0.066 * pow(maturity_group,2); // hrs; Setiyono et al., 2007, Fig 4
    
    // 2c. R0 (end of floral induction) to R1 (flowering)
    const double Tmin_R0R1 = *Tmin_R0R1_ip; // degrees C
    const double Topt_R0R1 = *Topt_R0R1_ip; // degrees C
    const double Tmax_R0R1 = *Tmax_R0R1_ip; // degrees C
    
    const double Rmax_R0R1 = (0.2551 + 0.0965 * maturity_group) / (1 + 2.1107 * maturity_group); // day^-1; Setiyono et al., 2007, Fig 11a
    
    // 3. R1 (flowering) to R7 (maturity)
    const double Tmin_R1R7 = *Tmin_R1R7_ip; // degrees C
    const double Topt_R1R7 = *Topt_R1R7_ip; // degrees C
    const double Tmax_R1R7 = *Tmax_R1R7_ip; // degrees C
    
    const double Rmax_R1R7 = (0.0563 + 0.0228 * maturity_group) / (1 + 1.9683 * maturity_group); // day^-1; Setiyono et al., 2007, Fig 11d
    const double Popt_R1R7 = 10.6595 + 2.9706 / maturity_group; // hrs; Setiyono et al., 2007, Fig 12c
    const double Pcrit_R1R7 = 16.1257 + 4.3143 / maturity_group; // hrs; Setiyono et al., 2007, Fig 12c
    
    // Calculate the soybean_development_rate
    double soybean_development_rate; // day^-1
    
    if (DVI >= -1 && DVI < 0) {
        // 1. Sowing to emergence
        double temp_diff = temp - Tbase_emr; // degrees C
        soybean_development_rate = temp_diff / TTemr_threshold; // day^-1
        
    } else if (DVI >= 0 && DVI < 1) {
        // 2. Vegetative stages
        if (DVI < 0.333) {
            // 2a. Emr - V0; r = Rmax * f(T)
            soybean_development_rate = Rmax_emrV0 * tempFunc(temp, Tmin_emrV0, Topt_emrV0, Tmax_emrV0) / 3.0; // day^-1
        } else if (DVI >= 0.333 && DVI < 0.667) {
            // 2b. V0 - R0; r = Rmax * f(P)
            double day_length_civil = 1.072 * day_length; // hrs; converts day length calculated based on solar elevation = -0.83 degrees to civil day length (based on solar elevation angle = -6 degrees)
            soybean_development_rate = Rmax_V0R0 * photoFunc(day_length_civil, Popt_V0R0, Pcrit_V0R0) / 3.0; // day^-1
        } else {
            // 2c. R0 - R1; r = Rmax * f(T)
            soybean_development_rate = Rmax_R0R1 * tempFunc(temp, Tmin_R0R1, Topt_R0R1, Tmax_R0R1) / 3.0; // day^-1
        }
    } else if (DVI >= 1) {
        // 3. Reproductive stages, R1 - R7; r = Rmax * f(T) * f(P)
        soybean_development_rate = Rmax_R1R7 * tempFunc(temp, Tmin_R1R7, Topt_R1R7, Tmax_R1R7) * photoFunc(day_length, Popt_R1R7, Pcrit_R1R7); // day^-1
        
    } else {
        // error, DVI out of bounds, this should never occur, but prevents warning messages when compiling biocro
        soybean_development_rate = 0; //
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


