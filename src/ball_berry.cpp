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
double ballBerry(double Amu, double Cappm, double Temp, double RelH, double beta0, double beta1)
{

    const double gbw = 1.2;  // micromole / square_meter / second.  Collatz et al. (1992) Aust. J. Plant Physiol. pg. 526.
    const double atmospheric_pressure = 101325;  // pascal.

    double wa, wi, gswmol, Cs, acs;
    double aaa, bbb, ccc, ddd;
    double gsmol;
    double hs;

    double leaf_temperature = Temp + 273.15;  // kelvin.
    double pwi = water_saturation_vapor_pressure(leaf_temperature);  // kilopascal.
    double pwaPa = RelH * pwi;  // kilopascal.
    double Camf = Cappm * 1e-6;  // mol / m^2 / s.
    double assimn = Amu * 1e-6;  // mol / m^2 / s.

    /* Calculate mole fraction (mixing ratio) of water vapor in */
    /* atmosphere from partial pressure of water in the atmosphere and*/
    /* the total air pressure */
    wa  = pwaPa / atmospheric_pressure;
    /* Get saturation vapor pressure for water in the leaf based on */
    /* current idea of the leaf temperature in Kelvin*/
    /* Already calculated above */
    /* Calculate mole fraction of water vapor in leaf interior */
    wi  = pwi / atmospheric_pressure;

    if (assimn < 0.0) {
        /* Set stomatal conductance to the minimum value, beta0*/
        gswmol = beta0;
        /* Calculate leaf surface relative humidity, hs, (as fraction)*/
        /* for when C assimilation rate is <= 0*/
        /* hs = (beta0 + (wa/wi)*gbw)/(beta0 + gbw); ! unused here??*/
    } else {
        /* leaf surface CO2, mole fraction */
        Cs  = Camf - (1.4/gbw)*assimn;
        if (Cs < 0.0)
            Cs = 1;
        /* Constrain the ratio assimn/cs to be > 1.e-6*/
        acs = assimn/Cs;

        if (acs < 1e-6)
            acs = 1e-6;		

        /* Calculate leaf surface relative humidity, hs, from quadratic */
        /* equation: aaa*hs^2 + bbb*hs + ccc = 0 */
        /*  aaa= beta1 * assimn / cs */
        aaa = beta1 * acs;
        /*      bbb= beta0 + gbw - (beta1 * assimn / cs)*/
        bbb = beta0 + gbw - (beta1 * acs);
        ccc = -(wa / wi) * gbw - beta0;

        /* Solve the quadratic equation for leaf surface relative humidity */
        /* (as fraction) */
        ddd = bbb * bbb - 4*aaa*ccc;

        hs  = (-bbb + sqrt(ddd)) / (2* aaa);

        /* Ball-Berry equation (Collatz'91, eqn 1) */
        gswmol = beta1 * hs * acs + beta0;
    }
    gsmol = gswmol * 1000; /* converting to mmol */

    if (gsmol <= 0) gsmol = 1e-5;

    return(gsmol);
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
