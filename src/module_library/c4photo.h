#ifndef C4PHOTO_H
#define C4PHOTO_H

#include "photosynthesis_outputs.h"  // for photosynthesis_outputs

photosynthesis_outputs c4photoC(
    double const Qp,
    double const Tl,
    double const RH,
    double const vmax,
    double const alpha,
    double const kparm,
    double const theta,
    double const beta,
    double const Rd,
    double const bb0,
    double const bb1,
    double const Gs_min,
    double const StomaWS,
    double const Ca,
    double const atmospheric_pressure,
    int const water_stress_approach,
    double const upperT,
    double const lowerT,
    double const gbw);

#endif
