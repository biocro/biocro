#ifndef C3PHOTO_H
#define C3PHOTO_H
/*
 *  /src/c4photo.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 */

struct c3_str {

  double Assim;
  double Gs;
  double Ci;
  double GrossAssim;

};

/* Function needed for ballBerry */
extern double fnpsvp(double Tkelvin);
extern double ballBerry(double Amu, double Cappm, double Temp, double RelH, double beta0, double beta1);
extern struct c3_str c3photoC(double Qp, double Tleaf, double RH, double Vcmax0, double Jmax0, 
			      double Rd0, double bb0, double bb1, double Ca, double O2, double theta, double StomWS,int ws);



#endif

