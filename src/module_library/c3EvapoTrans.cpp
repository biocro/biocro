#include <cmath>  // For std::abs
#include <stdexcept>
#include "c3photo.h"
#include "BioCro.h"
#include "water_and_air_properties.h"  // for saturation_vapor_pressure,
                                       // TempToDdryA, TempToLHV, SlopeFS
#include "../framework/constants.h"    // for ideal_gas_constant, molar_mass_of_water,
                                       // stefan_boltzmann, celsius_to_kelvin
/**
 * Many of the equations used in this function come from Chapter 14 of Thornley
 * and Johnson (1990).
 *
 * Thornley, J.H.M. and Johnson, I.R. (1990) Plant and Crop Modelling. A
 * Mathematical Approach to Plant and Crop Physiology.
 */
struct ET_Str c3EvapoTrans(
    double absorbed_shortwave_radiation,  // J / m^2 / s
    double air_temperature,               // degrees C
    double RH,                            // Pa / Pa
    double WindSpeed,                     // m / s
    double CanopyHeight,                  // meters
    double specific_heat_of_air,          // J / kg / K
    double stomatal_conductance,          // mmol / m^2 / s
    double minimum_gbw,                   // mol / m^2 / s
    double WindSpeedHeight                // m
)
{
    const double DdryA = TempToDdryA(air_temperature);               // kg / m^3
    const double LHV = TempToLHV(air_temperature);                   // J / kg
    const double SlopeFS = TempToSFS(air_temperature);               // kg / m^3 / K
    const double SWVP = saturation_vapor_pressure(air_temperature);  // Pa

    // TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the
    // model state. (1 * R * temperature) / pressure
    constexpr double volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol

    double const minimum_gbw_in_m_per_s = minimum_gbw * volume_of_one_mole_of_air;  // m / s

    if (stomatal_conductance <= 0) {
        throw std::range_error("Thrown in c3EvapoTrans: stomatal conductance is not positive.");
    }

    double conductance_in_m_per_s = stomatal_conductance * 1e-3 * volume_of_one_mole_of_air;  // m / s

    if (RH > 1) {
        throw std::range_error("Thrown in c3EvapoTrans: RH (relative humidity) is greater than 1.");
    }

    /* SOLAR RADIATION COMPONENT*/

    // Convert from vapor pressure to vapor density using the ideal gas law.
    // This is approximately right for temperatures what won't kill plants.
    const double SWVC =
        SWVP / physical_constants::ideal_gas_constant /
        (air_temperature + conversion_constants::celsius_to_kelvin) *
        physical_constants::molar_mass_of_water;  // kg / m^3

    if (SWVC < 0) {
        throw std::range_error("Thrown in c3EvapoTrans: SWVC is less than 0.");
    }

    // Eq. 14.4g from Thornley and Johnson (1990).
    const double PsycParam = DdryA * specific_heat_of_air / LHV;  // kg / m^3 / K

    // Eq. 14.4d from Thornley and Johnson (1990).
    const double DeltaPVa = SWVC * (1 - RH);  // kg / m^3

    /* Calculation of ga */
    const double ga = leaf_boundary_layer_conductance_thornley(
        CanopyHeight,
        WindSpeed,
        minimum_gbw_in_m_per_s,
        WindSpeedHeight);  // m / s

    if (ga < 0) {
        throw std::range_error("Thrown in c3EvapoTrans: ga is less than zero.");
    }

    /* Temperature of the leaf according to Campbell and Norman (1998) Chp 14.*/
    /* This version is non-iterative and an approximation*/
    /* Stefan-Boltzmann law: B = sigma * T^4. where sigma is the Boltzmann constant. */
    /* From Table A.3 in Campbell and Norman.*/

    /* This is the original from WIMOVAC*/
    double Deltat = 0.01;  // degrees C
    double PhiN;
    {
        double ChangeInLeafTemp = 10;  // degrees C
        for (int Counter = 0; (ChangeInLeafTemp > 0.5) && (Counter <= 10); ++Counter) {
            double OldDeltaT = Deltat;

            double rlc = 4.0 * physical_constants::stefan_boltzmann *
                         pow(conversion_constants::celsius_to_kelvin + air_temperature, 3.0) *
                         Deltat;  // W / m^2

            PhiN = absorbed_shortwave_radiation - rlc;  // W / m^2

            // DeltaT equation from Thornley and Johnson 1990, Eq. 14.11e
            double TopValue = PhiN * (1 / ga + 1 / conductance_in_m_per_s) - LHV * DeltaPVa;       // J / m^3
            double BottomValue = LHV * (SlopeFS + PsycParam * (1 + ga / conductance_in_m_per_s));  // J / m^2 / K

            Deltat = TopValue / BottomValue;  // Kelvin. It is also degrees C, because it is a temperature difference.
            Deltat = std::min(std::max(Deltat, -5.0), 5.0);

            ChangeInLeafTemp = std::abs(OldDeltaT - Deltat);  // Kelvin. It is also degrees C, because it is a temperature difference.
        }
    }

    if (PhiN < 0) {
        PhiN = 0;
    }

    /* TransR is the Penman-Monteith equation that describe transpiration rate
    * as a function of the energy `PhiN` available for evaporation, the vapour
    * density deficit `DeltaPVa`, the canopy and boundary layer conductances, gc
    * and ga respectively, and the physical parameters `SlopeFS`, `PsycParam`,
    * and `LHV`.
    *
    * Thornley, J.H.M. and Johnson, I.R. (1990) Plant and Crop Modelling. A
    * Mathematical Approach to Plant and Crop Physiology
    */
    const double TransR =
        (SlopeFS * PhiN + LHV * PsycParam * ga * DeltaPVa) /
        (LHV * (SlopeFS + PsycParam * (1 + ga / conductance_in_m_per_s)));  // kg / m^2 / s

    const double EPen =
        (SlopeFS * PhiN + LHV * PsycParam * ga * DeltaPVa) /
        (LHV * (SlopeFS + PsycParam));  // kg / m^2 / s

    const double EPries = 1.26 * (SlopeFS * PhiN / (LHV * (SlopeFS + PsycParam)));  // kg / m^2 / s

    // TransR has units of kg / m^2 / s.
    // Convert to mm / m^2 / s.
    /* 1e3 - g / kg  */
    /* 18 - g / mol for water */
    /* 1e3 - mmol / mol */

    // TransR has units of kg / m^2 / s. Convert to mmol / m^2 / s using the
    // molar mass of water (in kg / mol) and noting that 1e3 mmol = 1 mol
    double cf = 1e3 / physical_constants::molar_mass_of_water;  // mmol / kg for water

    struct ET_Str et_results;
    et_results.TransR = TransR * cf;                                         // mmol / m^2 / s
    et_results.EPenman = EPen * cf;                                          // mmol / m^2 / s
    et_results.EPriestly = EPries * cf;                                      // mmol / m^2 / s
    et_results.Deltat = Deltat;                                              // degrees C
    et_results.boundary_layer_conductance = ga / volume_of_one_mole_of_air;  // mol / m^2 / s

    return et_results;
}
