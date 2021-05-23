/*
 *  /src/c3photo.c by Fernando Ezequiel Miguez  Copyright (C) 2009
 *
 */

#include <cmath>
#include "c3photo.hpp"
#include "ball_berry.hpp"
#include "biocro_units.h"

using namespace biocro_units;

/*! Calculate the exponential term of the Arrhenius function.
 *
 * The Arrhenius equation is A * e^(c - E_a / R / temperature).
 * The result has the same units as A, which depends on the quantity of interest.
 * In order to make this function reusable, it only calculates the exponential term, which is always dimensionless.
 */
quantity<dimensionless> arrhenius_exponent(quantity<dimensionless> c, quantity<energy_over_amount> activation_energy, quantity<temperature> temperature)
{
    const quantity<energy_over_temperature_amount> R = 8.314472 * joule / kelvin / mole ;
    return exp(c - activation_energy / (R * temperature));
}

struct c3_str c3photoC(double _Qp, double _Tleaf, double RH, double _Vcmax0, double _Jmax0, double _TPU_rate_max,
               double _Rd0, double bb0, double bb1, double Gs_min, double Ca, double AP, double _O2,
               double thet, double StomWS, int water_stress_approach, double electrons_per_carboxylation, double electrons_per_oxygenation)
{
    // Assign units to the input parameters. The parameters can be renamed and this section can be removed when call functions
    // that call c3photoC() are also using units.
    const quantity<flux> Rd0 = _Rd0 * 1e-6 * mole / square_meter / second;
    const quantity<flux> Vcmax0 = _Vcmax0 * 1e-6 * mole / square_meter / second;
    const quantity<mole_fraction> atmospheric_oxygen_mole_fraction = _O2 * 1e-3 * mole / mole;
    const quantity<flux> Jmax0 = _Jmax0 * 1e-6 * mole / square_meter / second;
    const quantity<temperature> leaf_temperature = (_Tleaf + 273.15) * kelvin;
    const quantity<flux> Qp = _Qp * 1e-6 * mole / square_meter / second;
    const quantity<flux> Gsw_min = Gs_min * mole / square_meter / second;

    const quantity<pressure> atmospheric_pressure = AP * pascal;
    const quantity<dimensionless> leaf_reflectance = 0.2;
    const quantity<flux> maximum_tpu_rate = _TPU_rate_max * 1e-6 * mole / square_meter / second;


    /* From:
     Bernacchi et al. (2003) Plant, Cell and Environment, 26(9), 1419-1430. https://doi.org/10.1046/j.0016-8025.2003.01050.x
     Bernacchi et al. (2001) Plant, Cell and Environment, 24(2), 253-259. https://doi.org/10.1111/j.1365-3040.2001.00668.x */
    /* Note: Values in Dubois and Bernacchi are incorrect. */
    const quantity<mole_fraction> Kc = 1e-6 * arrhenius_exponent(38.05, 79.43e3 * joule / mole, leaf_temperature);
    const quantity<mole_fraction> Ko = 1e-3 * arrhenius_exponent(20.30, 36.38e3 * joule / mole, leaf_temperature);
    const quantity<mole_fraction> Gstar = 1e-6 * arrhenius_exponent(19.02, 37.83e3 * joule / mole, leaf_temperature);
    const quantity<flux> Vcmax = Vcmax0 * arrhenius_exponent(26.35, 65.33e3 * joule / mole, leaf_temperature);
    const quantity<flux> Jmax = Jmax0 * arrhenius_exponent(17.57, 43.54e3 * joule / mole,
        leaf_temperature);
    const quantity<flux> Rd = Rd0 * arrhenius_exponent(18.72, 46.39e3 * joule / mole, leaf_temperature);

    const double leaf_temperature_celsius = leaf_temperature.value() - 273.15;
    const quantity<dimensionless> theta = thet + 0.018 * leaf_temperature_celsius - 3.7e-4 * pow(leaf_temperature_celsius, 2);

    /* Light limited */
    const quantity<dimensionless> dark_adapted_phi_PSII = 0.352 + 0.022 * leaf_temperature_celsius - 3.4 * pow(leaf_temperature_celsius, 2) / 10000;  // Bernacchi et al. (2003).  See reference above.
    const quantity<flux> I2 = Qp * dark_adapted_phi_PSII * (1.0 - leaf_reflectance) / 2.0;

    const quantity<flux> J = (Jmax + I2 - root<2>(pow<2>(Jmax + I2) - 4.0 * theta * I2 * Jmax )) / (2.0 * theta);

    const quantity<mole_fraction> Oi = atmospheric_oxygen_mole_fraction * solo(leaf_temperature_celsius);

    if(Ca <= 0)
        Ca = 1e-4;

    const quantity<pressure> Ca_pa = Ca * 1e-6 * atmospheric_pressure;  // Pa.

    quantity<pressure> Ci_pa = 0.0 * pascal;
    quantity<flux> Vc;
    quantity<flux> Tol = 0.01 * 1e-6 * mole / square_meter / second;
    quantity<flux> Gs;
    quantity<flux> co2_assimilation_rate = 0 * mole / square_meter / second;
    quantity<mole_fraction> Ci;
    //TPU temperature limit. (Fig.7. DOI: 10.1007/s00425-015-2436-8)
    double TPU_c = 25.5,Ha = 62.99, S = 0.588, Hd = 182.14,TPU_rate_scaler25 = 306.7509,R=8.314472E-3; 
    double LeafTemperatureKelvin = leaf_temperature.value(); 
    double top = LeafTemperatureKelvin *exp(TPU_c-Ha/(R*LeafTemperatureKelvin));
    double bot = 1.0+ exp((S*LeafTemperatureKelvin - Hd)/(R*LeafTemperatureKelvin));
    double TPU_rate_scaler = top /bot / TPU_rate_scaler25; //normalize to 25 C
    //
    int iterCounter = 0;
    int max_iter = 1000;
    while (iterCounter < max_iter) {
        quantity<flux> OldAssim = co2_assimilation_rate;

        /* Rubisco limited carboxylation */
        Ci = Ci_pa / atmospheric_pressure;
        quantity<flux> Ac1 =  Vcmax * (Ci - Gstar);
        double Ac2 = Ci + Kc * (1 + Oi / Ko);
        quantity<flux> Ac = Ac1 / Ac2;

        /* Light limited portion */
        quantity<flux> Aj1 = J * (Ci - Gstar);
        double Aj2 = electrons_per_carboxylation * Ci + 2.0 * electrons_per_oxygenation * Gstar;
        quantity<flux> Aj = Aj1 / Aj2;
        if (Aj < 0.0 * mole / square_meter / second)
            Aj = 0.0 * mole / square_meter / second;

        /* Limited by tri phos utilization */
        quantity<flux> Ap = 3.0 * maximum_tpu_rate * TPU_rate_scaler;

        if(Ac < Aj && Ac < Ap){
            Vc = Ac;
        } else if (Aj < Ac && Aj < Ap){
            Vc = Aj;
        } else if (Ap < Ac && Ap < Aj){
            if (Ap < 0 * mole / square_meter / second)
                Ap = 0 * mole / square_meter / second;
            Vc = Ap;
        }

        co2_assimilation_rate = Vc - Rd;

        if (water_stress_approach == 0) co2_assimilation_rate *= quantity<dimensionless>(StomWS);
        /* milimole per meter square per second*/
        Gs = ball_berry(co2_assimilation_rate.value(), Ca * 1e-6, RH, bb0, bb1) * 1e-3 * mole / square_meter / second;

        if (water_stress_approach == 1)
            //Gs *= quantity<dimensionless>(StomWS);
            Gs = Gsw_min + StomWS * (Gs - Gsw_min);

        if (Gs <= 0 * mole / square_meter / second)
            Gs = 1e-5 * 1e-3 * mole / square_meter / second;

        if (Gs > 800 * 1e-3 * mole / square_meter / second)
            Gs = 800 * 1e-3 * mole / square_meter / second;

        Ci_pa = Ca_pa - co2_assimilation_rate * 1.6 * atmospheric_pressure / Gs;

        if (Ci_pa < 0 * pascal)
            Ci_pa = 1e-5 * pascal;

        if (abs(OldAssim - co2_assimilation_rate) < Tol) {
            break;
        }

        ++iterCounter;
    }

    struct c3_str result;
    result.Assim = co2_assimilation_rate.value() * 1e6;  // micromole / m^2 / s.
    result.Gs = Gs.value() * 1e3;  // mmol / m^2 / s.
    result.Ci = Ci.value() * 1e6;  // micromole / mol.
    result.GrossAssim = (co2_assimilation_rate.value() + Rd.value()) * 1e6;  // micromole / m^2 / s.
    return result;
}

double solc(double LeafT){

    double tmp;

    if(LeafT > 24 && LeafT < 26){
        tmp = 1;
    }else{
        tmp = (1.673998 - 0.0612936 * LeafT + 0.00116875 * pow(LeafT,2) - 8.874081e-06 * pow(LeafT,3)) / 0.735465;
    }

    return(tmp);
}

double solo(double LeafT){

    double tmp;

    if(LeafT > 24 && LeafT < 26){
        tmp = 1;
    }else{
        tmp = (0.047 - 0.0013087 * LeafT + 2.5603e-05 * pow(LeafT,2) - 2.1441e-07 * pow(LeafT,3)) / 0.026934;
    }

    return(tmp);
}

