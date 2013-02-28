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

struct Can_Str CanAC(double LAI,int DOY,int hr,double solarR,double Temp,
		     double RH,double WindSpeed,double lat,int nlayers, double Vmax, double Alpha, 
		     double Kparm, double theta, double beta, double Rd, double Catm, double b0, 
		     double b1, double StomataWS, int ws, double kd, double chil, double heightf,
		     double leafN, double kpLN, double lnb0, double lnb1, int lnfun,double upperT,double lowerT);

struct lai_str laiLizasoFun(double thermalt, double phenostage, double phyllochron1,
			    double phyllochron2, double Ax, double LT, double k0, 
			    double a1, double a2, double L0, double LLx, double Lx, 
			    double LNl);

struct lai_str laiBirchdiscontinuousFun(double phenostage, double Lt, double Amax, 
					double c1, double c2, double c3, double c4);

struct lai_str laiBirchcontinuousFun(double phenostage, double a, double b, double Amax, 
				     double x0, double f, double g, double TT, double LT);
