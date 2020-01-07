#ifndef SENESCENCE_COEFFICIENT_LOGISTIC_H
#define SENESCENCE_COEFFICIENT_LOGISTIC_H

#include <cmath>
#include "../modules.h"

/**
 * \brief
 */

class senescence_coefficient_logistic : public SteadyModule {
	public:
		senescence_coefficient_logistic(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("senescence_coefficient_logistic"),
			// Get pointers to input parameters
            DVI_ip(get_ip(input_parameters, "DVI")),
            alphaSeneStem_ip(get_ip(input_parameters, "alphaSeneStem")),
            alphaSeneLeaf_ip(get_ip(input_parameters, "alphaSeneLeaf")),
            betaSeneStem_ip(get_ip(input_parameters, "betaSeneStem")),
            betaSeneLeaf_ip(get_ip(input_parameters, "betaSeneLeaf")),
            rateSeneLeaf_ip(get_ip(input_parameters, "rateSeneLeaf")),
            rateSeneStem_ip(get_ip(input_parameters, "rateSeneStem")),
                
			// Get pointers to output parameters
			kSeneStem_op(get_op(output_parameters, "kSeneStem")),
			kSeneLeaf_op(get_op(output_parameters, "kSeneLeaf")),
			kSeneRoot_op(get_op(output_parameters, "kSeneRoot")),
			kSeneRhizome_op(get_op(output_parameters, "kSeneRhizome"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
        const double* DVI_ip;
        const double* alphaSeneStem_ip;
		const double* alphaSeneLeaf_ip;
		const double* betaSeneStem_ip;
		const double* betaSeneLeaf_ip;
        const double* rateSeneLeaf_ip;
        const double* rateSeneStem_ip;
		
		// Pointers to output parameters
		double* kSeneStem_op;
		double* kSeneLeaf_op;
		double* kSeneRoot_op;
		double* kSeneRhizome_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> senescence_coefficient_logistic::get_inputs() {
	return {
		"DVI", "alphaSeneStem", "alphaSeneLeaf",
        "betaSeneStem", "betaSeneLeaf", "rateSeneLeaf", "rateSeneStem"
	};
}

std::vector<std::string> senescence_coefficient_logistic::get_outputs() {
	return {
		"kSeneStem", "kSeneLeaf", "kSeneRoot", "kSeneRhizome"
	};
}

void senescence_coefficient_logistic::do_operation() const {
    double const DVI = *DVI_ip; // dimensionless; development index
    double rateSeneLeaf = *rateSeneLeaf_ip; // maximum percentage of leaf senesced at a given timestep, dimensionless
    double rateSeneStem = *rateSeneStem_ip; // maximum percentage of stem senesced at a give time step, dimensionless
    
	double kSeneStem, kSeneLeaf, kSeneRoot, kSeneRhizome;
    
    // Calculate coefficients representing fraction of biomass senesced for each component. Coefficents are represented as a logistic function depending on thermal time
    
    kSeneStem = rateSeneStem / (1.0 + exp((*alphaSeneStem_ip + *betaSeneStem_ip * DVI))); // dimensionless; fraction of stem senecsed at current time step
    kSeneLeaf = rateSeneLeaf / (1.0 + exp((*alphaSeneLeaf_ip + *betaSeneLeaf_ip * DVI))); // dimensionless; fraction of leaf senesced at current time step
    kSeneRoot = 0.0; // dimensionless; fraction of root senesced at current time step, not currently including root senescence
    kSeneRhizome = 0.0; // dimensionless; fraction of rhizome senesced at current time step, not currently including rhizome senescence
    
	// Update the output parameters
	update(kSeneStem_op, kSeneStem); // dimensionless
	update(kSeneLeaf_op, kSeneLeaf); // dimensionless
	update(kSeneRoot_op, kSeneRoot); // dimensionless
	update(kSeneRhizome_op, kSeneRhizome); // dimensionless
	
}

#endif
