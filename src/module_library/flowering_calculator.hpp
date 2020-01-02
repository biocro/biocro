#ifndef FLOWERING_CALCULATOR_H
#define FLOWERING_CALCULATOR_H

#include "../modules.h"

class flowering_calculator : public SteadyModule {
	public:
		flowering_calculator(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("flowering_calculator"),
			// Get pointers to input parameters
			solar_ip(get_ip(input_parameters, "solar")),
			dawn_ip(get_ip(input_parameters, "dawn")),
			doy_dbl_ip(get_ip(input_parameters, "doy_dbl")),
			// Get pointers to output parameters
			dawn_op(get_op(output_parameters, "dawn"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* solar_ip;
		const double* dawn_ip;
		const double* doy_dbl_ip;
		// Pointers to output parameters
		double* dawn_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> flowering_calculator::get_inputs() {
	return {
		"solar",
		"dawn",
		"doy_dbl"
	};
}

std::vector<std::string> flowering_calculator::get_outputs() {
	return {
		"dawn"
	};
}

void flowering_calculator::do_operation() const {	
	// Collect inputs and make calculations
	double solar = *solar_ip;
	double dawn = *dawn_ip;	// In hours; a value of 100 means it's currently night. Other values indicate dawn for that day.
	double doy_dbl = *doy_dbl_ip;
	
	// Get the hour from the fractional part of doy_dbl
	double hour = 24.0 * (doy_dbl - floor(doy_dbl));
	
	// Determine the new dawn value by detecting dawn and night conditions
	if(solar > 0 && fabs(dawn - 100.0) < eps) {
		// In this case, the previous value of dawn was 100, indicating night, but now solar is nonzero
		// I.e., it is dawn, so update the dawn value for the day
		//dawn = hour - 100.0;	// This was in the original code but doesn't make any sense to me, so I just set dawn = hour (EBL)
		dawn = hour;
	}
	else if(fabs(solar) < eps) {
		// In this case, solar is zero, indicating night
		// So we should change dawn to its night value
		//dawn = 100.0 - dawn;	// This was in the original code but doesn't make any sense to me, so I just set dawn = 100 (EBL)
		dawn = 100.0;
	}
	
	// Update the output parameter list
	update(dawn_op, dawn);
}

#endif