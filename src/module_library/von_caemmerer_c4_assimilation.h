
#ifndef VON_CAEMMERER_C4_H
#define VON_CAEMMERER_C4_H

// standard library includes
#include <cmath>
#include <algorithm>
#include <array>
#include <limits>

// includes from biocro
#include "../framework/quadratic_root.h"
#include "../framework/constants.h"

using std::array;



/**
 * @brief A object for the Michaelis Menten affinity parameters.
 */
class K_type {

    array<double, 3> val;

    public:
        K_type(double kc, double ko, double kp) :
         val({kc, ko, kp}) {}

        double c(){return val[0];}
        double o(){return val[1];}
        double p(){return val[2];}

};

/**
 * @brief A object for the Michaelis Menten maximum rate parameters:
 * the maximum rubisco carboxylation rate (Vcmax), the maximum PEP carboxylation rate,
 * the maximum PEP regeneration rate.
 */
class Vmax_type {

    array<double, 3> ref_value;

    public:
        Vmax_type(double vcmax, double vpmax, double vpr) :
         ref_value({vcmax, vpmax, vpr}) {}
        double rubisco_carboxylation(){return ref_value[0];}
        double pep_carboxylation(){return ref_value[1];}
        double pep_regeneration(){return ref_value[2];}

};


/**
 * @brief A object that implements the electron transport rate model,
 * and associated parameters.
 */
class ElectronTransportModel {

public:
    double Jmax;
    double f_spectral;
    double f_cyc;
    double leaf_absorptance;
    double photons_required_per_atp; // h in VC (2021)
    double theta;
    double mesophyll_electron_transport_fraction;

    double atp_production_electron_transport_ratio();  // z in VC (2021)
    double light_nondimensionalize(double light_photon_flux_density);
    double operator()(double light_photon_flux_density);
};



/**
 * @brief A object that holds other parameters for the C4 photosynthesis model.
 */
struct VC_C4_Param_type {
    double bundle_sheath_photosystem_ii_fraction; // alpha_psii
    double bundle_sheath_conductance;     // g_bs
    double co2_o2_conductance_ratio;      // ao
    double co2_compensation_point_per_o2; // gamma_star
    double respiration_mitochondrial_light;  // RL
    double respiration_mesophyll_fraction;   // Rm_frac
};

/**
 * @brief A object that holds the computed assimilation rate; and
 */
struct VC_C4_output {
    double assimilation;
    double enzyme_limited_assimilation;
    double light_limited_assimilation;
    double electron_transport_rate;
};


/**
 * @brief A object that implements the electron transport rate model,
 * and associated parameters. See source file for implementation details.
 */
class  VC_C4_Model {

    public:
    ElectronTransportModel electron_transport;
    K_type K;
    VC_C4_Param_type param;
    Vmax_type V;



    double enzyme_limited_assimilation(double co2_mesophyll, double oxygen_mesophyll);
    double light_limited_assimilation(
        double electron_transport_rate,
        double co2_mesophyll,
        double oxygen_mesophyll);
    VC_C4_output get_assimilation(
        double light_photon_flux_density,
        double co2_mesophyll,
        double oxygen_mesophyll);

    double operator()(
        double light_photon_flux_density,
        double co2_mesophyll,
        double oxygen_mesophyll);
};


/**
 * @brief Michaelis-Menten Kinetics function.
 */
template <typename T>
T michaelis_menten(T x, T Vmax, T Km)
{
    T s = x/(x + Km);
    return Vmax * s;
}


#endif
