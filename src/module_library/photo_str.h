#ifndef PHOTO_STR_H
#define PHOTO_STR_H

// A simple structure for holding the output of photosynthesis calculations.
struct photo_str {
    double Assim;
    double Gs;
    double Ci;
    double GrossAssim;
    double Assim_conductance;
    int iterations;
};

#endif
