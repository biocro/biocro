#ifndef C4PHOTO_H
#define C4PHOTO_H

#include "photosynthesis_outputs.h"  // for photosynthesis_outputs

photosynthesis_outputs c4photoC(
    double const Qp,
    double const leaf_temperature,
    double const ambient_temperature,
    double const relative_humidity,
    double const Vcmax_at_25,
    double const alpha,
    double const kparm,
    double const theta,
    double const beta,
    double const RL,
    double const bb0,
    double const bb1,
    double const Gs_min,
    double const StomaWS,
    double const Ca,
    double const atmospheric_pressure,
    double const upperT,
    double const lowerT,
    double const gbw);

#endif
