/*
 *  /src/c3EvapoTrans.c by Fernando Ezequiel Miguez  Copyright (C) 2010
 *
 *  Part of the code here (sunML, EvapoTrans, SoilEvapo, TempTo and
 *  the *prof functions) are based on code in WIMOVAC. WIMOVAC is
 *  copyright of Stephen Long and Stephen Humphries.
 *  Documentation for WIMOVAC can be found at
 *  http://www.life.illinois.edu/plantbio/wimovac/ (checked 02-13-2010)
 *
 */

#include <cmath>
#include <stdexcept>
#include "c3photo.hpp"
#include "AuxBioCro.h"
#include "BioCro.h"
#include "../constants.h"  // for ideal_gas_constant, molar_mass_of_water

struct ET_Str c3EvapoTrans(
    double absorbed_shortwave_radiation,  // J / m^2 / s
    double air_temperature,               // degrees C
    double RH,                            // Pa / Pa
    double WindSpeed,                     // m / s
    double CanopyHeight,                  // meters
    double specific_heat_of_air,          // J / kg / K
    double stomatal_conductance)          // mmol / m^2 / s
{
    constexpr double StefanBoltzmann = 5.67037e-8;  // J / m^2 / s^1 / K^4
    constexpr double kappa = 0.41;                  // dimensionless. von Karmon's constant. Thornley and Johnson pgs 414 and 416.
    constexpr double WindSpeedHeight = 5;           // meters
    constexpr double dCoef = 0.77;                  // dimensionless
    constexpr double ZetaCoef = 0.026;              // dimensionless
    constexpr double ZetaMCoef = 0.13;              // dimensionless
    const double Zeta = ZetaCoef * CanopyHeight;    // meters
    const double Zetam = ZetaMCoef * CanopyHeight;  // meters
    const double d = dCoef * CanopyHeight;          // meters

    if (CanopyHeight < 0.1) {
        CanopyHeight = 0.1;
    }

    const double DdryA = TempToDdryA(air_temperature);               // kg / m^3
    const double LHV = TempToLHV(air_temperature);                   // J / kg
    const double SlopeFS = TempToSFS(air_temperature);               // kg / m^3 / K. It is also kg / m^3 / degrees C since it's a change in temperature.
    const double SWVP = saturation_vapor_pressure(air_temperature);  // Pa

    constexpr double volume_of_one_mole_of_air = 24.39e-3;                                    // m^3 / mol. TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the model state. (1 * R * temperature) / pressure
    double conductance_in_m_per_s = stomatal_conductance * 1e-3 * volume_of_one_mole_of_air;  // m / s

    /* Prevent errors due to extremely low Layer conductance. */
    if (conductance_in_m_per_s <= 0) {
        conductance_in_m_per_s = 0.01;
    }

    if (RH > 1) {
        throw std::range_error("Thrown in c3EvapoTrans: RH (relative humidity) is greater than 1.");
    }

    /* SOLAR RADIATION COMPONENT*/

    // Convert from vapor pressure to vapor density using the ideal gas law.
    // This is approximately right for temperatures what won't kill plants.
    const double SWVC =
        SWVP / physical_constants::ideal_gas_constant /
        (air_temperature + 273.15) * physical_constants::molar_mass_of_water;  // kg / m^3

    if (SWVC < 0) {
        throw std::range_error("Thrown in c3EvapoTrans: SWVC is less than 0.");
    }

    const double PsycParam = DdryA * specific_heat_of_air / LHV;  // kg / m^3 / K

    const double DeltaPVa = SWVC * (1 - RH);  // kg / m^3

    /* AERODYNAMIC COMPONENT */
    if (WindSpeed < 0.5) {
        WindSpeed = 0.5;
    }

    /* Calculation of ga */
    /* According to thornley and Johnson pg. 416 */
    const double ga0 = pow(kappa, 2) * WindSpeed;                   // m / s
    const double ga1 = log((WindSpeedHeight + Zeta - d) / Zeta);    // dimensionless
    const double ga2 = log((WindSpeedHeight + Zetam - d) / Zetam);  // dimensionless
    const double ga = ga0 / (ga1 * ga2);                            // m / s

    if (ga < 0) {
        throw std::range_error("Thrown in c3EvapoTrans: ga is less than zero.");
    }

    /* Temperature of the leaf according to Campbell and Norman (1998) Chp 4.*/
    /* This version is non-iterative and an approximation*/
    /* Stefan-Boltzmann law: B = sigma * T^4. where sigma is the Boltzmann constant: 5.67 * 1e-8 W m^-2 K^-4. */
    /* From Table A.3 in Campbell and Norman.*/

    /* This is the original from WIMOVAC*/
    double Deltat = 0.01;  // degrees C
    double PhiN;
    {
        double ChangeInLeafTemp = 10;  // degrees C
        for (int Counter = 0; (ChangeInLeafTemp > 0.5) && (Counter <= 10); ++Counter) {
            double OldDeltaT = Deltat;

            double rlc = 4.0 * StefanBoltzmann * pow(273.0 + air_temperature, 3.0) * Deltat;  // W / m^2

            PhiN = absorbed_shortwave_radiation - rlc;  // W / m^2

            double TopValue = PhiN * (1 / ga + 1 / conductance_in_m_per_s) - LHV * DeltaPVa;       // J / m^3
            double BottomValue = LHV * (SlopeFS + PsycParam * (1 + ga / conductance_in_m_per_s));  // J / m^2 / K
            Deltat = TopValue / BottomValue;                                                       // kelvin. It is also degrees C, because it is a temperature difference.
            Deltat = std::min(std::max(Deltat, -5.0), 5.0);

            ChangeInLeafTemp = fabs(OldDeltaT - Deltat);  // kelvin. It is also degrees C, because it is a temperature difference.
        }
    }

    if (PhiN < 0) {
        PhiN = 0;
    }

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
    et_results.TransR = TransR * cf;     // mmol / m^2 / s
    et_results.EPenman = EPen * cf;      // mmol / m^2 / s
    et_results.EPriestly = EPries * cf;  // mmol / m^2 / s
    et_results.Deltat = Deltat;          // degrees C

    return et_results;
}
