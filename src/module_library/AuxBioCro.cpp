/*
 *  /src/AuxBioCro.c by Fernando Ezequiel Miguez  Copyright (C) 2007 - 2015
 *
 *  Part of the code here (sunML, EvapoTrans, SoilEvapo, TempTo and
 *  the *prof functions) are based on code in WIMOVAC. WIMOVAC is
 *  copyright of Stephen Long and Stephen Humphries.
 *  Documentation for WIMOVAC can be found at
 *  http://www.life.illinois.edu/plantbio/wimovac/ (checked 02-13-2010)
 *
 */

/* This file will contain functions which are common to several */
/* routines in the BioCro package. These are functions needed */
/* internally. The normal user will not need them */

#include <stdexcept>
#include <string>
#include <cmath>
#include "../constants.h" // for pi and e
#include "c4photo.h"
#include "BioCro.h"

double poisson_density(int x, double lambda)
{
    // The probability density for the Poisson distribution is
    // e^-lambda * lambda^x / x!
    // The factorial term produces numbers too large to hold, so perform the calculation in the log domain.
    // n! can be estimated by the approximation sqrt(2 * pi * x) * (x / e)^x.
    using math_constants::pi;
    using math_constants::e;

    // Stirling's approximation for n!:
    double factorial_x = sqrt(2 * pi * x) * pow((x / e), x);

    double log_result = -lambda + x * log(lambda) - log(factorial_x);

    return exp(log_result);
}

/**
 * Computation of the cosine of the zenith angle from latitute (`lat`), day of
 * the year (`DOY`), and time of day (`td`).
 *
 * For the values of angles in radians, we'll use the common practice of
 * denoting the latitude by `phi` (\f$\phi\f$), the declination by `delta`
 * (\f$\delta\f$), and the hour angle by `tau` (\f$\tau\f$).  `NDS` denotes the
 * number of days after December solstice and `omega` (\f$\omega\f$) denotes the
 * angle (in radians) of the orbital position of the earth around the sun
 * relative to its position at the December solstice.
 *
 * Denoting the axial tilt of the earth by \f$\varepsilon\f$, the equation used
 * for declination (`delta`) is
 * \f[
 *     \delta = -\varepsilon \cos(\omega)
 * \f]
 * which is an approximation (always accurate to within 0.26 degrees) of the
 * more accurate formula
 * \f[
 *     \sin(\delta) = -\sin(\varepsilon) \cos(\omega)
 * \f]
 *
 * The cosine of the solar zenith angle \f$\theta_s\f$ may be calculated from
 * the declination \f$\delta\f$, the latitude \f$\phi\f$, and the hour angle
 * \f$\tau\f$ by the formula
 * \f[
 *     \cos(\theta_s) = \sin(\delta) \sin(\phi) + \cos(\delta) \cos(\phi) \cos(\tau)
 * \f]
 * which is a straight-forward application of the law of cosines for spherical
 * triangles, substituting cofunctions of coangles in the case of latitude and
 * declination.
 */
double cos_zenith_angle(const double latitude, const int day_of_year,
                        const double hour_of_day)
{
    constexpr double radians_per_degree = math_constants::pi/180;
    constexpr int solar_noon = 12;
    constexpr double radians_rotation_per_hour = 15 * radians_per_degree;
    constexpr double axial_tilt = 23.5 * radians_per_degree;

    const double phi = latitude * radians_per_degree;
    const int NDS = day_of_year + 10;

    const double omega = 360.0 * (NDS / 365.0) * radians_per_degree;

    const double delta = -axial_tilt * cos(omega);

    const double tau = (hour_of_day - solar_noon) * radians_rotation_per_hour;

    return sin(delta) * sin(phi) + cos(delta) * cos(phi) * cos(tau);
}

/**
 * Light Macro Environment
 */
Light_model lightME(const double latitude, const int day_of_year,
                    const double hour_of_day)
{
    // day_of_year and hour_of_day are given as local time for the specified latitude.
    //
    // Return values.
    //  light_model.direct_irradiance_fraction: The fraction of irradiance that is direct radiation. dimensionless.
    //  light_model.diffuse_irradiance_fraction: The fraction of irradiance that is diffuse radiation. dimensionless.
    //  light_model.cosine_zenith_angle: The cosine of the zenith angle of the Sun. When the Sun is directly overhead, the angle is 0 and cos(angle) is 1. dimensionless.
    //
    // The basis for this function is given in chapter 11 of Norman and Campbell. An Introduction to Environmental Biophysics. 2nd edition.

    const double cosine_zenith_angle = cos_zenith_angle(latitude, day_of_year, hour_of_day);  // dimensionless.
    double direct_irradiance_transmittance;
    double diffuse_irradiance_transmittance;

    if (cosine_zenith_angle <= 0) { // Check if the Sun is at or below the horizon. If it is, directly set direct_irradiance_transmittance and diffuse_irradiance_transmittance because the equations below would otherwise produce erroneous results. The Sun is at or below the horizon when cosine_zenith_angle is less than or equal to 0.
        direct_irradiance_transmittance = 0;
        diffuse_irradiance_transmittance = 1;
    } else { // If the Sun is above the horizon, calculate diffuse and direct irradiance from the Sun's angle to the ground and the path through the atmosphere.
        constexpr double atmospheric_transmittance = 0.85; // dimensionless.
        constexpr double atmospheric_pressure_at_sea_level = 1e5; // Pa.
        constexpr double local_atmospheric_pressure = 1e5; // Pa.
        constexpr double pressure_ratio = local_atmospheric_pressure / atmospheric_pressure_at_sea_level; // dimensionless.
        constexpr double proportion_of_irradiance_scattered = 0.3; // dimensionless.

        direct_irradiance_transmittance = pow(atmospheric_transmittance, (pressure_ratio / cosine_zenith_angle)); // Modified from equation 11.11 of Norman and Campbell.
        diffuse_irradiance_transmittance = proportion_of_irradiance_scattered * (1 - direct_irradiance_transmittance) * cosine_zenith_angle; // Modified from equation 11.13 of Norman and Campbell.
    }

    Light_model light_model;
    light_model.direct_irradiance_fraction = direct_irradiance_transmittance / (direct_irradiance_transmittance + diffuse_irradiance_transmittance); // dimensionless.
    light_model.diffuse_irradiance_fraction = diffuse_irradiance_transmittance / (direct_irradiance_transmittance + diffuse_irradiance_transmittance); // dimensionless.
    light_model.cosine_zenith_angle = cosine_zenith_angle; // dimensionless.

    return light_model;
}

/**
 * Computation of an n-layered light profile from the direct light `Idir`,
 * diffuse light `Idiff`, leaf area index `LAI`, the cosine of the zenith angle
 * `cosTheta` and other parameters.
 *
 * Preconditions:
 *    `Idir` and `Idiff` are positive.
 *    `LAI` is non-negative.
 *    `nlayers` is at least 1 and not more than MAXLAY, the compile-time
 *        constant defining the maximum array size for arrays used to store
 *        profile data (enforced).
 *    `cosTheta` is greater than 0 and at most 1 (enforced).
 *    `kd` is between 0 and 1.
 *    `chil` is non-negative.
 *    `heightf` is positive.
 */
Light_profile sunML(double Idir,          // micromole / m^2 / s
        double Idiff,                     // micromole / m^2 / s
        double LAI,                       // dimensionless from m^2 / m^2
        int nlayers,
        double cosTheta,                  // dimensionless
        double kd,                        //
        double chil,                      // dimensionless from m^2 / m^2.
        double heightf)                   // m^-1 from m^2 / m^2 / m.  Leaf area density; LAI per height of canopy.
{
    if (nlayers < 1 || nlayers > MAXLAY) {
        throw std::out_of_range("nlayers must be at least 1 but no more than MAXLAY.");
    }
    if (cosTheta > 1 || cosTheta < -1) {
        throw std::out_of_range("cosTheta must be between -1 and 1.");
    }
    if (kd > 1 || kd < 0) {
        throw std::out_of_range("kd must be between 0 and 1.");
    }
    if (chil < 0) {
        throw std::out_of_range("chil must be non-negative.");
    }
    if (heightf <= 0) {
        throw std::out_of_range("heightf must greater than zero.");
    }

    constexpr double alphascatter = 0.8;

    double theta = acos(cosTheta);
    double k0 = sqrt( pow(chil, 2) + pow(tan(theta), 2) );
    double k1 = chil + 1.744 * pow((chil + 1.183), -0.733);
    double k = k0 / k1;  // Canopy extinction coefficient for an ellipsoidal leaf angle distribution. Page 251, Campbell and Norman. Environmental Biophysics. second edition.

    double LAIi = LAI / nlayers;

    Light_profile light_profile;
    const double Ibeam = Idir * cosTheta;
    double Isolar = Ibeam * k;
    for (int i = 0; i < nlayers; ++i) {
        const double CumLAI = LAIi * (i + 0.5);

        const double Iscat = Ibeam * (exp(-k * sqrt(alphascatter) * CumLAI) - exp(-k * CumLAI));

        double Idiffuse = Idiff * exp(-kd * CumLAI) + Iscat;  // The exponential term is equation 15.6, pg 255 of Campbell and Normal. Environmental Biophysics. with alpha=1 and Kbe(phi) = Kd.
        const double Ls = (1 - exp(-k * LAIi)) * exp(-k * CumLAI) / k;

        double Fsun = Ls / LAIi;
        double Fshade = 1 - Fsun;

        double Iaverage = (Fsun * (Isolar + Idiffuse) + Fshade * Idiffuse) * (1 - exp(-k * LAIi)) / k;

        // For values of cosTheta close to or less than 0, in place of the
        // calculations above, we want to use the limits of the above
        // expressions as cosTheta approaches 0 from the right:
        if (cosTheta <= 1E-10) {
            Isolar = Idir / k1;
            Idiffuse = Idiff * exp(-kd * CumLAI);
            Fsun = 0;
            Fshade = 1;
            Iaverage = 0;
        }

        light_profile.direct_irradiance[i] = Isolar + Idiffuse;  // micromole / m^2 / s
        light_profile.scattered_irradiance[i] = Iscat;  // micromole / m^2 / s
        light_profile.diffuse_irradiance[i]= Idiffuse;  // micromole / m^2 / s
        light_profile.total_irradiance[i] = Iaverage;  // micromole / m^2 / s
        light_profile.sunlit_fraction[i] = Fsun;  // dimensionless from m^2 / m^2
        light_profile.shaded_fraction[i] = Fshade;  // dimensionless from m^2 / m^2
        light_profile.height[i] = (LAI - CumLAI) / heightf;  // meters
    }
    return light_profile;
}

/* Additional Functions needed for EvapoTrans */


/**
 * RH and Wind profile function
 *
 * Preconditions:
 *     `WindSpeed` is non-negative.
 *     `LAI` is non-negative
 *     `nlayers` is at least 1 and at most MAXLAY.
 *     `wind_speed_profile` is an array of at least size `nlayers`.
 */
void WINDprof(double WindSpeed, double LAI, int nlayers,
              double* wind_speed_profile)
{
    constexpr double k = 0.7;
    double LI = LAI / nlayers;

    for (int i = 0; i < nlayers; ++i)
    {
        double CumLAI = LI * (i + 1);
        wind_speed_profile[i] = WindSpeed * exp(-k * (CumLAI - LI));
    }
}

/**
 * @brief Calculates a relative humidity profile throughout a multilayer
 * canopy.
 *
 * @param[in] RH relative humidity just above the canopy `(0 <= RH <= 1)`
 *
 * @param[in] nlayers number of layers in the canopy `(1 <= nlayers <= MAXLAY)`
 *
 * @param[out] relative_humidity_profile array of relative humidity values
 * expressed as fractions between 0 and 1, where the value at index `i`
 * represents relative humidity at the bottom of canopy layer `i` and `i = 0`
 * corresponds to the top canopy layer.
 *
 * One can derive an expression for the relative humidity (`h`) throughout a
 * plant canopy by making the following assumptions:
 * - `h` at the top of the canopy is the same as the ambient value (`h = h0`)
 * - `h` at the bottom of the canopy is one hundred percent (`h = 1`)
 * - `h` follows an exponential profile throughout the canopy
 *
 * To enforce the exponential profile, we can write
 *
 * `h(x) = A * exp(B * x)`              [Equation (1)]
 *
 * where `x` is a normalized expression of depth within the canopy (`x = 0` at
 * the canopy top and `x = 1` at the bottom) and the values of `A` and `B` are
 * yet to be determined.
 *
 * To enforce the `h` value at the canopy top, we require `h0 = h(0) = A`; in
 * other words, `A = h0`. To enforce the `h` value at the canopy bottom, we
 * require `1 = h(1) = h0 * exp(B)`; in other words, `B = -ln(h0)`. Putting it
 * all together, we see that under these assumptions, `h` throughout the canopy
 * is given by
 *
 * `h(x) = h0 * exp(-ln(h0) * x)`       [Equation (2)]
 *
 * If we additionally assume that `h0` is close to 1, we can simplify
 * `B = -ln(h0)` by taking just the linear part of the Taylor series for
 * `-ln(h0)` centered at `h0 = 1`, which is `B = -(h0 - 1)`. With this
 * modification, we have
 *
 * `h(x) = h0 * exp((1 - h0) * x)`      [Equation (3)]
 *
 * When `h0 = 1`, Equations (2) and (3) are in perfect agreement, reducing to
 * `h(x) = 1`. However, as `h0` deviates further from `h0 = 1`, the
 * linearization for `B` becomes less accurate and the two versions diverge,
 * especially deeper in the canopy. For example, when `h0 = 0.7`, Equation (2)
 * becomes `h(x) = 0.7 * exp(0.357 * x)` while Equation (3) becomes
 * `h(x) = 0.7 * exp(0.300 * x)`. Both versions agree at the top of the canopy
 * where `x = 0` and `h(x) = 1`, but are different at the bottom: 1.00 vs. 0.94.
 * For lower `h0` values, the difference at the canopy bottom becomes even more
 * pronounced, significantly violating one of the original assumptions. However,
 * there isn't a strong scientific justification for assuming humidity is 1 at
 * the bottom of *every* canopy, so the error due to the approximation for `B`
 * is deemed to be acceptable.
 *
 * ---
 *
 * In BioCro, we divide the canopy into equally sized layers; i.e., the interval
 * `0 <= x <= 1` is divided into `n` segments of length `1 / n` by `n + 1`
 * boundaries occurring at `x = 0`, `x = 1 / n`, `x = 2 / n`, ..., `x = 1`. Here
 * we wish to find the `h` value at the bottom of each layer, so we use Equation
 * (3) with `x = 1 / n`, `x = 2 / n`, ..., `x = 1`.
 *
 * In the code below, the `RH` input argument corresponds to `h0`, the
 * exponential growth constant `kh` corresponds to `B`, and `nlayers`
 * corresponds to `n`.
 *
 * ---
 *
 * Note 1: the explanation for this code was "reverse engineered" by EBL from
 * some crytpic documentation found in earlier versions of this function and
 * from Stephen Humphries's thesis, which is not available online:
 *
 * Humphries, S. "Will mechanistically rich models provide us with new insights
 * into the response of plant production to climate change? : development and
 * experiments with WIMOVAC : (Windows Intuitive Model of Vegetation response
 * to Atmosphere & Climate Change)" (University of Essex, 2002).
 *
 * ---
 *
 * Note 2: Equation (2) can be rewritten by noting that `h0 = exp(ln(h0))`. With
 * this replacement, the equation becomes
 *
 * `h(x) = exp(ln(h0) - ln(h0) * x) = exp(-B * (1 - x))`
 *
 * The equation takes this form in Humphries's thesis.
 *
 */
void RHprof(double RH, int nlayers, double* relative_humidity_profile)
{
    if (RH > 1 || RH < 0) {
        throw std::out_of_range("RH must be between 0 and 1.");
    }
    if (nlayers < 1 || nlayers > MAXLAY) {
        throw std::out_of_range("nlayers must be at least 1 but no more than MAXLAY.");
    }

    const double kh = 1 - RH;

    for (int i = 0; i < nlayers; ++i)
    {
        double j = i + 1;  // Explicitly make j a double so that j / nlayers isn't truncated.
        double temp_rh = RH * exp(kh * (j / nlayers));
        relative_humidity_profile[i] = temp_rh;
    }
}

void LNprof(double LeafN, double LAI, int nlayers, double kpLN, double* leafN_profile)
{
    double LI = LAI / nlayers;
    for(int i = 0; i < nlayers; ++i)
    {
        double CumLAI = LI * (i + 1);
        leafN_profile[i] = LeafN * exp(-kpLN * (CumLAI - LI));
    }
}

double TempToDdryA(
        double air_temperature)  // degrees C
{
    return 1.295163636 + -0.004258182 * air_temperature;  // kg / m^3
}

double dry_air_density(
/*! Calculate the density of dry air from temperature and pressure.
 * This is based on the ideal gas law. See https://en.wikipedia.org/wiki/Density_of_air for details.
 * */
        const double air_temperature,       // kelvin
        const double air_pressure)          // Pa
{
    constexpr double r_specific = 287.058;  // J / kg / K. The specific gas constant for dry air, i.e., the gas constant divided by the molar mass of air.
    return air_pressure / r_specific / air_temperature;  // kg / m^3
}

double TempToLHV(
/*! Calculate latent heat of vaporization from air temperature.
 */
        double air_temperature)  // degrees C
{
    return 2501000 + -2372.727 * air_temperature;  // J / kg.
}

double water_latent_heat_of_vaporization_henderson(
/*! Calculate the latent heat of vaporization of water from temperature.
 * B. Henderson-Sellers. 1984. Quarterly journal of royal meterological society. 110(446): 1186-1190. DOI: 10.1002/qj.49711046626.
 * Equation 8.
 */
        double temperature)  // kelvin
{
    return 1.91846e6 * pow(temperature / (temperature - 33.91), 2);  // J / kg
}

double TempToSFS(
        double air_temperature)  // degrees C
{
    return (0.338376068 + 0.011435897 * air_temperature + 0.001111111 * pow(air_temperature, 2)) * 1e-3;  //  kg / m^3 / degree C
}

/**
 * @brief Determine saturation water vapor pressure (Pa) from air temperature
 * (degrees C) using the Arden Buck equation.
 *
 * Equations of this form were used by Arden Buck to model saturation water
 * vapor pressure in 1981: [A. L. Buck, J. Appl. Meteor. 20, 1527–1532 (1981)]
 * (https://doi.org/10.1175/1520-0450(1981)020%3C1527:NEFCVP%3E2.0.CO;2)
 *
 * In 1996, some of the fitting coefficients were updated. Here we use these values,
 * found in [a Buck hygrometer manual]
 * (http://www.hygrometers.com/wp-content/uploads/CR-1A-users-manual-2009-12.pdf),
 * and also displayed on the [Wikipedia page for the Arden Buck equation]
 * (https://en.wikipedia.org/wiki/Arden_Buck_equation).
 *
 * We use the values for vapor over water (rather than ice) and disregard the
 * "enhancement factor" since we are only concerned with the pressure at
 * saturation.
 */
double saturation_vapor_pressure(double air_temperature)
{
    double a = (18.678 - air_temperature / 234.5) * air_temperature;
    double b = 257.14 + air_temperature;
    return 611.21 * exp(a / b);  // Pa
}

struct ET_Str EvapoTrans2(
        const double Rad,                                // micromoles / m^2 / s
        const double Iave,                               // micromoles / m^2 / s
        const double airTemp,                            // degrees C
        const double RH,                                 // dimensionless from Pa / Pa
        double WindSpeed,                                // m / s
        const double LeafAreaIndex,                      // dimensionless from m^2 / m^2
        double CanopyHeight,                             // meters
        const double stomatal_conductance,               // mmol / m^2 / s
        const double leaf_width,                         // meter
        const int eteq)                                  // unitless parameter
{
    constexpr double StefanBoltzmann = 5.67037e-8;       // J / m^2 / s / K^4
    constexpr double tau = 0.2;                          // dimensionless. Leaf transmission coefficient.
    constexpr double LeafReflectance = 0.2;              // dimensionless.
    constexpr double SpecificHeat = 1010;                // J / kg / K
    constexpr double molar_mass_of_water = 18.01528e-3;  // kg / mol
    constexpr double R = 8.314472;                       // joule / kelvin / mole.
    //constexpr double atmospheric_pressure = 101325;      // Pa

    CanopyHeight = fmax(0.1, CanopyHeight); // ensure CanopyHeight >= 0.1

    double WindSpeedHeight = 2; // This is the height at which the wind speed was measured.
    // When the height at which wind was measured is lower than the canopy height, there can be problems with the calculations.
    // This is a very crude way of solving this problem.
    if (WindSpeedHeight < CanopyHeight + 1) {
        WindSpeedHeight = CanopyHeight + WindSpeedHeight;
    }

    const double DdryA = TempToDdryA(airTemp);  // kg / m^3. Density of dry air.,
    const double LHV = TempToLHV(airTemp);  // J / kg
    const double SlopeFS = TempToSFS(airTemp);  // kg / m^3 / K. It is also kg / m^3 / degrees C since it's a change in temperature.
    const double SWVP = saturation_vapor_pressure(airTemp);  // Pa.

    double constexpr volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol. TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the model state. (1 * R * temperature) / pressure
    double conductance_in_m_per_s = stomatal_conductance * 1e-3 * volume_of_one_mole_of_air;  // m / s

    if (conductance_in_m_per_s <= 0.001) {
        conductance_in_m_per_s = 0.001;
    }

    if (RH > 1)
        throw std::range_error("Thrown in EvapoTrans2: RH (relative humidity) is greater than 1.");


    const double SWVC = SWVP / R / (airTemp + 273.15) * molar_mass_of_water;  // kg / m^3. Convert from vapor pressure to vapor density using the ideal gas law. This is approximately right for temperatures what won't kill plants.

    if (SWVC < 0)
        throw std::range_error("Thrown in EvapoTrans2: SWVC is less than 0.");

    const double PsycParam = DdryA * SpecificHeat / LHV;  // kg / m^3 / K

    const double vapor_density_deficit = SWVC * (1 - RH);  // kg / m^3

    const double ActualVaporPressure = RH * SWVP;  // Pa

    /* SOLAR RADIATION COMPONENT */
    // Convert from PPFD to irradiance.
    double constexpr fraction_of_irradiance_in_PAR = 0.5;  // dimensionless.
    double constexpr joules_per_micromole_PAR = 0.235;   // J / micromole. For the wavelengths that make up PAR in sunlight, one mole of photons has, on average, approximately 2.35 x 10^5 joules:
    double const total_irradiance = Rad * joules_per_micromole_PAR / fraction_of_irradiance_in_PAR;  // W / m^2.
    double const total_average_irradiance = Iave * joules_per_micromole_PAR / fraction_of_irradiance_in_PAR;  // W / m^2

    /* With a clear sky, irradiance may exceed 1000 W / m^2 in some parts of the world. Thornley and Johnson pg 400. */
    /* This value cannot possibly be higher than 1300 W / m^2. */
    if (total_irradiance > 1300) {
        throw std::range_error("Thrown in EvapoTrans2: total irradiance is " + std::to_string(total_irradiance) + ", which is too high.");
    }
    const double Ja = total_irradiance * (1 - LeafReflectance - tau) / (1 - tau);  // W / m^2

    /* The value below is only for leaf temperature */
    const double Ja2 = total_average_irradiance * (1 - LeafReflectance - tau) / (1 - tau);  // W / m^2

    /* AERODYNAMIC COMPONENT */
    if (WindSpeed < 0.5) WindSpeed = 0.5;

    /* This is the original from WIMOVAC*/
    double Deltat = 0.01;  // degrees C
    double ga;
    double rlc; /* Long wave radiation for iterative calculation */
    {
        double ChangeInLeafTemp = 10.0;  // degrees C
        double Counter = 0;
        do {
            ga = leaf_boundary_layer_conductance(WindSpeed, leaf_width, airTemp, Deltat, conductance_in_m_per_s, ActualVaporPressure);  // m / s
            /* In WIMOVAC, ga was added to the canopy conductance */
            /* ga = (ga * gbcW)/(ga + gbcW); */

            double OldDeltaT = Deltat;

            rlc = 4 * StefanBoltzmann * pow(273 + airTemp, 3) * Deltat;  // W / m^2

            /* rlc = net long wave radiation emittted per second = radiation emitted per second - radiation absorbed per second = sigma * (Tair + deltaT)^4 - sigma * Tair^4
             * To make it a linear function of deltaT, do a Taylor series about deltaT = 0 and keep only the zero and first order terms.
             * rlc = sigma * Tair^4 + deltaT * (4 * sigma * Tair^3) - sigma * Tair^4 = 4 * sigma * Tair^3 * deltaT
             * where 4 * sigma * Tair^3 is the derivative of sigma * (Tair + deltaT)^4 evaluated at deltaT = 0,
             */

            const double PhiN2 = Ja2 - rlc;  // W / m^2

            /* This equation is from Thornley and Johnson pg. 418 */
            const double TopValue = PhiN2 * (1 / ga + 1 / conductance_in_m_per_s) - LHV * vapor_density_deficit;  // J / m^3
            const double BottomValue = LHV * (SlopeFS + PsycParam * (1 + ga / conductance_in_m_per_s));  // J / m^3 / K
            Deltat = fmin(fmax(TopValue / BottomValue, -10), 10); // kelvin. Confine Deltat to the interval [-10, 10]:

            ChangeInLeafTemp = fabs(OldDeltaT - Deltat);  // kelvin
        } while ( (++Counter <= 10) && (ChangeInLeafTemp > 0.5) );
    }

    /* Net radiation */
    const double PhiN = fmax(0, Ja - rlc);  // W / m^2

    //Rprintf("SlopeFS %f, PhiN %f, LHV %f, PsycParam %f, ga %f, vapor_density_deficit %f, conductance_in... %f\n", SlopeFS, PhiN, LHV, PsycParam, ga, vapor_density_deficit, conductance_in_m_per_s);
    const double penman_monieth = (SlopeFS * PhiN + LHV * PsycParam * ga * vapor_density_deficit)
        / (LHV * (SlopeFS + PsycParam * (1 + ga / conductance_in_m_per_s)));  // kg / m^2 / s.  Thornley and Johnson. 1990. Plant and Crop Modeling. Equation 14.4k. Page 408.

    const double EPen = (SlopeFS * PhiN + LHV * PsycParam * ga * vapor_density_deficit)
        / (LHV * (SlopeFS + PsycParam));  // kg / m^2 / s

    const double EPries = 1.26 * SlopeFS * PhiN / (LHV * (SlopeFS + PsycParam));  // kg / m^2 / s

    /* Choose equation to report */
    double TransR;
    switch (eteq) {
    case 1:
        TransR = EPen;
        break;
    case 2:
        TransR = EPries;
        break;
    default:
        TransR = penman_monieth;
        break;
    }

    // TransR has units of kg / m^2 / s.
    // Convert to mm / m^2 / s.
    /* 1e3 - g / kg  */
    /* 18 - g / mol for water */
    /* 1e3 - mmol / mol */

    struct ET_Str et_results;
    et_results.TransR = TransR * 1e3 * 1e3 / 18;    // mmol / m^2 / s
    et_results.EPenman = EPen * 1e3 * 1e3 / 18;     // mmol / m^2 / s
    et_results.EPriestly = EPries * 1e3 * 1e3 / 18; // mmol / m^2 / s
    et_results.Deltat = Deltat;                     // degrees C
    et_results.boundary_layer_conductance = ga / volume_of_one_mole_of_air; // mol / m^2 / s

    return et_results;
}


double leaf_boundary_layer_conductance(
        double windspeed,  // m / s
        double leafwidth,  // m
        double air_temperature,  // degrees C
        double delta_t,  // degrees C
        double stomcond,  // m / s
        double water_vapor_pressure)  // Pa
{
    /* This is the leaf boundary layer computed using the approach in MLcan
       which is based on (Nikolov, Massman, Schoettle),         %
       Ecological Modelling, 80 (1995), 205-235 */
    constexpr double p = 101325;  // Pa. atmospheric pressure

    double leaftemp = air_temperature + delta_t;  // degrees C
    double gsv = stomcond;  // m / s
    double Tak = air_temperature + 273.15;  // K
    double Tlk = leaftemp + 273.15;  // K
    double ea = water_vapor_pressure;  // Pa
    double lw = leafwidth;  // m

    double esTl = saturation_vapor_pressure(leaftemp);  // Pa.

    /* Forced convection */
    constexpr double cf = 1.6361e-3;  // TODO: Nikolov et. al equation 29 use cf = 4.322e-3, not cf = 1.6e-3 as is used here.
    double gbv_forced = cf *  pow(Tak, 0.56) * pow((Tak + 120) * ((windspeed / lw) / p), 0.5);  // m / s.
    double gbv_free = gbv_forced;
    double eb = (gsv * esTl + gbv_free * ea) / (gsv + gbv_free); // Pa. Eq 35

    double Tvdiff = (Tlk / (1 - 0.378 * eb / p)) - (Tak / (1 - 0.378 * ea / p)); // kelvin. It is also degrees C since it is a temperature difference. Eq. 34

    if (Tvdiff < 0) Tvdiff = -Tvdiff;

    gbv_free = cf * pow(Tlk, 0.56) * pow((Tlk + 120) / p, 0.5) * pow(Tvdiff / lw, 0.25);  // m / s. Eq. 33

    double gbv = std::max(gbv_forced, gbv_free);

    return gbv;  // m / s
}




/* Soil Evaporation Function */
/* Variables I need */
/* LAI = Leaf Area Index */
/* k = extinction coefficient */
/* ppfd = Photosynthetic photon flux density */
/* awc, wiltp, fieldc = available water content, wilting point and field capacity */
/* winds = wind speed */

double SoilEvapo(double LAI, double k, double air_temperature, double ppfd, double soil_water_content,
        double fieldc, double wiltp, double winds, double RelH, double rsec,
        double soil_clod_size, double soil_reflectance, double soil_transmission, double specific_heat, double stefan_boltzman )
{
    int method = 1;
    /* A simple way of calculating the proportion of the soil that is hit by direct radiation. */
    double soil_area_sunlit_fraction = exp(-k * LAI);  // dimensionless.

    /* For now the temperature of the soil will be the same as the air.
       At a later time this can be made more accurate. I looked at the equations for this and the issue is that it is strongly dependent on
       depth. Since the soil model now has a single layer, this cannot be implemented correctly at the moment.  */
    double SoilTemp = air_temperature;

    /* From Campbell and Norman. Environmental Biophysics. */
    /* If relative available water content is */
    double rawc = (soil_water_content - wiltp) / (fieldc - wiltp);  // dimensionless. relative available water content.

    /* Campbell and Norman. Environmental Physics, page 142 */
    double maximum_uptake_rate = 1 - pow((1 + 1.3 * rawc), -5);  // dimenionless
    /* This is a useful idea because dry soils evaporate little water when dry*/

    /* Total Radiation */
    /* Convert light assuming 1 micromole PAR photons = 0.235 J/s Watts*/
    /* At the moment soil evaporation is grossly overestimated. In WIMOVAC the light reaching the last layer of leaves is used. Here instead
       of calculating this again, I will for now assume a 10% as a rough estimate. Note that I could maybe get this since layIdir and
       layIDiff in sunML are external variables.  Rprintf("ppfd %.5f",layIdir[0],"\n"); Update: 03-13-2009. I tried printing this
       value but it is still too high and will likely overestimate soil evaporation. However, this is still a work in progress.
       */
    ppfd *= rsec; /* Radiation soil evaporation coefficient */

    double TotalRadiation = ppfd * 0.235;

    double DdryA = TempToDdryA(air_temperature);
    double LHV = TempToLHV(air_temperature);  // J / kg
    double SlopeFS = TempToSFS(air_temperature);
    double SWVC = saturation_vapor_pressure(air_temperature) * 1e-5;

    double PsycParam = (DdryA * specific_heat) / LHV;
    double vapor_density_deficit = SWVC * (1 - RelH / 100);

    double BoundaryLayerThickness = 4e-3 * sqrt(soil_clod_size / winds);
    double DiffCoef = 2.126e-5 * 1.48e-7 * SoilTemp;
    double SoilBoundaryLayer = DiffCoef / BoundaryLayerThickness;

    double Ja = 2 * TotalRadiation * ((1 - soil_reflectance - soil_transmission) / (1 - soil_transmission));

    double rlc = 4 * stefan_boltzman * pow((273 + SoilTemp), 3) * 0.005;
    /* the last term should be the difference between air temperature and soil. This is not actually calculated at the moment. Since this is
       mostly relevant to the first soil layer where the temperatures are similar. I will leave it like this for now. */

    double PhiN = Ja - rlc; /* Calculate the net radiation balance*/
    if (PhiN < 0) PhiN = 1e-7;

    double Evaporation = 0.0;
    if (method == 0) {
        /* Priestly-Taylor */
        Evaporation = 1.26 * (SlopeFS * PhiN) / (LHV * (SlopeFS + PsycParam));  // kg / m^2 / s.
    } else {
        /* Penman-Monteith */
        Evaporation = (SlopeFS * PhiN + LHV * PsycParam * SoilBoundaryLayer * vapor_density_deficit) / (LHV * (SlopeFS + PsycParam));  // kg / m^2 / s.
    }

    Evaporation *= soil_area_sunlit_fraction * maximum_uptake_rate;  // kg / m^2 / s.
    if (Evaporation < 0) Evaporation = 1e-6;  // Prevent odd values at very low light levels.

    return Evaporation;  // kg / m^2 / s.
}

double compute_wsPhoto(int wsFun, double fieldc, double wiltp, double phi1, double awc) {
    // Three different type of equations for modeling the effect of
    // water stress on vmax and leaf area expansion.  The equation for
    // leaf area expansion is more severe than the one for vmax. */
    double wsPhoto;
    switch (wsFun) {
    case 0: { /* linear */
        double slp = 1 / (fieldc - wiltp);
        double intcpt = 1 - fieldc * slp;
        wsPhoto = slp * awc + intcpt;
        break;
    }
    case 1: {
        double phi10 = (fieldc + wiltp) / 2;
        wsPhoto = 1 / (1 + exp((phi10 - awc) / phi1));
        break;
    }
    case 2: {
        double slp = (1 - wiltp) / (fieldc - wiltp);
        double intcpt = 1 - fieldc * slp;
        double theta = slp * awc + intcpt;
        wsPhoto = (1 - exp(-2.5 * (theta - wiltp)/(1 - wiltp))) / (1 - exp(-2.5));
        break;
    }
    case 3:
        wsPhoto = 1;
        break;
    default:
        wsPhoto = 0;
    }

    // wsPhoto can be mathematically lower than zero in some cases but
    // I should prevent that:
    if (wsPhoto <= 0) {
        wsPhoto = 1e-10;
    }

    // Apparently wsPhoto can be greater than 1.
    if (wsPhoto > 1) wsPhoto = 1;

    return wsPhoto;
}

/* This is a new function that attempts to keep a water budget and then
   calcualte an empirical coefficient that reduces the specific leaf area.
   This results from the general idea that water stress reduces first the
   rate of leaf expansion. */

/* This is meant to be a simple function that calculates a
   simple empirical coefficient that reduces specifi leaf area
   according to the water stress of the plant. This is done
   for now, with a very simple empirical approach. */

struct ws_str watstr(double precipit, double evapo, double cws, double soildepth, double fieldc,
                     double wiltp, double phi1, double phi2, double soil_saturation_capacity, double sand,
                     double Ks, double air_entry, double b)
{
    constexpr double g = 9.8; // m / s^2  ##  http://en.wikipedia.org/wiki/Standard_gravity

    double precipM = precipit * 1e-3; /* convert precip in mm to m*/
    double soil_water_fraction = precipM / soildepth + cws;  // m^3 m^-3

    double runoff = 0.0;
    double Nleach = 0.0;  /* Nleach is the NO3 leached. */

    if (soil_water_fraction > soil_saturation_capacity) {
        runoff = (soil_water_fraction - soil_saturation_capacity) * soildepth; /* This is in meters */
        /* Here runoff is interpreted as water content exceeding saturation level */
        /* Need to convert to units used in the Parton et al 1988 paper. */
        /* The data come in mm/hr and need to be in cm/month */
        Nleach = runoff / 18 * (0.2 + 0.7 * sand);
        soil_water_fraction = soil_saturation_capacity;
    }

    /* The density of water is 0.9982 Mg / m^3 at 20 degrees Celsius. */
    /* evapo is demanded water (Mg / ha) */
    double npaw = soil_water_fraction - wiltp - evapo / 0.9982 / 1e4 / soildepth;  // fraction of saturation.

    /* If demand exceeds supply, the crop is getting close to wilting point and transpiration
       will be over estimated. In this one layer model though, the crop is practically dead. */
    if (npaw < 0) npaw = 0.0;
    double awc = npaw + wiltp;

    /* Calculating the soil water potential based on equations from Norman and Campbell */
    /* tmp.psim = soTexS.air_entry * pow((awc/soTexS.fieldc*1.1),-soTexS.b); */
    /* New version of the soil water potential is based on
     * "Dynamic Simulation of Water Deficit Effects upon Maize Yield" R. F. Grant Agricultural Systems. 33(1990) 13-39. */
    ws_str tmp;
    tmp.psim = -exp(log(0.033) + (log(fieldc) - log(awc)) / (log(fieldc) - log(wiltp)) * (log(1.5) - log(0.033))) * 1e3; /* This last term converts from MPa to kPa */

    /* This is drainage */
    if (awc > fieldc) {
        double K_psim = Ks * pow((air_entry / tmp.psim), 2 + 3 / b); /* This is hydraulic conductivity */

        double J_w = -K_psim * (-tmp.psim / (soildepth * 0.5)) - g * K_psim; /*  Campbell, pg 129 do not ignore the graviational effect. I multiply soil depth by 0.5 to calculate the average depth */
        double drainage = J_w * 3600 * 0.9982 * 1e-3; /* This is flow in m^3 / (m^2 * hr). */
        awc = awc + drainage / soildepth;
    }

    tmp.awc = awc;
    tmp.runoff = runoff;
    tmp.Nleach = Nleach;
    return tmp;
}

/* Function to simulate the multilayer behavior of soil water. In the
   future this could be coupled with Campbell (BASIC) ideas to
   esitmate water potential. */
struct soilML_str soilML(double precipit, double transp, double *cws, double soildepth, double *depths,
        double soil_field_capacity, double soil_wilting_point, double soil_saturation_capacity, double soil_air_entry, double soil_saturated_conductivity,
        double soil_b_coefficient, double soil_sand_content, double phi1, double phi2, int wsFun,
        int layers, double rootDB, double LAI, double k, double AirTemp,
        double IRad, double winds, double RelH, int hydrDist, double rfl,
        double rsec, double rsdf, double soil_clod_size, double soil_reflectance, double soil_transmission,
        double specific_heat, double stefan_boltzman )
{
    constexpr double g = 9.8; /* m / s-2  ##  http://en.wikipedia.org/wiki/Standard_gravity */

    soilML_str return_value;

    /* Crude empirical relationship between root biomass and rooting depth*/
    double rootDepth = fmin(rootDB * rsdf, soildepth);

    rd_str root_distribution = rootDist(layers, rootDepth, &depths[0], rfl);

    /* unit conversion for precip */
    double oldWaterIn = 0.0;
    double waterIn = precipit * 1e-3; /* convert precip in mm to m */

    double drainage = 0.0;
    double wsPhotoCol = 0.0;
    double LeafWSCol = 0.0;
    double Sevap = 0.0;
    double oldEvapoTra = 0.0;

    for (int i = layers - 1; i >= 0; --i) { /* The counter, i, decreases because I increase the water content due to precipitation in the last layer first*/

        double layerDepth = depths[i+1] - depths[i];  /* This supports unequal depths. */

        if (hydrDist > 0) {
            /* For this section see Campbell and Norman "Environmental BioPhysics" Chapter 9*/
            /* First compute the matric potential */
            double psim1 = soil_air_entry * pow((cws[i]/soil_saturation_capacity), -soil_b_coefficient); /* This is matric potential of current layer */
            double dPsim;
            if (i > 0) {
                double psim2 = soil_air_entry * pow((cws[i-1]/soil_saturation_capacity), -soil_b_coefficient); /* This is matric potential of next layer */
                dPsim = psim1 - psim2;
                /* The substraction is from the layer i - (i-1). If this last term is positive then it will move upwards. If it is negative it will move downwards. Presumably this term is almost always positive. */
            } else {
                dPsim = 0.0;
            }

            double K_psim = soil_saturated_conductivity * pow((soil_air_entry/psim1), 2+3/soil_b_coefficient); /* This is hydraulic conductivity */
            double J_w = K_psim * (dPsim/layerDepth) - g * K_psim; /*  Campbell, pg 129 do not ignore the graviational effect*/
            /* Notice that K_psim is positive because my reference system is reversed */
            /* This last result should be in kg/(m2 * s) */
            J_w *= 3600 * 0.9882 * 1e-3; /* This is flow in m3 / (m^2 * hr). */

            if (i == (layers-1) && J_w < 0) {
                /* cws[i] = cws[i] + J_w / layerDepth; Although this should be done it drains the last layer too much.*/
                drainage += J_w;
            } else {
                    cws[i] = cws[i] - J_w / layerDepth;
                if (i > 0) {
                    cws[i - 1] = cws[i - 1] + J_w / layerDepth;
                }
            }
        }

        if (cws[i] > soil_saturation_capacity) cws[i] = soil_saturation_capacity;
        if (cws[i] < soil_wilting_point) cws[i] = soil_wilting_point;
        // Here is a convention aw is available water in volume and awc
        // is available water content as a fraction of the soil section being investigated.
        double aw = cws[i] * layerDepth;
        /* Available water (for this layer) is the current water status times the layer depth */

        if (waterIn > 0) {
            /* There is some rain. Need to add it.*/
            aw += waterIn / layers + oldWaterIn; /* They are both in meters so it works */
            /* Adding the same amount to water to each layer */
            /* In case there is overflow */
            double diffw = soil_field_capacity * layerDepth - aw;

            if (diffw < 0) {
                /* This means that precipitation exceeded the capacity of the first layer */
                /* Save this amount of water for the next layer */
                oldWaterIn = -diffw;
                aw = soil_field_capacity * layerDepth;
            } else {
                oldWaterIn = 0.0;
            }
        }

        /* Root Biomass */
        double rootATdepth = rootDB * root_distribution.rootDist[i];
        return_value.rootDist[i] = rootATdepth;
        /* Plant available water is only between current water status and permanent wilting point */
        /* Plant available water */
        double pawha = (aw - soil_wilting_point * layerDepth) * 1e4;

        if (pawha < 0) pawha = 0;

        double Ctransp = 0.0;
        double EvapoTra = 0.0;
        double Newpawha;

        if (i == 0) {
            /* Only the first layer is affected by soil evaporation */
            double awc2 = aw / layerDepth;
            /* SoilEvapo function needs soil water content  */
            Sevap = SoilEvapo(LAI, k, AirTemp, IRad, awc2, soil_field_capacity, soil_wilting_point, winds, RelH, rsec,
                soil_clod_size, soil_reflectance, soil_transmission, specific_heat, stefan_boltzman ) * 3600 * 1e-3 * 10000;  // Mg / ha / hr. 3600 s / hr * 1e-3 Mg / kg * 10000 m^2 / ha.
            /* I assume that crop transpiration is distributed simlarly to
               root density.  In other words the crop takes up water proportionally
               to the amount of root in each respective layer.*/
            Ctransp = transp * root_distribution.rootDist[0];
            EvapoTra = Ctransp + Sevap;
            constexpr double density_of_water_at_20_celcius = 0.9982;  // Mg m^-3.
            Newpawha = pawha - EvapoTra / density_of_water_at_20_celcius;
            /* The first term in the rhs pawha is the m3 of water available in this layer.
               EvapoTra is the Mg H2O ha-1 of transpired and evaporated water. 1/0.9882 converts from Mg to m3 */
        } else {
            Ctransp = transp * root_distribution.rootDist[i];
            EvapoTra = Ctransp;
            Newpawha = pawha - (EvapoTra + oldEvapoTra);
        }

        if (Newpawha < 0) {
            /* If the Demand is not satisfied by this layer. This will be stored and added to subsequent layers*/
            oldEvapoTra = -Newpawha;
            aw = soil_wilting_point * layerDepth;
        }

        double awc = Newpawha / 1e4 / layerDepth + soil_wilting_point;

        /* This might look like a weird place to populate the structure, but is more convenient*/
        return_value.cws[i] = awc;

        // To-do: Replace this block with a call to compute_wsPhoto.
    /* three different type of equations for modeling the effect of water stress on vmax and leaf area expansion.
       The equation for leaf area expansion is more severe than the one for vmax. */
        double wsPhoto = 0.0;
        double slp = 0.0;
        double intcpt = 0.0;
        double theta = 0.0;

        if (wsFun == 0) { /* linear */
            slp = 1/(soil_field_capacity - soil_wilting_point);
            intcpt = 1 - soil_field_capacity * slp;
            wsPhoto = slp * awc + intcpt;
        } else if (wsFun == 1) {
            double phi10 = (soil_field_capacity + soil_wilting_point)/2;
            wsPhoto = 1/(1 + exp((phi10 - awc)/ phi1));
        } else if (wsFun == 2) {
            slp = (1 - soil_wilting_point)/(soil_field_capacity - soil_wilting_point);
            intcpt = 1 - soil_field_capacity * slp;
            theta = slp * awc + intcpt;
            wsPhoto = (1 - exp(-2.5 * (theta - soil_wilting_point)/(1 - soil_wilting_point))) / (1 - exp(-2.5));
        } else if (wsFun == 3) {
            wsPhoto = 1;
        }

        if (wsPhoto <= 0 )
            wsPhoto = 1e-10; /* This can be mathematically lower than zero in some cases but I should prevent that. */

        wsPhotoCol += wsPhoto;

        double LeafWS = pow(awc, phi2) * 1/pow(soil_field_capacity, phi2);
        if (wsFun == 3) {
            LeafWS = 1;
        }
        LeafWSCol += LeafWS;

    }

    if (waterIn > 0) {
        drainage = waterIn;
        /* Need to convert to units used in the Parton et al 1988 paper. */
        /* The data comes in mm/hr and it needs to be in cm/month */
        return_value.Nleach = drainage * 0.1 * (1/24 * 30) / (18 * (0.2 + 0.7 * soil_sand_content));
    }
    else {
        return_value.Nleach = 0.0;
    }


    return_value.rcoefPhoto = (wsPhotoCol/layers);
    return_value.drainage = drainage;
    return_value.rcoefSpleaf = (LeafWSCol/layers);
    return_value.SoilEvapo = Sevap;

    return return_value;
}

/* Respiration. It is assumed that some of the energy produced by the
   plant has to be used in basic tissue maintenance. This is handled
   quite empirically by some relationships developed by McCree (1970)
   and Penning de Vries (1972) */

double resp(double comp, double mrc, double temp) {
    double ans = comp *  (1 - (mrc * pow(2, (temp / 10.0))));

    if (ans <0) ans = 0;

    return ans;

}

struct seqRD_str seqRootDepth(double to, int lengthOut ) {
    double by = to / lengthOut;

    struct seqRD_str result;
    for (int i = 0; i <= lengthOut; ++i) {
        result.rootDepths[i] = i * by;
    }
    return result;
}

struct rd_str rootDist(int n_layers, double rootDepth, double *depths, double rfl)
{
    /*
     * Calculate the fraction of total root mass for each layer in `depths` assuming the mass
     * is follows a Poisson distribution along the depth.
     *
     * Returns a struct with one array of size `n_layers`.
     * Each element in the array is the fraction of total root mass in that layer.
     * The sum of all elements of the result equals 1.
     */

    double layerDepth = 0.0;
    double CumLayerDepth = 0.0;
    int CumRootDist = 1;
    double rootDist[n_layers];
    double cumulative_a = 0.0;

    for (int i = 0; i < n_layers; ++i) {
        if (i == 0) {
            layerDepth = depths[1];
        } else {
            layerDepth = depths[i] - depths[i-1];
        }

        CumLayerDepth += layerDepth;

        if (rootDepth > CumLayerDepth) {
            CumRootDist++;
        }
    }

    for (int j = 0; j < n_layers; ++j) {
        if (j < CumRootDist) {
            double a = poisson_density(j + 1, (double)CumRootDist * rfl);
            rootDist[j] = a;
            cumulative_a += a;
        } else {
            rootDist[j] = 0;
        }
    }

    struct rd_str result;
    for (int k = 0; k < n_layers; ++k) {
        result.rootDist[k] = rootDist[k] / cumulative_a;
    }
    return  result;
}


soilText_str get_soil_properties(int soiltype) {
    return get_soil_properties(static_cast<SoilType>(soiltype));
}

const soilText_str get_soil_properties(SoilType soiltype)
{
    return soil_parameters.at(soiltype);
}

inline double arrhenius_exponent(double c, double activation_energy, double thermodynamic_temperature) {
    constexpr double R = 8.314472; // joule / kelvin / mole.
    return exp(c - activation_energy / (R * thermodynamic_temperature));
};

