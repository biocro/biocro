#ifndef CIRCADIAN_CLOCK_H
#define CIRCADIAN_CLOCK_H

#include "../modules.h"

class circadian_clock : public DerivModule {
	public:
		circadian_clock(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("circadian_clock"),
			// Get pointers to input parameters
			light_ip(get_ip(input_parameters, "light")),
			night_tracker_ip(get_ip(input_parameters, "night_tracker")),
			day_tracker_ip(get_ip(input_parameters, "day_tracker")),
			dawn_x_ip(get_ip(input_parameters, "dawn_x")),
			dawn_v_ip(get_ip(input_parameters, "dawn_v")),
			dusk_x_ip(get_ip(input_parameters, "dusk_x")),
			dusk_v_ip(get_ip(input_parameters, "dusk_v")),
			ref_x_ip(get_ip(input_parameters, "ref_x")),
			ref_v_ip(get_ip(input_parameters, "ref_v")),
			dawn_phase_ip(get_ip(input_parameters, "dawn_phase")),
			dusk_phase_ip(get_ip(input_parameters, "dusk_phase")),
			allday_ip(get_ip(input_parameters, "allday")),
			doy_dbl_ip(get_ip(input_parameters, "doy_dbl")),
			// Get pointers to output parameters
			night_tracker_op(get_op(output_parameters, "night_tracker")),
			day_tracker_op(get_op(output_parameters, "day_tracker")),
			dawn_x_op(get_op(output_parameters, "dawn_x")),
			dawn_v_op(get_op(output_parameters, "dawn_v")),
			dusk_x_op(get_op(output_parameters, "dusk_x")),
			dusk_v_op(get_op(output_parameters, "dusk_v")),
			ref_x_op(get_op(output_parameters, "ref_x")),
			ref_v_op(get_op(output_parameters, "ref_v")),
			allday_op(get_op(output_parameters, "allday"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* light_ip;
		const double* night_tracker_ip;
		const double* day_tracker_ip;
		const double* dawn_x_ip;
		const double* dawn_v_ip;
		const double* dusk_x_ip;
		const double* dusk_v_ip;
		const double* ref_x_ip;
		const double* ref_v_ip;
		const double* dawn_phase_ip;
		const double* dusk_phase_ip;
		const double* allday_ip;
		const double* doy_dbl_ip;
		// Pointers to output parameters
		double* night_tracker_op;
		double* day_tracker_op;
		double* dawn_x_op;
		double* dawn_v_op;
		double* dusk_x_op;
		double* dusk_v_op;
		double* ref_x_op;
		double* ref_v_op;
		double* allday_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> circadian_clock::get_inputs() {
	return {
		"light",
		"night_tracker",
		"day_tracker",
		"dawn_x",
		"dawn_v",
		"dusk_x",
		"dusk_v",
		"ref_x",
		"ref_v",
		"dawn_phase",
		"dusk_phase",
		"allday",
		"doy_dbl"
	};
}

std::vector<std::string> circadian_clock::get_outputs() {
	return {
		"night_tracker",
		"day_tracker",
		"dawn_x",
		"dawn_v",
		"dusk_x",
		"dusk_v",
		"ref_x",
		"ref_v",
		"allday"
	};
}

void circadian_clock::do_operation() const {
	//////////////////////////////////////////
	// Collect inputs and make calculations //
	//////////////////////////////////////////
	
	// Get the doy
	double doy_dbl = *doy_dbl_ip;
	
	// Get the current light value
	double light = *light_ip;
	//if(doy_dbl > 190.5) light = 1.0;
	
	// Get the current values of the night and day trackers
	double night_tracker = *night_tracker_ip;
	double day_tracker = *day_tracker_ip;
	
	// Get the current state of the dawn tracking oscillator
	double dawn_x = *dawn_x_ip;
	double dawn_v = *dawn_v_ip;
	
	// Get the current state of the dusk tracking oscillator
	double dusk_x = *dusk_x_ip;
	double dusk_v = *dusk_v_ip;
	
	// Get the current state of the reference oscillator
	double ref_x = *ref_x_ip;
	double ref_v = *ref_v_ip;
	
	// Define the constants related to the night and day trackers
	// The tracker rate was chosen so that exp(-tracker_rate * 1 hour) = 0.01
	double tracker_rate = 4.6;
	
	// Define the constants for the tracking oscillators
	double period_0 = 25.0;										// 25 hour natural period
	double omega_0 = 2.0 * 3.14159265358979323846 / period_0;	// Corresponding angular frequency
	//double kick_strength;
	//if(doy_dbl > 190) kick_strength = 0.0;
	//else kick_strength = 8.0;
	double kick_strength = 8.0;
	//double kick_strength = 2.0;
	
	/*
	// Calculate the damping coefficients based on the current oscillator energies
	double base_damping = 0.1;
	double dawn_damping = base_damping * (dawn_v * dawn_v + dawn_x * dawn_x - 1.0);
	double dusk_damping = base_damping * (dusk_v * dusk_v + dusk_x * dusk_x - 1.0);
	double ref_damping = base_damping * (ref_v * ref_v + ref_x * ref_x - 1.0);
	*/
	
	// Calculate the damping coefficients based on the current oscillator energies
	double base_damping = 0.1;
	double dawn_damping = base_damping * (sqrt(dawn_v * dawn_v + dawn_x * dawn_x) - 1.0);
	double dusk_damping = base_damping * (sqrt(dusk_v * dusk_v + dusk_x * dusk_x) - 1.0);
	double ref_damping = base_damping * (sqrt(ref_v * ref_v + ref_x * ref_x) - 1.0);
	
	// Get the current values of the dusk and dawn phases
	double dawn_phase = *dawn_phase_ip;
	double dusk_phase = *dusk_phase_ip;
	
	// Get the current value of the allday function
	double allday = *allday_ip;
	
	// Define the constant for the allday function
	double allday_decay = 1.0;
	
	//////////////////////////////////////
	// Update the output parameter list //
	//////////////////////////////////////
	
	/*
	// The night tracker is produced at a constant rate in the darkness and decays exponentially in the light
	// The day tracker is produced at a constant rate in the light and decays exponentially in the dark
	// The constants have been chosen so that:
	//   (1) The night (day) tracker fully decays during the day (night)
	//   (2) The night (day) tracker counts the number of hours since sunset (sunrise) during the night (day)
	// They won't be accurate for the first day of a simulation but will quickly reset
	// These quantities were inspired by the light-sensitive protein P in
	//  Locke at. al, Journal of Theoretical Biology 234, 383–393 (2005)
	update(night_tracker_op, (1.0 - light) * tracker_growth_rate - light * tracker_decay_rate * night_tracker);
	update(day_tracker_op, light * tracker_growth_rate - (1.0 - light) * tracker_decay_rate * day_tracker);
	*/
	
	// The day and night trackers are produced at a light-dependent rate and decay exponentially independent
	//  of the light level
	// The constants have been chosen so that:
	//  (1) The trackers quickly reach their equilibrium values (within 1% after 1 hour of light)
	//  (2) Each tracker takes values between 0 and 1
	// These quantities were inspired by the light-sensitive protein P in
	//  Locke at. al, Journal of Theoretical Biology 234, 383–393 (2005)
	update(night_tracker_op, tracker_rate * ((1.0 - light) - night_tracker));
	update(day_tracker_op, tracker_rate * (light - day_tracker));
	
	// The dawn tracking oscillator is driven by a small kick at dawn
	// The kick is created by taking the night tracker value during the day,
	//  which is just a short decay portion
	// The damping term hastens the decay of transients in the signal
	
	/*
	update(dawn_x_op, omega_0 * dawn_v);
	update(dawn_v_op, - omega_0 * dawn_x - dawn_damping * dawn_v + light * kick_strength * night_tracker);
	*/
	
	update(dawn_x_op, omega_0 * dawn_v - dawn_damping * dawn_x);
	update(dawn_v_op, - omega_0 * dawn_x - dawn_damping * dawn_v + light * kick_strength * night_tracker);
	
	// The dusk tracking oscillator is driven by a small kick at dusk
	// The kick is created by taking the day tracker value during the night,
	//  which is just a short decay portion
	// The damping term hastens the decay of transients in the signal
	
	/*
	update(dusk_x_op, omega_0 * dusk_v);
	update(dusk_v_op, - omega_0 * dusk_x - dusk_damping * dusk_v + (1.0 - light) * kick_strength * day_tracker);
	*/
	
	update(dusk_x_op, omega_0 * dusk_v - dusk_damping * dusk_x);
	update(dusk_v_op, - omega_0 * dusk_x - dusk_damping * dusk_v + (1.0 - light) * kick_strength * day_tracker);
	
	// The reference oscillator is not driven
	
	/*
	update(ref_x_op, omega_0 * ref_v);
	update(ref_v_op, - omega_0 * ref_x - ref_damping * ref_v);
	*/
	
	update(ref_x_op, omega_0 * ref_v - ref_damping * ref_x);
	update(ref_v_op, - omega_0 * ref_x - ref_damping * ref_v);
	
	
	// The allday function grows starting at dawn and decays starting at dusk
	//  In the morning, growth is controlled by dawn_phase via a Gaussian centered at a low phase value
	//  This ensures that growth levels off in the afternoon
	//  In the evening, an exponential decay is controlled by dusk_phase via another Gaussian
	//  In this case, we first advance the dusk_phase (so decay starts before dusk) and then use the
	//   Gaussian to determine the decay rate
	double dusk_phase_advanced = dusk_phase + 1.0;
	if(dusk_phase_advanced >= 2*3.14159265358979323846) dusk_phase_advanced -= 2*3.14159265358979323846;	// Make sure the phase stays in the correct range
	update(allday_op, exp(-(dawn_phase - 0.75)*(dawn_phase - 0.75)/(0.75*0.75)) - allday_decay * allday * exp(-(dusk_phase_advanced - 1.0)*(dusk_phase_advanced - 1.0)));
}

#endif
