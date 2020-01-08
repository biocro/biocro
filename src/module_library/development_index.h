

#ifndef DEVELOPMENT_INDEX_h
#define DEVELOPMENT_INDEX_h

#include "../modules.h"

/**
 * \brief This module should be used in conjunction with the soybean_growth_stages_calculator module. The soybean_development_rate_per_hour calculated in that module is used in this module to update the development index (DVI) describing soybean growth. The DVI can be used to determine how carbon is partitioned between the different plant parts and when senescence starts (see partitioning_coefficient_logistic and senescence_coefficient_logistic modules).
 *
 * We define the DVI similarly to how its defined in Osborne et al., 2015, with:
 *  -1 <= DVI < 0  :  period between sowing and soybean emergence
 *  0 <= DVI < 1 : emergence to beginning of flowering (R1)
 *  1 <= DVI < 2 : the reproductive stages flowering (R1) to maturity (R7)
 *
 * The DVI is allowed to continue accumulating above 2, indicating the crop has reached maturity before the final given timepoint.
 *
 * For more information on how the DVI and soybean_development_rate_per_hour are calculated, see soybean_growth_stages_calculator.h.
 *
 *  References:
 *  Osborne, T. et al. 2015. “JULES-Crop: A Parametrisation of Crops in the Joint UK Land Environment Simulator.” Geoscientific Model Development 8(4): 1139–55. https://doi.org/10.5194/gmd-8-1139-2015
 *
 */

class development_index : public DerivModule
{
public: development_index( const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
    
    // Define basic module properties by passing its name to its parent class
    DerivModule("development_index"),
    
    // Get pointers to input parameters
    development_rate_per_hour_ip(get_ip(input_parameters,"development_rate_per_hour")),
    
    // Get pointers to output parameters
    DVI_op(get_op(output_parameters,"DVI"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();
    
private:
    // Pointers to input parameters
    const double* development_rate_per_hour_ip;
    
    // Pointers to output parameters
    double* DVI_op;
    
    // Main operation
    void do_operation() const;
};

std::vector<std::string> development_index::get_inputs()
{
    return {
        "development_rate_per_hour"
    };
}

std::vector<std::string> development_index::get_outputs()
{
    return {
        "DVI"
    };
}

void development_index::do_operation() const
{
    // Collect inputs
    const double development_rate_per_hour = *development_rate_per_hour_ip; // hr^-1; development rate per hour
    
    // Update the output parameter list
    update(DVI_op, development_rate_per_hour); // dimensionless
}

#endif
