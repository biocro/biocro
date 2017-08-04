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

#include <math.h>
#include "c3photo.h"
#include "AuxBioCro.h"
#include "BioCro.h"

/* EvapoTrans function */
struct ET_Str c3EvapoTrans(
        double Itot, 
        double air_temperature, 
        double RH,
        double WindSpeed,
        double CanopyHeight,
        double stomatal_conductance)

{
    const double kappa = 0.41;
    const double WindSpeedHeight = 5;
    const double dCoef = 0.77;
    const double tau = 0.2;
    const double ZetaCoef = 0.026;
    const double ZetaMCoef = 0.13;
    const double LeafReflectance = 0.2;
    const double SpecificHeat = 1010;

    if (CanopyHeight < 0.1)
        CanopyHeight = 0.1; 

    double DdryA = TempToDdryA(air_temperature);
    double LHV = TempToLHV(air_temperature) * 1e6; // Convert from MJ kg^-1 to J kg^-1.
    double SlopeFS = TempToSFS(air_temperature) * 1e-3;
    double SWVC = TempToSWVC(air_temperature) * 1e-3;

    double Zeta = ZetaCoef * CanopyHeight;
    double Zetam = ZetaMCoef * CanopyHeight;
    double d = dCoef * CanopyHeight;

    /* RH2 = RH * 1e2; A high value of RH makes the 
       difference of temperature between the leaf and the air huge.
       This is what is causing the large difference in DeltaT at the 
       bottom of the canopy. I think it is very likely.  */
    double LayerRelativeHumidity = RH * 100;
    if (LayerRelativeHumidity > 100) 
        throw std::range_error("Thrown in c3EvapoTrans: LayerRelativehumidity is greater than 100."); 

    if (WindSpeed < 0.5) WindSpeed = 0.5;

    double LayerWindSpeed = WindSpeed;

    /* Convert light assuming 1 micromole PAR photons = 0.235 J/s */
    double totalradiation = Itot * 0.235;

    double LayerConductance = stomatal_conductance * 1e-6 * 24.39;  

    /* Thornley and Johnson use m s^-1 on page 418 */

    if (LayerConductance <= 0) /* Prevent errors due to extremely low Layer conductance. */
        LayerConductance = 0.01;

    if (SWVC < 0)
        throw std::range_error("Thrown in c3EvapoTrans: SWVC is less than 0."); 
    /* Now RHprof returns relative humidity in the 0-1 range */
    double DeltaPVa = SWVC * (1 - LayerRelativeHumidity / 100);

    double PsycParam =(DdryA * SpecificHeat) / LHV;

    double Ja = (2 * totalradiation * ((1 - LeafReflectance - tau) / (1 - tau)));
    /* Calculation of ga */
    /* According to thornley and Johnson pg. 416 */
    double ga0 = pow(kappa,2) * LayerWindSpeed;
    double ga1 = log((WindSpeedHeight + Zeta - d)/Zeta);
    double ga2 = log((WindSpeedHeight + Zetam - d)/Zetam);
    double ga = ga0/(ga1*ga2);

    if (ga < 0)
        throw std::range_error("Thrown in c3EvapoTrans: ga is less than zero."); 

    /* Temperature of the leaf according to Campbell and Norman (1998) Chp 4.*/
    /* This version is non-iterative and an approximation*/
    /* Stefan-Boltzmann law: B = sigma * T^4. where sigma is the Boltzmann constant: 5.67 * 1e-8 W m^-2 K^-4. */
    /* From Table A.3 in Campbell and Norman.*/

    /* This is the original from WIMOVAC*/
    double Deltat = 0.01;
    double ChangeInLeafTemp = 10;
    int Counter = 0;
    double PhiN;
    while ((ChangeInLeafTemp > 0.5) && (Counter <= 10))
    {
        double OldDeltaT = Deltat;

        double rlc = (4 * (5.67*1e-8) * pow(273 + air_temperature, 3) * Deltat);  

        PhiN = (Ja - rlc);

        double TopValue = PhiN * (1 / ga + 1 / LayerConductance) - LHV * DeltaPVa;
        double BottomValue = LHV * (SlopeFS + PsycParam * (1 + ga / LayerConductance));
        Deltat = TopValue / BottomValue;
        if (Deltat > 5)	Deltat = 5;
        if (Deltat < -5)	Deltat = -5;

        ChangeInLeafTemp = OldDeltaT - Deltat;
        if (ChangeInLeafTemp <0)
            ChangeInLeafTemp = -ChangeInLeafTemp;
        ++Counter;
    }

    if (PhiN < 0)
        PhiN = 0;

    double TransR = (SlopeFS * PhiN + (LHV * PsycParam * ga * DeltaPVa)) / (LHV * (SlopeFS + PsycParam * (1 + ga / LayerConductance)));
    double EPries = 1.26 * ((SlopeFS * PhiN) / (LHV * (SlopeFS + PsycParam)));
    double EPen = ((SlopeFS * PhiN) + LHV * PsycParam * ga * DeltaPVa) / (LHV * (SlopeFS + PsycParam));

    struct ET_Str et_results;
    et_results.TransR = TransR * 1e6 / 18; 
    et_results.EPenman = EPen * 1e6 / 18; 
    et_results.EPriestly = EPries * 1e6 / 18; 
    et_results.Deltat = Deltat;

    return(et_results);
}

