#ifndef C4PHOTO_H
#define C4PHOTO_H
/*
 *  /src/c4photo.h by Fernando Ezequiel Miguez  Copyright (C) 2007-2008
 *
 */

struct c4_str {
    double Assim;
    double Gs;
    double Ci;
    double GrossAssim;
};

/* Function needed for ballBerry */
extern double fnpsvp(double Tkelvin);
extern double ballBerry(double Amu, double Cappm, double Temp, double RelH, double beta0, double beta1);
extern struct c4_str c4photoC(double Qp, double Tl, double RH, double vmax, double alpha, 
        double kparm, double theta, double beta, double Rd, double bb0, double bb1, double StomaWS, double Ca, int water_stress_approach,double upperT,double lowerT);

/* Declaring the RSS_C4photo */
extern double RSS_C4photo(double oAssim[], double oQp[], double oTemp[], 
        double oRH[], double vmax, double alpha, double kparm,
        double theta, double beta,
        double Rd, double Catm, double b0, double b1, double StomWS, int water_stress_approach,double upperT,double lowerT, int nObs);

#endif

