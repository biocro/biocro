/*
 *  BioCro/src/Century.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
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
