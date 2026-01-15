#ifndef ATMOSPHERIC_PRESSURE_FROM_ELEVATION_H
#define ATMOSPHERIC_PRESSURE_FROM_ELEVATION_H

#include <cmath>                     // for pow
#include "../framework/constants.h"  // for atmospheric_pressure_at_sea_level
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class atmospheric_pressure_from_elevation
 *
 *  @brief Estimates the atmospheric pressure from the site elevation using
 *  Equation 3 (or 34) from ASCE (2005).
 *
 *  At sea level, elevation is zero and the equation in ASCE (2005) predicts
 *  an atmospheric pressure of 101.3 kPa. Here we instead use the default BioCro
 *  value, expressed in Pa rather than kPa.
 *
 *  References:
 *
 *  - ["Calculating Standardized Reference Crop Evapotranspiration" in "The ASCE
 *    Standardized Reference Evapotranspiration Equation" 7–45 (2005)]
 *    (https://doi.org/10.1061/9780784408056.ch04)
 */
class atmospheric_pressure_from_elevation : public direct_module
{
   public:
    atmospheric_pressure_from_elevation(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          elevation{get_input(input_quantities, "elevation")},

          // Get pointers to output quantities
          atmospheric_pressure_op{get_op(output_quantities, "atmospheric_pressure")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "atmospheric_pressure_from_elevation"; }

   private:
    // References to input quantities
    double const& elevation;

    // Pointers to output quantities
    double* atmospheric_pressure_op;

    // Main operation
    void do_operation() const;
};

string_vector atmospheric_pressure_from_elevation::get_inputs()
{
    return {
        "elevation"  // m
    };
}

string_vector atmospheric_pressure_from_elevation::get_outputs()
{
    return {
        "atmospheric_pressure"  // Pa
    };
}

void atmospheric_pressure_from_elevation::do_operation() const
{
    update(atmospheric_pressure_op,
           physical_constants::atmospheric_pressure_at_sea_level *
               pow(((293.0 - 0.0065 * elevation) / 293.0), 5.26));  // Pa
}

}  // namespace standardBML
#endif
