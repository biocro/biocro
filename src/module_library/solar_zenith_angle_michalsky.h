#ifndef SOLAR_ZENITH_ANGLE_MICHALSKY_H
#define SOLAR_ZENITH_ANGLE_MICHALSKY_H

#include <cmath>                     // for floor, fmod
#include "../degree_trigonometry.h"  // for atan2_deg, cos_deg, sin_deg, acos_deg
#include "../modules.h"
#include "../state_map.h"

/**
 *  @class solar_zenith_angle_michalsky
 *
 *  @brief Calculates the solar zenith angle using the model described in
 *  [Michalsky, J. J. "The Astronomical Almanac's algorithm for approximate
 *  solar position (1950–2050)" Solar Energy 40, 227–235 (1988)]
 *  (https://doi.org/10.1016/0038-092X(88)90045-X).
 *
 *  As the paper's title indicates, this method is only recommended for years
 *  between 1950 - 2050.
 *
 *  In this module, we neglect the refraction correction from the Michalsky
 *  paper. The correction is small and was not found to have a significant
 *  effect on the output of a BioCro simulation; furthermore, it is
 *  discontinuous, which can cause problems for differential equation solvers.
 */
class solar_zenith_angle_michalsky : public direct_module
{
   public:
    solar_zenith_angle_michalsky(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
          direct_module("solar_zenith_angle_michalsky"),

          // Get references to input quantities
          lat(get_input(input_quantities, "lat")),
          longitude(get_input(input_quantities, "longitude")),
          time(get_input(input_quantities, "time")),
          time_zone_offset(get_input(input_quantities, "time_zone_offset")),
          year(get_input(input_quantities, "year")),

          // Get pointers to output quantities
          cosine_zenith_angle_op(get_op(output_quantities, "cosine_zenith_angle")),
          julian_date_op(get_op(output_quantities, "julian_date")),
          solar_L_op(get_op(output_quantities, "solar_L")),
          solar_g_op(get_op(output_quantities, "solar_g")),
          solar_ell_op(get_op(output_quantities, "solar_ell")),
          solar_ep_op(get_op(output_quantities, "solar_ep")),
          solar_ra_op(get_op(output_quantities, "solar_ra")),
          solar_dec_op(get_op(output_quantities, "solar_dec")),
          gmst_op(get_op(output_quantities, "gmst")),
          lmst_op(get_op(output_quantities, "lmst")),
          lha_op(get_op(output_quantities, "lha")),
          solar_zenith_angle_op(get_op(output_quantities, "solar_zenith_angle")),
          solar_elevation_angle_op(get_op(output_quantities, "solar_elevation_angle"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();

   private:
    // References to input quantities
    double const& lat;
    double const& longitude;
    double const& time;
    double const& time_zone_offset;
    double const& year;

    // Pointers to output quantities
    double* cosine_zenith_angle_op;
    double* julian_date_op;
    double* solar_L_op;
    double* solar_g_op;
    double* solar_ell_op;
    double* solar_ep_op;
    double* solar_ra_op;
    double* solar_dec_op;
    double* gmst_op;
    double* lmst_op;
    double* lha_op;
    double* solar_zenith_angle_op;
    double* solar_elevation_angle_op;

    // Main operation
    void do_operation() const;
};

string_vector solar_zenith_angle_michalsky::get_inputs()
{
    return {
        "lat",               // degrees (North is positive)
        "longitude",         // degrees (East is positive)
        "time",              // time expressed as a fractional day of year
        "time_zone_offset",  // the offset of the time zone relative to UTC
        "year"               // a year between 1950 and 2050
    };
}

string_vector solar_zenith_angle_michalsky::get_outputs()
{
    return {
        "cosine_zenith_angle",   // dimensionless
        "julian_date",           // days
        "solar_L",               // degrees
        "solar_g",               // degrees
        "solar_ell",             // degrees
        "solar_ep",              // degrees
        "solar_ra",              // degrees
        "solar_dec",             // degrees
        "gmst",                  // hours
        "lmst",                  // hours
        "lha",                   // degrees
        "solar_zenith_angle",    // degrees
        "solar_elevation_angle"  // degrees
    };
}

void solar_zenith_angle_michalsky::do_operation() const
{
    // Define some constants
    double constexpr deg_to_hr = 1.0 / 15.0;
    double constexpr hr_to_deg = 15.0;
    double constexpr hr_per_day = 24.0;
    double constexpr deg_per_rev = 360.0;

    double constexpr jd_ref_1948 = 2432916.5;  // Julian date at midnight on 31 December 1948 (UTC)
    double constexpr jd_ref_2000 = 2451545.0;  // Julian date at noon on 1 January 2000 (UTC)

    // Unpack the doy and hour in UTC
    double time_utc = time - time_zone_offset / hr_per_day;     // days
    double const doy_utc = floor(time_utc);                     // days
    double const hour_utc = hr_per_day * (time_utc - doy_utc);  // hours

    // Calculate the Julian date
    double const delta = year - 1949.0;
    double const leap = floor(0.25 * delta);
    double const jd = jd_ref_1948 + delta * 365.0 + leap + doy_utc + hour_utc / hr_per_day;  // days

    // Calculate the n, which represents the difference in Julian date between
    // the current time and 1 January 2000 (UTC). This is the basis for
    // calculations used by the Astronomical Almanac.
    double const n = jd - jd_ref_2000;  // days

    // Calculate the ecliptic coordinates of the sun (in degrees):
    // - L (mean longitude)
    // - g (mean anomaly)
    // - ell (ecliptic anomaly)
    // - ep (obliquity of the elliptic)
    // Here we assume that the ecliptic latitude (beta) is zero. See
    // https://en.wikipedia.org/wiki/Ecliptic_coordinate_system#Spherical_coordinates
    // for more information about this coordinate system.
    double const L = fmod(280.460 + 0.9856474 * n, deg_per_rev);                            // degrees
    double const g = fmod(357.528 + 0.9856003 * n, deg_per_rev);                            // degrees
    double const ell = fmod(L + 1.915 * sin_deg(g) + 0.020 * sin_deg(2 * g), deg_per_rev);  // degrees
    double const ep = 23.439 - 0.0000004 * n;                                               // degrees

    // Calculate the equatorial celestial coordinates of the sun (in degrees):
    // - ra (right ascension; analgous to longitude)
    // - dec (declination; analogous to latitude)
    // See https://en.wikipedia.org/wiki/Astronomical_coordinate_systems#Equatorial_system
    // for more information about this coordinate system.
    double const ra = atan2_deg(cos_deg(ep) * sin_deg(ell), cos_deg(ell));  // degrees
    double const dec = asin_deg(sin_deg(ep) * sin_deg(ell));                // degrees

    // Convert to local coordinates (in degrees):
    // - za (zenith angle)
    // - ea (elevation angle)
    // - lha (local hour angle)
    // See https://en.wikipedia.org/wiki/Solar_zenith_angle for more information
    // about this coordinate system.

    // In order to find these, we need to first calculate the Greenwich mean
    // sidereal time (GMST) and the local mean sidereal time (LMST) before
    // determining the sun's angular position in the local sky. These times are
    // expressed in units of hours. See https://en.wikipedia.org/wiki/Sidereal_time
    // for more information about sidereal time.
    double const gmst = fmod(6.697375 + 0.0657098242 * n + hour_utc, hr_per_day);  // hours
    double const lmst = fmod(gmst + longitude * deg_to_hr, hr_per_day);            // hours
    double const lha = fmod(lmst * hr_to_deg - ra, deg_per_rev);                   // degrees
    double const za = acos_deg(sin_deg(dec) * sin_deg(lat) +
                               cos_deg(dec) * cos_deg(lat) * cos_deg(lha));  // degrees
    double const ea = 90.0 - za;                                             // degrees

    // Determine the cosine of the zenith angle
    double const cosine_zenith_angle = cos_deg(za);  // dimensionless

    // Update the output pointers
    update(cosine_zenith_angle_op, cosine_zenith_angle);
    update(julian_date_op, jd);
    update(solar_L_op, L);
    update(solar_g_op, g);
    update(solar_ell_op, ell);
    update(solar_ep_op, ep);
    update(solar_ra_op, ra);
    update(solar_dec_op, dec);
    update(gmst_op, gmst);
    update(lmst_op, lmst);
    update(lha_op, lha);
    update(solar_zenith_angle_op, za);
    update(solar_elevation_angle_op, ea);
}

#endif
