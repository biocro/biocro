
#include "von_caemmerer_c4_assimilation.h"


double electron_transport_normalized(
    double x, double theta  // dimensionless
)
{
    return quadratic_root_lower<double>(theta, -(x + 1), x);
}

double electron_transport(double Qin, Light_Param_type param){
    double x = Qin/param.Jmax * param.get_beta();
    return electron_transport_normalized(x, param.theta) * param.Jmax;
}


double Light_Param_type::atp_production_electron_transport_ratio()
{
    return 0.25 * (3 - f_cyc) / (1 - f_cyc);
}

double Light_Param_type::rho()
{
    return (1 - f_cyc) / (2 - f_cyc);
}

double Light_Param_type::get_beta()
{
    return absorptance * (1 - f_spectral) * rho();
}


double enzyme_limited_assimilation(
    double co2_mesophyll,
    double oxygen_mesophyll,
    VC_C4_param_type param)
{
    double RLm = param.mesophyll_respiration_fraction * param.light_mitochondrial_respiration;  // micromol / m^2 / s
    double gbs = param.bundle_sheath_conductance;

    // Equations 4.17 and 4.19
    double Vpc = co2_mesophyll * param.rate.Vpmax / (co2_mesophyll + param.K.p);  // micromol / m^2 / s
    double Vp = std::min(Vpc, param.rate.Vpr);                                    // micromol / m^2 / s

    double x0 = (gbs* co2_mesophyll + Vp  - RLm )/param.K.c/gbs;
    double x1 = - param.rate.Vcmax/gbs/param.K.c;

    double y0 = oxygen_mesophyll/param.K.o;
    double y1 = param.alpha_psii* param.rate.Vcmax/param.ao/gbs/param.K.o;

    // Equation (20)
    // p(A) = ec + eb*A + ea * A * A = 0
    double Rp = param.light_mitochondrial_respiration/param.rate.Vcmax;
    double gamma = param.gamma_star * param.K.o /param.K.c;
    double ea = x1 + y1;  // dimensionless
    double eb = (y0 + x0 + 1)  + Rp * (x1 + y1) - (x1 - gamma*y1) ;  // micromol / m^2 / s
    double ec = Rp *( x0 + y0 + 1 ) - (x0 - gamma*y0);  // (micromol / m^2 / s)^2

    // normalize to  A*A + p1 * A + p0 = 0
    double p1 = eb/ea;
    double p0 = ec/ea;
    return param.rate.Vcmax * quadratic_root_lower<double>(1, p1, p0);
}

double light_limited_assimilation(
    double electron_transport_rate,
    double co2_mesophyll,
    double oxygen_mesophyll,
    VC_C4_param_type param)
{
    double RL = param.light_mitochondrial_respiration;
    double RLm = param.mesophyll_respiration_fraction * RL;  // micromol / m^2 / s
    double gbs = param.bundle_sheath_conductance;

    double x_etr = param.light.mesophyll_electron_transport_fraction;
    double z = param.light.atp_production_electron_transport_ratio();
    double J = electron_transport_rate;

    double Ajm = z * x_etr * J / 2 - RLm + gbs * co2_mesophyll;  // micromol / m^2 / s (Equation 4.45)
    double Ajbs = z * (1 - x_etr) * J / 3 - RL;                  // micromol / m^2 / s (Equation 4.45)

    double f1 = param.alpha_psii / param.ao;           // dimensionless
    double f5 = 7 * param.gamma_star / 3;              // dimensionless
    double f6 = z * (1 - x_etr) * J / 3 + 7 * RL / 3;  // micromol / m^2 / s
    // Equation 4.42 (here we use `la` rather than `a`, where `l` stands for
    // `light`)
    double la = 1.0 - f1 * f5;  // dimensionless

    // Equation 4.43 (here we use `lb` rather than `b` as in Equation 4.43)
    double lb = -(Ajm + Ajbs + gbs * oxygen_mesophyll * f5 + param.gamma_star * f1 * f6);  // micromol / m^2 / s

    // Equation 4.45 (here we use `lc` rather than `c` as in Equation 4.44)
    double lc = Ajm * Ajbs - param.gamma_star * gbs * oxygen_mesophyll * f6;  // (micromol / m^2 / s)^2

    // Equation 4.41 for the light-limited assimilation rate
    return quadratic_root_lower<double>(la, lb, lc);  // micromol / m^2 / s
}

VC_C4_output vc_c4_assim(
    double light_photon_flux_density,
    double co2_mesophyll,
    double oxygen_mesophyll,
    VC_C4_param_type param)
{
    VC_C4_output out;

    // Equations 2.14 and 2.15
    double J = electron_transport( light_photon_flux_density, param.light);
    out.electron_transport_rate = J;
    out.light_limited_assimilation = light_limited_assimilation(J, co2_mesophyll, oxygen_mesophyll, param);
    // Equation 4.21 for the enzyme-limited assimilation rate
    out.enzyme_limited_assimilation = enzyme_limited_assimilation(co2_mesophyll, oxygen_mesophyll, param);

    // Equation 4.47 for the overall assimilation rate
    out.assimilation = std::min(out.enzyme_limited_assimilation, out.light_limited_assimilation);  // micromol / m^2 / s

    return out;
}

