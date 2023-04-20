#ifndef BALL_BERRY_GS_H
#define BALL_BERRY_GS_H

#include "stomata_outputs.h"

stomata_outputs ball_berry_gs(
    double assimilation,
    double ambient_c,
    double ambient_rh,
    double bb_offset,
    double bb_slope,
    double gbw,
    double leaf_temperature,
    double ambient_air_temperature);

#endif
