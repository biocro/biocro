#ifndef COLLATZ_PHOTO_H
#define COLLATZ_PHOTO_H

struct collatz_result {
    double assimilation;
    double gross_assimilation;
};

struct collatz_result collatz_photo(double Qp,  // micromole / m^2 / s
                            double leaf_temperature,  // degrees C
                            double vmax,  // micromole / m^2 / s
                            double alpha,  // mol / mol
                            double kparm,  // mol / m%2 / s
                            double theta,
                            double beta,
                            double Rd,
                            double upperT,
                            double lowerT,
                            double k_Q10, // dimensionless
                            double intercellular_co2_molar_fraction  // micromole / mol
                            );

#endif
