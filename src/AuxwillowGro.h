#ifndef AUXWILLOWGRO_H
#define AUXWILLOWGRO_H
/*
 *  /src/AuxwillowGro.h by Dandan Wang and Deepak Jaiswal  Copyright (C) 2010-2015
 *
 */

#define MAXLAY    50 /* Maximum number of layers */

/* These are global variables. */
struct ET_Str c3EvapoTrans(double Rad, 
    	   double Itot, 
			   double Airtemperature, 
			   double RH,
			   double WindSpeed,
			   double LeafAreaIndex, 
			   double CanopyHeight, 
			   double vcmax2, 
			   double jmax2, 
			   double Rd2, 
			   double b02, 
			   double b12,
			   double Catm2,
			   double O2,
			   double theta2);


#endif

