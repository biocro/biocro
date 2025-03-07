#ifndef SOLAR_POSITION_MICHALSKY_H
#define SOLAR_POSITION_MICHALSKY_H

#include <cmath>                               // for floor, fmod
#include "../framework/degree_trigonometry.h"  // for atan2_deg, cos_deg, sin_deg, acos_deg
#include "../framework/module.h"
#include "../framework/state_map.h"

namespace standardBML
{
/**
 *  @class solar_position_michalsky
 *
 *  [michalsky]: https://doi.org/10.1016/0038-092X(88)90045-X "Michalsky article"

 *  @brief Calculates the solar position using the model described in
 *  Michalsky, J. J. ["The Astronomical Almanac's algorithm for
 *  approximate solar position (1950–2050)"][michalsky] Solar Energy
 *  40, 227–235 (1988)
 *
 *  As the paper's title indicates, this method is only recommended for years
 *  between 1950 - 2050.
 *
 *  In this module, we neglect the refraction correction from the Michalsky
 *  paper. The correction is small and was not found to have a significant
 *  effect on the output of a BioCro simulation; furthermore, it is
 *  discontinuous, which can cause problems for differential equation solvers.
 *
 *  The cosine of the solar zenith angle is provided as a convenience since
 *  several other BioCro modules only use the zenith angle via its cosine.
 *
 *  ### Model details
 *
 *  The calculations in the model follow this general structure:
 *
 *  - First, the time is converted from the more familiar format of year, day,
 *    hour, etc into a Julian date, which uniquely specifies a time using one
 *    number representing the continuous count of days since the beginning of
 *    the Julian period. An additional operation converts the Julian date into
 *    an "n" date representing the difference in Julian date between the
 *    current time and 1 January 2000 (UTC). Although this format is less common
 *    than the Julian date, the "n" date is the basis for the calculations
 *    used by the Astronomical Almanac. These "date conversion" operations
 *    follow Section 2.1 of the Michalsky paper.
 *
 *  - Second, the time (expressed as an "n" date) is used to calculate the
 *    ecliptic coordinates of the sun: the mean longitude, the mean anomaly,
 *    the ecliptic anomaly, and the obliquity of the ecliptic. These four
 *    numbers represent the location of the Sun relative to the Earth; see
 *    https://en.wikipedia.org/wiki/Ecliptic_coordinate_system#Spherical_coordinates
 *    for more information about this coordinate system. These operations follow
 *    Section 2.2 of the Michalsky paper.
 *
 *  - Third, the ecliptic coordinates are used to determine the Sun's position
 *    in celestial coordinates; here the conversion is accomplished using purely
 *    geometric factors with no explicit reference to the time. In this system,
 *    the Sun's position is specified using right ascension (analogous to
 *    longitude) and declination (analogous to latitude). See
 *    https://en.wikipedia.org/wiki/Astronomical_coordinate_systems#Equatorial_system
 *    for more information about this coordinate system. These operations follow
 *    Section 2.3 of the Michalsky paper.
 *
 *  - Fourth, two additional expressions of time are determined: Greenwich mean
 *    sidereal time and local mean sidereal time. See
 *    https://en.wikipedia.org/wiki/Sidereal_time for more information about
 *    sidereal time. These operations follow the first two equations in Section
 *    2.4 of the Michalsky paper.
 *
 *  - Fifth, the coordinates of the Sun's position in the local sky (the
 *    local hour, azimuthal, and zenith angles) can finally be determined from
 *    the celestial coordinates and the local mean sidereal time. See
 *    https://en.wikipedia.org/wiki/Solar_zenith_angle for more information
 *    about this coordinate system. These operations follow Equations 3 - 5 in
 *    the Michalsky paper.
 *
 *  ### Symbols and quantity names used in this module
 *
 *  | Name                         | Variable in code | BioCro quantity           |
 *  | :--------------------------: | :--------------: | :-----------------------: |
 *  | Julian date                  | `jd`             | ``'julian_date'``         |
 *  | "n" date                     | `n`              | NA                        |
 *  | mean longitude               | `L`              | ``'solar_L'``             |
 *  | mean anomaly                 | `g`              | ``'solar_g'``             |
 *  | ecliptic anomaly             | `ell`            | ``'solar_ell'``           |
 *  | obliquity of the ecliptic    | `ep`             | ``'solar_ep'``            |
 *  | right ascension              | `ra`             | ``'solar_ra'``            |
 *  | declination                  | `dec`            | ``'solar_dec'``           |
 *  | Greenwich mean sidereal time | `gmst`           | ``'gmst'``                |
 *  | local mean sidereal time     | `lmst`           | ``'lmst'``                |
 *  | local hour angle             | `lha`            | ``'lha'``                 |
 *  | zenith angle                 | `zen`            | ``'solar_zenith_angle'``  |
 *  | azimuthal angle              | `az`             | ``'solar_azimuth_angle'`` |
 *  | cosine of the zenith angle   | `cos_zen`        | ``'cosine_zenith_angle'`` |
 *
 */
class solar_position_michalsky : public direct_module
{
   public:
    solar_position_michalsky(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // Get references to input quantities
          lat{get_input(input_quantities, "lat")},
          longitude{get_input(input_quantities, "longitude")},
          fractional_doy{get_input(input_quantities, "fractional_doy")},
          time_zone_offset{get_input(input_quantities, "time_zone_offset")},
          year{get_input(input_quantities, "year")},

          // Get pointers to output quantities
          cosine_zenith_angle_op{get_op(output_quantities, "cosine_zenith_angle")},
          julian_date_op{get_op(output_quantities, "julian_date")},
          solar_L_op{get_op(output_quantities, "solar_L")},
          solar_g_op{get_op(output_quantities, "solar_g")},
          solar_ell_op{get_op(output_quantities, "solar_ell")},
          solar_ep_op{get_op(output_quantities, "solar_ep")},
          solar_ra_op{get_op(output_quantities, "solar_ra")},
          solar_dec_op{get_op(output_quantities, "solar_dec")},
          gmst_op{get_op(output_quantities, "gmst")},
          lmst_op{get_op(output_quantities, "lmst")},
          lha_op{get_op(output_quantities, "lha")},
          solar_zenith_angle_op{get_op(output_quantities, "solar_zenith_angle")},
          solar_azimuth_angle_op{get_op(output_quantities, "solar_azimuth_angle")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "solar_position_michalsky"; }

   private:
    // References to input quantities
    double const& lat;
    double const& longitude;
    double const& fractional_doy;
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
    double* solar_azimuth_angle_op;

    // Main operation
    void do_operation() const;
};

string_vector solar_position_michalsky::get_inputs()
{
    return {
        "lat",               // degrees (North is positive)
        "longitude",         // degrees (East is positive)
        "fractional_doy",    // days
        "time_zone_offset",  // the offset of the time zone relative to UTC
        "year"               // a year between 1950 and 2050
    };
}

string_vector solar_position_michalsky::get_outputs()
{
    return {
        "cosine_zenith_angle",  // dimensionless
        "julian_date",          // days
        "solar_L",              // degrees
        "solar_g",              // degrees
        "solar_ell",            // degrees
        "solar_ep",             // degrees
        "solar_ra",             // degrees
        "solar_dec",            // degrees
        "gmst",                 // hr
        "lmst",                 // hr
        "lha",                  // degrees
        "solar_zenith_angle",   // degrees
        "solar_azimuth_angle"   // degrees
    };
}

void solar_position_michalsky::do_operation() const
{
    // Define some constants
    double constexpr deg_to_hr = 1.0 / 15.0;
    double constexpr hr_to_deg = 15.0;
    double constexpr hr_per_day = 24.0;
    double constexpr deg_per_rev = 360.0;

    double constexpr jd_ref_1948 = 2432916.5;  // Julian date at midnight on 31 December 1948 (UTC)
    double constexpr jd_ref_2000 = 2451545.0;  // Julian date at noon on 1 January 2000 (UTC)

    // Unpack the doy and hour in UTC
    double const fractional_doy_utc = fractional_doy - time_zone_offset / hr_per_day;  // days
    double const doy_utc = std::floor(fractional_doy_utc / hr_per_day);                // days
    double const hour_utc = hr_per_day * (fractional_doy_utc - doy_utc);               // hr

    // Calculate the Julian date
    double const delta = year - 1949.0;
    double const leap = floor(0.25 * delta);
    double const jd = jd_ref_1948 + delta * 365.0 + leap + doy_utc + hour_utc / hr_per_day;  // days

    // Calculate the "n" date
    double const n = jd - jd_ref_2000;  // days

    // Calculate the ecliptic coordinates of the sun
    double const L = fmod(280.460 + 0.9856474 * n, deg_per_rev);                            // degrees
    double const g = fmod(357.528 + 0.9856003 * n, deg_per_rev);                            // degrees
    double const ell = fmod(L + 1.915 * sin_deg(g) + 0.020 * sin_deg(2 * g), deg_per_rev);  // degrees
    double const ep = 23.439 - 0.0000004 * n;                                               // degrees

    // Calculate the equatorial celestial coordinates of the sun
    double const ra = atan2_deg(cos_deg(ep) * sin_deg(ell), cos_deg(ell));  // degrees
    double const dec = asin_deg(sin_deg(ep) * sin_deg(ell));                // degrees

    // Calculate the sidereal time
    double const gmst = fmod(6.697375 + 0.0657098242 * n + hour_utc, hr_per_day);  // hours
    double const lmst = fmod(gmst + longitude * deg_to_hr, hr_per_day);            // hours

    // Convert to local coordinates
    double const lha = fmod(lmst * hr_to_deg - ra, deg_per_rev);  // degrees

    double const zen = acos_deg(sin_deg(dec) * sin_deg(lat) +
                                cos_deg(dec) * cos_deg(lat) * cos_deg(lha));  // degrees

    double az = asin_deg(-cos_deg(dec) * sin_deg(lha) / cos_deg(90.0 - zen));  // degrees

    // Make sure azimuth is in the correct quadrant
    double const el = 90.0 - zen;                                      // degrees
    double const el_critical = asin_deg(sin_deg(dec) / sin_deg(lat));  // degrees

    if (el >= el_critical) {
        az = 180.0 - az;  // degrees
    } else if (lha > 0) {
        az = 360.0 + az;  // degrees
    }

    // Determine the cosine of the zenith angle
    double const cos_zen = cos_deg(zen);  // dimensionless

    // Update the output pointers
    update(cosine_zenith_angle_op, cos_zen);
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
    update(solar_zenith_angle_op, zen);
    update(solar_azimuth_angle_op, az);
}

}  // namespace standardBML
#endif
