#include <cmath>                          // for pow, exp
#include <limits>                         // for std::numeric_limits
#include "../framework/constants.h"       // for dr_stomata, dr_boundary
#include "../framework/quadratic_root.h"  // for quadratic_root_min
#include "ball_berry_gs.h"                // for ball_berry_gs
#include "conductance_helpers.h"          // for sequential_conductance
#include "conductance_limited_assim.h"    // for conductance_limited_assim
#include "../math/roots/onedim/dekker.h"  // for dekker
#include "c4photo.h"

using physical_constants::dr_boundary;
using physical_constants::dr_stomata;

/*
  The secant method is used to solve for assimilation, Ci, and stomatal conductance,
  because of known convergence issues when using fixed-point iteration, based on
  Sun et al. (2012) "A numerical issue in calculating the coupled carbon and
  water fluxes in a climate model." *Journal of Geophysical Research*
  https://dx.doi.org/10.1029/2012JD018059

*/
photosynthesis_outputs c4photoC(
    double const Qp,                    // micromol / m^2 / s
    double const leaf_temperature,      // degrees C
    double const ambient_temperature,   // degrees C
    double const relative_humidity,     // dimensionless from Pa / Pa
    double const Vcmax_at_25,           // micromol / m^2 / s
    double const alpha,                 // mol / mol
    double const kparm,                 // mol / m^2 / s
    double const theta,                 // dimensionless
    double const beta,                  // dimensionless
    double const RL_at_25,              // micromol / m^2 / s
    double const bb0,                   // mol / m^2 / s
    double const bb1,                   // dimensionless from [mol / m^2 / s] / [mol / m^2 / s]
    double const Gs_min,                // mol / m^2 / s
    double const StomaWS,               // dimensionless
    double const Ca,                    // micromol / mol
    double const atmospheric_pressure,  // Pa
    double const upperT,                // degrees C
    double const lowerT,                // degrees C
    double const gbw                    // mol / m^2 / s
)
{
    // Define infinity
    double const inf = std::numeric_limits<double>::infinity();

    // Check inputs
    if (Qp < 0) {
        throw std::out_of_range("Input `absorbed_ppfd` cannot be negative. Check `solar` is not negative.");
    }

    constexpr double k_Q10 = 2;  // dimensionless. Increase in a reaction rate per temperature increase of 10 degrees Celsius.

    double const Ca_pa = Ca * 1e-6 * atmospheric_pressure;  // Pa

    double const kT = kparm * pow(k_Q10, (leaf_temperature - 25.0) / 10.0);  // mol / m^2 / s

    // Collatz 1992. Appendix B. Equation set 5B.
    double const Vtn = Vcmax_at_25 * pow(2, (leaf_temperature - 25.0) / 10.0);                                       // micromol / m^2 / s
    double const Vtd = (1 + exp(0.3 * (lowerT - leaf_temperature))) * (1 + exp(0.3 * (leaf_temperature - upperT)));  // dimensionless
    double const VT = Vtn / Vtd;                                                                                     // micromol / m^2 / s

    // Collatz 1992. Appendix B. Equation set 5B.
    double const Rtn = RL_at_25 * pow(2, (leaf_temperature - 25) / 10);  // micromol / m^2 / s
    double const Rtd = 1 + exp(1.3 * (leaf_temperature - 55));           // dimensionless
    double const RT = Rtn / Rtd;                                         // micromol / m^2 / s

    // Collatz 1992. Appendix B. Quadratic coefficients from Equation 2B.
    double const b0 = VT * alpha * Qp;
    double const b1 = -(VT + alpha * Qp);
    double const b2 = theta;

    // Calculate the smaller of the two quadratic roots, as mentioned following
    // Equation 3B in Collatz 1992.
    double const M = quadratic_root_min(b2, b1, b0);  // micromol / m^2 / s

    // Adjust Ball-Berry parameters in response to water stress
    double const bb0_adj = StomaWS * bb0 + Gs_min * (1.0 - StomaWS);
    double const bb1_adj = StomaWS * bb1;

    // Function to compute the biochemical assimilation rate according to the
    // Collatz model. Here, InterCellularCO2 should be expressed in Pa.
    auto collatz_assim = [=](double const InterCellularCO2) {
        // Collatz 1992. Appendix B. Quadratic coefficients from Equation 3B.
        double kT_IC_P = kT * InterCellularCO2 / atmospheric_pressure * 1e6;  // micromol / m^2 / s
        double a = beta;
        double b = -(M + kT_IC_P);
        double c = M * kT_IC_P;

        // Calculate the smaller of the two quadratic roots, as mentioned
        // following Equation 3B in Collatz 1992.
        double gross_assim = quadratic_root_min(a, b, c);  // micromol / m^2 / s
        return gross_assim - RT;                           // micromol / m^2 / s
    };

    // Initialize loop variables. These will be updated as a side effect during
    // the secant method's iterations.
    stomata_outputs BB_res;
    double Assim{0};  // micromol / mol (initial guess)
    double Gs{1e3};   // mol / m^2 / s (initial guess)

    // This lambda function equals zero only if Ci satisfies both the Collatz
    // and Ball-Berry models. Here, Ci_pa should be expressed in Pa.
    auto check_assim_rate = [=, &BB_res, &Assim, &Gs](double Ci_pa) {
        // Use Ci to compute the assimilation rate according to the Collatz
        // model.
        Assim = collatz_assim(Ci_pa);

        // Use Assim to compute the stomatal conductance according to the
        // Ball-Berry model. If Assim is too high, Cs will take a negative
        // value, which is not allowed by the Ball-Berry model. To avoid this,
        // we clamp Assim to the value that produces Cs = 0; this will result
        // in Gs = infinity.
        BB_res = ball_berry_gs(
            std::min(Assim, conductance_limited_assim(Ca, gbw, inf)) * 1e-6,
            Ca * 1e-6,
            relative_humidity,
            bb0_adj,
            bb1_adj,
            gbw,
            leaf_temperature,
            ambient_temperature);

        Gs = BB_res.gsw;  // mol / m^2 / s

        // Using Ci and Gs, make a new estimate of the assimilation rate. If
        // the initial value of Ci was correct, this should be identical to
        // Assim.
        double Gt = sequential_conductance(gbw / dr_boundary, Gs / dr_stomata);  // mol / m^2 / s

        return Gt * (Ca_pa - Ci_pa) / atmospheric_pressure * 1e6 - Assim;  // micromol / m^2 / s
    };

    // Max possible Ci value

    double const Ci_max =
        Ca_pa + 1e-6 * atmospheric_pressure * RT *
                    (dr_boundary / gbw + dr_stomata / bb0_adj);  // Pa
    // Run the dekker method
    using namespace root_finding;
    dekker solver(500, 1e-12, 1e-12);
    result_t result = solver.solve(

        check_assim_rate,
        0.5 * Ca_pa,
        0,
        Ci_max * 1.01);

    // throw exception if not converged
    if (!is_successful(result.flag)) {
        throw std::runtime_error(
            "Ci solver reports failed convergence with termination flag:\n    " +
            flag_message(result.flag));
    }

    // Get final values
    double const Ci = result.root / atmospheric_pressure * 1e6;            // micromol / mol
    double const an_conductance = conductance_limited_assim(Ca, gbw, Gs);  // micromol / m^2 / s

    return photosynthesis_outputs{
        /* .Assim = */ Assim,                       // micromol / m^2 /s
        /* .Assim_check = */ result.residual,       // micromol / m^2 / s
        /* .Assim_conductance = */ an_conductance,  // micromol / m^2 / s
        /* .Ci = */ Ci,                             // micromol / mol
        /* .Cs = */ BB_res.cs,                      // micromol / m^2 / s
        /* .GrossAssim = */ Assim + RT,             // micromol / m^2 / s
        /* .Gs = */ Gs,                             // mol / m^2 / s
        /* .RHs = */ BB_res.hs,                     // dimensionless from Pa / Pa
        /* .RL = */ RT,                             // micromol / m^2 / s
        /* .Rp = */ 0,                              // micromol / m^2 / s
        /* .iterations = */ result.iteration        // not a physical quantity
    };
}
