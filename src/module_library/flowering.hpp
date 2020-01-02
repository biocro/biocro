#ifndef FLOWERING_H
#define FLOWERING_H

#include "../modules.h"

class flowering : public DerivModule {
	public:
		flowering(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			DerivModule("flowering"),
			// Get pointers to input parameters
			solar_ip(get_ip(input_parameters, "solar")),
			doy_dbl_ip(get_ip(input_parameters, "doy_dbl")),
			dawn_ip(get_ip(input_parameters, "dawn")),
			GI_on_ip(get_ip(input_parameters, "GI_on")),
			GI_off_ip(get_ip(input_parameters, "GI_off")),
			FKF1_on_ip(get_ip(input_parameters, "FKF1_on")),
			FKF1_off_ip(get_ip(input_parameters, "FKF1_off")),
			CDF_on_ip(get_ip(input_parameters, "CDF_on")),
			CDF_off_ip(get_ip(input_parameters, "CDF_off")),
			FT_on_ip(get_ip(input_parameters, "FT_on")),
			FT_off_ip(get_ip(input_parameters, "FT_off")),
			FT_inhibition_ip(get_ip(input_parameters, "FT_inhibition")),
			FKF1_timing_ip(get_ip(input_parameters, "FKF1_timing")),
			CDF_timing_ip(get_ip(input_parameters, "CDF_timing")),
			GI_ip(get_ip(input_parameters, "GI")),
			FKF1_ip(get_ip(input_parameters, "FKF1")),
			CDF_ip(get_ip(input_parameters, "CDF")),
			FT_ip(get_ip(input_parameters, "FT")),
			// Get pointers to output parameters
			GI_op(get_op(output_parameters, "GI")),
			FKF1_op(get_op(output_parameters, "FKF1")),
			CDF_op(get_op(output_parameters, "CDF")),
			FT_op(get_op(output_parameters, "FT"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* solar_ip;
		const double* doy_dbl_ip;
		const double* dawn_ip;
		const double* GI_on_ip;
		const double* GI_off_ip;
		const double* FKF1_on_ip;
		const double* FKF1_off_ip;
		const double* CDF_on_ip;
		const double* CDF_off_ip;
		const double* FT_on_ip;
		const double* FT_off_ip;
		const double* FT_inhibition_ip;
		const double* FKF1_timing_ip;
		const double* CDF_timing_ip;
		const double* GI_ip;
		const double* FKF1_ip;
		const double* CDF_ip;
		const double* FT_ip;
		// Pointers to output parameters
		double* GI_op;
		double* FKF1_op;
		double* CDF_op;
		double* FT_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> flowering::get_inputs() {
	return {
		"solar",
		"doy_dbl",
		"dawn",
		"GI_on",
		"GI_off",
		"FKF1_on",
		"FKF1_off",
		"CDF_on",
		"CDF_off",
		"FT_on",
		"FT_off",
		"FT_inhibition",
		"FKF1_timing",
		"CDF_timing",
		"GI",
		"FKF1",
		"CDF",
		"FT"
	};
}

std::vector<std::string> flowering::get_outputs() {
	return {
		"GI",
		"FKF1",
		"CDF",
		"FT"
	};
}

void flowering::do_operation() const {	
	// Collect inputs and make calculations
	double solar = *solar_ip;
	double doy_dbl = *doy_dbl_ip;
	double dawn = *dawn_ip;	// In hours; a value of 100 means it's currently night. Other values indicate dawn for that day.
	double GI_on = *GI_on_ip;
	double GI_off = *GI_off_ip;
	double FKF1_on = *FKF1_on_ip;
	double FKF1_off = *FKF1_off_ip;
	double CDF_on = *CDF_on_ip;
	double CDF_off = *CDF_off_ip;
	double FT_on = *FT_on_ip;
	double FT_off = *FT_off_ip;
	double FT_inhibition = *FT_inhibition_ip;
	double FKF1_timing = *FKF1_timing_ip;
	double CDF_timing = *CDF_timing_ip;
	double GI = *GI_ip;
	double FKF1 = *FKF1_ip;
	double CDF = *CDF_ip;
	double FT = *FT_ip;
	
	// Get the hour from the fractional part of doy_dbl
	double hour = 24.0 * (doy_dbl - floor(doy_dbl));
	
	// Initialize the derivatives
	double d_GI, d_FKF1, d_CDF, d_FT;
	
	// FKF1 starts being produced at a rate proportional to the solar radiation strength
	//  a certain number of hours after dawn (set by FKF1_timing)
	// FKF1 also has an exponential decay term that is always present, regardless of
	//  sunlight or dawn
	if(hour > dawn + FKF1_timing) d_FKF1 = FKF1_on * solar - FKF1_off * FKF1;
	else d_FKF1 = - FKF1_off * FKF1;
	
	// GI starts being produced at a rate proportional to the solar radiation strength
	//  three hours after FKF1 starts being produced (hard-coded)
	// GI also has an exponential decay term that is always present, regardless of
	//  sunlight or dawn
	if(hour > dawn + FKF1_timing + 3.0) d_GI = GI_on * solar - GI_off * GI;
	else d_GI = - GI_off * GI;
	
	// CDF starts being produced at a rate proportional to the solar radiation strength
	//  a certain number of hours after dawn (set by CDF_timing)
	// CDF has an exponential decay term that is always present and is accelerated by
	//  the simultaneous presence of FKF1 and GI
	if(hour > dawn + CDF_timing) d_CDF = CDF_on * solar - CDF_off * (1 + GI * FKF1) * CDF;
	else d_CDF = - CDF_off * (1 + GI * FKF1) * CDF;
	
	// FT is produced at a constant rate in the absence of CDF, but the CDF inhibits FT production
	//  by decreasing this rate
	// FT has an exponential decay term that is always present
	d_FT = FT_on / (1 + FT_inhibition * CDF) - FT_off * FT;
	
	// Update the output parameter list
	update(GI_op, d_GI);
	update(FKF1_op, d_FKF1);
	update(CDF_op, d_CDF);
	update(FT_op, d_FT);
}

#endif