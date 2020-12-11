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

struct c4_str c4photoC(double Qp, double Tl, double RH, double vmax, double alpha,
        double kparm, double theta, double beta, double Rd, double bb0, double bb1,
        double Gs_min, double StomaWS, double Ca, int water_stress_approach,double upperT,
        double lowerT);

#endif

