#ifndef C3EVAPOTRANS_H
#define C3EVAPOTRANS_H
/*
 *  /src/c3EvapoTrans.c by Fernando Ezequiel Miguez  Copyright (C) 2010
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


#endif

