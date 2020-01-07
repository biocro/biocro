#ifndef THERMAL_TIME_SENESCENCE_LOGISTIC_H
#define THERMAL_TIME_SENESCENCE_LOGISTIC_H

#include "../modules.h"

class thermal_time_senescence_logistic : public DerivModule {
	public:
		thermal_time_senescence_logistic(const std::unordered_map<std::string, double>* input_parameters, std::unordered_map<std::string, double>* output_parameters) :
			// Define basic module properties by passing its name to its parent class
            DerivModule("thermal_time_senescence_logistic"),
			// Get pointers to input parameters
            Leaf_ip(get_ip(input_parameters, "Leaf")),
            Stem_ip(get_ip(input_parameters, "Stem")),
            Root_ip(get_ip(input_parameters, "Root")),
            Rhizome_ip(get_ip(input_parameters, "Rhizome")),
			kSeneLeaf_ip(get_ip(input_parameters, "kSeneLeaf")),
			kSeneStem_ip(get_ip(input_parameters, "kSeneStem")),
			kSeneRoot_ip(get_ip(input_parameters, "kSeneRoot")),
			kSeneRhizome_ip(get_ip(input_parameters, "kSeneRhizome")),
            kLeaf_ip(get_ip(input_parameters, "kLeaf")),
			kStem_ip(get_ip(input_parameters, "kStem")),
			kRoot_ip(get_ip(input_parameters, "kRoot")),
			kRhizome_ip(get_ip(input_parameters, "kRhizome")),
			kGrain_ip(get_ip(input_parameters, "kGrain")),
            remobilization_fraction_ip(get_ip(input_parameters, "remobilization_fraction")),
			// Get pointers to output parameters
			Leaf_op(get_op(output_parameters, "Leaf")),
			LeafLitter_op(get_op(output_parameters, "LeafLitter")),
			Stem_op(get_op(output_parameters, "Stem")),
			StemLitter_op(get_op(output_parameters, "StemLitter")),
			Root_op(get_op(output_parameters, "Root")),
			RootLitter_op(get_op(output_parameters, "RootLitter")),
			Rhizome_op(get_op(output_parameters, "Rhizome")),
			RhizomeLitter_op(get_op(output_parameters, "RhizomeLitter")),
			Grain_op(get_op(output_parameters, "Grain"))
		{}
		static std::vector<std::string> get_inputs();
		static std::vector<std::string> get_outputs();
	private:
		// Pointers to input parameters
        const double* Leaf_ip;
        const double* Stem_ip;
        const double* Root_ip;
        const double* Rhizome_ip;
		const double* kSeneLeaf_ip;
		const double* kSeneStem_ip;
		const double* kSeneRoot_ip;
		const double* kSeneRhizome_ip;
        const double* kLeaf_ip;
		const double* kStem_ip;
		const double* kRoot_ip;
		const double* kRhizome_ip;
		const double* kGrain_ip;
        const double* remobilization_fraction_ip;
		// Pointers to output parameters
		double* Leaf_op;
		double* LeafLitter_op;
		double* Stem_op;
		double* StemLitter_op;
		double* Root_op;
		double* RootLitter_op;
		double* Rhizome_op;
		double* RhizomeLitter_op;
		double* Grain_op;
		// Main operation
		void do_operation() const;
};

std::vector<std::string> thermal_time_senescence_logistic::get_inputs() {
	return {
		"Leaf", "Stem", "Root", "Rhizome",
        "kSeneLeaf", "kSeneStem", "kSeneRoot", "kSeneRhizome",
		"kLeaf", "kStem", "kRoot", "kRhizome", "kGrain",
        "remobilization_fraction"
	};
}

std::vector<std::string> thermal_time_senescence_logistic::get_outputs() {
	return {
		"Leaf", "LeafLitter","Stem", "StemLitter",
		"Root", "RootLitter", "Rhizome", "RhizomeLitter", "Grain"
	};
}

void thermal_time_senescence_logistic::do_operation() const {
    // This module calculates the change in plant component biomasses due to senescence.  The amount that each plant component is senesced is determined as a percentage (kSeneLeaf, kSeneStem, kSeneRoot, kSeneRhizome) of its current biomass. Remobilization of senesced leaf tissue is also included based on the partioning growth parameters. A seperate module (e.g., senescence_coefficient_logistic) is needed to solve for the senescence parameters (i.e., kSeneLeaf, etc.) or these parameters need to be passed in the inital BioCro call.
	
	// Collect inputs and make calculations
    
    double Leaf = *Leaf_ip; //Current leaf biomass, Mg/ha
    double Stem = *Stem_ip; // current stem biomass, Mg/ha
    double Root = *Root_ip; // current root biomass, Mg/ha
    double Rhizome = *Rhizome_ip; // current rhizome biomass, Mg/ha
	
	double kSeneLeaf = *kSeneLeaf_ip; // percentage of biomass senesced, dimensionless
	double kSeneStem = *kSeneStem_ip; // percentage of biomass senesced, dimensionless
	double kSeneRoot = *kSeneRoot_ip; // percentage of biomass senesced, dimensionless
	double kSeneRhizome = *kSeneRhizome_ip; // percentage of biomass senesced, dimensionless
	
    double kLeaf = *kLeaf_ip; // partitioning growth coefficient, dimensionless
    double kStem = *kStem_ip; // partitioning growth coefficient, dimensionless
	double kRoot = *kRoot_ip; // partitioning growth coefficient, dimensionless
	double kRhizome = *kRhizome_ip; // partitioning growth coefficient, dimensionless
	double kGrain = *kGrain_ip; // partitioning growth coefficient, dimensionless
    
    double remobilization_fraction = *remobilization_fraction_ip; // fraction of lost leaf tissue that is remobilized to other parts of the plant. Remainder goes to the litter
	
    
    double senescence_leaf = kSeneLeaf * Leaf; // Amount of leaf senesced, Mg/ha
    double senescence_stem = kSeneStem * Stem; // Amount of stem senesced, Mg/ha
    double senescence_root = kSeneRoot * Root; // Amount of root senesced, Mg/ha
    double senescence_rhizome = kSeneRhizome * Rhizome; // Amount of rhizome senesced, Mg/ha
    
    double dLeaf = -senescence_leaf + kLeaf * senescence_leaf * remobilization_fraction; //change in leaf biomass = minus amount senesced + new leaf tissue from remobilized amount (Allows for leaves to start senescing while new leaves are still being produced), Mg/ha
    double dLeafLitter = senescence_leaf * (1 - remobilization_fraction); // Change in amount of biomass in leaf litter, Mg/ha
    
    double dStem = -senescence_stem + kStem * senescence_leaf * remobilization_fraction; // change in stem biomass = minus amount senesced + new stem tissue from remobilized leaf fraction, Mg/ha
    double dStemLitter = senescence_stem; // Amount of stem senesced in this time step, Mg/ha
    
    double dRoot = -senescence_root + kRoot * senescence_leaf * remobilization_fraction; //change in root biomass = minus amount senesced + new root tissue from remobilized leaf fraction, Mg/ha
    double dRootLitter = senescence_root; // Amount of root senesced in this time step, Mg/ha
    
    double dRhizome = -senescence_rhizome + kRhizome * senescence_leaf * remobilization_fraction; // change in rhizome biomass = minus amount senesced + new rhizome from remobilized leaf fraction, Mg/ha
    double dRhizomeLitter = senescence_rhizome; // amount of rhizome tissue senesced in this time step, Mg/ha

    double dGrain = kGrain * senescence_leaf * remobilization_fraction; // change in grain biomass = new grain from remobilized leaf fraction, Mg/ha. Currently do not include grain senescence.
    
    update(Leaf_op, dLeaf); // Mg/ha
    update(Stem_op, dStem); // Mg/ha
    update(Root_op, dRoot); // Mg/ha
    update(Rhizome_op, dRhizome); // Mg/ha
    update(Grain_op, dGrain); // Mg/ha
    
    update(LeafLitter_op, dLeafLitter); // Mg/ha
    update(StemLitter_op, dStemLitter); // Mg/ha
    update(RootLitter_op, dRootLitter); // Mg/ha
    update(RhizomeLitter_op, dRhizomeLitter); // Mg/ha
    
}

#endif
