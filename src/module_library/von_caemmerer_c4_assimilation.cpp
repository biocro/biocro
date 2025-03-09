
#include "von_caemmerer_c4_assimilation.h"

double ElectronTransportModel::operator()(double light_photon_flux_density){
    double x = light_nondimensionalize(light_photon_flux_density);
    if (std::isinf(x)){
        return Jmax;
    }
    return quadratic_root_minus(theta, -(x + 1), x) * Jmax;
}

double ElectronTransportModel::atp_production_electron_transport_ratio()
{
    // Equation (31) in VC (2021)
    return  (3 - f_cyc) / (1 - f_cyc)/photons_required_per_atp;
}

double ElectronTransportModel::light_nondimensionalize(double light_photon_flux_density)
{
    // Equation (35) in VC (2021)
    double rho = (1 - f_cyc) / (2 - f_cyc);
    double beta =  leaf_absorptance * (1 - f_spectral) * rho;
    return beta/Jmax * light_photon_flux_density;
}

double VC_C4_Model::enzyme_limited_assimilation(
    double co2_mesophyll,
    double oxygen_mesophyll)
{

    // non-dimensionalization
    double gamma = param.co2_compensation_point_per_o2 *K.o() /K.c();
    double scale =  param.bundle_sheath_conductance * K.c();
    double Vpmax_scaled = V.pep_carboxylation()/scale;
    double Vpr_scaled = V.pep_regeneration()/scale;
    double Rd = param.respiration_mitochondrial_light/V.rubisco_carboxylation();
    double Rm = param.respiration_mesophyll_fraction *  param.respiration_mitochondrial_light /scale;

    double co2_scaled = co2_mesophyll/K.c();
    double o2_scaled =  oxygen_mesophyll/K.o();

    // Equation (17) in VC (2021)
    double co2_limited_pep_carboxylation_rate = michaelis_menten<double>(
        co2_scaled,  Vpmax_scaled, K.p()/K.c());

    // Equation (18) and (19) in VC (2021)
    double pep_carboxylation_rate = std::min(co2_limited_pep_carboxylation_rate, Vpr_scaled);

    // non-dimesionalized Equation (12): x = x0 + x1 * A_scaled;
    double x0 = co2_scaled + pep_carboxylation_rate - Rm;
    double x1 = - V.rubisco_carboxylation()/ scale;

    // non-dimesionalized Equation (16): y = y0 + y1 * A_scaled;
    double y0 = o2_scaled;
    double y1 = param.bundle_sheath_photosystem_ii_fraction * V.rubisco_carboxylation()/(
        param.co2_o2_conductance_ratio * param. bundle_sheath_conductance * K.o());

    // In equation (10):  Ac =  V - Rd;
    // V = P/Q implies f(Ac) = Q(Ac) * (Ac + Rd) - P(Ac) = 0
    // Q = x + y + 1, P = x - g*y
    double P0 = x0 - gamma*y0; //P(0)
    double P1 = x1 - gamma*y1; //P'(0)

    double Q0 = x0 + y0 + 1; // Q(0)
    double Q1 = x1 + y1; //Q'(0)
    // f(Ac) = ec + eb*Ac + ea * Ac * Ac
    double ea = Q1;
    double eb = Rd * Q1 - P1 + Q0;
    double ec = Rd * Q0 - P0;

    // solve for lower root of f(Ac) = 0
    return V.rubisco_carboxylation()* quadratic_root_minus(1, eb/ea, ec/ea); // (micromol / m^2 / s)^2
}

double VC_C4_Model::light_limited_assimilation(
    double electron_transport_rate,
    double co2_mesophyll,
    double oxygen_mesophyll)
{

    double Rd = param.respiration_mitochondrial_light;
    double Rm = param.respiration_mesophyll_fraction * Rd;
    double x_etr = electron_transport.mesophyll_electron_transport_fraction;
    double z = electron_transport.atp_production_electron_transport_ratio();
    double J = electron_transport_rate;

    double Ajm = z * x_etr * J / 2 - Rm + param.bundle_sheath_conductance * co2_mesophyll;  // micromol / m^2 / s (Equation 4.45)
    double Ajbs = z * (1 - x_etr) * J / 3 - Rd;                  // micromol / m^2 / s (Equation 4.45)

    double f1 = param.bundle_sheath_photosystem_ii_fraction / param.co2_o2_conductance_ratio;           // dimensionless
    double f5 = 7 * param.co2_compensation_point_per_o2 / 3;              // dimensionless
    double f6 = z * (1 - x_etr) * J / 3 + 7 * Rd/  3;  // micromol / m^2 / s
    // Equation 4.42 (here we use `la` rather than `a`, where `l` stands for
    // `light`)
    double la = 1.0 - f1 * f5;  // dimensionless

    // Equation 4.43 (here we use `lb` rather than `b` as in Equation 4.43)
    double lb = -(Ajm + Ajbs + param.bundle_sheath_conductance * oxygen_mesophyll * f5 + param.co2_compensation_point_per_o2 * f1 * f6);  // micromol / m^2 / s

    // Equation 4.45 (here we use `lc` rather than `c` as in Equation 4.44)
    double lc = Ajm * Ajbs - param.co2_compensation_point_per_o2 * param.bundle_sheath_conductance * oxygen_mesophyll * f6;  // (micromol / m^2 / s)^2

    // Equation 4.41 for the light-limited assimilation rate
    return quadratic_root_minus(1, lb/la, lc/la);;  // micromol / m^2 / s
}


/**
 *  @brief Computes the net CO2 assimilation rate (and other values) using the
 *         von-Caemmerer model for C4 photosynthesis.
 *
 *  The model computes the minimum of a light-limited assimilation rate \f$A_j\f$ and an
 *  an enzyme-limited assimilation rate \f$A_c\f$ given the photon flux density of photosynthetically
 *  active radiation, and the partial pressure of CO2 and O2 in the mesophyll.
 *
 *  \f[
 *      A = \min \left(A_c, A_j \right) \qquad \text{(45)}
 *  \f]
 *
 *
 *  @param [in] light_photon_flux_density Photosynthetic photon flux density in micromol / m^2 / s.
 *
 *  @param [in] co2_mesophyll The partial pressure of CO2 in the mesophyll (microbar, or units of Kc).
 *
 *  @param [in] oxygen_mesophyll The partial pressure of O2 in the mesophyll (microbar, or units of Ko).
 *
 *
 *  @return A struct of type VC_C4_output holding the computed assimilation, enzyme-limited,
 *       light-limited, and electron transport rates.
 */
VC_C4_output VC_C4_Model::get_assimilation(
    double light_photon_flux_density,
    double co2_mesophyll,
    double oxygen_mesophyll)
{
    VC_C4_output out;

    out.electron_transport_rate = electron_transport(light_photon_flux_density);
    out.light_limited_assimilation = light_limited_assimilation(out.electron_transport_rate, co2_mesophyll, oxygen_mesophyll);
    out.enzyme_limited_assimilation = enzyme_limited_assimilation(co2_mesophyll, oxygen_mesophyll);

    // Equation (45) in VC (2021)
    out.assimilation = std::min(out.enzyme_limited_assimilation, out.light_limited_assimilation);  // micromol / m^2 / s

    return out;
}


double VC_C4_Model::operator()(
        double light_photon_flux_density,
        double co2_mesophyll,
        double oxygen_mesophyll){

    VC_C4_output out = VC_C4_Model::get_assimilation(
        light_photon_flux_density,
        co2_mesophyll,
        oxygen_mesophyll);
    return out.assimilation;
    }
