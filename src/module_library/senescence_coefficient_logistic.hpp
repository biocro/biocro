#ifndef SENESCENCE_COEFFICIENT_LOGISTIC_H
#define SENESCENCE_COEFFICIENT_LOGISTIC_H

#include <math.h>
#include "../modules.h"

class senescence_coefficient_logistic : public SteadyModule {
	public:
		senescence_coefficient_logistic(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
			SteadyModule("senescence_coefficient_logistic"),
			// Get pointers to input parameters
//			TTemr_ip(get_ip(input_parameters, "TTemr")),
//            TTveg_ip(get_ip(input_parameters, "TTveg")),
//            TTrep_ip(get_ip(input_parameters, "TTrep")),
            DVI_ip(get_ip(input_parameters, "DVI")),
            alphaSeneStem_ip(get_ip(input_parameters, "alphaSeneStem")),
            alphaSeneLeaf_ip(get_ip(input_parameters, "alphaSeneLeaf")),
            betaSeneStem_ip(get_ip(input_parameters, "betaSeneStem")),
            betaSeneLeaf_ip(get_ip(input_parameters, "betaSeneLeaf")),
//            TTc_ip(get_ip(input_parameters, "TTc")),
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
//		const double* TTemr_ip;
//		const double* TTveg_ip;
//		const double* TTrep_ip;
        const double* DVI_ip;
        const double* alphaSeneStem_ip;
		const double* alphaSeneLeaf_ip;
		const double* betaSeneStem_ip;
		const double* betaSeneLeaf_ip;
//		const double* TTc_ip;
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
//	double TTc = *TTc_ip; // Thermal time accumulated, degrees C * day
//	double TTemr = *TTemr_ip; // Thermal time until emergence, degrees C * day
//	double TTveg = *TTveg_ip; // Thermal time of vegetative stage, degrees C * day
//	double TTrep = *TTrep_ip; // Thermal time of reproductive stage, degrees C * day
    double const DVI = *DVI_ip; // dimensionless; development index
    double rateSeneLeaf = *rateSeneLeaf_ip; // maximum percentage of leaf senesced at a given timestep, dimensionless
    double rateSeneStem = *rateSeneStem_ip; // maximum percentage of stem senesced at a give time step, dimensionless
    
	double kSeneStem, kSeneLeaf, kSeneRoot, kSeneRhizome;
    
//    // Determine the DVI (development index) based on the accumulated thermal time
//    // -1<=DVI<0 is before crop emergence, 0<=DVI<1 is during the vegetative stage, and 1<=DVI<2 is the reproductive stage
//    // DVI=2 is considered the end of the growing season
//    // from Osborne et al., 2015 JULES-crop doi:10.5194/gmd-8-1139-2015
//    if (0.0 <= TTc && TTc < TTemr) {
//        DVI = -1.0 + TTc / TTemr; //dimensionless
//    }
//    else if (TTemr <= TTc && TTc < (TTveg + TTemr)) {
//        DVI = (TTc - TTemr) / TTveg; //dimensionless
//    }
//    else if ((TTveg + TTemr) <= TTc) {
//        DVI = 1.0 + (TTc - TTemr - TTveg) / TTrep; //dimensionless
//    }
//    else DVI = 5.0;
// //        throw std::out_of_range(std::string("TTc not in range, thrown by senescence_coefficient_logistic module.\n"));
//
    // Calculate coefficients representing fraction of biomass senesced for each component. Coefficents are represented as a logistic function depending on thermal time
    
    kSeneStem = rateSeneStem / (1.0 + exp((*alphaSeneStem_ip + *betaSeneStem_ip * DVI))); // fraction of stem senecsed at current time step, dimensionless
    kSeneLeaf = rateSeneLeaf / (1.0 + exp((*alphaSeneLeaf_ip + *betaSeneLeaf_ip * DVI))); // fraction of leaf senesced at current time step, dimensionless
    kSeneRoot = 0.0; // fraction of root senesced at current time step, dimensionless, not currently including root senescence - mlm
    kSeneRhizome = 0.0; // fraction of rhizome senesced at current time step, dimensionless, not currently including rhizome senescence - mlm
    
	// Update the output parameters
	update(kSeneStem_op, kSeneStem); // dimensionless
	update(kSeneLeaf_op, kSeneLeaf); // dimensionless
	update(kSeneRoot_op, kSeneRoot); // dimensionless
	update(kSeneRhizome_op, kSeneRhizome); // dimensionless
	
}

#endif
