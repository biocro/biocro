/*
 *  /src/AuxBioCro.c by Fernando Ezequiel Miguez  Copyright (C) 2007 - 2012
 *
 *  Part of the code here (sunML, EvapoTrans, SoilEvapo, TempTo and
 *  the *prof functions) are based on code in WIMOVAC. WIMOVAC is
 *  copyright of Stephen Long and Stephen Humphries.
 *  Documentation for WIMOVAC can be found at
 *  http://www.life.illinois.edu/plantbio/wimovac/ (checked 02-13-2010)
 *
 */


/* This file will contain functions which are common to several */
/* routines in the BioCro package. These are functions needed */
/* internally. The normal user will not need them */

#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>
#include "c4photo.h"
#include "AuxBioCro.h"
#include "BioCro.h"


/* lightME function. Light Macro Environment */
void lightME(double lat, int DOY, int td)
{

        extern double tmp1[];
        double *ip1;
        ip1 = &tmp1[0];
        double omega, delta0, delta, deltaR;
        double tf, SSin, CCos, PPo;
        double CosZenithAngle, CosHour;
        double CosHourDeg;
        double Idir, Idiff, propIdir, propIdiff;
        const double DTR = M_PI/180;
        const double tsn = 12.0;
        const double alpha = 0.85;
        const double SolarConstant = 2650;
        const double atmP = 1e5;

        omega = lat * DTR;
        delta0 = 360.0 * ((DOY + 10)/365.0);
        delta = -23.5 * cos(delta0*DTR);
        deltaR = delta * DTR;

        tf = (15.0*(td - tsn))*DTR;
        SSin = sin(deltaR) * sin(omega);
        CCos = cos(deltaR) * cos(omega);

        CosZenithAngle = SSin + CCos * cos(tf);
        if(CosZenithAngle < pow(10,-10))
                CosZenithAngle = pow(10,-10);

        CosHour = -tan(omega) * tan(deltaR);
        CosHourDeg = (1/DTR)*CosHour;
        if(CosHourDeg < -57)
                CosHour = -0.994;

        PPo = 1e5 / atmP;
        Idir = SolarConstant * (pow(alpha,(PPo/CosZenithAngle)));
        Idiff = 0.3 * SolarConstant *(1 - pow(alpha,(PPo/CosZenithAngle))) * CosZenithAngle ;

        propIdir = Idir / (Idir + Idiff);
        propIdiff = Idiff / (Idir + Idiff);

        *ip1 = propIdir;
        *(ip1+1) = propIdiff;
        *(ip1+2) = CosZenithAngle;
        return;
}

void sunML(double Idir, double Idiff, double LAI, int nlayers,
           double cosTheta, double kd, double chil, double heightf)
{
        extern int sp1, sp2, sp3, sp4, sp5, sp6;
        extern double layIdir[], layIdiff[], layItotal[], layFsun[], layFshade[], layHeight[];
        double i;
        double k0, k1, k;
        double LAIi, CumLAI;
        double Isolar, Idiffuse, Ibeam, Iscat, Itotal,alphascatter;
        double Ls, Ld;
        double Fsun, Fshade;
        alphascatter=0.8;
        k0 = sqrt(pow(chil ,2) + pow(tan(acos(cosTheta)),2));
        k1 = chil + 1.744*pow((chil+1.183),-0.733);
        k = k0/k1;
        if(k<0)
                k = -k;

        LAIi = LAI / nlayers;

        for(i=0;i<nlayers;i++)
        {
                CumLAI = LAIi * (i+0.5);
                
                Ibeam=Idir*cosTheta;
                Iscat = Ibeam * exp(-k *sqrt(alphascatter)* CumLAI)-Ibeam * exp(-k * CumLAI);
                
                
                Isolar = Ibeam*k;
                Idiffuse = Idiff * exp(-kd * CumLAI) + Iscat;
                
                
                Ls = (1-exp(-k*LAIi))*exp(-k*CumLAI)/k;
                Ld=LAIi-Ls;

                Fsun=Ls/(Ls+Ld);
                Fshade=Ld/(Ls+Ld);
                /*fraction intercepted*/
                Itotal =(Fsun*Isolar + Idiffuse) * (1-exp(-k*LAIi))/k;

                /* collecting the results */
                layIdir[sp1++] = Isolar + Idiffuse;
                layIdiff[sp2++] = Idiffuse;
                layItotal[sp3++] = Itotal;
                layFsun[sp4++] = Fsun;
                layFshade[sp5++] = Fshade;
                layHeight[sp6++] = CumLAI/heightf;
        }
}

/* Additional Functions needed for EvapoTrans */


/* RH and Wind profile function */
void WINDprof(double WindSpeed, double LAI, int nlayers)
{
        int i;
        double k=0.7;
        double LI, CumLAI;
        double Wind;

        LI  = LAI / nlayers;
        for(i=0;i<nlayers;i++)
        {
                CumLAI = LI * (i + 1);
                Wind = WindSpeed * exp(-k * (CumLAI-LI));
                tmp3[tp3++] = Wind;
        }
}

void RHprof(double RH, int nlayers)
{
        int i;
        double kh, hsla, j;

        kh = 1 - RH;
        /* kh = 0.2; */
        /*kh = log(1/RH);*/
        for(i=0;i<nlayers;i++)
        {
                j = i + 1;
                hsla = RH * exp(kh * (j/nlayers));
//              /*hsla = RH * exp(-kh * (j/nlayers));  /*new simpler version from Joe Iverson*/
                if(hsla > 1) hsla = 0.99; 
                tmp4[tp4++] = hsla;
        }
        /* It should return values in the 0-1 range */
}

void LNprof(double LeafN, double LAI, int nlayers, double kpLN)
{

        int i;
        double leafNla, LI, CumLAI;

        LI  = LAI / nlayers;
        for(i=0;i<nlayers;i++)
        {
                CumLAI = LI * (i + 1);
                leafNla = LeafN * exp(-kpLN * (CumLAI-LI));
                tmp5[tp5++] = leafNla;
        }

}

double TempToDdryA(double Temp)
{
        double DdryA;
        DdryA = 1.295163636 + -0.004258182 * Temp;
        return(DdryA);
}

double TempToLHV(double Temp)
{
        double LHV;
        LHV = 2.501 + -0.002372727 * Temp;
        return(LHV);
}

double TempToSFS(double Temp)
{
        double SlopeFS;
        SlopeFS = 0.338376068 +  0.011435897 * Temp +  0.001111111 * pow(Temp,2);
        return(SlopeFS);
}

double TempToSWVC(double Temp)
{
        double SWVC;
        SWVC =  4.90820192 +   0.06387253 * Temp +    0.02745742 * pow(Temp,2);
        return(SWVC);
}

/* EvapoTrans function */
struct ET_Str EvapoTrans(double Rad, double Itot, double Airtemperature, double RH,
                         double WindSpeed,double LeafAreaIndex, double CanopyHeight, double StomataWS, int ws,
                         double vmax2, double alpha2, double kparm, double theta, double beta, double Rd2, double b02, double b12,double upperT,double lowerT, double Catm)
{
        /* creating the structure to return */
        struct ET_Str tmp;
        struct c4_str tmpc4;

        const double LeafWidth = 0.04;
        const double kappa = 0.41;
        const double WindSpeedHeight = 5;
        const double dCoef = 0.77;
        const double tau = 0.2;
        const double ZetaCoef = 0.026;
        const double ZetaMCoef = 0.13;
        const double LeafReflectance = 0.2;
        const double SpecificHeat = 1010;

        double Tair, WindSpeedTopCanopy;
        double DdryA, LHV, SlopeFS, SWVC;
        double LayerRelativeHumidity, LayerWindSpeed, totalradiation;
        double LayerConductance, DeltaPVa, PsycParam, ga;
        double BoundaryLayerThickness, DiffCoef,LeafboundaryLayer;
        double d, Zeta, Zetam, ga0, ga1, ga2; 
        double Ja, Deltat;
        double PhiN;
        double TopValue, BottomValue;
        double EPen, TransR,EPries; 
        double OldDeltaT, rlc, ChangeInLeafTemp; 
        int Counter;

        WindSpeedTopCanopy = WindSpeed;
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

        /*' Convert light assuming 1 µmol PAR photons = 0.235 J/s Watts*/
        totalradiation = Itot * 0.235;

        tmpc4 = c4photoC(Itot,Airtemperature,RH,vmax2,alpha2,kparm,theta,beta,Rd2,b02,b12,StomataWS, Catm, ws,upperT,lowerT); 
        LayerConductance = tmpc4.Gs;

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

        DiffCoef = (2.126 * 1e-5) + ((1.48 * 1e-7) * Airtemperature);
        BoundaryLayerThickness = 0.004 * sqrt(LeafWidth / LayerWindSpeed);
        LeafboundaryLayer = DiffCoef / BoundaryLayerThickness;

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
        /*  res[0] = ((TransR * 1e3) / 18) * 1e3 ; */
        /*  res[1] = Deltat; */
        /*  res[2] = LayerConductance; */
        /* Let us return the structure now */

        tmp.TransR = TransR * 1e6 / 18; 
        tmp.EPenman = EPen * 1e6 / 18; 
        tmp.EPriestly = EPries * 1e6 / 18; 
        tmp.Deltat = Deltat;
        tmp.LayerCond = LayerConductance * 1e6 * (1/24.39);   
        /*    tmp.LayerCond = RH2;   */
        /*   tmp.LayerCond = 0.7; */
        return(tmp);
}

/* Soil Evaporation Function */
/* Variables I need */
/* LAI = Leaf Area Index */
/* k = extinction coefficient */
/* Temp = Air Temperature */
/* DirectRad = Direct Total Radiation */
/* awc, wiltp, fieldc = available water content, wilting point and field capacity */
/* winds = wind speed */

double SoilEvapo(double LAI, double k, double AirTemp, double IRad,
                 double awc, double fieldc, double wiltp, double winds, double RelH, double rsec ){

        double SoilArea;
        double SoilTemp;
        double Up; /*Maximum Dimensionless Uptake Rate */
        double TotalRadiation;
        double BoundaryLayerThickness, DiffCoef;
        double SoilBoundaryLayer, Ja, rlc;
        double PhiN, PsycParam, DeltaPVa;
        double Evaporation = 0.0;  
        double DdryA, LHV, SlopeFS, SWVC;

        double rawc; /* Relative available water content */

        int method = 1;

        /* some constants */
        const double SoilClodSize = 0.04;
        const double SoilReflectance = 0.2;
        const double SoilTransmission = 0.01;
        const double SpecificHeat = 1010;
        const double StefanBoltzman = 5.67e-8; /* Stefan Boltzman Constant */

        const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 

        /* For Transpiration */
        /* 3600 converts seconds to hours */
        /* 1e-3 converts mili mols to mols */
        /* 18 is the grams in one mol of H20 */
        /* 1e-6 converts g to Mg */
        /* 10000 scales from meter squared to hectare */

        /* Let us assume a simple way of calculating the proportion of the
           soil with direct radiation */
        SoilArea = exp(-k * LAI);

        /* For now the temperature of the soil will be the same as the air.
           At a later time this can be made more accurate. I looked at the
           equations for this and the issue is that it is strongly dependent on
           depth. Since the soil model now has a single layer, this cannot be
           implemented correctly at the moment.  */

        SoilTemp = AirTemp;

        /* Let us use an idea of Campbell and Norman. Environmental
           Biophysics. */
        /* If relative available water content is */
        rawc = (awc - wiltp)/(fieldc - wiltp);

        /* Page 142 */
        /* Maximum Dimensionless Uptake Rate */
        Up = 1 - pow((1 + 1.3 * rawc),-5);  
        /* This is a useful idea because dry soils evaporate little water when dry*/

        /* Total Radiation */
        /*' Convert light assuming 1 µmol PAR photons = 0.235 J/s Watts*/
        /* At the moment soil evaporation is grossly overestimated. In WIMOVAC
           the light reaching the last layer of leaves is used. Here instead
           of calculating this again, I will for now assume a 10% as a rough
           estimate. Note that I could maybe get this since layIdir and
           layIDiff in sunML are external variables.  Rprintf("IRad
           %.5f",layIdir[0],"\n"); Update: 03-13-2009. I tried printing this
           value but it is still too high and will likely overestimate soil
           evaporation. However, this is still a work in progress.
        */
        IRad *= rsec; /* Radiation soil evaporation coefficient */ 

        TotalRadiation = IRad * 0.235;
 
        DdryA = TempToDdryA(AirTemp) ;
        LHV = TempToLHV(AirTemp) * 1e6 ; 
/* Here LHV is given in MJ kg-1 and this needs to be converted
   to Joules kg-1  */
        SlopeFS = TempToSFS(AirTemp) * 1e-3;
        SWVC = TempToSWVC(AirTemp) * 1e-3;

        PsycParam = (DdryA * SpecificHeat) / LHV;
        DeltaPVa = SWVC * (1 - RelH / 100);

        BoundaryLayerThickness = 4e-3 * sqrt(SoilClodSize / winds); 
        DiffCoef = 2.126e-5 * 1.48e-7 * SoilTemp;
        SoilBoundaryLayer = DiffCoef / BoundaryLayerThickness;

        Ja = 2 * TotalRadiation * ((1 - SoilReflectance - SoilTransmission) / (1 - SoilTransmission));

        rlc = 4 * StefanBoltzman * pow((273 + SoilTemp),3) * 0.005;
/* the last term should be the difference between air temperature and
   soil. This is not actually calculated at the moment. Since this is
   mostly relevant to the first soil layer where the temperatures are
   similar. I will leave it like this for now. */

        PhiN = Ja - rlc; /* Calculate the net radiation balance*/
        if(PhiN < 0) PhiN = 1e-7;

        /* Priestly-Taylor */
        if(method == 0){
                Evaporation = 1.26 * (SlopeFS * PhiN) / (LHV * (SlopeFS + PsycParam));
        }else{
                /* Penman-Monteith */
                Evaporation = (SlopeFS * PhiN + LHV * PsycParam * SoilBoundaryLayer * DeltaPVa) / (LHV * (SlopeFS + PsycParam));
        }
/*  Report back the soil evaporation rate in Units mmoles/m2/s */
/*     Evaporation = Evaporation * 1000:   ' Convert Kg H20/m2/s to g H20/m2/s */
/*     Evaporation = Evaporation / 18:     ' Convert g H20/m2/s to moles H20/m2/s */
/*     Evaporation = Evaporation * 1000:   ' Convert moles H20/m2/s to mmoles H20/m2/s */
    
/*     If Evaporation <= 0 Then Evaporation = 0.00001: 
       ' Prevent any odd looking values which might get through at very low light levels */

        Evaporation *= 1e6/18;
        /* Adding the area dependence and the effect of drying */
        /* Converting from m2 to ha (times 1e4) */
        /* Converting to hour */
        Evaporation *= SoilArea * Up * cf2; 
        if(Evaporation < 0) Evaporation = 1e-6;

        return(Evaporation);
}


/* CanA function */
/* This file will contain the function which will consolidate
   the previous functions into one. The idea is to go from
   existing LAI and weather conditions to canopy assimilation
   and transpiration */

struct Can_Str CanAC(double LAI,int DOY, int hr,double solarR,double Temp,
                     double RH,double WindSpeed,double lat,int nlayers, double Vmax,
                     double Alpha, double Kparm, double theta, double beta,
                     double Rd, double Catm, double b0, double b1,
                     double StomataWS, int ws, double kd, double chil, double heightf,
                     double leafN, double kpLN, double lnb0, double lnb1, int lnfun,double upperT, double lowerT,struct nitroParms nitroP)
{

        struct ET_Str tmp5_ET, tmp6_ET;
        struct Can_Str ans;
        struct c4_str tmpc4;
        struct c4_str tmpc42;

        int i;
        double Idir, Idiff, cosTh;
        double LAIc;
        double IDir, IDiff, Itot, rh, WS;
        double pLeafsun, pLeafshade;
        double Leafsun, Leafshade;
        double CanHeight;

        double vmax1, leafN_lay;
        double TempIdir,TempIdiff,AssIdir,AssIdiff,GAssIdir,GAssIdiff;

        double CanopyA, CanopyT,GCanopyA;

        const double cf = 3600 * 1e-6 * 30 * 1e-6 * 10000;
        const double cf2 = 3600 * 1e-3 * 18 * 1e-6 * 10000; 

        /* For Assimilation */
        /* 3600 converts seconds to hours */
        /* 1e-6 converts micro mols to mols */
        /* 30 is the grams in one mol of CO2 */
        /* 1e-6 converts g to Mg */
        /* 10000 scales from meter squared to hectare */

        /* For Transpiration */
        /* 3600 converts seconds to hours */
        /* 1e-3 converts mili mols to mols */
        /* 18 is the grams in one mol of H20 */
        /* 1e-6 converts g to Mg */
        /* 10000 scales from meter squared to hectare */

        lightME(lat,DOY,hr);

        Idir = tmp1[0] * solarR;
        Idiff = tmp1[1] * solarR;
        cosTh = tmp1[2];
    
        sunML(Idir,Idiff,LAI,nlayers,cosTh, kd, chil, heightf);

        /* results from multilayer model */
        LAIc = LAI / nlayers;
        /* Next I need the RH and wind profile */
        RHprof(RH,nlayers);
        WINDprof(WindSpeed,LAI,nlayers);

        LNprof(leafN, LAI, nlayers, kpLN);
        /* It populates tmp5 */

        /* Next use the EvapoTrans function */
        CanopyA=0.0;
        GCanopyA=0.0;
        CanopyT=0.0;
        for(i=0;i<nlayers;i++)
        {
                leafN_lay = tmp5[--tp5];
                if(lnfun == 0){
                        vmax1 = Vmax;
                }else{
                        vmax1=nitroP.Vmaxb1*leafN_lay+nitroP.Vmaxb0;
  				              if(vmax1<0) vmax1=0.0;
					              Alpha=nitroP.alphab1*leafN_lay+nitroP.alphab0;
					               Rd=nitroP.Rdb1*leafN_lay+nitroP.Rdb0;
               /* For now alpha is not affected by leaf nitrogen */
                }

                IDir = layIdir[--sp1];
                Itot = layItotal[--sp3];

                rh = tmp4[--tp4];
                WS = tmp3[--tp3];
                pLeafsun = layFsun[--sp4];
                CanHeight = layHeight[--sp6];
                Leafsun = LAIc * pLeafsun;
                tmp5_ET = EvapoTrans(IDir,Itot,Temp,rh,WS,LAIc,CanHeight,StomataWS,ws,vmax1,Alpha,Kparm,theta,beta,Rd,b0,b1,upperT,lowerT,Catm);
                TempIdir = Temp + tmp5_ET.Deltat;
                tmpc4 = c4photoC(IDir,TempIdir,rh,vmax1,Alpha,Kparm,theta,beta,Rd,b0,b1,StomataWS, Catm, ws,upperT,lowerT);
                AssIdir = tmpc4.Assim;
                GAssIdir =tmpc4.GrossAssim;

                IDiff = layIdiff[--sp2];
                pLeafshade = layFshade[--sp5];
                Leafshade = LAIc * pLeafshade;
                tmp6_ET = EvapoTrans(IDiff,Itot,Temp,rh,WS,LAIc,CanHeight,StomataWS,ws,vmax1,Alpha,Kparm,theta,beta,Rd,b0,b1,upperT,lowerT,Catm);
                TempIdiff = Temp + tmp6_ET.Deltat;
                tmpc42 = c4photoC(IDiff,TempIdiff,rh,vmax1,Alpha,Kparm,theta,beta,Rd,b0,b1,StomataWS, Catm, ws,upperT,lowerT);
                AssIdiff = tmpc42.Assim;
                GAssIdiff = tmpc42.GrossAssim;
                CanopyA += Leafsun * AssIdir + Leafshade * AssIdiff;
                GCanopyA += Leafsun * GAssIdir + Leafshade * GAssIdiff;
// I am evaluating CanopyT using Penman Method because it gives realistic results
// IN future canopyT needs to be fixed
//                CanopyT += Leafsun * tmp5_ET.TransR + Leafshade * tmp6_ET.TransR;
                CanopyT += Leafsun * tmp5_ET.EPenman + Leafshade * tmp6_ET.EPenman;
        }
        /*## These are micro mols of CO2 per m2 per sec for Assimilation
          ## and mili mols of H2O per m2 per sec for Transpiration
          ## Need to convert to 
          ## 3600 converts seconds to hours
          ## 10^-6 converts micro mols to mols
          ## 30 converts mols of CO2 to grams
          ## (1/10^6) converts grams to Mg
          ## 10000 scales up to ha */
/* A similar conversion is made for water but
   replacing 30 by 18 and mili mols are converted to
   mols (instead of micro) */
        ans.Assim = cf * CanopyA ;
        ans.Trans = cf2 * CanopyT; 
        ans.GrossAssim=cf*GCanopyA;
        return(ans);
}


/* This is a new function that attempts to keep a water budget and then
   calcualte an empirical coefficient that reduces the specific leaf area.
   This results from the general idea that water stress reduces first the 
   rate of leaf expansion. */ 

/* This is meant to be a simple function that calculates a
   simple empirical coefficient that reduces specifi leaf area
   according to the water stress of the plant. This is done
   for now, with a very simple empirical approach. */

struct ws_str watstr(double precipit, double evapo, double cws, double soildepth, 
                     double fieldc, double wiltp, double phi1, double phi2, 
                     int soiltype, /* soil type indicator */ 
                     int wsFun) /* flag for which water stress function to use */
{

        struct ws_str tmp;
        struct soilText_str soTexS;
        /* Variables */
        double precipM;
        /* available water and per hectare */
        double aw, naw; 
        double pawha, Newpawha, npaw; /* new 04-27-2009 */
        double runoff = 0.0, runoff2 = 0.0;
        /* variable needed for calculation of water stress*/
        double wsPhoto = 0.0, wsSpleaf, phi10;
        double slp = 0.0, intcpt = 0.0, theta = 0.0; 
        double Nleach = 0.0;
        /* Nleach is the NO3 leached and Ts is the sand content of the soil*/

        /* Specify the soil type */
        soTexS = soilTchoose(soiltype);
/*   Ts = soTexS.sand; */

        if(fieldc < 0){
                fieldc = soTexS.fieldc;
        }
        if(wiltp < 0){
                wiltp = soTexS.wiltp;
        }

        /* unit conversion for precip */
        precipM = precipit * 1e-3; /* convert precip in mm to m*/

        /*    cws is current water status */
        /*    available water */

        aw = precipM + cws;

        /* if(aw > soTexS.satur){  */
        /*      runoff = aw - soTexS.satur; /\* Here runoff is interpreted as water content exceeding saturation level *\/ */
        /*      /\* Need to convert to units used in the Parton et al 1988 paper. *\/ */
        /*      /\* The data comes in mm/hr and it needs to be in cm/month *\/ */
        /*      runoff2 = runoff * 0.10 * (1/24*30); */
        /*      Nleach = runoff /18 * (0.2 + 0.7 * soTexS.sand); */
        /*      aw = soTexS.satur; */
        /* } */

/* These equations are not correct as runoff would only occur when it exceeds
   saturation, but from the point of view of a crop only field capacity matters */
/* I'm not sure about what to do about this */

        if(aw > fieldc){ 
                runoff = aw - fieldc; /* Here runoff is interpreted as water content exceeding saturation level */
                /* Need to convert to units used in the Parton et al 1988 paper. */
                /* The data comes in mm/hr and it needs to be in cm/month */
                runoff2 = runoff * 0.10 * (1/24*30);
                Nleach = runoff /18 * (0.2 + 0.7 * soTexS.sand);
                aw = fieldc;
        }


        /* Tipping bucket need to collect it if want to estimate runoff */ 
        /* plant available water per hectare (pawha) */
        pawha = (aw - wiltp) * 1e4 * soildepth;
        /* The density of water is 998.2 kg/m3 at 20 degrees Celsius */
        /* or 0.9882 Mg/m3 */
        /* pawha is plant available water (m3) per hectare */
        /* evapo is demanded water (Mg) per hectare */

        Newpawha = pawha - evapo / 0.9882; /* New version 04-27-2009 */

        /*  Here both are in m3 of water per ha-1 so this */
        /*  subtraction should be correct */
        /* go back to original units of water in the profile */

        npaw = Newpawha * 1e-4 * (1/soildepth); /* New 04-27-2009 */

        if(npaw < 0) npaw = 0.0;

        naw = npaw + wiltp;

        /* Calculating the soil water potential based on equations from Norman and Campbell */
        /* tmp.psim = soTexS.air_entry * pow((naw/soTexS.fieldc*1.1),-soTexS.b) ; */
        /* New version of the soil water potential is based on
         * "Dynamic Simulation of Water Deficit Effects upon Maize
         * Yield" R. F. Grant Agricultural Systems. 33(1990) 13-39. */
        tmp.psim = -exp(log(0.033) + ((log(fieldc) - log(naw))/(log(fieldc) - log(wiltp)) * (log(1.5) - log(0.033)))) * 1e3; /* This last term converts from MPa to kPa */

        /* three different type of equations for modeling the effect of water stress on vmax and leaf area expansion. 
           The equation for leaf area expansion is more severe than the one for vmax. */
        if(wsFun == 0){ /* linear */
                slp = 1/(fieldc - wiltp);
                intcpt = 1 - fieldc * slp;
                wsPhoto = slp * naw + intcpt ;
        }else
        if(wsFun == 1){
                phi10 = (fieldc + wiltp)/2;
                wsPhoto = 1/(1 + exp((phi10 - naw)/ phi1));
        }else
        if(wsFun == 2){
                slp = (1 - wiltp)/(fieldc - wiltp);
                intcpt = 1 - fieldc * slp;
                theta = slp * naw + intcpt ;
                wsPhoto = (1 - exp(-2.5 * (theta - wiltp)/(1 - wiltp))) / (1 - exp(-2.5));
        }else
                if(wsFun == 3){
                        wsPhoto = 1;
                }

        if(wsPhoto <= 0 )
                wsPhoto = 1e-20; /* This can be mathematically lower than zero in some cases but I should prevent that. */

        wsSpleaf = pow(naw,phi2) * 1/pow(fieldc,phi2); 
        if(wsFun == 3){ 
                wsSpleaf = 1;
        }

/* Apparently wsPhoto and wsSpleaf can be greater than 1 */
        if(wsPhoto > 1) wsPhoto = 1;
        if(wsSpleaf > 1) wsSpleaf = 1;

        /* returning the structure*/
        tmp.rcoefPhoto = wsPhoto;
        tmp.rcoefSpleaf = wsSpleaf;
        tmp.awc = naw;
        tmp.runoff = runoff;
        tmp.Nleach = Nleach;
        return(tmp);
}

/* Function to simulate the multilayer behavior of soil water. In the
   future this could be coupled with Campbell (BASIC) ideas to
   esitmate water potential. */
struct soilML_str soilML(double precipit, double transp, double *cws, double soildepth, double *depths, double fieldc, double wiltp, double phi1, double phi2, struct soilText_str soTexS, int wsFun, int layers, double rootDB, double LAI, double k, double AirTemp, double IRad, double winds, double RelH, int hydrDist, double rfl, double rsec, double rsdf){

        struct rd_str tmp4;
        struct seqRD_str tmp3;
        struct soilML_str tmp;
        /* Constant */
        /* const double G = 6.67428e-11;  m3 / (kg * s-2)  ##  http://en.wikipedia.org/wiki/Gravitational_constant */
        const double g = 9.8; /* m / s-2  ##  http://en.wikipedia.org/wiki/Standard_gravity */
        /* Variables */
        double waterIn, oldWaterIn = 0.0;
/* Here is a convention aw is available water in volume and awc
   is available water content as a fraction of the soil section being investigated.
   paw is plant available water aw - wiltp */
        double aw, paw, awc, awc2, Newpawha;
        double drainage = 0.0;
        double wsPhoto = 0.0, wsSpleaf = 0.0, phi10;
        double wsPhotoCol = 0.0, wsSpleafCol = 0.0;
        double slp = 0.0, intcpt = 0.0, theta = 0.0; 
        double Nleach = 0.0;
        double layerDepth;
        double diffw;
        double rootATdepth, rootDepth;
        double EvapoTra = 0.0, oldEvapoTra = 0.0, Sevap = 0.0, Ctransp = 0.0;
        double psim1 = 0.0, psim2 = 0.0, K_psim = 0.0, J_w = 0.0, dPsim = 0.0;
        double theta_s; /* This is the saturated soil water content. Larger than FieldC.*/
        int i;
        int j = layers - 1; 

        /* Specify the soil type */

        if(fieldc < 0){
                fieldc = soTexS.fieldc;
        }
        if(wiltp < 0){
                wiltp = soTexS.wiltp;
        }

        theta_s = soTexS.satur;
        /* rooting depth */
        /* Crude empirical relationship between root biomass and rooting depth*/
        rootDepth = rootDB * rsdf;
        if(rootDepth > soildepth) rootDepth = soildepth;

        tmp3 = seqRootDepth(rootDepth,layers);
        tmp4 = rootDist(layers,rootDepth,&depths[0],rfl);

        /* unit conversion for precip */
        waterIn = precipit * 1e-3; /* convert precip in mm to m*/

        for(j=0,i=layers-1;j<layers;j++,i--){
        /* for(i=0;i<layers;i++){ */
                /* It decreases because I increase the water content due to precipitation in the last layer first*/

                /* This supports unequal depths. */
                if(i == 0){
                        layerDepth = depths[1];
                }else{
                        layerDepth = depths[i] - depths[i-1];
                }


                if(hydrDist > 0){
                        /* For this section see Campbell and Norman "Environmental BioPhysics" Chapter 9*/
                        /* First compute the matric potential */
                        psim1 = soTexS.air_entry * pow((cws[i]/theta_s),-soTexS.b) ; /* This is matric potential of current layer */
                        if(i > 0){
                                psim2 = soTexS.air_entry * pow((cws[i-1]/theta_s),-soTexS.b) ; /* This is matric potential of next layer */
                                dPsim = psim1 - psim2;
                                /* The substraction is from the layer i - (i-1). If this last term is positive then it will move upwards. If it is negative it will move downwards. Presumably this term is almost always positive. */
                        }else{
                                dPsim = 0;
                        }
                        K_psim = soTexS.Ks * pow((soTexS.air_entry/psim1),2+3/soTexS.b); /* This is hydraulic conductivity */
                        J_w = K_psim * (dPsim/layerDepth) - g * K_psim ; /*  Campbell, pg 129 do not ignore the graviational effect*/
                        /* Notice that K_psim is positive because my
                            reference system is reversed */
                        /* This last result should be in kg/(m2 * s)*/
                         J_w *= 3600 * 0.9882 * 1e-3 ; /* This is flow in m3 / (m^2 * hr). */
                        /* Rprintf("J_w %.10f \n",J_w);  */
                        if(i == (layers-1) && J_w < 0){
                                        /* cws[i] = cws[i] + J_w /
                                         * layerDepth; Although this
                                         * should be done it drains
                                         * the last layer too much.*/
                                        drainage += J_w;
                        }else{
                                if(i > 0){
                                        cws[i] = cws[i] -  J_w / layerDepth;
                                        cws[i - 1] =  cws[i-1] +  J_w / layerDepth;
                                }else{
                                        cws[i] = cws[i] -  J_w / layerDepth;
                                }
                        }
                }

                 if(cws[i] > theta_s) cws[i] = theta_s; 
                /* if(cws[i+1] > fieldc) cws[i+1] = fieldc; */
                 if(cws[i] < wiltp) cws[i] = wiltp; 
                /* if(cws[i+1] < wiltp) cws[i+1] = wiltp;  */

                aw = cws[i] * layerDepth;
/* Available water (for this layer) is the current water status times the layer depth */

                if(waterIn > 0){
                        /* There is some rain. Need to add it.*/
                        aw += waterIn / layers + oldWaterIn; /* They are both in meters so it works */
                        /* Adding the same amount to water to each layer */
                        /* In case there is overflow */
                        diffw = fieldc * layerDepth - aw;

                        if(diffw < 0){
                                /* This means that precipitation exceeded the capacity of the first layer */
                                /* Save this amount of water for the next layer */
                                oldWaterIn = -diffw;
                                aw = fieldc * layerDepth;
                        }else{
                                oldWaterIn = 0.0;
                        }
                }

                /* Root Biomass */
                rootATdepth = rootDB * tmp4.rootDist[i];
                tmp.rootDist[i] = rootATdepth;
/* Plant available water is only between current water status and permanent wilting point */
                /* Plant available water */
                paw = aw - wiltp * layerDepth;
                if(paw < 0) paw = 0; 

                if(i == 0){
                        /* Only the first layer is affected by soil evaporation */
                        awc2 = aw / layerDepth;
                        /* SoilEvapo function needs soil water content  */
                        Sevap = SoilEvapo(LAI,k,AirTemp,IRad,awc2,fieldc,wiltp,winds,RelH,rsec);
                        /* I assume that crop transpiration is distributed simlarly to
                           root density.  In other words the crop takes up water proportionally
                           to the amount of root in each respective layer.*/
                        Ctransp = transp*tmp4.rootDist[0];
                        EvapoTra = Ctransp + Sevap;
                        Newpawha = (paw * 1e4) - EvapoTra / 0.9982; /* See the watstr function for this last number 0.9882 */
                        /* The first term in the rhs (paw * 1e4) is the m3 of water available in this layer.
                           EvapoTra is the Mg H2O ha-1 of transpired and evaporated water. 1/0.9882 converts from Mg to m3 */
                }else{
                        Ctransp = transp*tmp4.rootDist[i];
                        EvapoTra = Ctransp;
                        Newpawha = (paw * 1e4) - (EvapoTra + oldEvapoTra);
                }

                if(Newpawha < 0){
/* If the Demand is not satisfied by this layer. This will be stored and added to subsequent layers*/
                        oldEvapoTra = -Newpawha;
                         aw = wiltp * layerDepth; 
                }

                paw = Newpawha / 1e4 ;
                awc = paw / layerDepth + wiltp;   

/* This might look like a weird place to populate the structure, but is more convenient*/
                tmp.cws[i] = awc;
                tmp.Wflux[i] =J_w;
                if(wsFun == 0){
                        slp = 1/(fieldc - wiltp);
                        intcpt = 1 - fieldc * slp;
                        wsPhoto = slp * awc + intcpt ;
                }else
                if(wsFun == 1){
                        phi10 = (fieldc + wiltp)/2;
                        wsPhoto = 1/(1 + exp((phi10 - awc)/ phi1));
                }else
                if(wsFun == 2){
                        slp = (1 - wiltp)/(fieldc - wiltp);
                        intcpt = 1 - fieldc * slp;
                        theta = slp * awc + intcpt ;
                        wsPhoto = (1 - exp(-2.5 * (theta - wiltp)/(1 - wiltp))) / (1 - exp(-2.5));
                }else
                if(wsFun == 3){
                        wsPhoto = 1;
                }

                if(wsPhoto <= 0 )
                        wsPhoto = 1e-20; /* This can be mathematically lower than zero in some cases but I should prevent that. */

                wsPhotoCol += wsPhoto;

                wsSpleaf = pow(awc,phi2) * 1/pow(fieldc,phi2); 
                if(wsFun == 3){ 
                        wsSpleaf = 1;
                }
                wsSpleafCol += wsSpleaf;

        }

        if(waterIn > 0){ 
                drainage = waterIn;
                /* Need to convert to units used in the Parton et al 1988 paper. */
                /* The data comes in mm/hr and it needs to be in cm/month */
                Nleach = drainage * 0.1 * (1/24*30) / (18 * (0.2 + 0.7 * soTexS.sand));
        }

/* Apparently wsPhoto and wsSpleaf can be greater than 1 */
        if(wsPhoto > 1) wsPhoto = 1;
        if(wsSpleaf > 1) wsSpleaf = 1;

/* returning the structure */
        tmp.rcoefPhoto = (wsPhotoCol/layers);
        tmp.drainage = drainage;
        tmp.Nleach = Nleach;
        tmp.rcoefSpleaf = (wsSpleafCol/layers);
        tmp.SoilEvapo = Sevap;

        return(tmp);
}

/* Respiration. It is assumed that some of the energy produced by the
   plant has to be used in basic tissue maintenance. This is handled
   quite empirically by some relationships developed by McCree (1970)
   and Penning de Vries (1972) */

double resp(double comp, double mrc, double temp){

        double ans;

        ans = comp *  (1 - (mrc * pow(2,(temp/10.0))));

        if(ans <0) ans = 0;

        return(ans);

}

/* Function to select the correct dry biomass partitioning coefficients */
/* It should take a vector of size 24 as an argument and return a structure with four numbers */

struct dbp_str sel_dbp_coef(double coefs[25], double TherPrds[6], double TherTime){

        struct dbp_str tmp;

        tmp.kLeaf = 0.0;
        tmp.kStem = 0.0;
        tmp.kRoot = 0.0;
        tmp.kRhiz = 0.0;
        tmp.kGrain = 0.0; /* kGrain is always zero except for the last thermal period */

        if(TherTime < TherPrds[0])
        {
                tmp.kStem = coefs[0];
                tmp.kLeaf = coefs[1];
                tmp.kRoot = coefs[2];
                tmp.kRhiz = coefs[3];

        } else
                if( TherTime < TherPrds[1] )
                {
                        tmp.kStem = coefs[4];
                        tmp.kLeaf = coefs[5];
                        tmp.kRoot = coefs[6];
                        tmp.kRhiz = coefs[7];

                } else
                        if( TherTime < TherPrds[2])
                        {
                                tmp.kStem = coefs[8];
                                tmp.kLeaf = coefs[9];
                                tmp.kRoot = coefs[10];
                                tmp.kRhiz = coefs[11];

                        } else
                                if(TherTime < TherPrds[3])
                                {
                                        tmp.kStem = coefs[12];
                                        tmp.kLeaf = coefs[13];
                                        tmp.kRoot = coefs[14];
                                        tmp.kRhiz = coefs[15];

                                } else
                                        if(TherTime < TherPrds[4])
                                        {
                                                tmp.kStem = coefs[16];
                                                tmp.kLeaf = coefs[17];
                                                tmp.kRoot = coefs[18];
                                                tmp.kRhiz = coefs[19];

                                        } else{
                                                
                                                        tmp.kStem = coefs[20];
                                                        tmp.kLeaf = coefs[21];
                                                        tmp.kRoot = coefs[22];
                                                        tmp.kRhiz = coefs[23];
                                                        tmp.kGrain = coefs[24];
                                                
                                                }


        return(tmp);

}

struct seqRD_str seqRootDepth(double to, int lengthOut){

        struct seqRD_str tmp;
        int i;
        double by;

        /* This is because in this case from is always zero */
        by = to / lengthOut;

        for(i=0;i<=lengthOut;i++){

                tmp.rootDepths[i] = i * by;

        }

        return(tmp);

}


struct rd_str rootDist(int layer, double rootDepth, double *depthsp, double rfl){

        struct rd_str tmp;  
        int i, j, k;
        double layerDepth = 0.0;
        double CumLayerDepth = 0.0;
        double CumRootDist = 1.0;
        double rootDist[layer];
        double ca = 0.0, a = 0.0;

        for(i=0;i<layer;i++){

                if(i == 0){
                        layerDepth = depthsp[1];
                }else{
                        layerDepth = depthsp[i] - depthsp[i-1];
                }

                CumLayerDepth += layerDepth;

                if(rootDepth > CumLayerDepth){
                        CumRootDist++;
                }
        }

        for(j=0;j<layer;j++){
                if(j < CumRootDist){ 
                        a = dpois(j+1,CumRootDist*rfl,0);
                        rootDist[j] = a;
                        ca += a;
                }else{
                        rootDist[j] = 0;
                }
        }

        for(k=0;k<layer;k++){
                tmp.rootDist[k] = rootDist[k] / ca; 
        }

        return(tmp);
}


struct soilText_str soilTchoose(int soiltype){

        /* This function is based on Campbell and Norman.
           Introduction to Environmental Biophysics. pg 130. */

        struct soilText_str tmp;

        tmp.silt = 0;
        tmp.clay = 0;
        tmp.sand = 0;
        tmp.air_entry = 0;
        tmp.b = 0;
        tmp.Ks = 0;
        tmp.satur = 0;
        tmp.fieldc = 0;
        tmp.wiltp = 0;

        if(soiltype == 0){
        /* sand soil */
        tmp.silt = 0.05;
        tmp.clay = 0.03;
        tmp.sand = 0.92;
        tmp.air_entry = -0.7;
        tmp.b = 1.7;
        tmp.Ks = 5.8e-3;
        tmp.satur = 0.87;
        tmp.fieldc = 0.09;
        tmp.wiltp = 0.03;

        } else

        if(soiltype == 1){
        /* loamy sand */
        tmp.silt = 0.12;
        tmp.clay = 0.07;
        tmp.sand = 0.81;
        tmp.air_entry = -0.9;
        tmp.b = 2.1;
        tmp.Ks = 1.7e-3;
        tmp.satur = 0.72;
        tmp.fieldc = 0.13;
        tmp.wiltp = 0.06;

        } else

        if(soiltype == 2){
        /* sandy loam */
        tmp.silt = 0.25;
        tmp.clay = 0.10;
        tmp.sand = 0.65;
        tmp.air_entry = -1.5;
        tmp.b = 3.1;
        tmp.Ks = 7.2e-4;
        tmp.satur = 0.57;
        tmp.fieldc = 0.21;
        tmp.wiltp = 0.10;

        } else

        if(soiltype == 3){
        /* loam */
        tmp.silt = 0.40;
        tmp.clay = 0.18;
        tmp.sand = 0.52;
        tmp.air_entry = -1.1;
        tmp.b = 4.5;
        tmp.Ks = 3.7e-4;
        tmp.satur = 0.57;
        tmp.fieldc = 0.27;
        tmp.wiltp = 0.12;

        } else

        if(soiltype == 4){
        /* silt loam */
        tmp.silt = 0.65;
        tmp.clay = 0.15;
        tmp.sand = 0.20;
        tmp.air_entry = -2.1;
        tmp.b = 4.7;
        tmp.Ks = 1.9e-4;
        tmp.satur = 0.59;
        tmp.fieldc = 0.33;
        tmp.wiltp = 0.13;

        } else

        if(soiltype == 5){
        /* sandy clay loam */
        tmp.silt = 0.13;
        tmp.clay = 0.27;
        tmp.sand = 0.60;
        tmp.air_entry = -2.8;
        tmp.b = 4;
        tmp.Ks = 1.2e-4;
        tmp.satur = 0.48;
        tmp.fieldc = 0.26;
        tmp.wiltp = 0.15;

        } else

        if(soiltype == 6){
        /* clay loam */
        tmp.silt = 0.34;
        tmp.clay = 0.34;
        tmp.sand = 0.32;
        tmp.air_entry = -2.6;
        tmp.b = 5.2;
        tmp.Ks = 6.4e-5;
        tmp.satur = 0.52;
        tmp.fieldc = 0.32;
        tmp.wiltp = 0.20;

        } else

        if(soiltype == 7){
        /* silty clay loam */
        tmp.silt = 0.58;
        tmp.clay = 0.33;
        tmp.sand = 0.09;
        tmp.air_entry = -3.3;
        tmp.b = 6.6;
        tmp.Ks = 4.2e-5;
        tmp.satur = 0.52;
        tmp.fieldc = 0.37;
        tmp.wiltp = 0.21; /* Correction from the book from here http://www.public.iastate.edu/~bkh/teaching/505/norman_book_corrections.pdf */

        } else

        if(soiltype == 8){
        /* sandy clay */
        tmp.silt = 0.07;
        tmp.clay = 0.40;
        tmp.sand = 0.53;
        tmp.air_entry = -2.9;
        tmp.b = 6;
        tmp.Ks = 3.3e-5;
        tmp.satur = 0.51;
        tmp.fieldc = 0.34;
        tmp.wiltp = 0.24;

        } else

        if(soiltype == 9){
        /* silty clay */
        tmp.silt = 0.45;
        tmp.clay = 0.45;
        tmp.sand = 0.10;
        tmp.air_entry = -3.4;
        tmp.b = 7.9;
        tmp.Ks = 2.5e-5;
        tmp.satur = 0.52;
        tmp.fieldc = 0.39;
        tmp.wiltp = 0.25;

        } else

        if(soiltype == 10){
        /* clay */
        tmp.silt = 0.20;
        tmp.clay = 0.60;
        tmp.sand = 0.20;
        tmp.air_entry = -3.7;
        tmp.b = 7.6;
        tmp.Ks = 1.7e-5;
        tmp.satur = 0.53;
        tmp.fieldc = 0.4;
        tmp.wiltp = 0.27;

        }

        return(tmp);

}

void cropcent_dbp(double coefs[25],double TherPrds[6], double TherTime, struct crop_phenology *cropdbp)

// struct cropcent_phenolohy cropcent_dbp(double coefs[25], double TherPrds[6], double TherTime)

     {     
        struct dbp_str tmp;
        // call old biomass partiitoning coefficient function
        tmp =sel_dbp_coef(coefs, TherPrds, TherTime);
        //assing it to the relevant sub structure of new cropdbp, to be retirned back to the main program
        cropdbp->DBP=tmp;
       // Here I must define what are CN ratios of different plant components under excess N fertilization
       
       
       // I can leave it empty for now
       return;
     }


