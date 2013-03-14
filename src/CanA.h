/*
 *  BioCro/src/CanA.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 *
 */


struct ET_Str EvapoTrans(double Rad, double Itot, double Airtemperature, double RH,
			 double WindSpeed,double LeafAreaIndex, double CanopyHeight, double StomataWS, int ws,
			 double vmax2, double alpha2, double kparm, double theta, double beta, double Rd2, double b02, double b12,double upperT,double lowerT);

double TempToSWVC(double Temp);
double TempToSFS(double Temp);
double TempToLHV(double Temp);
double TempToDdryA(double Temp);
void RHprof(double RH, int nlayers);
void WINDprof(double WindSpeed, double LAI, int nlayers);
void LNprof(double LeafN, double LAI, int nlayers, double kpLN);
void sunML(double Idir, double Idiff, double LAI, int nlayers, 
	   double cosTheta, double kd, double chil, double heightf);
void lightME(double lat, int DOY, int td);
