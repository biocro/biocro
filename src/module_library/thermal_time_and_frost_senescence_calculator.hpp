#ifndef THERMAL_TIME_AND_FROST_SENESCENCE_CALCULATOR_H
#define THERMAL_TIME_AND_FROST_SENESCENCE_CALCULATOR_H

#include "../modules.h"

class thermal_time_and_frost_senescence_calculator : public SteadyModule {
	public:
		thermal_time_and_frost_senescence_calculator(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("thermal_time_and_frost_senescence_calculator"),
			// Get pointers to input parameters
			TTc_ip(get_ip(input_parameters, "TTc")),
			seneLeaf_ip(get_ip(input_parameters, "seneLeaf")),
			lat_ip(get_ip(input_parameters, "lat")),
			doy_dbl_ip(get_ip(input_parameters, "doy_dbl")),
			leafdeathrate_ip(get_ip(input_parameters, "leafdeathrate")),
			temp_ip(get_ip(input_parameters, "temp")),
			Tfrostlow_ip(get_ip(input_parameters, "Tfrostlow")),
			Tfrosthigh_ip(get_ip(input_parameters, "Tfrosthigh")),
			// Get pointers to output parameters
			leafdeathrate_op(get_op(output_parameters, "leafdeathrate"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
		const double* TTc_ip;
		const double* seneLeaf_ip;
		const double* lat_ip;
		const double* doy_dbl_ip;
		const double* leafdeathrate_ip;
		const double* temp_ip;
		const double* Tfrostlow_ip;
		const double* Tfrosthigh_ip;
		// Pointers to output parameters
		double* leafdeathrate_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> thermal_time_and_frost_senescence_calculator::get_inputs() {
	return {
		"TTc",
		"seneLeaf",
		"lat", 
		"doy_dbl", 
		"leafdeathrate",
		"temp", 
		"Tfrostlow",
		"Tfrosthigh"
	};
}

std::vector<std::string> thermal_time_and_frost_senescence_calculator::get_outputs() {
	return {
		"leafdeathrate"
	};
}

void thermal_time_and_frost_senescence_calculator::do_operation() const {	
	// Collect inputs and make calculations
	double TTc = *TTc_ip;
	double seneLeaf = *seneLeaf_ip;
	double lat = *lat_ip;
	double doy_dbl = *doy_dbl_ip;
	double leafdeathrate = *leafdeathrate_ip;
	double temp = *temp_ip;
	double Tfrostlow = *Tfrostlow_ip;
	double Tfrosthigh = *Tfrosthigh_ip;
	
	double frost_leaf_death_rate = 0.0;
	
	if(TTc >= seneLeaf) {	// Leaf senescence has started
		bool A = lat >= 0.0;		// In Northern hemisphere
		bool B = doy_dbl >= 180.0;	// In second half of the year
		if((A && B) || ((!A) && (!B))) {	// Winter in either hemisphere
			if(temp > Tfrostlow) {
				// frost_leaf_death_rate changes linearly from 100 to 0 as temp changes from Tfrostlow to Tfrosthigh
				frost_leaf_death_rate = 100 * (temp - Tfrosthigh) / (Tfrostlow - Tfrosthigh);
				frost_leaf_death_rate = (frost_leaf_death_rate > 100.0) ? 100.0 : frost_leaf_death_rate;	// Is this right? I don't think the death rate can exceed 100, but it CAN become negative. (EBL)
			}
			else frost_leaf_death_rate = 0.0;	// No frost death rate when the temperature is too low?
		}
	}
	
	// The current leaf death rate is the larger of the previous leaf death rate and the frost death rate...
	//  why do we want to do this? (EBL)
	double current_leaf_death_rate = (leafdeathrate > frost_leaf_death_rate) ? leafdeathrate : frost_leaf_death_rate;
	
	// From original:
	// derivs["leafdeathrate"] = current_leaf_death_rate - leafdeathrate;	// Why would we store this difference? (EBL)
	
	// Update the output parameter list
	update(leafdeathrate_op, current_leaf_death_rate);
}

#endif