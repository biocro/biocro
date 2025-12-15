#include <cmath>
#include "collatz_photo.h"

struct collatz_result collatz_photo(
    double Qp,                // micromol / m^2 / s. Incident photon flux density.
    double leaf_temperature,  // degrees C
    double Vcmax_at_25,       // micromol / m^2 / s
    double alpha,             // mol / mol. CO2 fixed per incident photon flux density.
    double kparm,             // mol / m%2 / s
    double theta,
    double beta,
    double RL,
    double upperT,
    double lowerT,
    double k_Q10,                            // dimensionless
    double intercellular_co2_molar_fraction  // micromol / mol
)
{
    double kT = kparm * pow(k_Q10, (leaf_temperature - 25.0) / 10.0);  // dimensionless

    // Collatz 1992. Appendix B. Equation set 5B.
    double Vtn = Vcmax_at_25 * pow(2, (leaf_temperature - 25.0) / 10.0);                                       // micromol / m^2 / s
    double Vtd = (1 + exp(0.3 * (lowerT - leaf_temperature))) * (1 + exp(0.3 * (leaf_temperature - upperT)));  // dimensionless
    double VT = Vtn / Vtd;                                                                                     // micromol / m^2 / s

    // Collatz 1992. Appendix B. Equation set 5B.
    double Rtn = RL * pow(2, (leaf_temperature - 25) / 10);  // micromol / m^2 / s
    double Rtd = 1 + exp(1.3 * (leaf_temperature - 55));     // dimensionless
    double RT = Rtn / Rtd;                                   // micromol / m^2 / s

    // Collatz 1992. Appendix B. Equation 2B.
    double b0 = VT * alpha * Qp;
    double b1 = VT + alpha * Qp;
    double b2 = theta;

    /* Calculate the two roots */
    double const c1 = sqrt(b1 * b1 - 4 * b0 * b2);
    double const c2 = 2 * b2;
    double M1 = (b1 + c1) / c2;
    double M2 = (b1 - c1) / c2;

    double M = M1 < M2 ? M1 : M2;  // Use the smallest root.

    // Collatz 1992. Appendix B. Equation 3B.
    double kT_IC_P = kT * intercellular_co2_molar_fraction;  // micromol / m^2 / s
    double a = M * kT_IC_P;
    double b = M + kT_IC_P;
    double c = beta;

    double gross_assim = (b - sqrt(b * b - 4 * a * c)) / 2 / c;  // micromol / m^2 / s

    double Assim = gross_assim - RT;  // micromol / m^2 / s.

    struct collatz_result result {
        /* .assimilation = */ Assim,                // micromol / m^2 / s. Leaf area basis.
            /* .gross_assimilation = */ Assim + RT  // micromol / m^2 / s. Leaf area basis.
    };

    return result;
}
