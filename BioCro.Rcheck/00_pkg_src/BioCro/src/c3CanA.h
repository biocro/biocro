#ifndef C3CAN_H
#define C3CAN_H
/*
 *  BioCro/src/c3CanA.c by Fernando Ezequiel Miguez  Copyright (C) 2010
 *
 */


/* EvapoTrans function */
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
