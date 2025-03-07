#ifndef BOUNDARY_LAYER_CONDUCTANCE_H
#define BOUNDARY_LAYER_CONDUCTANCE_H

double leaf_boundary_layer_conductance_nikolov(
    double air_temperature,  // degrees C
    double delta_t,          // degrees C
    double ea,               // Pa
    double gsv,              // m / s
    double lw,               // m
    double windspeed,        // m / s
    double p                 // Pa
);

double canopy_boundary_layer_conductance_thornley(
    double CanopyHeight,    // m
    double WindSpeed,       // m / s
    double minimum_gbw,     // m / s
    double WindSpeedHeight  // m
);

#endif
