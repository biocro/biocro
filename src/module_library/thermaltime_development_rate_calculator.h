#ifndef THERMALTIME_DEVELOPMENT_RATE_CALCULATOR_h
#define THERMALTIME_DEVELOPMENT_RATE_CALCULATOR_h

#include "../modules.h"
#include <cmath>

/**
 * \brief This module uses photothermal functions to determine the hourly development rate based on Thermal Time ranges from Osborne et al., 2015
 *
 * References:
 *
 *
 *  Osborne, T. et al. 2015. “JULES-Crop: A Parametrisation of Crops in the Joint UK Land Environment Simulator.” Geoscientific Model Development 8(4): 1139–55. https://doi.org/10.5194/gmd-8-1139-2015
 */

class thermaltime_development_rate_calculator : public SteadyModule
{
public: thermaltime_development_rate_calculator( const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
    
    // Define basic module properties by passing its name to its parent class
    SteadyModule("thermaltime_development_rate_calculator"),
    
    // Get pointers to input parameters
    DVI_ip(get_ip(input_parameters,"DVI")),
    temp_ip(get_ip(input_parameters,"temp")),
    tbase_ip(get_ip(input_parameters,"tbase")),
    TTemr_ip(get_ip(input_parameters,"TTemr")),
    TTveg_ip(get_ip(input_parameters, "TTveg")),
    TTrep_ip(get_ip(input_parameters, "TTrep")),
    
    // Get pointers to output parameters
  development_rate_per_hour_op(get_op(output_parameters,"development_rate_per_hour"))
    
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();
    
    private:
    // Pointers to input parameters
    const double* DVI_ip;
    const double* temp_ip;
    const double* tbase_ip;
    const double* TTemr_ip;
    const double* TTveg_ip;
    const double* TTrep_ip;
    
    // Pointers to output parameters
    double* development_rate_per_hour_op;
    
    // Main operation
    void do_operation() const;
    
};

std::vector<std::string> thermaltime_development_rate_calculator::get_inputs()
{
    return {
        "DVI",
        "temp",
        "tbase",
        "TTemr",
        "TTveg",
        "TTrep"
    };
}

std::vector<std::string> thermaltime_development_rate_calculator::get_outputs()
{
    return {
        "development_rate_per_hour"
    };
}

void thermaltime_development_rate_calculator::do_operation() const
{
    // Gather parameters not specific to growth stages
    const double DVI = *DVI_ip; // dimensionless; development index, see Osborne et al., 2015, and notes above.
    const double temp = *temp_ip; // degrees C
    const double tbase = *tbase_ip; // degrees C
    const double TTemr = *TTemr_ip; // degrees C * days; thermal time from sowing to emergence
    const double TTveg = *TTveg_ip; // degrees C * days; thermal time from emergence to end of vegetative stages (or beginning of reproductive)
    const double TTrep = *TTrep_ip; // degrees C * days; thermal time of reproductive stages
    
    // Calculate the development_rate
    double development_rate; // day^-1
    double temp_diff = temp - tbase; // degrees C
    
    if (DVI >= -1 && DVI < 0) {
        // 1. Sowing to emergence
        development_rate = temp_diff / TTemr; // day^-1
        
    } else if (DVI >= 0 && DVI < 1) {
        // 2. Vegetative stages
        development_rate = temp_diff / TTveg; // day^-1
        
    } else if (DVI >= 1) {
        // 3. Reproductive Stages
        development_rate = temp_diff / TTrep; // day^-1
        
    } else {
        // this should never occur, but prevents warning messages when compiling biocro
        development_rate = 0;
        // error, DVI out of bounds
//        throw std::out_of_range(std::string("DVI not in range, thrown by thermaltime_development_rate_calculator.\n"));
    }
    
    double development_rate_per_hour = development_rate / 24.0; // hr^-1
    
    // Update the output parameter list
    update(development_rate_per_hour_op, (development_rate_per_hour > eps) ? development_rate_per_hour : 0);
}

#endif


