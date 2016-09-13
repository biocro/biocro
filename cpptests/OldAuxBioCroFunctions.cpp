#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include <stdexcept>
#include "../src/c4photo.h"
#include "../src/BioCro.h"
#include "OldAuxBioCroFunctions.h"

// This is a verbatim copy of the sumML function as of commit
// 043053a4702b391265beb6dfd0d7be3e205fa252 (Mon Sep 12 16:33:21 2016 -0500),
// before beginning further revision.
Light_profile OldsunML(double Idir, double Idiff, double LAI, int nlayers,
        double cosTheta, double kd, double chil, double heightf)
{
    if (nlayers < 1 || nlayers > MAXLAY) {
        throw std::out_of_range("nlayers must be at least 1 but no more than 200");
    }
    if (cosTheta > 1 || cosTheta <= 0) {
        throw std::out_of_range("cosTheta must be positive but no more than 1.");
    }

    constexpr auto alphascatter = 0.8;

    auto theta = acos(cosTheta);
    auto k0 = sqrt( pow(chil, 2) + pow(tan(theta), 2) );
    auto k1 = chil + 1.744 * pow((chil + 1.183), -0.733);
    auto k = (k1 > 0) ? k0/k1 : -k0/k1;

    auto LAIi = LAI / nlayers;

    Light_profile light_profile;
    for (int i = 0; i < nlayers; i++) {
        auto CumLAI = LAIi * (i + 0.5);

        auto Ibeam = Idir * cosTheta;
        auto Iscat = Ibeam * exp(-k * sqrt(alphascatter) * CumLAI)
            - Ibeam * exp(-k * CumLAI);

        auto Isolar = Ibeam * k;
        auto Idiffuse = Idiff * exp(-kd * CumLAI) + Iscat;

        auto Ls = (1 - exp(-k * LAIi)) * exp(-k * CumLAI) / k;
        auto Ld = LAIi - Ls;

        auto Fsun = Ls/(Ls + Ld);
        auto Fshade = Ld/(Ls + Ld);

        auto Iaverage = (Fsun * (Isolar + Idiffuse) + Fshade * Idiffuse)
            * (1 - exp(-k * LAIi)) / k;

        light_profile.direct_irradiance[i] = Isolar + Idiffuse;
        light_profile.diffuse_irradiance[i]= Idiffuse;
        light_profile.total_irradiance[i] = Iaverage;
        light_profile.sunlit_fraction[i] = Fsun;
        light_profile.shaded_fraction[i] = Fshade;
        light_profile.height[i] = (LAI - CumLAI)/heightf;
    }
    return light_profile;
}


// This is a verbatim copy of the EvapoTrans function as of commit
// 2cac4bda659694b8e2e6cc6580f2bbcc91485923 (Wed Sep 7 16:37:32 2016 -0500),
// before beginning the process of simplification.
struct ET_Str OldEvapoTrans(double Rad,
        double Itot,
        double Airtemperature,
        double RH,
        double WindSpeed,
        double LeafAreaIndex,
        double CanopyHeight,
        double StomataWS,
        int ws,
        double vmax2,
        double alpha2,
        double kparm,
        double theta,
        double beta,
        double Rd2,
        double b02,
        double b12,
        double upperT,
        double lowerT,
        double Catm)
{
    /* creating the structure to return */
    struct ET_Str et_results;
    struct c4_str photo_results;

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

    DdryA = TempToDdryA(Tair);
    LHV = TempToLHV(Tair) * 1e6; 
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

    photo_results = c4photoC(Itot,Airtemperature,RH,vmax2,alpha2,kparm,theta,beta,Rd2,b02,b12,StomataWS, Catm, ws,upperT,lowerT); 
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
        if(Deltat > 5)  Deltat = 5;
        if(Deltat < -5) Deltat = -5;


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
    /*  res[0] = ((TransR * 1e3) / 18) * 1e3; */
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


// This is a verbatim copy of the EvapoTrans2 function as of commit
// 2cac4bda659694b8e2e6cc6580f2bbcc91485923 (Wed Sep 7 16:37:32 2016 -0500),
// before beginning the process of simplification.
struct ET_Str OldEvapoTrans2(double Rad,
        double Iave,
        double Airtemperature,
        double RH,
        double WindSpeed,
        double LeafAreaIndex,
        double CanopyHeight,
        double stomatacond,
        double leafw,
        int eteq)
{

    /* creating the structure to return */
    struct ET_Str tmp;

    // const double kappa = 0.41; /* von Karmans constant */ unused
    double WindSpeedHeight = 2; /* This is the height at which the wind speed was measured */
    // const double dCoef = 0.77;  unused
    const double tau = 0.2; /* Leaf transmission coefficient */
    // const double ZetaCoef = 0.026; unused
    // const double ZetaMCoef = 0.13; unused
    const double LeafReflectance = 0.2; 
    const double SpecificHeat = 1010; /* J kg-1 K-1 */
    const double StefanBoltzmann = 5.67037e-8; /* J m^-2 s^-1 K^-4 */

    double Tair;
    double DdryA, LHV, SlopeFS, SWVC, SWVP;
    double LayerWindSpeed, totalradiation;
    double DeltaPVa, PsycParam, ga;
    double gvs; /* Conductance to vapor from stomata same as stomatacond (input variable) */ 
    double ActualVaporPressure;
    double Ja, Ja2, Deltat;
    double PhiN, PhiN2;
    double TopValue, BottomValue;
    double EPen, TransR,EPries; 
    double OldDeltaT, ChangeInLeafTemp;
    double rlc; /* Long wave radiation for iterative calculation */
    int Counter;

    // double WindSpeedTopCanopy = WindSpeed; // unused.
    Tair = Airtemperature;

    if(CanopyHeight < 0.1)
        CanopyHeight = 0.1; 

    /* When the height at which wind was measured is lower than the canopy height */
    /* There can be problems with the calculations */
    /* This is a very crude way of solving this problem */
    if(CanopyHeight + 1 > WindSpeedHeight)
        WindSpeedHeight = CanopyHeight + WindSpeedHeight;

    DdryA = TempToDdryA(Tair); /* Density of dry air, kg / m^3 */

    /* In the original code in WIMOVAC this is used in J kg-1
       but Thornley and Johnson use it as MJ kg-1  */
    LHV = TempToLHV(Tair); /* This should be MJ kg^-1*/
    LHV = LHV * 1e6; /* Now it is converted to Joules kg^-1*/
    SlopeFS = TempToSFS(Tair) * 1e-3; /* kg m^-3 K^-1 */
    SWVP = TempToSWVC(Tair); /* this is hecto Pascals */
    /* Convert to kg/m3 */
    SWVC = (DdryA * 0.622 * SWVP)/1013.25; /* This last number is atmospheric pressure in hecto pascals */
    /* SWVC is saturated water vapor concentration (or density) in kg/m3 */

    PsycParam =(DdryA * SpecificHeat) / LHV; /* This is in kg m-3 K-1 */

    DeltaPVa = SWVC * (1 - RH); /* kg/m3 */

    ActualVaporPressure = RH * SWVP; /* hecto Pascals */

    /* SOLAR RADIATION COMPONENT*/

    /* First calculate the Radiation term */
    /*' Convert light assuming 1 micromole PAR photons = 0.235 J */
    /* The next step converts from PAR photons to Joules */
    /* There are 2.35 x 10^5 Joules in a mol */
    /* or 0.235 Joules in a micro mol */
    /* A Watt is equivalent to J/s */
    totalradiation = Rad * 0.235; /* This is essentially Watts m^-2 */
    /* On a clear sky it may exceed 1000 in some parts of the world 
       Thornley and Johnson pg 400 */
    /* This values can not possibly be higher than 650 */
    if(totalradiation > 650) error("total radiation too high");

    /* Ja = (2 * totalradiation * ((1 - LeafReflectance - tau) / (1 - tau))) * LeafAreaIndex; */
    /* It seems that it is not correct to multiply by the leaf area index. The previous
       version was used in WIMOVAC (check) */
    Ja = (2 * totalradiation * ((1 - LeafReflectance - tau) / (1 - tau)));

    /* The value below is only for leaf temperature */
    Ja2 = (2 * Iave * 0.235 * ((1 - LeafReflectance - tau) / (1 - tau)));

    /* AERODYNAMIC COMPONENT */
    if(WindSpeed < 0.5) WindSpeed = 0.5;

    LayerWindSpeed = WindSpeed;

    /* Rprintf("Gs %.3f \n", stomatacond); */
    /* Leaf Conductance */
    gvs = stomatacond; 
    /* Convert from mmol H20/m2/s to m/s */
    gvs = gvs * (1.0/41000.0);
    /* 1/41000 is the same as 24.39 * 1e-6 */
    /* Thornley and Johnson use m s^-1 on page 418 */

    /* prevent errors due to extremely low Layer conductance */
    if(gvs <= 0.001)
        gvs = 0.001;

    /* This is the original from WIMOVAC*/
    Deltat = 0.01;
    ChangeInLeafTemp = 10;

    Counter = 0;
    while( (ChangeInLeafTemp > 0.5) && (Counter <= 10))
    {
        OldDeltaT = Deltat;

        rlc = 4 * StefanBoltzmann * pow(273 + Tair, 3) * Deltat;  

        /* rlc=net long wave radiation emittted per second =radiation emitted per second - radiation absorbed per second=sigma*(Tair+deltaT)^4-sigma*Tair^4 */

        /* Then you do a Taylor series about deltaT = 0 and keep only the zero and first order terms. */

        /* or rlc=sigma*Tair^4+deltaT*(4*sigma*Tair^3)-sigma*Tair^4=4*sigma*Tair^3*deltaT */

        /* where 4*sigma*Tair^3 is the derivative of sigma*(Tair+deltaT)^4 evaluated at deltaT=0, */

        ga = leafboundarylayer(LayerWindSpeed, leafw, 
                Airtemperature, Deltat,
                gvs, ActualVaporPressure);
        /* This returns leaf-level boundary layer conductance */ 
        /* In WIMOVAC this was added to the canopy conductance */
        /* ga = (ga * gbcW)/(ga + gbcW);  */

        PhiN2 = (Ja2 - rlc);  /* * LeafAreaIndex;  */

        TopValue = PhiN2 * (1 / ga + 1 / gvs) - LHV * DeltaPVa;
        BottomValue = LHV * (SlopeFS + PsycParam * (1 + ga / gvs));
        Deltat = TopValue / BottomValue; /* This equation is from Thornley and Johnson pg. 418 */
        if(Deltat > 10)	Deltat = 10;
        if(Deltat < -10) Deltat = -10;

        ChangeInLeafTemp = OldDeltaT - Deltat;
        if(ChangeInLeafTemp <0)
            ChangeInLeafTemp = -ChangeInLeafTemp;
        Counter++;
    }

    /* Net radiation */
    PhiN = Ja - rlc;

    if(PhiN < 0)
        PhiN = 0;

    TransR = (SlopeFS * PhiN + (LHV * PsycParam * ga * DeltaPVa)) / (LHV * (SlopeFS + PsycParam * (1 + ga / gvs)));

    /* Penman will use the WIMOVAC conductance */
    EPen = (((SlopeFS * PhiN) + LHV * PsycParam * ga * DeltaPVa)) / (LHV * (SlopeFS + PsycParam));

    EPries = 1.26 * ((SlopeFS * PhiN) / (LHV * (SlopeFS + PsycParam)));

    /* Choose equation to report */
    if(eteq == 1) {
        TransR = EPen;
    }
    if(eteq == 2) {
        TransR = EPries;
    }

    /* This values need to be converted from Kg/m2/s to
       mmol H20 /m2/s according to S Humphries */
    /* 1e3 - kgrams to grams  */
    /* 1e3 - mols to mmols */
    /* grams to mols - 18g in a mol */
    /* Let us return the structure now */

    tmp.TransR = TransR * 1e6 / 18; 
    tmp.EPenman = EPen * 1e6 / 18; 
    tmp.EPriestly = EPries * 1e6 / 18; 
    tmp.Deltat = Deltat;
    tmp.LayerCond = gvs * 41000;   
    return(tmp);
}
