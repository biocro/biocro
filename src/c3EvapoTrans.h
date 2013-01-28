/*
 *  /src/c3EvapoTrans.c by Fernando Ezequiel Miguez  Copyright (C) 2010
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
 *  Part of the code here (sunML, EvapoTrans, SoilEvapo, TempTo and
 *  the *prof functions) are based on code in WIMOVAC. WIMOVAC is
 *  copyright of Stephen Long and Stephen Humphries.
 *  Documentation for WIMOVAC can be found at
 *  http://www.life.illinois.edu/plantbio/wimovac/ (checked 02-13-2010)
 *
 */

/* Decalring functions used in this function */

/* c3EvapoTrans.c:82: warning: implicit declaration of function ‘TempToDdryA’ */
/* c3EvapoTrans.c:83: warning: implicit declaration of function ‘TempToLHV’ */
/* c3EvapoTrans.c:86: warning: implicit declaration of function ‘TempToSFS’ */
/* c3EvapoTrans.c:87: warning: implicit declaration of function ‘TempToSWVC’ */

double TempToDdryA(double Temp);
double TempToLHV(double Temp);
double TempToSFS(double Temp);
double TempToSWVC(double Temp);
