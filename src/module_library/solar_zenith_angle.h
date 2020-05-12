#ifndef SOLAR_ZENITH_ANGLE_H
#define SOLAR_ZENITH_ANGLE_H

#include "../modules.h"
#include "AuxBioCro.h"

/**
 * @class solar_zenith_angle
 * 
 * @brief Calculates the solar zenith angle using a simple model. A major
 * shortcoming of this model is that solar noon always occurs at 12:00 PM.
 */
class solar_zenith_angle : public SteadyModule
{
   public:
    solar_zenith_angle(
        const std::unordered_map<std::string, double>* input_parameters,
        std::unordered_map<std::string, double>* output_parameters)
        :  // Define basic module properties by passing its name to its parent class
          SteadyModule("solar_zenith_angle"),
          // Get references to input parameters
          lat(get_input(input_parameters, "lat")),
          doy_dbl(get_input(input_parameters, "doy_dbl")),
          // Get pointers to output parameters
          cosine_zenith_angle_op(get_op(output_parameters, "cosine_zenith_angle"))
    {
    }
    static std::vector<std::string> get_inputs();
    static std::vector<std::string> get_outputs();
    static std::string get_description();

   private:
    // References to input parameters
    double const& lat;
    double const& doy_dbl;
    // Pointers to output parameters
    double* cosine_zenith_angle_op;
    // Main operation
    void do_operation() const;
};

std::vector<std::string> solar_zenith_angle::get_inputs()
{
    return {
        "lat",      // degrees (North is positive)
        "doy_dbl",  // time expressed as a fractional day of year
    };
}

std::vector<std::string> solar_zenith_angle::get_outputs()
{
    return {
        "cosine_zenith_angle"  // dimensionless
    };
}

void solar_zenith_angle::do_operation() const
{
    // Unpack the doy and hour
    const double doy = floor(doy_dbl);
    const double hour = 24.0 * (doy_dbl - doy);
    
    // Update the output pointers
    update(cosine_zenith_angle_op, cos_zenith_angle(lat, doy, hour));
}

#endif
