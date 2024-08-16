
#include "von_caemmerer_c4_assimilation.h"

double get_beta(double absorptance, double f_spectral, double rho)
{
    return absorptance * (1 - f_spectral) * rho;
}

double j_from_jmax(
    double Jmax,  // micromol / m^2 / s
    double Qin,   // micromol / m^2 / s
    double beta,  // dimensionless
    double theta  // dimensionless
)
{
    // Equation 2.14
    double I2 = Qin * beta;  // # micromol / m^2 / s

    // Equation 2.15
    return quadratic_root_lower<double>(theta, -(I2 + Jmax), I2 * Jmax);  // # micromol / m^2 / s
}

VC_C4_output vc_c4_assim(
    double co2_mesophyll,
    double oxygen_mesophyll,
    double light_photon_flux_density,
    VC_C4_param_type param)
{
    VC_C4_output out;

    double RL = param.light_mitochondrial_respiration;
    double RLm = param.mesophyll_respiration_fraction * RL;  // micromol / m^2 / s

    // Equations 4.17 and 4.19
    double Vpc = co2_mesophyll * param.rate.Vpmax / (co2_mesophyll + param.K.p);  // micromol / m^2 / s
    double Vp = std::min(Vpc, param.rate.Vpr);                                    // micromol / m^2 / s

    // Equations 2.14 and 2.15
    double beta = get_beta(param.light.absorptance, param.light.f_spectral, param.light.rho);
    double J = j_from_jmax(
        param.light.Jmax,
        light_photon_flux_density,
        beta,
        param.light.theta);
    out.electron_transport_rate = J;

    // Calculate individual process-limited assimilation rates. These are not
    // explicitly given by any equations in the textbook, but do appear as terms
    // in some later calculations.
    double gbs = param.bundle_sheath_conductance;
    double x_etr = param.light.mesophyll_electron_transport_fraction;
    double Apr = param.rate.Vpr - RLm + gbs * co2_mesophyll;  // micromol / m^2 / s
    double Apc = Vpc - RLm + gbs * co2_mesophyll;             // micromol / m^2 / s
    double Ap = Vp - RLm + gbs * co2_mesophyll;               // micromol / m^2 / s (Equation 4.25)
    double Ar = param.rate.Vcmax - RL;                        // micromol / m^2 / s (Equation 4.25)
    double Ajm = x_etr * J / 2 - RLm + gbs * co2_mesophyll;   // micromol / m^2 / s (Equation 4.45)
    double Ajbs = (1 - x_etr) * J / 3 - RL;                   // micromol / m^2 / s (Equation 4.45)

    // Calculate terms that appear in several of the next equations
    double f1 = param.alpha_psii / param.ao;                             // dimensionless
    double f2 = gbs * param.K.c * (1.0 + oxygen_mesophyll / param.K.o);  // micromol / m^2 / s
    double f3 = param.gamma_star * param.rate.Vcmax;                     // micromol / m^2 / s
    double f4 = param.K.c / param.K.o;                                   // dimensionless
    double f5 = 7 * param.gamma_star / 3;                                // dimensionless
    double f6 = (1 - x_etr) * J / 3 + 7 * RL / 3;                        // micromol / m^2 / s

    // Equation 4.22 (here we use `ea` rather than `a`, where `e` stands for
    // `enzyme`)
    double ea = 1.0 - f1 * f4;  // dimensionless

    // Equation 4.23 (here we use `eb` rather than `b` as in Equation 4.22)
    double eb = -(Ap + Ar + f2 + f1 * (f3 + RL * f4));  // micromol / m^2 / s

    // Equation 4.24 (here we use `ec` rather than `c` as in Equation 4.22)
    double ec = Ar * Ap - (f3 * gbs * oxygen_mesophyll + RL * f2);  // (micromol / m^2 / s)^2

    // Equation 4.21 for the enzyme-limited assimilation rate
    out.enzyme_limited_assimilation = quadratic_root_lower<double>(ea, eb, ec);  // micromol / m^2 / s

    // Equation 4.42 (here we use `la` rather than `a`, where `l` stands for
    // `light`)
    double la = 1.0 - f1 * f5;  // dimensionless

    // Equation 4.43 (here we use `lb` rather than `b` as in Equation 4.43)
    double lb = -(Ajm + Ajbs + gbs * oxygen_mesophyll * f5 + param.gamma_star * f1 * f6);  // micromol / m^2 / s

    // Equation 4.45 (here we use `lc` rather than `c` as in Equation 4.44)
    double lc = Ajm * Ajbs - param.gamma_star * gbs * oxygen_mesophyll * f6;  // (micromol / m^2 / s)^2

    // Equation 4.41 for the light-limited assimilation rate
    out.light_limited_assimilation = quadratic_root_lower<double>(la, lb, lc);  // micromol / m^2 / s

    // Equation 4.47 for the overall assimilation rate
    out.assimilation = std::min(
        out.enzyme_limited_assimilation,
        out.light_limited_assimilation);  // micromol / m^2 / s

    return out;
}
