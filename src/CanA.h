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

double TempToSWVC(double Temp);
double TempToSFS(double Temp);
double TempToLHV(double Temp);
double TempToDdryA(double Temp);
void RHprof(double RH, int nlayers, double* relative_humidity_profile);
void WINDprof(double WindSpeed, double LAI, int nlayers, double* wind_speed_profile);
void LNprof(double LeafN, double LAI, int nlayers, double kpLN, double* leafN_layer);

#endif
