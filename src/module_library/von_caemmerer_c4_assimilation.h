
#ifndef VON_CAEMMERER_C4_H
#define VON_CAEMMERER_C4_H

#include <cmath>
#include <algorithm>

struct Rate_type {
    double Vcmax;
    double Vpmax;
    double Vpr;
};

struct K_type {
    double c;
    double o;
    double p;
};

struct Light_Param_type {
    double Jmax;
    double absorptance;
    double f_spectral;
    double rho;
    double theta;
    double mesophyll_electron_transport_fraction;  // x_etr
};

struct VC_C4_param_type {
    double light_mitochondrial_respiration;  // RL
    double mesophyll_respiration_fraction;   // Rm_frac

    K_type K;
    Rate_type rate;
    Light_Param_type light;

    double alpha_psii;
    double ao;
    double gamma_star;
    double bundle_sheath_conductance;
};

struct VC_C4_output {
    double assimilation;
    double enzyme_limited_assimilation;
    double light_limited_assimilation;
    double electron_transport_rate;
};

VC_C4_output vc_c4_assim(
    double co2_mesophyll,
    double oxygen_mesophyll,
    double light_photon_flux_density,
    VC_C4_param_type param);

template <typename T>
T quadratic_root_lower(T a, T b, T c)
{
    return (-b - std::sqrt(b*b - 4 * a * c)) / (2 * a);
}

#endif
