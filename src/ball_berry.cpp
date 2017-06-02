/*
 *  /src/ball_berry.cpp by Fernando Ezequiel Miguez  Copyright (C) 2007-2010
 *
 *  The ballBerry code is based on code provided by Joe Berry in an e-mail.
 *  The original function was written in Fortran. I translated it to C.
 * 
 */

#include <math.h>
#include "ball_berry.h"

/* Ball Berry stomatal conductance function */
double ballBerry(double assimilation, double atmospheric_co2_concentration, double atmospheric_relative_humidity, double beta0, double beta1)
{
    const double gbw = 1.2;  // micromole / m^2 / s.  Collatz et al. (1992) Aust. J. Plant Physiol. pg. 526.
    double gswmol;

    if (assimilation < 0.0) {
        /* Set stomatal conductance to the minimum value, beta0 */
        gswmol = beta0;
    } else {
        double Cs = atmospheric_co2_concentration - (1.4 / gbw) * assimilation;  // mol / mol.
        if (Cs < 0.0) {
            Cs = 1;
        }

        double acs = assimilation / Cs;
        if (acs < 1e-6) {
            acs = 1e-6;
        }

        /* Calculate leaf surface relative humidity, hs, from the quadratic equation:
         * a * hs^2 + b * hs + c = 0
         * 
         * These coefficients come from personal communication with Joe Berry to Steve Long.
         */
        double a = beta1 * acs;  // Equivalent to a = beta1 * assimilation / cs
        double b = beta0 + gbw - beta1 * acs;  // Equivalent to b = beta0 + gbw - beta1 * assimilation / cs
        double c = -atmospheric_relative_humidity * gbw - beta0;

        double root_term = b * b - 4 * a * c;
        double hs = (-b + sqrt(root_term)) / (2 * a);

        gswmol = beta1 * hs * acs + beta0; // Ball-Berry equation (Collatz 1991, equation 1).
    }

    if (gswmol <= 0) {
        gswmol = 1e-2;
    }

    return gswmol * 1000; // mmol / m^2 / s
}

/*! Calculate saturation vapor pressure of water from air temperature.
 *
 * This is the Arden Buck equation.  http://en.wikipedia.org/wiki/Arden_Buck_equation
 * temperature has units of kelvin.
 */
double water_saturation_vapor_pressure(double temperature) {

    double temperature_celsius = temperature - 273.15;
    double u = (18.678 - temperature_celsius / 234.5) * temperature_celsius;
    double v = 257.14 + temperature_celsius;

    return (6.1121 * exp(u/v) / 10);  // kilopascal.
}
