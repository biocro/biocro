#ifndef PHOTOSYNTHESIS_OUTPUTS_H
#define PHOTOSYNTHESIS_OUTPUTS_H

// A simple structure for holding the output of photosynthesis calculations.
struct photosynthesis_outputs {
    double Assim;
    double Gs;
    double Ci;
    double Assim_conductance;
    int iterations;
};

#endif
