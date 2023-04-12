#ifndef C4PHOTO_H
#define C4PHOTO_H

#include "photosynthesis_outputs.h"  // for photosynthesis_outputs

photosynthesis_outputs c4photoC(
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
