/*
 *  BioCro/src/BioCro.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2008
 *
 */

struct Can_Str CanAC(double LAI,int DOY,int hr,double solarR,double Temp,
		     double RH,double WindSpeed,double lat,int nlayers, double Vmax, double Alpha, 
		     double Kparm, double beta, double Rd, double Catm, double b0, 
		     double b1, double theta, double kd, double chil, double heightf,
		     double leafN, double kpLN, double lnb0, double lnb1, int lnfun,double upperT,
		     double lowerT,struct nitroParms nitroP, double leafwidth, int eteq, double StomataWS, int ws);

struct lai_str laiLizasoFun(double thermalt, double phenostage, double phyllochron1,
			    double phyllochron2, double Ax, double LT, double k0, 
			    double a1, double a2, double L0, double LLx, double Lx, 
			    double LNl);

struct lai_str laiBirchdiscontinuousFun(double phenostage, double Lt, double Amax, 
					double c1, double c2, double c3, double c4);

struct lai_str laiBirchcontinuousFun(double phenostage, double a, double b, double Amax, 
				     double x0, double f, double g, double TT, double LT);
