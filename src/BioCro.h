/*
 *  BioCro/src/BioCro.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2008
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 or 3 of the License
 *  (at your option).
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  A copy of the GNU General Public License is available at
 *  http://www.r-project.org/Licenses/
 *
 */

/* Global variables for BioGro function. */

double CanopyAssim[8760] ;
double Leafy[8760] ;
double Stemy[8760] ;
double Rooty[8760] ;
double Rhizomey[8760] ;
double Grainy[8760] ;
double LAIc[8760] ;

double RSS_BG(double oStem[], double oLeaf[],
	      double oRhizome[],double oRoot[],
	      double oGrain[],
	      double oLAI[],
	      double sStem[], double sLeaf[],
	      double sRhizome[],double sRoot[],
	      double sGrain[],
	      double sLAI[] ,int N1Dat);

void BioGro(double lat, int doy[],int hr[],double solar[],double temp[],double rh[],
	    double windspeed[],double precip[], double kd, double chil, 
	    double heightf, int nlayers,
            double iRhizome, double irtl, double sencoefs[], int timestep, int vecsize,
            double Sp, double SpD, double dbpcoefs[25], double thermalp[], double vmax1, 
	    double alpha1, double kparm, double theta, double beta, double Rd, double Catm, double b0, double b1, 
	    double soilcoefs[], double ileafn, double kLN, double vmaxb1,
	    double alphab1, double mresp[], int soilType, int wsFun, int ws, double centcoefs[],
	    double centks[], int centTimestep, int soilLayers, double soilDepths[],
	    double cws[], int hydrDist, double secs[], double kpLN, double lnb0, double lnb1, int lnfun ,double upperT,double lowerT);

struct Can_Str CanAC(double LAI,int DOY,int hr,double solarR,double Temp,
		     double RH,double WindSpeed,double lat,int nlayers, double Vmax, double Alpha, 
		     double Kparm, double theta, double beta, double Rd, double Catm, double b0, 
		     double b1, double StomataWS, int ws, double kd, double chil, double heightf,
		     double leafN, double kpLN, double lnb0, double lnb1, int lnfun,double upperT,double lowerT);

struct dbp_str sel_dbp_coef(double coefs[25], double TherPrds[6], double TherTime);

double resp(double comp, double mrc, double temp);

struct ws_str watstr(double precipit, double evapo, double cws, double soildepth, 
                     double fieldc, double wiltp, double phi1, double phi2, int soiltype, int wsFun);

/* struct ET_Str EvapoTrans(double Rad, double Itot, double Airtemperature, double RH, */
/* 			 double WindSpeed,double LeafAreaIndex, double CanopyHeight, double StomataWS, int ws, */
/* 			 double vmax2, double alpha2, double kparm, double theta, double beta, */
/* 			 double Rd2, double b02, double b12); */

double SoilEvapo(double LAI, double k, double AirTemp, double DirectRad,
		 double awc, double fieldc, double wiltp, double winds, double RelH, double rsec);

struct soilML_str soilML(double precipit, double transp, double *cws, double soildepth,
			 double *depths, double fieldc, double wiltp, double phi1, double phi2,
                         struct soilText_str soTexS, int wsFun, int layers, double rootDB,
			 double LAI, double k, double AirTemp, double IRad, double winds, double RelH,
			 int hydrDist, double rfl, double rsec, double rsdf);

double TempToSWVC(double Temp);
double TempToSFS(double Temp);
double TempToLHV(double Temp);
double TempToDdryA(double Temp);
void RHprof(double RH, int nlayers);
void WINDprof(double WindSpeed, double LAI, int nlayers);
void sunML(double Idir, double Idiff, double LAI, int nlayers, double cosTheta, double kd, double chil);
void lightME(double lat, int DOY, int td);

struct cenT_str Century(double *LeafL, double *StemL, double *RootL, double *RhizL, double smoist, double stemp, int timestep, 
			double SCs[9] , double leachWater, double Nfert, double MinN, double precip,
			double LeafL_Ln, double StemL_Ln, double RootL_Ln, double RhizL_Ln,
			double LeafL_N, double StemL_N, double RootL_N, double RhizL_N, int soilType,
			double Ks_cf[8]);

struct FL_str FmLcFun(double Lig, double Nit);

struct flow_str flow(double *SC, double CNratio, double A, double Lc, double Tm, double resp, int kno, double Ks[8]);
double AbiotEff(double smoist, double stemp);

double sel_phen(int phen);
