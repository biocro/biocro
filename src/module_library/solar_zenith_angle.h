#ifndef SOLAR_ZENITH_ANGLE_H
#define SOLAR_ZENITH_ANGLE_H

#include "../modules.h"
#include "../state_map.h"
#include "BioCro.h"  // For cos_zenith_angle

/**
 * @class solar_zenith_angle
 *
 * @brief Calculates the solar zenith angle using a simple model. A major
 * shortcoming of this model is that solar noon always occurs at 12:00 PM.
 */
class solar_zenith_angle : public direct_module
{
   public:
    solar_zenith_angle(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          direct_module("solar_zenith_angle"),
          // Get references to input quantities
          lat(get_input(input_quantities, "lat")),
          time(get_input(input_quantities, "time")),
          // Get pointers to output quantities
          cosine_zenith_angle_op(get_op(output_quantities, "cosine_zenith_angle"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // References to input quantities
    double const& lat;
    double const& time;
    // Pointers to output quantities
    double* cosine_zenith_angle_op;
    // Main operation
    void do_operation() const;
};

string_vector solar_zenith_angle::get_inputs()
{
    return {
        "lat",   // degrees (North is positive)
        "time",  // time expressed as a fractional day of year
    };
}

string_vector solar_zenith_angle::get_outputs()
{
    return {
        "cosine_zenith_angle"  // dimensionless
    };
}

void solar_zenith_angle::do_operation() const
{
    // Unpack the doy and hour
    const double doy = floor(time);
    const double hour = 24.0 * (time - doy);

    // Update the output pointers
    update(cosine_zenith_angle_op, cos_zenith_angle(lat, doy, hour));
}

#endif
