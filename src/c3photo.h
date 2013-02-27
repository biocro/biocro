/*
 *  /src/c4photo.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
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

struct c3_str {

  double Assim;
  double Gs;
  double Ci;

};

/* Function needed for ballBerry */
extern double fnpsvp(double Tkelvin);
extern double ballBerry(double Amu, double Cappm, double Temp, double RelH, double beta0, double beta1);
extern struct c3_str c3photoC(double Qp, double Tleaf, double RH, double Vcmax0, double Jmax0, 
			      double Rd0, double bb0, double bb1, double Ca, double O2, double theta);





