#ifndef DEVELOPMENT_INDEX_H
#define DEVELOPMENT_INDEX_H

#include "../modules.h"
#include "../state_map.h"

/**
 * \brief This module calculates the development index (DVI) corresponding to plant growth (Osborne et al., 2015) given the development_rate_per_hour as an input.
 *
 * Osborne et al., (2015) define DVI to be:
 *  -1 <= DVI < 0   : Sowing to Emergence
 *  0 <= DVI < 1    : Vegetative Growth Stages
 *  1 <= DVI < 2    : Reproductive Growth Stages
 *
 *  However, this definition is flexible. For example, for our soybean model (soybean_development_rate_calculator.h) we define the stages as:
 *  -1 <= DVI < 0 : Sowing to Emergence
 *  0 <= DVI < 1  :  Emergence to R1 (Flowering)
 *          0 <= DVI < 0.333        : Emergence to V0 (Cotyledon stage)
 *          0.333 <= DVI < 0.667 : V0 (Cotyledon stage) to R0 (End of Floral Induction)
 *          0.667 <= DVI < 1        : R0 (End of Floral Induction) to R1 (Flowering)
 *  1 <= DVI < 2 : R1 (Flowering) to R7 (Maturity)
 *
 * The DVI will continue accumulating above 2 if the crop is simulated to reach maturity before the final given timepoint.
 *
 * The DVI value can be used in any modules that are dependent on the plants growth stage, such as carbon partitioning or senescence. The modules partioning_coefficient_logistic and senescence_coefficient_logistic are two modules that use the DVI.
 *
 * This module takes the development_rate_per_hour variable as an input. For soybean, this rate is calculated in the module soybean_development_rate_calculator.
 *
 * References:
 * Osborne, T. et al. 2015. “JULES-Crop: A Parametrisation of Crops in the Joint UK Land Environment Simulator.” Geoscientific Model Development 8(4): 1139–55. https://doi.org/10.5194/gmd-8-1139-2015
 *
 */

class development_index : public DerivModule
{
public: development_index(
            const state_map* input_parameters,
            state_map* output_parameters
        )
    : DerivModule("development_index"),
    
    // Get references to input parameters
    development_rate_per_hour(get_input(input_parameters, "development_rate_per_hour")),
    
    // Get pointers to output parameters
    DVI_op(get_op(output_parameters, "DVI"))
    {}
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();
    
private:
    // References to input parameters
    const double& development_rate_per_hour;
    
    // Pointers to output parameters
    double* DVI_op;
    
    // Implement the pure virtual function do_operation():
    void do_operation() const override final;
};

string_vector development_index::get_inputs() {
    return {
        "development_rate_per_hour" // hour^-1
    };
}

string_vector development_index::get_outputs() {
    return {
        "DVI" // dimensionless
    };
}

void development_index::do_operation() const {
    // Update the output parameter list
    update(DVI_op, development_rate_per_hour); // dimensionless
}

#endif
