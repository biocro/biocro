#include <cmath>                          // for pow, exp
#include <limits>                         // for std::numeric_limits
#include "../framework/constants.h"       // for dr_stomata, dr_boundary
#include "../framework/quadratic_root.h"  // for quadratic_root_min
#include "../math/roots/onedim/dekker.h"  // for dekker
#include "ball_berry_gs.h"                // for ball_berry_gs
#include "conductance_helpers.h"          // for sequential_conductance
#include "conductance_limited_assim.h"    // for conductance_limited_assim
#include "leaf_energy_balance.h"          // for leaf_energy_balance
#include "c4photo.h"

using physical_constants::dr_boundary;
using physical_constants::dr_stomata;

double const inf = std::numeric_limits<double>::infinity();

/**
 *  @brief Computes the net CO2 assimilation rate according to the Collatz
 *  model.
 */
double collatz_assim(
    double const atmospheric_pressure,  // Pa
    double const beta,                  // dimensionless
    double const Ci_pa,                 // Pa
    double const kT,                    // mol / m^2 / s
    double const M,                     // micromol / m^2 / s
    double const RT                     // micromol / m^2 / s
)
{
    // Collatz 1992. Appendix B. Quadratic coefficients from Equation 3B.
    double const kT_IC_P = kT * Ci_pa / atmospheric_pressure * 1e6;  // micromol / m^2 / s
    double const a = beta;
    double const b = -(M + kT_IC_P);
    double const c = M * kT_IC_P;

    // Calculate the smaller of the two quadratic roots, as mentioned
    // following Equation 3B in Collatz 1992.
    double const gross_assim = quadratic_root_min(a, b, c);  // micromol / m^2 / s
    return gross_assim - RT;                                 // micromol / m^2 / s
};

/**
 *  @brief Calculates a difference in net CO2 assimilation rate; this function
 *  will return zero only if Cc satisfies the Collatz + Ball-Berry + 1D gas flow
 *  equations.
 */
double check_c4_assim_rate(
    double const ambient_temperature,   // degrees C
    double const atmospheric_pressure,  // Pa
    double const bb0_adj,               // mol / m^2 / s
    double const bb1_adj,               // dimensionless
    double const beta,                  // dimensionless
    double const Ca,                    // micromol / mol
    double const Ca_pa,                 // Pa
    double const Ci_pa,                 // Pa
    double const gbw,                   // mol / m^2 / s
    double const kT,                    // micromol / m^2 / s
    double const leaf_temperature,      // degrees C
    double const M,                     // micromol / m^2 / s
    double const relative_humidity,     // dimensionless
    double const RT                     // micromol / m^2 / s
)
{
    // Use Ci to compute the assimilation rate according to the Collatz
    // model.
    double const Assim = collatz_assim(
        atmospheric_pressure,
        beta,
        Ci_pa,
        kT,
        M,
        RT);  // micromol / m^2 / s

    // Use Assim to compute the stomatal conductance according to the
    // Ball-Berry model. If Assim is too high, Cs will take a negative
    // value, which is not allowed by the Ball-Berry model. To avoid this,
    // we clamp Assim to the value that produces Cs = 0; this will result
    // in Gs = infinity.
    stomata_outputs const BB_res = ball_berry_gs(
        std::min(Assim, conductance_limited_assim(Ca, gbw, inf)) * 1e-6,
        Ca * 1e-6,
        relative_humidity,
        bb0_adj,
        bb1_adj,
        gbw,
        leaf_temperature,
        ambient_temperature);

    double const Gs = BB_res.gsw;  // mol / m^2 / s

    // Using Ci and Gs, make a new estimate of the assimilation rate. If
    // the initial value of Ci was correct, this should be identical to
    // Assim.
    double const Gt =
        sequential_conductance({gbw / dr_boundary, Gs / dr_stomata});  // mol / m^2 / s

    return Gt * (Ca_pa - Ci_pa) / atmospheric_pressure * 1e6 - Assim;  // micromol / m^2 / s
};

/**
 *  @brief Solves for An, Ci, and gs
 *
 *  The Dekker method is used to solve the set of coupled equations, because of
 *  known convergence issues when using fixed-point iteration, based on Sun et
 *  al. (2012) "A numerical issue in calculating the coupled carbon and water
 *  fluxes in a climate model." *Journal of Geophysical Research*
 *  https://dx.doi.org/10.1029/2012JD018059
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

    // Use partial application to fix all inputs to `check_c3_assim_rate` except
    // Cc. To solve the photosynthesis equations, a root of this function must
    // be found.
    auto check_c4_assim_rate_partial = [=](double const Ci_pa) {
        return check_c4_assim_rate(
            ambient_temperature,   // degrees C
            atmospheric_pressure,  // Pa
            bb0_adj,               // mol / m^2 / s
            bb1_adj,               // dimensionless
            beta,                  // dimensionless
            Ca,                    // micromol / mol
            Ca_pa,                 // Pa
            Ci_pa,                 // Pa
            gbw,                   // mol / m^2 / s
            kT,                    // micromol / m^2 / s
            leaf_temperature,      // degrees C
            M,                     // micromol / m^2 / s
            relative_humidity,     // dimensionless
            RT                     // micromol / m^2 / s
        );
    };

    // Max possible Ci value
    double const Ci_max =
        Ca_pa + 1e-6 * atmospheric_pressure * RT *
                    (dr_boundary / gbw + dr_stomata / bb0_adj);  // Pa

    // Run the dekker method
    using namespace root_finding;
    dekker solver(500, 1e-12, 1e-12);
    result_t result = solver.solve(
        check_c4_assim_rate_partial,
        0.5 * Ca_pa,   // guess
        0,             // lower
        Ci_max * 1.01  // upper
    );

    // Throw exception if not converged
    if (!is_successful(result.flag)) {
        throw std::runtime_error(
            "Ci solver reports failed convergence:\n    " +
            result.message());
    }

    // Get final values
    double const Ci_pa = result.root;                      // Pa
    double const Ci = Ci_pa / atmospheric_pressure * 1e6;  // micromol / mol

    double const Assim = collatz_assim(
        atmospheric_pressure,
        beta,
        Ci_pa,
        kT,
        M,
        RT);  // micromol / m^2 / s

    stomata_outputs const BB_res = ball_berry_gs(
        std::min(Assim, conductance_limited_assim(Ca, gbw, inf)) * 1e-6,
        Ca * 1e-6,
        relative_humidity,
        bb0_adj,
        bb1_adj,
        gbw,
        leaf_temperature,
        ambient_temperature);

    double const Gs = BB_res.gsw;                                          // mol / m^2 / s
    double const an_conductance = conductance_limited_assim(Ca, gbw, Gs);  // micromol / m^2 / s

    return photosynthesis_outputs{
        /* .Assim = */ Assim,                       // micromol / m^2 /s
        /* .Assim_conductance = */ an_conductance,  // micromol / m^2 / s
        /* .Cc = */ 0,                              // micromol / mol
        /* .Ci = */ Ci,                             // micromol / mol
        /* .Cs = */ BB_res.cs,                      // micromol / m^2 / s
        /* .GrossAssim = */ Assim + RT,             // micromol / m^2 / s
        /* .Gs = */ Gs,                             // mol / m^2 / s
        /* .RHs = */ BB_res.hs,                     // dimensionless from Pa / Pa
        /* .RL = */ RT,                             // micromol / m^2 / s
        /* .Rp = */ 0,                              // micromol / m^2 / s
        /* .residual = */ result.residual,          // micromol / m^2 / s
        /* .iteration = */ result.iteration         // not a physical quantity
    };
}

/**
 *  @brief Calculates a difference in stomatal conductance; this function will
 *  return zero only if Gs satisfies the energy balance + Collatz + Ball-Berry +
 *  1D gas flow equations.
 */
double check_c4_gs(
    double const absorbed_longwave,     // J / (m^2 leaf) / s
    double const absorbed_shortwave,    // J / (m^2 leaf) / s
    double const alpha,                 // mol / mol
    double const ambient_temperature,   // degrees C
    double const atmospheric_pressure,  // Pa
    double const b0,                    // mol / m^2 / s
    double const b1,                    // dimensionless
    double const beta,                  // dimensionless
    double const Catm,                  // micromol / mol
    double const current_gs,            // mol / m^2 / s
    double const gbw_canopy,            // m / s
    double const Gs_min,                // mol / m^2 / s
    double const incident_ppfd,         // micromol / m^2 / s
    double const kparm,                 // mol / m^2 / s
    double const leafwidth,             // m
    double const lowerT,                // degrees C
    double const rh,                    // dimensionless
    double const RL_at_25,              // micromol / m^2 / s
    double const StomataWS,             // dimensionless
    double const theta,                 // dimensionless
    double const upperT,                // degrees C
    double const Vcmax_at_25,           // micromol / m^2 / s
    double const windspeed              // m / s
)
{
    // Solve energy balance with current gs
    energy_balance_outputs const et = leaf_energy_balance(
        absorbed_longwave,
        absorbed_shortwave,
        atmospheric_pressure,
        ambient_temperature,
        gbw_canopy,
        leafwidth,
        rh,
        current_gs,
        windspeed);

    // Get new leaf temperature
    double const current_Tleaf = ambient_temperature + et.Deltat;  // degrees C

    // Recalculate gs with current Tleaf
    photosynthesis_outputs const photo = c4photoC(
        incident_ppfd,
        current_Tleaf,
        ambient_temperature,
        rh,
        Vcmax_at_25,
        alpha,
        kparm,
        theta,
        beta,
        RL_at_25,
        b0,
        b1,
        Gs_min,
        StomataWS,
        Catm,
        atmospheric_pressure,
        upperT,
        lowerT,
        et.gbw_molar);

    return photo.Gs - current_gs;  // mol / m^2 / s
};

root_finding::result_t solve_c4_gs(
    double const absorbed_longwave,     // J / (m^2 leaf) / s
    double const absorbed_shortwave,    // J / (m^2 leaf) / s
    double const alpha,                 // mol / mol
    double const ambient_temperature,   // degrees C
    double const atmospheric_pressure,  // Pa
    double const b0,                    // mol / m^2 / s
    double const b1,                    // dimensionless
    double const beta,                  // dimensionless
    double const Catm,                  // micromol / mol
    double const gbw_canopy,            // m / s
    double const Gs_min,                // mol / m^2 / s
    double const incident_ppfd,         // micromol / m^2 / s
    double const kparm,                 // mol / m^2 / s
    double const leafwidth,             // m
    double const lowerT,                // degrees C
    double const rh,                    // dimensionless
    double const RL_at_25,              // micromol / m^2 / s
    double const StomataWS,             // dimensionless
    double const theta,                 // dimensionless
    double const upperT,                // degrees C
    double const Vcmax_at_25,           // micromol / m^2 / s
    double const windspeed              // m / s
)
{
    // Make an initial guess for boundary layer conductance
    double const gbw_guess{1.2};  // mol / m^2 / s

    // Get an initial estimate of stomatal conductance, assuming the leaf is at
    // air temperature
    const double initial_stomatal_conductance =
        c4photoC(
            incident_ppfd,
            ambient_temperature,
            ambient_temperature,
            rh,
            Vcmax_at_25,
            alpha,
            kparm,
            theta,
            beta,
            RL_at_25,
            b0,
            b1,
            Gs_min,
            StomataWS,
            Catm,
            atmospheric_pressure,
            upperT,
            lowerT,
            gbw_guess)
            .Gs;  // mol / m^2 / s

    // Use partial application to fix all inputs to `check_c3_gs` except
    // current_gs. To solve the photosynthesis equations, a root of this
    // function must be found.
    auto check_c4_gs_partial = [=](double const current_gs) {
        return check_c4_gs(
            absorbed_longwave,     // J / (m^2 leaf) / s
            absorbed_shortwave,    // J / (m^2 leaf) / s
            alpha,                 // mol / mol
            ambient_temperature,   // degrees C
            atmospheric_pressure,  // Pa
            b0,                    // mol / m^2 / s
            b1,                    // dimensionless
            beta,                  // dimensionless
            Catm,                  // micromol / mol
            current_gs,            // mol / m^2 / s
            gbw_canopy,            // m / s
            Gs_min,                // mol / m^2 / s
            incident_ppfd,         // micromol / m^2 / s
            kparm,                 // mol / m^2 / s
            leafwidth,             // m
            lowerT,                // degrees C
            rh,                    // dimensionless
            RL_at_25,              // micromol / m^2 / s
            StomataWS,             // dimensionless
            theta,                 // dimensionless
            upperT,                // degrees C
            Vcmax_at_25,           // micromol / m^2 / s
            windspeed              // m / s
        );
    };

    // Run the Dekker method
    using namespace root_finding;
    root_finding::dekker solver(50, 1e-3, 1e-3);
    result_t result = solver.solve(
        check_c4_gs_partial,
        initial_stomatal_conductance,               // guess
        Gs_min,                                     // lower
        100.0 * initial_stomatal_conductance + 0.1  // upper
    );

    // Throw exception if not converged
    if (!is_successful(result.flag)) {
        throw std::runtime_error(
            "c4_leaf_photosynthesis solver reports failed convergence:\n    " +
            result.message());
    }

    return result;
}
