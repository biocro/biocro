#ifndef SOLAR_ZENITH_ANGLE_H
#define SOLAR_ZENITH_ANGLE_H

#include <cmath>
#include "../constants.h"
#include "../modules.h"

class solar_zenith_angle : public SteadyModule {
	public:
		solar_zenith_angle(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("solar_zenith_angle"),
			// Get pointers to input parameters
			lat_ip(get_ip(input_parameters, "lat")),
			doy_dbl_ip(get_ip(input_parameters, "doy_dbl")),
			// Get pointers to output parameters
			cosine_zenith_angle_op(get_op(output_parameters, "cosine_zenith_angle"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
		static std::string get_description();
	private:
		// Pointers to input parameters
		const double* lat_ip;
		const double* doy_dbl_ip;
		// Pointers to output parameters
		double* cosine_zenith_angle_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> solar_zenith_angle::get_inputs() {
	return {
		"lat",
		"doy_dbl"
	};
}

std::vector<std::string> solar_zenith_angle::get_outputs() {
	return {
		"cosine_zenith_angle"
	};
}

std::string solar_zenith_angle::get_description() {
	std::string description = std::string("  Calculates the sun's zenith angle in the sky based on the latitude, day, and time.\n");
	description += std::string("  Note: the doy_dbl variable encodes the DOY as the integer part and the time as the decimal part, i.e.,\n");
	description += std::string("  doy_dbl = DOY + hour/24");
	return description;
}

void solar_zenith_angle::do_operation() const {

    using math_constants::pi;

	// Collect inputs and make calculations
	const double lat = *lat_ip;
	const double doy_dbl = *doy_dbl_ip;
	
	// Unpack the doy and hour
	const double doy = floor(doy_dbl);
	const double hour = 24.0 * (doy_dbl - doy);
	
	// Convert latitude to radians (=phi)
	const double phi = lat * pi / 180.0;
	
	// Determine the number of days since the December solstice (=NDS)
	//  January 1 => DOY = 0
	//  December 21 => DOY = 355
	// We assume 365 days per year with solstice on December 21
	const double NDS = doy + 10.0;
	
	// Find the angular position of the Earth relative to the Sun in
	//   radians (=omega), where omega = 0 corresponds to the winter solstice
	const double omega = (NDS / 365.0) * (2 * pi);
	
	// Find the sun's declination in radians (=delta) using the Earth's axial tilt (23.5 degrees)
	constexpr double axial_tilt = 23.5 * pi / 180.0;
	const double delta = -axial_tilt * cos(omega);
	
	// Find the hour angle in radians (=tau), noting that the Earth rotates once every
	//  24 hours, i.e., turns at a rate of 2 * pi / 24 = pi / 12 radians per hour
	// Here we don't apply any corrections to calculate noon and assume it occurs at 12:00,
	//  in contrast to equation 11.3 in Campbell & Norman (1998)
	constexpr int solar_noon = 12;
	const double tau = (hour - solar_noon) * pi / 12.0;
	
	// Find the cosine of the solar zenith angle using a straight-forward application
	//  of the law of cosines for spherical triangles, substituting cofunctions of coangles
	//  in the case of latitude and declination
	// See, e.g., equation 11.1 in Campbell & Norman (1998)
	const double cosine_zenith_angle = sin(delta) * sin(phi) + cos(delta) * cos(phi) * cos(tau);
	
	// Update the output parameter list
	update(cosine_zenith_angle_op, cosine_zenith_angle);
}

#endif
