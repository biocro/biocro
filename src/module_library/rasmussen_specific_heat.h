#ifndef RASMUSSEN_SPECIFIC_HEAT_H
#define RASMUSSEN_SPECIFIC_HEAT_H

#include "../framework/module.h"
#include "../framework/state_map.h"

/**
 *  @brief Calculates the specific heat capacity of air at contant pressure from
 *  its temperature and moisture content using the method from Rasmussen (1997).
 *
 *  @param [in] air_temperature Temeperature of the air in K
 *
 *  @param [in] mole_fraction_h2o Water vapor concentration in the air expressed
 *                                as a dimensionless mole fraction
 *
 *  @return Specific heat capacity of air at contant pressure in J / kg / K
 *
 *  Here we use the polynomial equation for specific heat capacity of air at
 *  constant pressure (`C_p`) defined on pages 13 & 15. Note that the equation,
 *  as given, calculates `C_p` in units of cal / g / K. In the code, the
 *  polynomial coefficients are multiplied by a conversion factor so the output
 *  is in coherent SI units (J / kg / K).
 *
 *  Source:
 *  [Rasmussen, K. "Calculation methods for the physical properties of air used
 *  in the calibration of microphones." (1997)]
 *  (https://orbit.dtu.dk/en/publications/calculation-methods-for-the-physical-properties-of-air-used-in-th)
 */
double rasmussen_specific_heat_of_air(
    double air_temperature,   // K
    double mole_fraction_h2o  // dimensionless
);

namespace standardBML
{
/**
 * @class rasmussen_specific_heat
 *
 * @brief Determines the specific heat capacity of atmospheric air at constant
 * pressure using the `rasmussen_specific_heat_of_air()` function.
 */
class rasmussen_specific_heat : public direct_module
{
   public:
    rasmussen_specific_heat(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          temp{get_input(input_quantities, "temp")},
          mole_fraction_h2o_atmosphere{get_input(input_quantities, "mole_fraction_h2o_atmosphere")},

          // Get pointers to output quantities
          specific_heat_of_air_op{get_op(output_quantities, "specific_heat_of_air")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "rasmussen_specific_heat"; }

   private:
    // References to input quantities
    double const& temp;
    double const& mole_fraction_h2o_atmosphere;

    // Pointers to output quantities
    double* specific_heat_of_air_op;

    // Main operation
    void do_operation() const;
};

}  // namespace standardBML
#endif
