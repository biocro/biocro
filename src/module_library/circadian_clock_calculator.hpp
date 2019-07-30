#ifndef CIRCADIAN_CLOCK_CALCULATOR_H
#define CIRCADIAN_CLOCK_CALCULATOR_H

#include "../modules.h"

class circadian_clock_calculator : public SteadyModule {
	public:
		circadian_clock_calculator(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("circadian_clock_calculator"),
			// Get pointers to input parameters
			solar_ip(get_ip(input_parameters, "solar")),
			dawn_x_ip(get_ip(input_parameters, "dawn_x")),
			dawn_v_ip(get_ip(input_parameters, "dawn_v")),
			dusk_x_ip(get_ip(input_parameters, "dusk_x")),
			dusk_v_ip(get_ip(input_parameters, "dusk_v")),
			ref_x_ip(get_ip(input_parameters, "ref_x")),
			ref_v_ip(get_ip(input_parameters, "ref_v")),
			allday_ip(get_ip(input_parameters, "allday")),
			// Get pointers to output parameters
			light_op(get_op(output_parameters, "light")),
			dawn_phase_op(get_op(output_parameters, "dawn_phase")),
			dusk_phase_op(get_op(output_parameters, "dusk_phase")),
			ref_phase_op(get_op(output_parameters, "ref_phase")),
			afterdawn_op(get_op(output_parameters, "afterdawn")),
			FTmRNA_op(get_op(output_parameters, "FTmRNA"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* solar_ip;
		const double* dawn_x_ip;
		const double* dawn_v_ip;
		const double* dusk_x_ip;
		const double* dusk_v_ip;
		const double* ref_x_ip;
		const double* ref_v_ip;
		const double* allday_ip;
		// Pointers to output parameters
		double* light_op;
		double* dawn_phase_op;
		double* dusk_phase_op;
		double* ref_phase_op;
		double* afterdawn_op;
		double* FTmRNA_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> circadian_clock_calculator::get_inputs() {
	return {
		"solar",
		"dawn_x",
		"dawn_v",
		"dusk_x",
		"dusk_v",
		"ref_x",
		"ref_v",
		"allday"
	};
}

std::vector<std::string> circadian_clock_calculator::get_outputs() {
	return {
		"light",
		"dawn_phase",
		"dusk_phase",
		"ref_phase",
		"afterdawn",
		"FTmRNA"
	};
}

void circadian_clock_calculator::do_operation() const {
	//////////////////////////////////////////
	// Collect inputs and make calculations //
	//////////////////////////////////////////
	
	// Get the current level of solar radiation
	double solar = *solar_ip;
	
	// Get the current state of the dawn and dusk tracking oscillators
	double dawn_x = *dawn_x_ip;
	double dawn_v = *dawn_v_ip;
	double dusk_x = *dusk_x_ip;
	double dusk_v = *dusk_v_ip;
	double ref_x = *ref_x_ip;
	double ref_v = *ref_v_ip;
	
	// Get the current state of the allday function
	double allday = *allday_ip;
	
	// Check whether the plant is illuminated
	// Rather than simply using a step function (i.e., light = solar > 0),
	//  this logistic function smoothly turns on as the solar radiation
	//  increases
	// Coefficients have been chosen so that light:
	//  (1) roughly equals exp(-10) for solar = 0
	//  (2) roughly equals 1 - exp(-10) for solar = 40
	double light = 1.0 / (1.0 + exp(-0.5 * (solar - 20.0)));
	
	// Calculate the dawn phase angle, which is zero around dawn
	//  and increases throughout the day
	double dawn_phase = atan2(dawn_x, dawn_v);					// Output lies within [-pi,pi]
	if(dawn_phase < 0) dawn_phase += 2*3.14159265358979323846;	// Change output domain to [0,2*pi)
	
	// Calculate the dusk phase angle, which is zero around dusk
	//  and increases throughout the night
	double dusk_phase = atan2(dusk_x, dusk_v);					// Output lies within [-pi,pi]
	if(dusk_phase < 0) dusk_phase += 2*3.14159265358979323846;	// Change output domain to [0,2*pi)
	
	// Calculate the reference phase angle
	double ref_phase = atan2(ref_x, ref_v);						// Output lies within [-pi,pi]
	if(ref_phase < 0) ref_phase += 2*3.14159265358979323846;	// Change output domain to [0,2*pi)
	
	// Calculate the afterdawn function, which peaks a certain number of hours after dawn
	//  This behavior is implemented with a Gaussian peaked at dawn_phase = 3.0
	//  This value is chosen so that afterdawn peaks in sunlight on long days
	//   but after sunset on short days
	double afterdawn = 4.0 * exp(-(dawn_phase - 3.0)*(dawn_phase - 3.0)/(0.3*0.3));
	
	// Calculate the GI-FKF1 complex function, which is simply based on the amount of
	//  afterdawn (FKF1 protein), allday (GI protein), and light
	// double FTmRNA = allday * afterdawn / 4.0;
	double FTmRNA = allday * afterdawn * light / 2.0;
	
	//////////////////////////////////////
	// Update the output parameter list //
	//////////////////////////////////////
	
	update(light_op, light);
	update(dawn_phase_op, dawn_phase);
	update(dusk_phase_op, dusk_phase);
	update(ref_phase_op, ref_phase);
	update(afterdawn_op, afterdawn);
	update(FTmRNA_op, FTmRNA);
}

#endif
