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
struct Light_profile sunML(double Idir, double Idiff, double LAI, int nlayers, 
	   double cosTheta, double kd, double chil, double heightf);
struct Light_model lightME(double lat, int DOY, int td);

#endif
