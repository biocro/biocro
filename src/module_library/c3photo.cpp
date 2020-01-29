/*
 *  /src/c3photo.c by Fernando Ezequiel Miguez  Copyright (C) 2009
 *
 */

#include <cmath>
#include "c3photo.hpp"
#include "ball_berry.hpp"
#include "biocro_units.h"
#include <iostream>	// temporary for debugging
#include <fstream>	// temporary for debugging

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

struct c3_str c3photoC(double _Qp, double _Tleaf, double RH, double _Vcmax0, double _Jmax, 
               double _Rd0, double bb0, double bb1, double Ca, double _O2,
               double thet, double StomWS, int water_stress_approach, double electrons_per_carboxylation, double electrons_per_oxygenation)
{
    // Assign units to the input parameters. The parameters can be renamed and this section can be removed when call functions
    // that call c3photoC() are also using units.
    const quantity<flux> Rd0 = _Rd0 * 1e-6 * mole / square_meter / second;
    const quantity<flux> Vcmax0 = _Vcmax0 * 1e-6 * mole / square_meter / second;
    const quantity<mole_fraction> atmospheric_oxygen_mole_fraction = _O2 * 1e-3 * mole / mole;
    const quantity<flux> Jmax = _Jmax * 1e-6 * mole / square_meter / second;
    const quantity<temperature> leaf_temperature = (_Tleaf + 273.15) * kelvin;
    const quantity<flux> Qp = _Qp * 1e-6 * mole / square_meter / second;

    const quantity<pressure> atmospheric_pressure = 101325 * pascal;
    const quantity<dimensionless> leaf_reflectance = 0.2;
    const quantity<flux> maximum_tpu_rate = 23 * 1e-6 * mole / square_meter / second;

    /* From Bernacchi 2001. Improved temperature response functions. */
    /* Note: Values in Dubois and Bernacchi are incorrect. */    
    const quantity<mole_fraction> Kc = 1e-6 * arrhenius_exponent(38.05, 79.43e3 * joule / mole, leaf_temperature);
    const quantity<mole_fraction> Ko = 1e-3 * arrhenius_exponent(20.30, 36.38e3 * joule / mole, leaf_temperature);
    const quantity<mole_fraction> Gstar = 1e-6 * arrhenius_exponent(19.02, 37.83e3 * joule / mole, leaf_temperature);
    const quantity<flux> Vcmax = Vcmax0 * arrhenius_exponent(26.35, 65.33e3 * joule / mole, leaf_temperature);
    const quantity<flux> Rd = Rd0 * arrhenius_exponent(18.72, 46.39e3 * joule / mole, leaf_temperature);

    const double leaf_temperature_celsius = leaf_temperature.value() - 273.15;
    const quantity<dimensionless> theta = thet + 0.018 * leaf_temperature_celsius - 3.7e-4 * pow(leaf_temperature_celsius, 2);

    /* Light limited */
    const quantity<dimensionless> dark_adapted_phi_PSII = 0.352 + 0.022 * leaf_temperature_celsius - 3.4 * pow(leaf_temperature_celsius, 2) / 10000;  // Bernacchi et al. 2003 Plant, Cell and Environment 26, 14191430 doi: 10.1046/j.0016- 8025.2003.01050.x
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
    int iterCounter = 0;
    int max_iter = 1000;
    while (iterCounter < max_iter) {
        quantity<flux> OldAssim = co2_assimilation_rate;

        /* Rubisco limited carboxylation */
        Ci = Ci_pa / atmospheric_pressure;
        quantity<flux> Ac1 =  Vcmax * (Ci - Gstar);
        double Ac2 = Ci + Kc * (1 + Oi / Ko);
        quantity<flux> Ac = Ac1 / Ac2;

        /* Light lmited portion */ 
        quantity<flux> Aj1 = J * (Ci - Gstar);
        double Aj2 = electrons_per_carboxylation * Ci + 2.0 * electrons_per_oxygenation * Gstar;
        quantity<flux> Aj = Aj1 / Aj2;
        if (Aj < 0.0 * mole / square_meter / second)
            Aj = 0.0 * mole / square_meter / second;

        /* Limited by tri phos utilization */
        quantity<flux> Ap = 3.0 * maximum_tpu_rate / (1 - Gstar / Ci);

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
            Gs *= StomWS; 

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
    if(iterCounter == max_iter) {
		/*
    	// The rate failed to converge! Rerun in debug mode, then throw an error
		c3_str result;
		result = c3photoCdb(_Qp, _Tleaf, RH, _Vcmax0, _Jmax, 
               _Rd0, bb0, bb1, Ca, _O2,
               thet, StomWS, water_stress_approach, electrons_per_carboxylation, electrons_per_oxygenation);
		throw std::range_error("Thrown in c3photo: co2_assimilation_rate did not converge");
		*/
		
		//std::cout << "co2_assimilation_rate did not converge\n";
    }
    struct c3_str result;
    result.Assim = co2_assimilation_rate.value() * 1e6;  // micromole / m^2 / s.
    result.Gs = Gs.value() * 1e3;  // mmol / m^2 / s.
    result.Ci = Ci.value() * 1e6;  // micromole / m^2 / s.
    result.GrossAssim = (co2_assimilation_rate.value() + Rd.value()) * 1e6;  // micromole / m^2 / s.
    return result;
}

struct c3_str c3photoCdb(double _Qp, double _Tleaf, double RH, double _Vcmax0, double _Jmax, 
               double _Rd0, double bb0, double bb1, double Ca, double _O2,
               double thet, double StomWS, int water_stress_approach, double electrons_per_carboxylation, double electrons_per_oxygenation)
{
    // Get ready for outputs
	std::ofstream myfile;
	myfile.open ("c3photo_results.txt", std::ios::out | std::ios::trunc);
	myfile << "The inputs were:\n";
	myfile << _Qp << "\n";
	myfile << _Tleaf << "\n";
	myfile << RH << "\n";
	myfile << _Vcmax0 << "\n";
	myfile << _Jmax << "\n"; 
	myfile << _Rd0 << "\n";
	myfile << bb0 << "\n";
	myfile << bb1 << "\n";
	myfile << Ca << "\n";
	myfile << _O2 << "\n";
    myfile << thet << "\n";
	myfile << StomWS << "\n";
	myfile << water_stress_approach << "\n";
	myfile << electrons_per_carboxylation << "\n";
	myfile << electrons_per_oxygenation << "\n";
	
	// Assign units to the input parameters. The parameters can be renamed and this section can be removed when call functions
    // that call c3photoC() are also using units.
    const quantity<flux> Rd0 = _Rd0 * 1e-6 * mole / square_meter / second;
    const quantity<flux> Vcmax0 = _Vcmax0 * 1e-6 * mole / square_meter / second;
    const quantity<mole_fraction> atmospheric_oxygen_mole_fraction = _O2 * 1e-3 * mole / mole;
    const quantity<flux> Jmax = _Jmax * 1e-6 * mole / square_meter / second;
    const quantity<temperature> leaf_temperature = (_Tleaf + 273.15) * kelvin;
    const quantity<flux> Qp = _Qp * 1e-6 * mole / square_meter / second;

    const quantity<pressure> atmospheric_pressure = 101325 * pascal;
    const quantity<dimensionless> leaf_reflectance = 0.2;
    const quantity<flux> maximum_tpu_rate = 23 * 1e-6 * mole / square_meter / second;

    /* From Bernacchi 2001. Improved temperature response functions. */
    /* Note: Values in Dubois and Bernacchi are incorrect. */    
    const quantity<mole_fraction> Kc = 1e-6 * arrhenius_exponent(38.05, 79.43e3 * joule / mole, leaf_temperature);
    const quantity<mole_fraction> Ko = 1e-3 * arrhenius_exponent(20.30, 36.38e3 * joule / mole, leaf_temperature);
    const quantity<mole_fraction> Gstar = 1e-6 * arrhenius_exponent(19.02, 37.83e3 * joule / mole, leaf_temperature);
    const quantity<flux> Vcmax = Vcmax0 * arrhenius_exponent(26.35, 65.33e3 * joule / mole, leaf_temperature);
    const quantity<flux> Rd = Rd0 * arrhenius_exponent(18.72, 46.39e3 * joule / mole, leaf_temperature);

    const double leaf_temperature_celsius = leaf_temperature.value() - 273.15;
    const quantity<dimensionless> theta = thet + 0.018 * leaf_temperature_celsius - 3.7e-4 * pow(leaf_temperature_celsius, 2);

    /* Light limited */
    const quantity<dimensionless> dark_adapted_phi_PSII = 0.352 + 0.022 * leaf_temperature_celsius - 3.4 * pow(leaf_temperature_celsius, 2) / 10000;  // Bernacchi et al. 2003 Plant, Cell and Environment 26, 14191430 doi: 10.1046/j.0016- 8025.2003.01050.x
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
    int iterCounter = 0;
    int max_iter = 1000;
	myfile << "\nIteration,co2_assimilation_rate\n";
    while (iterCounter < max_iter) {
        quantity<flux> OldAssim = co2_assimilation_rate;
        myfile << iterCounter << "," << OldAssim.value() << "\n";

        /* Rubisco limited carboxylation */
        Ci = Ci_pa / atmospheric_pressure;
        quantity<flux> Ac1 =  Vcmax * (Ci - Gstar);
        double Ac2 = Ci + Kc * (1 + Oi / Ko);
        quantity<flux> Ac = Ac1 / Ac2;

        /* Light lmited portion */ 
        quantity<flux> Aj1 = J * (Ci - Gstar);
        double Aj2 = electrons_per_carboxylation * Ci + 2.0 * electrons_per_oxygenation * Gstar;
        quantity<flux> Aj = Aj1 / Aj2;
        if (Aj < 0.0 * mole / square_meter / second)
            Aj = 0.0 * mole / square_meter / second;

        /* Limited by tri phos utilization */
        quantity<flux> Ap = 3.0 * maximum_tpu_rate / (1 - Gstar / Ci);

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
            Gs *= StomWS; 

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
    result.Ci = Ci.value() * 1e6;  // micromole / m^2 / s.
    result.GrossAssim = (co2_assimilation_rate.value() + Rd.value()) * 1e6;  // micromole / m^2 / s.
    
	myfile << "The final returned value was " << co2_assimilation_rate.value() * 1e6 << "\n";
	myfile.close();
	
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

