#ifndef CIRCADIAN_CLOCK_CALCULATOR2_H
#define CIRCADIAN_CLOCK_CALCULATOR2_H

#include "../modules.h"

class circadian_clock_calculator2 : public SteadyModule {
	public:
		circadian_clock_calculator2(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("circadian_clock_calculator2"),
			// Get pointers to input parameters
			phi_ip(get_ip(input_parameters, "phi")),
			solar_ip(get_ip(input_parameters, "solar")),
			// Get pointers to output parameters
			R_op(get_op(output_parameters, "R")),
			F_op(get_op(output_parameters, "F")),
			omega_op(get_op(output_parameters, "omega"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* phi_ip;
		const double* solar_ip;
		// Pointers to output parameters
		double* R_op;
		double* F_op;
		double* omega_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> circadian_clock_calculator2::get_inputs() {
	return {
		"phi",
		"solar"
	};
}

std::vector<std::string> circadian_clock_calculator2::get_outputs() {
	return {
		"R",
		"F",
		"omega"
	};
}

void circadian_clock_calculator2::do_operation() const {
	//////////////////////////////////////////
	// Collect inputs and make calculations //
	//////////////////////////////////////////
	
	// Get the current phi value
	double phi = *phi_ip;
	
	// Get the current solar value
	double solar = *solar_ip;
	
	// Check whether the plant is illuminated
	// Rather than simply using a step function (i.e., light = solar > 0),
	//  this logistic function smoothly turns on as the solar radiation
	//  increases
	// Coefficients have been chosen so that light:
	//  (1) roughly equals exp(-10) for solar = 0
	//  (2) roughly equals 1 - exp(-10) for solar = 40
	double light = 1.0 / (1.0 + exp(-0.5 * (solar - 20.0)));
	
	// Define the parameters for the response function
	
	/*
	double d = 2.0 * 3.14159265358979323846264338327950 / 3.0;			// Dead zone width is 1/3 of a period
	double delta = 2.0 * 3.14159265358979323846264338327950 / 12.0;		// Asymmetry is 1/12 of a period
	double scale = 0.04;												// Scaling factor
	*/
	
	/*
	double d = 0.0;														// Minimal dead zone
	double delta = 0.0;													// No asymmetry
	double scale = 0.01;												// Scaling factor
	*/
	
	/*
	double d = 2.0 * 3.14159265358979323846264338327950 / 12.0;			// Dead zone width is 1/12 of a period (2 hours)
	double delta = 0.0;													// No asymmetry
	double scale = 0.02;												// Scaling factor
	*/
	
	double d = 2.0 * 3.14159265358979323846264338327950 / 4.0;			// Dead zone width is 1/4 of a period (6 hours)
	double delta = -2.0 * 3.14159265358979323846264338327950 / 24.0;	// Small asymmetry, making the morning response narrower and sharper
	double scale = 1.1;													// Scaling factor
	
	// Define the natural rate
	double omega_0 = 2.0 * 3.14159265358979323846264338327950 / 25.0;
	
	// Determine a and b for the response function
	double a = 3.14159265358979323846264338327950 - d / 2.0 + delta;
	double b = 3.14159265358979323846264338327950 - d / 2.0 - delta;
	
	// Bring phi back to the [0, 2*pi) range
	phi -= 2.0 * 3.14159265358979323846264338327950 * floor(phi / (2.0 * 3.14159265358979323846264338327950));
	
	// Calculate the response
	double R = scale;
	if(0.0 <= phi && phi < a) R *= -6.0 * phi * (phi - a) / (a * a * a);
	else if(phi < 2.0 * 3.14159265358979323846264338327950 - b) R *= 0.0;
	else if(phi < 2.0 * 3.14159265358979323846264338327950) R *= 6.0 * (phi - 2.0 * 3.14159265358979323846264338327950) * (phi - 2.0 * 3.14159265358979323846264338327950 + b) / (b * b * b);
	else throw std::logic_error(std::string("Thrown by circadian_clock_calculator2: something is wrong with phi!\n"));
	
	//////////////////////////////////////
	// Update the output parameter list //
	//////////////////////////////////////
	
	update(R_op, R);
	//update(F_op, R*solar);
	update(F_op, R*light);
	//update(omega_op, omega_0 * (1 + R*solar));
	update(omega_op, omega_0 * (1 + R*light));
}

#endif
