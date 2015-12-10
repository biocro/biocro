/*
 *  BioCro/src/CanA.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 *
 */

#ifndef CANA_H
#define CANA_H

struct ET_Str EvapoTrans2(double Rad, double Iave, double Airtemperature, double RH,
			 double WindSpeed,double LeafAreaIndex, double CanopyHeight, 
			  double stomatacond, double leafw, int eteq);
struct ET_Str EvapoTrans(double Rad, double Itot, double Airtemperature, double RH,
			 double WindSpeed,double LeafAreaIndex, double CanopyHeight, double StomataWS, int ws,
			 double vmax2, double alpha2, double kparm, double theta, double beta, double Rd2, double b02, double b12,double upperT,double lowerT);

struct Light_model {
	double irradiance_direct;
	double irradiance_diffuse;
	double cosine_zenith_angle;
};

double TempToSWVC(double Temp);
double TempToSFS(double Temp);
double TempToLHV(double Temp);
double TempToDdryA(double Temp);
void RHprof(double RH, int nlayers, double* relative_humidity_profile);
void WINDprof(double WindSpeed, double LAI, int nlayers, double* wind_speed_profile);
void LNprof(double LeafN, double LAI, int nlayers, double kpLN, double* leafN_layer);
void sunML(double Idir, double Idiff, double LAI, int nlayers, 
	   double cosTheta, double kd, double chil, double heightf);
struct Light_model lightME(double lat, int DOY, int td);

#endif
