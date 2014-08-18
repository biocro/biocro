#ifndef CENTURY_H
#define CENTURY_H
/*
 *  BioCro/src/Century.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 */

struct cenT_str{

  double SCs[9];
  double SNs[9];
  double MinN;
  double Resp;

};

struct FL_str{

  double Fm;
  double Lc;

};

struct flow_str{

  double SC;
  double fC;
  double Resp;
  double MinN;

};


struct flow_str flow(double *SC, double CNratio, double A, double Lc, double Tm, double resp, int kno, double Ks[8]);

struct FL_str FmLcFun(double Lig, double Nit);


double AbiotEff(double smoist, double stemp);
#endif
