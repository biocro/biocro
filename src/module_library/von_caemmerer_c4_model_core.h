#ifndef VON_CAEMMERER_C4_MODEL_CORE
#define VON_CAEMMERER_C4_MODEL_CORE

#include <algorithm> // std::min
#include <array>
#include <numeric>
#include "../framework/quadratic_root.h" //

struct vc_c4_result { double Ac, Aj, An; };
vc_c4_result vc_c4_module_core(
    double alpha_psii,
    double ao,
    double Cm,
    double f_cyc,
    double gamma_star,
    double gbs,
    double J,
    double Kc,
    double Ko,
    double Kp,
    double Om,
    double Rd,
    double Rm,
    double Vcmax,
    double Vpmax,
    double Vpr,
    double x_etr
    );

double check_Ac(
    double Ac,
    double alpha_psii,
    double ao,
    double Cm,
    double f_cyc,
    double gamma_star,
    double gbs,
    double J,
    double Kc,
    double Ko,
    double Kp,
    double Om,
    double Rd,
    double Rm,
    double Vcmax,
    double Vpmax,
    double Vpr,
    double x_etr
    );

double check_Aj(
    double Ac,
    double alpha_psii,
    double ao,
    double Cm,
    double f_cyc,
    double gamma_star,
    double gbs,
    double J,
    double Kc,
    double Ko,
    double Kp,
    double Om,
    double Rd,
    double Rm,
    double Vcmax,
    double Vpmax,
    double Vpr,
    double x_etr
    );

#endif
