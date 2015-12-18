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

#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "c3photo.h"
#include "AuxBioCro.h"
#include "c3EvapoTrans.h"

/* EvapoTrans function */
struct ET_Str c3EvapoTrans(double Rad, 
        double Itot, 
        double Airtemperature, 
        double RH,
        double WindSpeed,
        double LeafAreaIndex, 
        double CanopyHeight, 
        double vcmax2, 
        double jmax2, 
        double Rd2, 
        double b02, 
        double b12,
        double Catm2,
        double O2,
        double theta2,
        double StomWS,
        int ws)

{
    /* creating the structure to return */
    struct ET_Str et_results;
    struct c3_str photo_results;

    // const double LeafWidth = 0.04; unused
    const double kappa = 0.41;
    const double WindSpeedHeight = 5;
    const double dCoef = 0.77;
    const double tau = 0.2;
    const double ZetaCoef = 0.026;
    const double ZetaMCoef = 0.13;
    const double LeafReflectance = 0.2;
    const double SpecificHeat = 1010;

    double Tair;
    double DdryA, LHV, SlopeFS, SWVC;
    double LayerRelativeHumidity, LayerWindSpeed, totalradiation;
    double LayerConductance, DeltaPVa, PsycParam, ga;
    // double BoundaryLayerThickness, DiffCoef, LeafboundaryLayer; unused
    double d, Zeta, Zetam, ga0, ga1, ga2; 
    double Ja, Deltat;
    double PhiN;
    double TopValue, BottomValue;
    double EPen, TransR,EPries; 
    double OldDeltaT, rlc, ChangeInLeafTemp; 
    int Counter;

    Tair = Airtemperature;

    if(CanopyHeight < 0.1)
        CanopyHeight = 0.1; 

    DdryA = TempToDdryA(Tair) ;
    LHV = TempToLHV(Tair) * 1e6 ; 
    /* Here LHV is given in MJ kg-1 and this needs to be converted
       to Joules kg-1  */
    SlopeFS = TempToSFS(Tair) * 1e-3;
    SWVC = TempToSWVC(Tair) * 1e-3;

    /* FIRST CALCULATIONS*/

    Zeta = ZetaCoef * CanopyHeight;
    Zetam = ZetaMCoef * CanopyHeight;
    d = dCoef * CanopyHeight;

    /* Relative Humidity is giving me a headache */
    /* RH2 = RH * 1e2; A high value of RH makes the 
       difference of temperature between the leaf and the air huge.
       This is what is causing the large difference in DeltaT at the 
       bottom of the canopy. I think it is very likely.  */
    LayerRelativeHumidity = RH * 100;
    if(LayerRelativeHumidity > 100) 
        error("LayerRelativehumidity > 100"); 

    if(WindSpeed < 0.5) WindSpeed = 0.5;

    LayerWindSpeed = WindSpeed;

    /* Convert light assuming 1 micromole PAR photons = 0.235 J/s */
    totalradiation = Itot * 0.235;

    photo_results = c3photoC(Rad,Airtemperature,RH,vcmax2,jmax2,Rd2,b02,b12,Catm2,O2,theta2,StomWS,ws); 
    LayerConductance = photo_results.Gs;

    /* Convert mmoles/m2/s to moles/m2/s
       LayerConductance = LayerConductance * 1e-3
       Convert moles/m2/s to mm/s
       LayerConductance = LayerConductance * 24.39
       Convert mm/s to m/s
       LayerConductance = LayerConductance * 1e-3 */

    LayerConductance = LayerConductance * 1e-6 * 24.39;  

    /* Thornley and Johnson use m s^-1 on page 418 */

    /* prevent errors due to extremely low Layer conductance */
    if(LayerConductance <=0)
        LayerConductance = 0.01;


    if(SWVC < 0)
        error("SWVC < 0");
    /* Now RHprof returns relative humidity in the 0-1 range */
    DeltaPVa = SWVC * (1 - LayerRelativeHumidity / 100);

    PsycParam =(DdryA * SpecificHeat) / LHV;

    /* Ja = (2 * totalradiation * ((1 - LeafReflectance - tau) / (1 - tau))) * LeafAreaIndex; */
    /* It seems that it is not correct to multiply by the leaf area index. Thornley
       and johnson pg. 400 suggest using (1-exp(-k*LAI) but note that for a full canopy
       this is 1. so I'm using 1 as an approximation. */
    Ja = (2 * totalradiation * ((1 - LeafReflectance - tau) / (1 - tau)));
    /* Calculation of ga */
    /* According to thornley and Johnson pg. 416 */
    ga0 = pow(kappa,2) * LayerWindSpeed;
    ga1 = log((WindSpeedHeight + Zeta - d)/Zeta);
    ga2 = log((WindSpeedHeight + Zetam - d)/Zetam);
    ga = ga0/(ga1*ga2);

    /*  Rprintf("ga: %.5f \n", ga); */
    if(ga < 0)
        error("ga is less than zero");

    // DiffCoef = (2.126 * 1e-5) + ((1.48 * 1e-7) * Airtemperature); set but not used
    // BoundaryLayerThickness = 0.004 * sqrt(LeafWidth / LayerWindSpeed); set but not used
    // LeafboundaryLayer = DiffCoef / BoundaryLayerThickness; // set but not used

    /* Temperature of the leaf according to Campbell and Norman (1998) Chp 4.*/
    /* This version is non-iterative and an approximation*/
    /* Stefan-Boltzmann law: B = sigma * T^4. where sigma is the Boltzmann
constant: 5.67 * 1e-8 W m^-2 K^-4. */

    /* From Table A.3 in the book we know that*/

    /*  rlc = (4 * (5.67*1e-8) * pow(273 + Airtemperature, 3) * Deltat) * LeafAreaIndex;   */
    /*  PhiN = Ja - rlc; */

    /*  dd = 0.72 * LeafWidth; */
    /*  gHa = 1.4 * 0.135 * sqrt(LayerWindSpeed * 0.72 * LeafWidth);  */
    /*  gva = 1.4 * 0.147 * sqrt(LayerWindSpeed * 0.72 * LeafWidth);  */

    /*  TopValue = Ja - (SWVC/7.28) * 5.67*1e-8 * pow(Airtemperature + 273.15,4) - LHV * LayerConductance * 1e-3 * DeltaPVa / 101.3; */
    /* Here I divide SWVC by 7.28 to approximately change from g/m3 to kPa */
    /* I'm also multiplying Layer conductance by 1e-3 since it should be in mol m^-2 s^-1 */

    /*  BottomValue = SpecificHeat * gHr + LHV * SlopeFS * LayerConductance * 1e-3;  */

    /*  Deltat = Airtemperature + TopValue/BottomValue; */

    /*  PhiN = TopValue; */

    /* This is the original from WIMOVAC*/
    Deltat = 0.01;
    ChangeInLeafTemp = 10;

    Counter = 0;
    while( (ChangeInLeafTemp > 0.5) && (Counter <= 10))
    {
        OldDeltaT = Deltat;

        /*         rlc = (4 * (5.67*1e-8) * pow(273 + Airtemperature, 3) * Deltat) * LeafAreaIndex;   */
        rlc = (4 * (5.67*1e-8) * pow(273 + Airtemperature, 3) * Deltat);  

        PhiN = (Ja - rlc);


        TopValue = PhiN * (1 / ga + 1 / LayerConductance) - LHV * DeltaPVa;
        BottomValue = LHV * (SlopeFS + PsycParam * (1 + ga / LayerConductance));
        Deltat = TopValue / BottomValue;
        if(Deltat > 5)	Deltat = 5;
        if(Deltat < -5)	Deltat = -5;


        ChangeInLeafTemp = OldDeltaT - Deltat;
        if(ChangeInLeafTemp <0)
            ChangeInLeafTemp = -ChangeInLeafTemp;
        Counter++;
    }


    if(PhiN < 0)
        PhiN = 0;

    TransR = (SlopeFS * PhiN + (LHV * PsycParam * ga * DeltaPVa)) / (LHV * (SlopeFS + PsycParam * (1 + ga / LayerConductance)));

    EPries = 1.26 * ((SlopeFS * PhiN) / (LHV * (SlopeFS + PsycParam)));

    EPen = (((SlopeFS * PhiN) + LHV * PsycParam * ga * DeltaPVa)) / (LHV * (SlopeFS + PsycParam));

    /* This values need to be converted from Kg/m2/s to
       mmol H20 /m2/s according to S Humphries */
    /*res[1,1] = EPen * 10e6 / 18;*/
    /*res[1,2] = EPries * 10e6 / 18;*/
    /* 1e3 - kgrams to grams  */
    /* 1e3 - mols to mmols */
    /*  res[0] = ((TransR * 1e3) / 18) * 1e3 ; */
    /*  res[1] = Deltat; */
    /*  res[2] = LayerConductance; */
    /* Let us return the structure now */

    et_results.TransR = TransR * 1e6 / 18; 
    et_results.EPenman = EPen * 1e6 / 18; 
    et_results.EPriestly = EPries * 1e6 / 18; 
    et_results.Deltat = Deltat;
    et_results.LayerCond = LayerConductance * 1e6 * (1/24.39);   
    /*    et_results.LayerCond = RH2;   */
    /*   et_results.LayerCond = 0.7; */
    return(et_results);
}

