/*
 *  BioCro/src/CanA.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
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


struct ET_Str EvapoTrans(double Rad, double Itot, double Airtemperature, double RH,
			 double WindSpeed,double LeafAreaIndex, double CanopyHeight, double StomataWS, int ws,
			 double vmax2, double alpha2, double kparm, double theta, double beta, double Rd2, double b02, double b12);

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
