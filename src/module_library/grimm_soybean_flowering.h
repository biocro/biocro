#ifndef GRIMM_SOYBEAN_FLOWERING_H
#define GRIMM_SOYBEAN_FLOWERING_H

#include "../modules.h"

/**
 * \brief Model for soybean development and flowering based on Grimm et al. (1993). See grimm_soybean_flowering_calculator for details.
 */
class grimm_soybean_flowering : public DerivModule
{
   public:
    grimm_soybean_flowering(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters) :  // Define basic module properties by passing its name to its parent class
                                                                      DerivModule("grimm_soybean_flowering"),
                                                                      // Get pointers to input parameters
                                                                      grimm_rate_ip(get_ip(input_parameters, "grimm_rate")),
                                                                      // Get pointers to output parameters
                                                                      grimm_physiological_age_op(get_op(output_parameters, "grimm_physiological_age"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();

   private:
    // Pointers to input parameters
    const double* grimm_rate_ip;
    // Pointers to output parameters
    double* grimm_physiological_age_op;
    // Main operation
    void do_operation() const;
};

std::vector<std::string> grimm_soybean_flowering::get_inputs()
{
    return {
        "grimm_rate"};
}

std::vector<std::string> grimm_soybean_flowering::get_outputs()
{
    return {
        "grimm_physiological_age"};
}

void grimm_soybean_flowering::do_operation() const
{
    // Collect inputs
    const double grimm_rate = *grimm_rate_ip;  // physiological days per hour

    // Update the output parameter list
    update(grimm_physiological_age_op, grimm_rate);
}

#endif
