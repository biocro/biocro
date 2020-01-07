

#ifndef SOYBEAN_GROWTH_STAGES_h
#define SOYBEAN_GROWTH_STAGES_h

#include "../modules.h"

/**
 * \brief
 *
 *  References:
 *  Setiyono, T.D. et al. 2007. “Understanding and Modeling the Effect of Temperature and Daylength on Soybean Phenology under High-Yield Conditions.” Field Crops Research 100(2–3): 257–71.
 *  Osborne, T. et al. 2015. “JULES-Crop: A Parametrisation of Crops in the Joint UK Land Environment Simulator.” Geoscientific Model Development 8(4): 1139–55.


 */

class soybean_growth_stages : public DerivModule
{
public: soybean_growth_stages( const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
    
    // Define basic module properties by passing its name to its parent class
    DerivModule("soybean_growth_stages"),
    
    // Get pointers to input parameters
    soybean_development_rate_per_hour_ip(get_ip(input_parameters,"soybean_development_rate_per_hour")),
    
    // Get pointers to output parameters
    DVI_op(get_op(output_parameters,"DVI"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();
    
private:
    // Pointers to input parameters
    const double* soybean_development_rate_per_hour_ip;
    
    // Pointers to output parameters
    double* DVI_op;
    
    // Main operation
    void do_operation() const;
};

std::vector<std::string> soybean_growth_stages::get_inputs()
{
    return {
        "soybean_development_rate_per_hour"
    };
}

std::vector<std::string> soybean_growth_stages::get_outputs()
{
    return {
        "DVI"
    };
}

void soybean_growth_stages::do_operation() const
{
    // Collect inputs
    const double soybean_development_rate_per_hour = *soybean_development_rate_per_hour_ip; // hr^-1; development rate per hour
    
    // Update the output parameter list
    update(DVI_op, soybean_development_rate_per_hour); // dimensionless
}

#endif
