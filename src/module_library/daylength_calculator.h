#ifndef DAYLENGTH_CALCULATOR_H
#define DAYLENGTH_CALCULATOR_H

#include <cmath>      // cos, sin
#include <algorithm>  // min, max
#include "../framework/module.h"
#include "../framework/state_map.h"
#include "../framework/constants.h"  //pi

namespace standardBML
{

/**
 * @class daylength_calculator
 *
 * @brief Computes day_length using a model described by Forsythe et al. (1995).
 *
 * Using equation (3) from the paper, the day length \f$D\f$ in hours is
 *
 * \f[ D = 24 - \frac{24}{\pi} \cos^{-1} \left( \frac{\sin \rho + \sin \lambda \sin \phi}{ \cos \lambda \cos \phi} \right) \f]
 *
 * where \f$\phi = \delta \pi / 180\f$ is the solar declination in radians (
 * typically computed elsewhere such as by the `solar_position_michalsky` module)
 * and \f$\lambda = \delta \pi / 180\f$ is the latitude on Earth in radians. As
 * described in the paper, sunrise and sunset have different definitions yielding
 * different day lengths. The parameter `sunrise_sunset_elevation` denoted \f$p\f$
 * sets the solar elevation (degrees) at which the sun rises and sets. `p = 0`
 * means the horizon.
 *
 * Table 1: Daylength definitions defined by the position of the sun with respect to the horizon
 * | | Daylength definition |  P |
 * | :--- |:--- | ---: |
 * | (with and without twilight) | (degrees) |
 * | 1 | Sunrise/Sunset (center at horizon) |  0.0 |
 * | 2 | Sunrise/Sunset (top/bottom at horizon)  | 0.26667 |
 * | 3 | Sunrise/Sunset (apparent top/bottom, US government definition)  | 0.8333 (a) |
 * | 4 | With civil twilight | 6.0 |
 * | 5 | With nautical twilight | 12.0 |
 * | 6 | With astronomical twilight | 18.0 |
 * (a) This value is the summation of the radius of the sun (in
 * degrees as seen from Earth) plus the adopted value for the
 * refraction of the light through the atmosphere of 34 minutes
 * (Astronomical Almanac 1992)
 *
 * Reference
 *
 * - William C. Forsythe, Edward J. Rykiel, Randal S. Stahl, Hsin-i Wu, Robert M.
 *   Schoolfield. "A model comparison for daylength as a function of latitude and
 *   day of year." Ecological Modelling, Volume 80, Issue 1, 1995, Pages 87-95,
 *   https://doi.org/10.1016/0304-3800(94)00034-F.
 */
class daylength_calculator : public direct_module
{
   public:
    daylength_calculator(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          lat{get_input(input_quantities, "lat")},
          solar_dec{get_input(input_quantities, "solar_dec")},
          p{get_input(input_quantities, "sunrise_sunset_elevation")},
          // Get pointers to output quantities
          daylength{get_op(output_quantities, "day_length")}

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "daylength_calculator"; }

   private:
    // References to input quantities
    const double& lat;
    const double& solar_dec;
    const double& p;

    // Pointers to output quantities
    double* daylength;
    // Main operation
    void do_operation() const;
};

string_vector daylength_calculator::get_inputs()
{
    return {
        "lat",                      // degrees
        "solar_dec",                // degrees
        "sunrise_sunset_elevation"  // degrees
    };
}

string_vector daylength_calculator::get_outputs()
{
    return {
        "day_length"  // hr
    };
}

void daylength_calculator::do_operation() const
{
    using math_constants::pi;
    constexpr double to_rad = pi / 180;                     // radian / degree
    double offset = std::sin(p * to_rad);                   // dimensionless
    double lam = lat * to_rad;                              // radians
    double phi = solar_dec * to_rad;                        // radians
    double num = offset + std::sin(lam) * std::sin(phi);    // dimensionaless
    double denom = std::cos(lam) * std::cos(phi);           // dimensionless
    double u = std::max(std::min(num / denom, 1.0), -1.0);  // dimensionless
    double D = 24 - 24 / pi * std::acos(u);                 // hours
    update(daylength, D);                                   // hours
}

}  // namespace standardBML

#endif
