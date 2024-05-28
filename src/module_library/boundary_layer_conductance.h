#ifndef BOUNDARY_LAYER_CONDUCTANCE_H
#define BOUNDARY_LAYER_CONDUCTANCE_H

double leaf_boundary_layer_conductance_nikolov(
    double windspeed,             // m / s
    double leafwidth,             // m
    double air_temperature,       // degrees C
    double delta_t,               // degrees C
    double stomcond,              // m / s
    double water_vapor_pressure,  // Pa
    double minimum_gbw            // m / s
);

double canopy_boundary_layer_conductance_thornley(
    double CanopyHeight,    // m
    double WindSpeed,       // m / s
    double minimum_gbw,     // m / s
    double WindSpeedHeight  // m
);

#endif
