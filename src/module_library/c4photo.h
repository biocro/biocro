#ifndef C4PHOTO_H
#define C4PHOTO_H

struct c4_str {
    double Assim;
    double Gs;
    double Ci;
    double GrossAssim;
};

struct c4_str c4photoC(
    double Qp,
    double Tl,
    double RH,
    double vmax,
    double alpha,
    double kparm,
    double theta,
    double beta,
    double Rd,
    double bb0,
    double bb1,
    double Gs_min,
    double StomaWS,
    double Ca,
    double atmospheric_pressure,
    int water_stress_approach,
    double upperT,
    double lowerT,
    double gbw);

#endif
